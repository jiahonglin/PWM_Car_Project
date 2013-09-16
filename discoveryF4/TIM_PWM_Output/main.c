/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
//Library config for this project!!!!!!!!!!!
#include "stm32f4xx_conf.h"

/** @addtogroup STM32F4-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment = 4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
  
uint16_t PrescalerValue = 0;

__IO uint32_t TimingDelay;
__IO uint8_t DemoEnterCondition = 0x00;
__IO uint8_t UserButtonPressed = 0x00;
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;  
__IO int8_t X_Offset, Y_Offset, Z_Offset  = 0x00;
uint8_t Buffer[6];
int Working_Time = 50000;
int Work_Count = 0;

/* Private function prototypes -----------------------------------------------*/
static uint32_t Demo_USBConfig(void);
static void TIM4_Config(void);
static void Demo_Exec(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    SystemInit();
    
    RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOD , ENABLE );
    
    GPIO_PIN_INIT();

    while(1)
    {
      Car_Test2();
    }
    
}

void GPIO_PIN_INIT(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_TIM3);
    
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;            // Alt Function - Push Pull
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init( GPIOD, &GPIO_InitStructure ); 
}

/**
 * @brief Control motors of wheels.
 * @param duration The count of PWM cycles, which is Delay(1000).
 * @param duty_left The duty cycle of left wheels, in range [0, 100].
 * @param duty_right The duty cycle of right wheels, in range [0, 100].
 */
void go(uint32_t duration, int16_t duty_left, int16_t duty_right)
{
  uint32_t delay[5];
  int bit[2], l=0, r=1, i=4;

  if(duty_left<duty_right) l=1, r=0;

  delay[l]=1000*(100-ABS(duty_left))/2;
  bit[l]= duty_left<0 ? GPIO_Pin_11 : GPIO_Pin_10;
  delay[3-l]=1000-delay[l];

  delay[r]=1000*(100-ABS(duty_right))/2;
  bit[r]= duty_right<0 ? GPIO_Pin_8 : GPIO_Pin_6;
  delay[3-r]=1000-delay[r];

  for(delay[4]=1000; i>0; i--) delay[i]-=delay[i-1];

  GPIO_ResetBits(GPIOD , GPIO_Pin_6|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11);

  for(i=0; i<duration; i++){
    Delay(delay[0]);

    GPIO_SetBits(GPIOD , bit[0]);
    Delay(delay[1]);

    GPIO_SetBits(GPIOD , bit[1]);
    Delay(delay[2]);

    GPIO_ResetBits(GPIOD , bit[1]);
    Delay(delay[3]);

    GPIO_ResetBits(GPIOD , bit[0]);
    Delay(delay[4]);
  }
}



void Car_Test(void){
  Car_forward(1000);
  Delay(10000);

  Car_Stop(1000);
  Delay(10000);

  Car_Backward(1000);
  Delay(10000);

  Car_Stop(1000);
  Delay(10000);
}

void Car_Test2(void){
   go(5000, 50, 50);

   go(5000, 75, 50);

   go(5000, 50, 75);

}


//PWM control the speed of motor
void Car_forward(int PWM_Delay){
  while(Work_Count <Working_Time){
    GPIO_SetBits(GPIOD , GPIO_Pin_6);
    GPIO_ResetBits(GPIOD , GPIO_Pin_8);
    Delay(PWM_Delay);

    GPIO_ResetBits(GPIOD , GPIO_Pin_6);
    GPIO_ResetBits(GPIOD , GPIO_Pin_8);
    Delay(1000);
  
    Work_Count++;
  }
  Work_Count = 0;
}

void Car_Stop(int Stop_Delay){
  GPIO_ResetBits(GPIOD , GPIO_Pin_6);
  GPIO_ResetBits(GPIOD , GPIO_Pin_8);
  Delay(Stop_Delay);
}

void Car_Backward(int PWM_Delay){
  while(Work_Count <Working_Time){
    GPIO_ResetBits(GPIOD , GPIO_Pin_6);
    GPIO_SetBits(GPIOD , GPIO_Pin_8);
    Delay(PWM_Delay);

    GPIO_ResetBits(GPIOD , GPIO_Pin_6);
    GPIO_ResetBits(GPIOD , GPIO_Pin_8);
    Delay(1000);
    Work_Count++;
  }
  Work_Count=0;
}

/**
  * @brief  Execute the demo application.
  * @param  None
  * @retval None
  */
static void Demo_Exec(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  uint8_t togglecounter = 0x00;
  
  while(1)
  {
    DemoEnterCondition = 0x00;
    
    /* Reset UserButton_Pressed variable */
    UserButtonPressed = 0x00;
    
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);  
    

    
    /* Waiting User Button is pressed */
    while (UserButtonPressed == 0x00)
    {
      
    }
    
    /* Waiting User Button is Released */
    while (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET)
    {}
    UserButtonPressed = 0x00;
    

    /* MEMS configuration */
    LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
    LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
    LIS302DL_InitStruct.Axes_Enable = LIS302DL_XYZ_ENABLE;
    LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
    LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
    LIS302DL_Init(&LIS302DL_InitStruct);
    
    /* Required delay for the MEMS Accelerometre: Turn-on time = 3/Output data Rate 
    = 3/100 = 30ms */
    Delay(30);
    
    DemoEnterCondition = 0x01;
    /* MEMS High Pass Filter configuration */
    LIS302DL_FilterStruct.HighPassFilter_Data_Selection = LIS302DL_FILTEREDDATASELECTION_OUTPUTREGISTER;
    LIS302DL_FilterStruct.HighPassFilter_CutOff_Frequency = LIS302DL_HIGHPASSFILTER_LEVEL_1;
    LIS302DL_FilterStruct.HighPassFilter_Interrupt = LIS302DL_HIGHPASSFILTERINTERRUPT_1_2;
    LIS302DL_FilterConfig(&LIS302DL_FilterStruct);
    
    LIS302DL_Read(Buffer, LIS302DL_OUT_X_ADDR, 6);
    X_Offset = Buffer[0];
    Y_Offset = Buffer[2];
    Z_Offset = Buffer[4];
    
    /* USB configuration */
    Demo_USBConfig();
    
    /* Waiting User Button is pressed */
    while (UserButtonPressed == 0x00)
    {}
    
    /* Waiting User Button is Released */
    while (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET)
    {}
    
    /* Disable SPI1 used to drive the MEMS accelerometre */
    SPI_Cmd(LIS302DL_SPI, DISABLE);
    
    /* Disconnect the USB device */
    DCD_DevDisconnect(&USB_OTG_dev);
    USB_OTG_StopDevice(&USB_OTG_dev);
  }
}

/**
  * @brief  Initializes the USB for the demonstration application.
  * @param  None
  * @retval None
  */
static uint32_t Demo_USBConfig(void)
{
  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc, 
            &USBD_HID_cb, 
            &USR_cb);
  
  return 0;
}


/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0){
      TimingDelay--;
  };
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
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
    Delay(5);
  }
}

/**
  * @brief  MEMS accelerometre management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LIS302DL_TIMEOUT_UserCallback(void)
{
  /* MEMS Accelerometer Timeout error occured during Test program execution */
  if (DemoEnterCondition == 0x00)
  {
    /* Timeout error occured for SPI TXE/RXNE flags waiting loops.*/
    Fail_Handler();    
  }
  /* MEMS Accelerometer Timeout error occured during Demo execution */
  else
  {
    while (1)
    {   
    }
  }
  return 0;  
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
