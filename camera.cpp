#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "";
const char* password = "";
const char* laptopUrl = "device local ip:port/upload"; 


#define TRIGGER_PIN 13
#define COMM_TX_PIN 2 
#define COMM_RX_PIN 14 

#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    21
#define SIOD_GPIO_NUM    26
#define SIOC_GPIO_NUM    27
#define Y9_GPIO_NUM      35
#define Y8_GPIO_NUM      34
#define Y7_GPIO_NUM      39
#define Y6_GPIO_NUM      36
#define Y5_GPIO_NUM      19
#define Y4_GPIO_NUM      18
#define Y3_GPIO_NUM       5
#define Y2_GPIO_NUM       4
#define VSYNC_GPIO_NUM   25
#define HREF_GPIO_NUM    23
#define PCLK_GPIO_NUM    22

volatile bool photoRequested = false;

void IRAM_ATTR onTrigger() {
    photoRequested = true;
}

void setup() {
    Serial.begin(115200);

    Serial2.begin(9600, SERIAL_8N1, COMM_RX_PIN, COMM_TX_PIN);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 1;

    if (esp_camera_init(&config) != ESP_OK) return;

    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    pinMode(TRIGGER_PIN, INPUT_PULLDOWN);
    attachInterrupt(TRIGGER_PIN, onTrigger, RISING);
}

void loop() {
    if (photoRequested) {
        delay(250); 
        

        for(int i = 0; i < 4; i++) {
            camera_fb_t * fb = esp_camera_fb_get();
            if (fb) { esp_camera_fb_return(fb); delay(50); }
        }

        camera_fb_t * fb = esp_camera_fb_get();
        if (fb) {
            HTTPClient http;
            http.begin(laptopUrl);
            http.addHeader("Content-Type", "image/jpeg");
            
            int httpResponseCode = http.POST(fb->buf, fb->len);
            
            if (httpResponseCode == 200) {
                // READ THE BIN NUMBER FROM PYTHON
                String binResponse = http.getString();
                Serial.println("Received Bin: " + binResponse);
                
                // SEND TO ELECTRONICS BOARD
                Serial2.print(binResponse); 
            }
            
            http.end();
            esp_camera_fb_return(fb);
        }
        photoRequested = false;
    }
}