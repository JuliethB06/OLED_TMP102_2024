/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
#include "I2C.h"
#include "ThisThread.h"
#include "mbed.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h" 
#include <cstring>

// Configuración de pines y constantes
#define TMP102_ADDR (0x48 << 1) // Dirección I2C correcta del TMP102
#define SAMPLE_RATE 1s

// Inicialización de objetos
I2C i2c(D14, D15);
BufferedSerial serial(USBTX, USBRX, 9600);
Adafruit_SSD1306_I2c oled(i2c, D0);
AnalogIn pot(A0);

// Función para leer temperatura del TMP102
float readTemperature() {
    char cmd = 0x00; // Registro de temperatura
    char data[2];
    
    i2c.write(TMP102_ADDR, &cmd, 1);
    i2c.read(TMP102_ADDR, data, 2);
    
    int16_t temp = (data[0] << 4) | (data[1] >> 4);
    return temp * 0.0625f;
}

// Función para mostrar datos en OLED y serial
void displayData(const char* label, float value) {
    char buffer[32];
    int intPart = int(value);
    int decPart = int((value - intPart) * 10000);
    
    sprintf(buffer, "%s\n%d.%04d\n", label, intPart, decPart);
    
    oled.printf(buffer);
    oled.display();
    
    // Usa sizeof(buffer) para calcular el tamaño del buffer
    serial.write(buffer, sizeof(buffer) - 1); // -1 para evitar enviar el byte nulo
}

// Función para inicializar la pantalla OLED
void initDisplay() {
    oled.begin();
    oled.setTextSize(1);
    oled.setTextColor(1);
    oled.clearDisplay();
    oled.display();
    printf("Programa iniciado\n");
}

int main() {
    // Inicialización
    initDisplay();
    
    while (true) {
        // Leer sensores
        float voltage = pot.read() * 3.3f;
        float temperature = readTemperature();
        
        // Mostrar datos
        oled.clearDisplay();
        displayData("Voltaje (V):", voltage);
        ThisThread::sleep_for(2s);
        
        oled.clearDisplay();
        displayData("Temp (C):", temperature);
        ThisThread::sleep_for(2s);
    }
}
