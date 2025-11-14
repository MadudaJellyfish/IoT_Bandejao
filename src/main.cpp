#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "who_detect.hpp"
#include "who_frame_cap.hpp"
#include "who_frame_lcd_disp.hpp"

static const char *TAG = "PERSON_COUNTER";


// Estrutura para armazenar dados da detecção
typedef struct {
    uint16_t person_count;
    uint32_t frame_count;
    uint32_t detection_time_ms;
} detection_result_t;

// Configuração de pinos para Freenove ESP32-S3
static camera_config_t camera_config = {
    .pin_pwdn = -1,
    .pin_reset = -1,
    .pin_xclk = 15,
    .pin_siod = 4,
    .pin_sioc = 5,
    .pin_d7 = 16,
    .pin_d6 = 17,
    .pin_d5 = 18,
    .pin_d4 = 12,
    .pin_d3 = 10,
    .pin_d2 = 8,
    .pin_d1 = 9,
    .pin_d0 = 11,
    .pin_vsync = 6,
    .pin_href = 7,
    .pin_pclk = 13,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_RGB565,
    .frame_size = FRAMESIZE_QVGA,
    .jpeg_quality = 12,
    .fb_count = 2,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY
};


extern "C" void app_main() {
    ESP_LOGI(TAG, "\n╔════════════════════════════════════╗");
    ESP_LOGI(TAG, "║  CONTADOR DE PESSOAS COM ESP-WHO  ║");
    ESP_LOGI(TAG, "╚════════════════════════════════════╝\n");
    
    ESP_LOGI(TAG, "Iniciando aplicação esp-who...");

    //captura de frames
    ESP_LOGI(TAG, "Inicializando câmera...");
    who_frame_cap_handle_t cap_handle = who_frame_cap_create_from_config(NULL);
    if (cap_handle == NULL) {
        ESP_LOGE(TAG, "Falha ao inicializar captura de frames");
        return;
    }

    ESP_LOGI(TAG, "Inicializando detector...");
    who_detect_handle_t det_handle = who_detect_create_from_config(NULL);
    if (det_handle == NULL) {
        ESP_LOGE(TAG, "Falha ao inicializar detector");
        who_frame_cap_delete(cap_handle);
        return;
    }

    ESP_LOGI(TAG, "✅ Sistema iniciado com sucesso!");
    ESP_LOGI(TAG, "Processando frames...\n");
    
    int frame_count = 0;
    int person_count = 0;
    
    while (1) {
        frame_count++;
        
        // Capturar frame
        camera_fb_t *fb = who_frame_cap_get(cap_handle);
        if (!fb) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        
        // Executar detecção
        std::list<who_detect_result_t> &detect_results = 
            who_detect_run(det_handle, fb, ORIGIN_UNSPECIFIED);
        
        // Contar pessoas
        person_count = 0;
        for (auto &res : detect_results) {
            // Se você quer detectar especificamente pessoas:
            // if (strcmp(res.class_name, "person") == 0) {
            person_count++;
        }
        
        ESP_LOGI(TAG, "Frame %d | Objetos detectados: %d", 
                 frame_count, person_count);
        
        // Liberar recursos
        who_frame_cap_return(cap_handle, fb);
        
        // Delay para não sobrecarregar
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // Opcional: sair após alguns frames
        if (frame_count > 1000) {
            ESP_LOGI(TAG, "Limite de frames atingido");
            break;
        }
    }
    
    // Limpeza
    who_detect_delete(det_handle);
    who_frame_cap_delete(cap_handle);
    
    ESP_LOGI(TAG, "Aplicação finalizada");
}