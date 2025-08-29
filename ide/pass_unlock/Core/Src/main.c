#include "main.h"
#include <string.h>
#include <stdio.h>
#include "stm32f1xx.h"
#include "lcd_i2c.h"
#include "keypad.h"

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

I2C_LCD_HandleTypeDef lcd;

// Define pulse values for servo angles
#define SERVO_0_DEGREE    1000    // 0.5 ms pulse
#define SERVO_90_DEGREE   1500   // 1.5 ms pulse
#define SERVO_180_DEGREE  2500   // 2.5 ms pulse

#define R1_PORT GPIOA
#define R1_Pin GPIO_PIN_7

#define R2_PORT GPIOA
#define R2_Pin GPIO_PIN_6

#define R3_PORT GPIOA
#define R3_Pin GPIO_PIN_5

#define R4_PORT GPIOA
#define R4_Pin GPIO_PIN_4

#define C1_PORT GPIOA
#define C1_Pin GPIO_PIN_3

#define C2_PORT GPIOA
#define C2_Pin GPIO_PIN_2

#define C3_PORT GPIOA
#define C3_Pin GPIO_PIN_1

#define C4_PORT GPIOA
#define C4_Pin GPIO_PIN_0

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
char Keys_Detect(void);

#define PASSWORD_LENGTH 4
char correctPassword[] = "1234";
char enteredPassword[PASSWORD_LENGTH + 1] = "";
uint8_t passwordIndex = 0;

void moveServo(uint16_t pulseWidth) {
		    // Move Servo to 90 degree
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulseWidth);
	HAL_Delay(300);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

}

void ringBuzzer(void)
{
  for (int i = 0; i < 3; i++)  // Ring 3 times
  {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); // Buzzer ON
    HAL_Delay(1000);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); // Buzzer OFF
    HAL_Delay(1000);
  }
}

int main(void)
{
    // Initialize
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM1_Init();

    HAL_Delay(1000);

    lcd.hi2c = &hi2c1;
    lcd.address = 0x27 << 1;

    HAL_Delay(1000);
    lcd_init(&lcd);
    lcd_gotoxy(&lcd, 0, 0); // Go to first row, first column
    HAL_Delay(1000);
    lcd_puts(&lcd, "Enter Password"); // Print message
    HAL_Delay(3000);

  while (1)
  {
	  HAL_Delay(3000);
	  lcd_gotoxy(&lcd, 0, 1);
	  lcd_send_cmd(&lcd, 0x0F);
	  HAL_Delay(1000);
	  //char key = Keys_Detect();
	  char key = get_key();
	  if (key != '\0' && passwordIndex < 4) {
          lcd_gotoxy(&lcd, 0, 1);
          lcd_putchar(&lcd, key);
		          enteredPassword[passwordIndex++] = key;  // Save key
		          if (passwordIndex == 4) {
		        	  enteredPassword[4] = '\0';
		          		  	        HAL_Delay(500);
		          		  	        if (strcmp(enteredPassword, correctPassword) == 0) {
		          		  	          lcd_clear(&lcd);
		          		  	          lcd_gotoxy(&lcd, 0, 0);
		          		  	          lcd_puts(&lcd, "Access Granted");
		          		  	          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); // Buzzer ON
		          		  	          HAL_Delay(200);
		          		  	          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); // Buzzer OFF
		          		  	          moveServo(SERVO_180_DEGREE);  // Move to 90 degrees (1.5 ms pulse)
		          		  	          HAL_Delay(3000);  // Wait for 3 seconds at 90 degrees
		          		  	          moveServo(SERVO_0_DEGREE);  // Move back to 0 degrees (1 ms pulse)
		          		  	        } else {
		          		  	          lcd_clear(&lcd);
		          		  	          lcd_gotoxy(&lcd, 0, 0);
		          		  	          lcd_puts(&lcd, "Wrong Password");
		          		  	          ringBuzzer();
		          		  	          moveServo(SERVO_180_DEGREE);  // Move to 90 degrees (1.5 ms pulse)// Wait for 3 seconds at 90 degrees
		          		  	          HAL_Delay(3000);  // Wait for 3 seconds at 90 degrees
		          		  	          moveServo(SERVO_0_DEGREE);  // Move back to 0 degrees (1 ms pulse)
		          		  	          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); // Buzzer OFF

		          		  	        }
		          		  	        HAL_Delay(2000);
		          		  	        lcd_clear(&lcd);
		          		  	        lcd_gotoxy(&lcd, 0, 0);
		          		  	        lcd_puts(&lcd, "Enter Password...");
		          		  	        passwordIndex = 0;
		          		  	        memset(enteredPassword, 0, sizeof(enteredPassword));
		          }

	  }


  }
}

