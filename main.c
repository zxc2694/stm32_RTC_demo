
/* Includes ------------------------------------------------------------------*/

//#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "main.h"
#include "stm32f4xx_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint16_t PrescalerValue = 0;

__IO uint32_t TimingDelay;
__IO uint8_t UserButtonPressed = 0x00;


/* Private function prototypes -----------------------------------------------*/
static void LED_task(void *pvParameters);
static void button_task(void *pvParameters);

/**
  * @brief  This function handles EXTI0_IRQ Handler.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  UserButtonPressed = 0x01;
  
  /* Clear the EXTI line pending bit */
  EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
}

int main(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  //initialize RTC
  RTC_InitTypeDef RTC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);   /* Enable the PWR clock */
  PWR_BackupAccessCmd(ENABLE);                          /* Allow access to RTC */

  RCC_LSICmd(ENABLE);                                   /* Enable the LSI OSC */    
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);   /* Wait till LSI is ready */  
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);               /* Select the RTC Clock Source */

  RCC_RTCCLKCmd(ENABLE);                                /* Enable the RTC Clock */
  RTC_WaitForSynchro();                                 /* Wait for RTC APB registers synchronisation */

  /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv = 0xF9;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);


  //setting time
  /* set 8:29:55 */
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_TimeStruct.RTC_Hours = 8;
  RTC_TimeStruct.RTC_Minutes = 29;
  RTC_TimeStruct.RTC_Seconds = 55;

  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);


  //initialize RTC alarm

  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);



  //set alarm time

  RTC_AlarmTypeDef RTC_AlarmStructure;

  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);   /* disable before setting or cann't write */

  /* set alarm time 8:30:0 everyday */
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = 0x00;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 8;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 30;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0;
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31; // Nonspecific
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay; // Everyday 
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

  /* Enable Alarm */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
  RTC_ClearFlag(RTC_FLAG_ALRAF);




  /* Initialize LEDs and User_Button on STM32F4-Discovery --------------------*/
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI); 

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
    
  /* Reset UserButton_Pressed variable */
  UserButtonPressed = 0x00;

  /* Create a task to flash the LED. */
  xTaskCreate(LED_task,
             (signed portCHAR *) "LED Flash",
             512 /* stack size */, NULL,
             tskIDLE_PRIORITY + 5, NULL);

  /* Create a task to button check. */
  xTaskCreate(button_task,
             (signed portCHAR *) "User Button",
             512 /* stack size */, NULL,
             tskIDLE_PRIORITY + 5, NULL);

  /* Start running the tasks. */
  vTaskStartScheduler(); 

  return 0;
}

static void LED_task(void *pvParameters)
{
  RCC_ClocksTypeDef RCC_Clocks;
  uint8_t togglecounter = 0x00;

  while(1)
  {    
      /* Toggle LED3 */
      STM_EVAL_LEDToggle(LED3);
      vTaskDelay(10);
      /* Toggle LED4 */
      STM_EVAL_LEDToggle(LED4);
      vTaskDelay(10);
      /* Toggle LED5 */
      STM_EVAL_LEDToggle(LED5);
      vTaskDelay(10);
      /* Toggle LED6 */
      STM_EVAL_LEDToggle(LED6);
      vTaskDelay(10);
  }
}

static void button_task(void *pvParameters)
{
	while (1)
	{
		    /* Waiting User Button is pressed */
    		    if (UserButtonPressed == 0x01)
    		    {
      		    	/* Toggle LED4 */
      			STM_EVAL_LEDToggle(LED4);
      			vTaskDelay(100);
      			/* Toggle LED3 */
      			STM_EVAL_LEDToggle(LED3);
      			vTaskDelay(100);
    		    }
		    /* Waiting User Button is Released */
    		    while (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET);
		    UserButtonPressed = 0x00;
	}
}


/**
  * @brief  This function handles the test program fail.
  * @param  None
  * @retval None
  */
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


