#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "RTC.h"
#include "main.h"
#include "stm32f4xx_conf.h"

#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555

static void LED_task(void *pvParameters);
static void LCD_display_task(void *pvParameters);
xTaskHandle *pvLEDTask;



int main(void)
{
  RTC_setting();
  
  /* Initialize LEDs to be managed by GPIO */
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);

  /* Turn OFF all LEDs */
  STM_EVAL_LEDOff(LED4);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED5);
  STM_EVAL_LEDOff(LED6);
    
  /* Create a task to display something in the LCD. */
  xTaskCreate(LCD_display_task,
             (signed portCHAR *) "Liquid Crystal Display",
             512 /* stack size */, NULL,
             tskIDLE_PRIORITY + 5, NULL);

  /* Create a task to flash the LED. */
  xTaskCreate(LED_task,
             (signed portCHAR *) "LED Flash",
             512 /* stack size */, NULL,
             tskIDLE_PRIORITY + 5,  pvLEDTask );

  /* Start running the tasks. */
  vTaskStartScheduler(); 

  return 0;
}

static void LCD_display_task(void *pvParameters)
{
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;

<<<<<<< HEAD
  uint8_t hour=23;
  uint8_t min=45;
  uint8_t sec=50;
  uint8_t year=13;
  uint8_t month=11;
  uint8_t data=24;
=======
>>>>>>> 9efbcffb81682db9d1453d562481f23f9984fb42
  LCD_GPIO_Init();
  Init_LCD();     //LCD  initialization       
 
  while(1){

    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
    showCalendar_time((uint8_t)RTC_TimeStruct.RTC_Hours,
                      (uint8_t)RTC_TimeStruct.RTC_Minutes,
                      (uint8_t)RTC_TimeStruct.RTC_Seconds);
    showCalendar_date((uint8_t)RTC_DateStruct.RTC_Year,
                      (uint8_t)RTC_DateStruct.RTC_Month,
                      (uint8_t)RTC_DateStruct.RTC_Date);

  }
}

static void LED_task(void *pvParameters)
{
  RCC_ClocksTypeDef RCC_Clocks;
  uint8_t togglecounter = 0x00;

  while(1)
  {    
      /* Toggle LED3 */
      STM_EVAL_LEDToggle(LED3);
      vTaskDelay(200);
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED4);
      vTaskDelay(200);
      /* Toggle LED5 */
      STM_EVAL_LEDToggle(LED5);
      vTaskDelay(200);
      /* Toggle LED6 */
  }
}



void Fail_Handler(void)
{
  /* Erase last sector */ 
  FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
  /* Write FAIL code at last word in the flash memory */
  FLASH_ProgramWord(TESTRESULT_ADDRESS, ALLTEST_FAIL);
  
  while(1)
  {
    /* Toggle Red LED */
    STM_EVAL_LEDToggle(LED5);
    vTaskDelay(5);
  }
}


void vApplicationTickHook()
{
}