char Keys_Detect(void)
{
    // Row 1, setting the first row high
    HAL_GPIO_WritePin(GPIOA, R1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, R2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R4_Pin, GPIO_PIN_RESET);

    if (HAL_GPIO_ReadPin(GPIOA, C1_Pin) == GPIO_PIN_SET)
    {
        // Return '1' if C1 is pressed
        return '1';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C2_Pin) == GPIO_PIN_SET)
    {
        // Return '2' if C2 is pressed
        return '2';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C3_Pin) == GPIO_PIN_SET)
    {
        // Return '3' if C3 is pressed
        return '3';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C4_Pin) == GPIO_PIN_SET)
    {
        // Return 'A' if C4 is pressed
        return 'A';
    }

    // Row 2, setting the second row high
    HAL_GPIO_WritePin(GPIOA, R1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, R3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R4_Pin, GPIO_PIN_RESET);

    if (HAL_GPIO_ReadPin(GPIOA, C1_Pin) == GPIO_PIN_SET)
    {
        // Return '4' if C1 is pressed in row 2
        return '4';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C2_Pin) == GPIO_PIN_SET)
    {
        // Return '5' if C2 is pressed in row 2
        return '5';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C3_Pin) == GPIO_PIN_SET)
    {
        // Return '6' if C3 is pressed in row 2
        return '6';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C4_Pin) == GPIO_PIN_SET)
    {
        // Return 'B' if C4 is pressed in row 2
        return 'B';
    }

    // Row 3, setting the third row high
    HAL_GPIO_WritePin(GPIOA, R1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, R4_Pin, GPIO_PIN_RESET);

    if (HAL_GPIO_ReadPin(GPIOA, C1_Pin) == GPIO_PIN_SET)
    {
        // Return '7' if C1 is pressed in row 3
        return '7';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C2_Pin) == GPIO_PIN_SET)
    {
        // Return '8' if C2 is pressed in row 3
        return '8';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C3_Pin) == GPIO_PIN_SET)
    {
        // Return '9' if C3 is pressed in row 3
        return '9';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C4_Pin) == GPIO_PIN_SET)
    {
        // Return '#' if C4 is pressed in row 3
        return '#';
    }

    // Row 4, setting the fourth row high
    HAL_GPIO_WritePin(GPIOA, R1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, R4_Pin, GPIO_PIN_SET);

    if (HAL_GPIO_ReadPin(GPIOA, C1_Pin) == GPIO_PIN_SET)
    {
        // Return '*' if C1 is pressed in row 4
        return '*';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C2_Pin) == GPIO_PIN_SET)
    {
        // Return '0' if C2 is pressed in row 4
        return '0';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C3_Pin) == GPIO_PIN_SET)
    {
        // Return '#' if C3 is pressed in row 4
        return '#';
    }

    if (HAL_GPIO_ReadPin(GPIOA, C4_Pin) == GPIO_PIN_SET)
    {
        // Return 'D' if C4 is pressed in row 4
        return 'D';
    }

    // Return an invalid character (e.g., '\0') if no key is pressed
    return '\0';
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{
  __HAL_RCC_I2C1_CLK_ENABLE();
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_TIM1_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 15;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 9999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim1);

}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();


    /*Configure GPIO pins : PA0 PA1 PA2 PA3 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PA4 PA5 PA6 PA7 */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure GPIO : PA8 Servo-motor
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;      // Alternate Function Push-Pull
  GPIO_InitStruct.Pull = GPIO_NOPULL;           // No pull-up or pull-down
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // High Speed
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 Buzzer */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 PB7 Display */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 LED */
  GPIO_InitStruct.Pin = GPIO_PIN_13;      // On-board LED is connected to PC13
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Push-pull mode for output
  GPIO_InitStruct.Pull = GPIO_NOPULL;         // No pull-up or pull-down resistors
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Low speed for general use
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);  // Toggle LED to indicate failure
	  HAL_Delay(500);
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif /* USE_FULL_ASSERT */
