#include <M5StickCPlus.h>
#include <driver/i2s.h>

const int servo_pin = 26;
int freq            = 50;
int ledChannel      = 0;
int resolution      = 10;
uint16_t oldy[160];
int16_t *adcBuffer = NULL;

void setup() {
    Serial.begin(115200);
    while(!Serial);
    M5.begin();
    M5.Lcd.setTextSize(2);
    M5.Lcd.setRotation(3);

    M5.Lcd.setCursor(25, 80);
    M5.Lcd.println("speaker test");

    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(servo_pin, ledChannel);
    ledcWrite(ledChannel, 256);  // 0°

    M5.Lcd.setCursor(25, 100);
    M5.Lcd.println("mic test");
    i2sInit();
}

void loop() {
    M5.update();
    M5.Lcd.setCursor(30, 60);
    bool btnA = M5.BtnA.read();
    if(btnA == 1) { 
        M5.Lcd.fillScreen(RED);
        M5.Lcd.setTextColor(WHITE, RED);
        // Play music here
    } else {
        M5.Lcd.fillScreen(GREEN);
        M5.Lcd.setTextColor(WHITE, GREEN);
        // Stop music here
    }
    M5.Lcd.print(btnA);    //Print the state of button A pressed.

    // Mic task execution
    size_t bytesread;
    i2s_read(I2S_NUM_0, (char *)adcBuffer, sizeof(oldy), &bytesread, portMAX_DELAY);
    showSignal();
}

void showSignal() {
    int y;
    for (int n = 0; n < 160; n++) {
        y = adcBuffer[n] / 16 + 64; // Scale and adjust
        M5.Lcd.drawPixel(n, oldy[n], WHITE);
        M5.Lcd.drawPixel(n, y, BLACK);
        oldy[n] = y;
    }
}

void i2sInit() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 128,
    };

    i2s_pin_config_t pin_config;
    pin_config.bck_io_num = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num = 0; // Change if necessary
    pin_config.data_in_num = 34; // Change if necessary

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}
