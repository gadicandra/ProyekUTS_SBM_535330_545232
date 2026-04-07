#include "dht22.h"

extern TIM_HandleTypeDef htim3;

static void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us);
}

static void set_output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = DHT22_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT22_GPIO_Port, &GPIO_InitStruct);
}

static void set_input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin  = DHT22_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT22_GPIO_Port, &GPIO_InitStruct);
}

DHT22_Data DHT22_Read(void) {
    DHT22_Data result = {0};
    uint8_t data[5] = {0};
    uint32_t timeout;

    // Start signal
    set_output();
    HAL_GPIO_WritePin(DHT22_GPIO_Port, DHT22_Pin, GPIO_PIN_RESET);
    HAL_Delay(18);
    HAL_GPIO_WritePin(DHT22_GPIO_Port, DHT22_Pin, GPIO_PIN_SET);
    delay_us(30);

    // Wait for sensor response
    set_input();

    // Tambahkan sedikit proteksi agar tidak nyangkut selamanya jika sensor dicabut
    timeout = 1000;
    while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET) {
        if (--timeout == 0) return (DHT22_Data){0, 0, 1}; // Timeout jika tetap HIGH
    }



    timeout = 10000;
    while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET)
        if (--timeout == 0) { result.status = 1; return result; }

    timeout = 10000;
    while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_RESET)
        if (--timeout == 0) { result.status = 1; return result; }

    timeout = 10000;
    while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET)
        if (--timeout == 0) { result.status = 1; return result; }

    // Read 40 bits
    for (int i = 0; i < 40; i++) {
        timeout = 10000;
        while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_RESET)
            if (--timeout == 0) { result.status = 1; return result; }

        delay_us(40);

        data[i / 8] <<= 1;
        if (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET)
            data[i / 8] |= 1;

        timeout = 10000;
        while (HAL_GPIO_ReadPin(DHT22_GPIO_Port, DHT22_Pin) == GPIO_PIN_SET)
            if (--timeout == 0) { result.status = 1; return result; }
    }

    // Checksum
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        result.status = 1;
        return result;
    }

    // Parse
    result.humidity    = ((data[0] << 8) | data[1]) / 10.0f;
    int16_t raw_temp   = ((data[2] & 0x7F) << 8) | data[3];
    result.temperature = raw_temp / 10.0f;
    if (data[2] & 0x80) result.temperature *= -1;

    result.status = 0;
    return result;
}
