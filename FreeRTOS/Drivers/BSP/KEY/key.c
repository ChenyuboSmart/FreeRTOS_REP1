/**
 ****************************************************************************************************
 * @file        key.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-20
 * @brief       �������� ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200420
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"
/*FreeRTOS********************************************************************/
#include "./FreeRTOS/include/FreeRTOS.h"
#include "task.h"
/**
 * @brief       ������ʼ������
 * @param       ��
 * @retval      ��
 */
void key_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    KEY0_GPIO_CLK_ENABLE();                                     /* KEY0ʱ��ʹ�� */
    KEY1_GPIO_CLK_ENABLE();                                     /* KEY1ʱ��ʹ�� */
    WKUP_GPIO_CLK_ENABLE();                                     /* WKUPʱ��ʹ�� */

    gpio_init_struct.Pin = KEY0_GPIO_PIN;                       /* KEY0���� */
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY0_GPIO_PORT, &gpio_init_struct);           /* KEY0����ģʽ����,�������� */

    gpio_init_struct.Pin = KEY1_GPIO_PIN;                       /* KEY1���� */
    gpio_init_struct.Mode = GPIO_MODE_INPUT;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLUP;                        /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(KEY1_GPIO_PORT, &gpio_init_struct);           /* KEY1����ģʽ����,�������� */

    gpio_init_struct.Pin = WKUP_GPIO_PIN;                       /* WKUP���� */
    gpio_init_struct.Mode = GPIO_MODE_IT_RISING;                    /* ���� */
    gpio_init_struct.Pull = GPIO_PULLDOWN;                      /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;              /* ���� */
    HAL_GPIO_Init(WKUP_GPIO_PORT, &gpio_init_struct);           /* WKUP����ģʽ����,�������� */

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0); /* 抢占0，子优先级0 ,函数格式 void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t     PreemptPriority, uint32_t SubPriority)*/
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);         /* 使能中断线0 ,函数格式 void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)*/
   
    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 1); /* 抢占0，子优先级1 ,函数格式 void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t     PreemptPriority, uint32_t SubPriority)*/
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);         /* 使能中断线0 ,函数格式 void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)*/
}

/**
 * @brief       ����ɨ�躯��
 * @note        �ú�������Ӧ���ȼ�(ͬʱ���¶������): WK_UP > KEY1 > KEY0!!
 * @param       mode:0 / 1, ���庬������:
 *   @arg       0,  ��֧��������(���������²���ʱ, ֻ�е�һ�ε��û᷵�ؼ�ֵ,
 *                  �����ɿ��Ժ�, �ٴΰ��²Ż᷵��������ֵ)
 *   @arg       1,  ֧��������(���������²���ʱ, ÿ�ε��øú������᷵�ؼ�ֵ)
 * @retval      ��ֵ, ��������:
 *              KEY0_PRES, 1, KEY0����
 *              KEY1_PRES, 2, KEY1����
 *              WKUP_PRES, 3, WKUP����
 */
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;  /* �������ɿ���־ */
    uint8_t keyval = 0;

    if (mode) key_up = 1;       /* ֧������ */

    if (key_up && (KEY0 == 0 || KEY1 == 0 || WK_UP == 1))  /* �����ɿ���־Ϊ1, ��������һ������������ */
    {
        vTaskDelay(10);
        key_up = 0;

        if (KEY0 == 0)  keyval = KEY0_PRES;

        if (KEY1 == 0)  keyval = KEY1_PRES;

        if (WK_UP == 1) keyval = WKUP_PRES;
    }
    else if (KEY0 == 1 && KEY1 == 1 && WK_UP == 0) /* û���κΰ�������, ��ǰ����ɿ� */
    {
        key_up = 1;
    }

    return keyval;              /* ���ؼ�ֵ */
}

void EXTI4_IRQHandler(void)                              //某一条EXTI线的外部中断函数,这个参考启动文件.s
{
    HAL_GPIO_EXTI_IRQHandler(KEY0_GPIO_PIN);         /* 调用中断处理公用函数 清除KEY0所在中断线 的中断标志位 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_GPIO_PIN);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

void EXTI0_IRQHandler(void)                  

            //某一条EXTI线的外部中断函数,这个参考启动文件.s
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_GPIO_PIN);         /* 调用中断处理公用函数 清除KEY0所在中断线 的中断标志位 */
    __HAL_GPIO_EXTI_CLEAR_IT(WKUP_GPIO_PIN);         /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

//4 定义实现中断后功能的回调函数
volatile uint8_t g_suspend_resume = 0;   //定义一个用来执行挂起解挂的标志位,bit0 为1挂起 bit1 为1解挂

void HAL_GPIO_EXTI_Callback (uint16_t GPIO_Pin)
{
    delay_ms(10);      /* 消抖 */    
    if(GPIO_PIN_0 == GPIO_Pin && WK_UP == 1 )   //如果是GPIO_PIN_0的中断线触发,则执行此函数.因为是公共的回调函数,所有外部中断可以用此格式来确认对应IO执行的任务
    {
     g_suspend_resume  |= 1 << 1;
    }

    if(GPIO_PIN_4 == GPIO_Pin && KEY0 == 0)   //如果是GPIO_PIN_0的中断线触发,则执行此函数.因为是公共的回调函数,所有外部中断可以用此格式来确认对应IO执行的任务
    {
    g_suspend_resume  |= 1 << 0;
    }

}















