#include "stm32f4xx_hal.h"
#include "main.h"

// mesafe sensörü pin tanımlamaları
#define TRIG_PIN GPIO_PIN_0
#define ECHO_PIN GPIO_PIN_1
#define TRIG_PORT GPIOA
#define ECHO_PORT GPIOA

// motor kontrol pinleri
#define MOTOR_A_PIN GPIO_PIN_2
#define MOTOR_B_PIN GPIO_PIN_3
#define MOTOR_PORT GPIOB

// LED kontrol pinleri
#define KIRMIZI_LED_PIN GPIO_PIN_4
#define SARI_LED_PIN GPIO_PIN_5
#define YESIL_LED_PIN GPIO_PIN_6
#define LED_PORT GPIOB

void SystemClock_Config(void);
void Error_Handler(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();

  // GPIO başlatma
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // mesafe sensörü için TRIG pinini çıkış için ayarlamak
  GPIO_InitStruct.Pin = TRIG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

  // mesafe sensörü için ECHO pinini giriş için ayarlamak
  GPIO_InitStruct.Pin = ECHO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO_PORT, &GPIO_InitStruct);

  // motor kontrol pinlerini çıkış için ayarlamak
  GPIO_InitStruct.Pin = MOTOR_A_PIN | MOTOR_B_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(MOTOR_PORT, &GPIO_InitStruct);

  // LED kontrol pinlerini çıkış için ayarlamak
  GPIO_InitStruct.Pin = KIRMIZI_LED_PIN | SARI_LED_PIN | YESIL_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  while (1) {
    // mesafe ölçümü yapmak için TRIG pini 10 mikrosaniye boyunca yükseğe çektim
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    // ECHO pininin yüksek olduğu süreyi ölçüyor (mesafe ölçümü)
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_RESET);
    uint32_t start_time = HAL_GetTick();

    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET);
    uint32_t end_time = HAL_GetTick();
    uint32_t pulse_duration = end_time - start_time;

    // mesafeyi hesaplama 
    float distance = (pulse_duration * 0.0343) / 2;

    // mesafe değerine göre motor ve LED kontrolü
    if (distance >= 250 && distance <= 350) {
      // mesafe 250 - 350 cm aralığında ise sarı LED yanmalı
      HAL_GPIO_WritePin(KIRMIZI_LED_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(YESIL_LED_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SARI_LED_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_B_PIN, GPIO_PIN_RESET);  // Motor dönmemeli
    } else if (distance > 350) {
      // mesafe 350 cm'den büyükse yeşil LED yanmalı ve motor saat yönünde dönmeli
      HAL_GPIO_WritePin(KIRMIZI_LED_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SARI_LED_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(YESIL_LED_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_B_PIN, GPIO_PIN_RESET);  // Motor saat yönünde dönmeli
    } else {
      // mesafe 250 cm'den küçükse kırmızı LED yanmalı ve motor saat yönünün tersine dönmeli
      HAL_GPIO_WritePin(YESIL_LED_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(SARI_LED_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(KIRMIZI_LED_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_A_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(MOTOR_PORT, MOTOR_B_PIN, GPIO_PIN_SET);  // Motor saat yönünün tersine dönmeli
    }

    HAL_Delay(1000);  // her hareket sonrası biraz beklemesi iyi olabilir
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

void Error_Handler(void) {
  while (1) {
    // hata vermesi durumunda
  }
}
