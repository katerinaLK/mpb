/******************************************************************************
 * @File: dma.c
 * @Author: Milandr, L.
 * @Project: #6.2_DAC_[Cyclic_Conversion]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 19.05.2020
 * @Purpose: Конфигурация прямого доступа к памяти
 ******************************************************************************/

// Подключение заголовка
#include "dma.h"

// Массив структур управляющих данных для DMA
// 32 канала; основная и альтернативная структуры; итого - 64 элемента
// Структура состоит из четырех 32-битных полей; итого - 1024 байта
__align(1024) volatile DMA_ControlDataTypeDef DMA_CONTROL_DATA[64];

// Деинициализация DMA
void DMA_Reset(void)
{
  MDR_DMA->CFG               = 0x00000000;
  MDR_DMA->CTRL_BASE_PTR     = 0x00000000;
  MDR_DMA->CHNL_SW_REQUEST   = 0x00000000;
  MDR_DMA->CHNL_ENABLE_SET   = 0xFFFFFFFF;  // Разрешение работы всех каналов
  MDR_DMA->CHNL_REQ_MASK_SET = 0xFFFFFFFF;  // Отключение запросов от всех каналов
  MDR_DMA->CHNL_USEBURST_CLR = 0xFFFFFFFF;  // Разрешение одиночных передач
  MDR_DMA->CHNL_PRI_ALT_CLR  = 0xFFFFFFFF;  // Использование первичных структур
  MDR_DMA->CHNL_PRIORITY_CLR = 0xFFFFFFFF;  // Обычный приоритет передач
  MDR_DMA->ERR_CLR           = 0x00000001;  // Сброс сигнала ошибки на шине
}

// Инициализация контроллера DMA
void DMA_Init(void)
{
  // Включение тактирования DMA
  MDR_RST_CLK->PER_CLOCK |= RST_CLK_PCLK_DMA_Msk;

  // Деинициализация DMA
  DMA_Reset();

  // Установка базового адреса управляющих данных
  MDR_DMA->CTRL_BASE_PTR = (uint32_t)&DMA_CONTROL_DATA;

  // Конфигурация первичной и альтернативной структуры управляющих данных канала 10 (TIMER1)
  DMA_CONTROL_DATA[10].DMA_CHANNEL_CFG =
  DMA_CONTROL_DATA[42].DMA_CHANNEL_CFG = (3 << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)             // Тип цикла (пинг-понг)
                                       | ((DAC_SRM - 1) << DMA_CHANNEL_CFG_N_MINUS_1_Pos)  // Кол-во передач в цикле (по размеру буфера)
                                       | (0 << DMA_CHANNEL_CFG_R_POWER_Pos)                // Кол-во передач за блочный запрос (не используется)
                                       | (1 << DMA_CHANNEL_CFG_SRC_PROT_CTRL_Pos)          // Защита источника (привилегированная)
                                       | (1 << DMA_CHANNEL_CFG_DST_PROT_CTRL_Pos)          // Защита приёмника (привилегированная)
                                       | (1 << DMA_CHANNEL_CFG_SRC_SIZE_Pos)               // Объём данных источника (2 байта)
                                       | (1 << DMA_CHANNEL_CFG_SRC_INC_Pos)                // Инкремент адреса источника (2 байта)
                                       | (1 << DMA_CHANNEL_CFG_DST_SIZE_Pos)               // Объём данных приёмника (2 байта)
                                       | (3U << DMA_CHANNEL_CFG_DST_INC_Pos);              // Инкремент адреса приёмника (отсутствует)

  // Адреса концов данных
  DMA_CONTROL_DATA[10].DMA_SRC_DATA_END_PTR = DMA_CONTROL_DATA[42].DMA_SRC_DATA_END_PTR = (uint32_t)&dac_sample[DAC_SRM - 1];  // Источник
  DMA_CONTROL_DATA[10].DMA_DST_DATA_END_PTR = DMA_CONTROL_DATA[42].DMA_DST_DATA_END_PTR = (uint32_t)&MDR_DAC->DAC2_DATA;       // Приёмник

  // Конфигурация канала 10
  MDR_DMA->CHNL_PRI_ALT_CLR  = (1 << 10);  // Структура управления (первичная)
  MDR_DMA->CHNL_PRIORITY_CLR = (1 << 10);  // Приоритет (обычный)
  MDR_DMA->CHNL_REQ_MASK_CLR = (1 << 10);  // Маскирование канала (отключено)

  // Конфигурация контроллера DMA
  MDR_DMA->CFG = (1 << DMA_CFG_MASTER_ENABLE_Pos)    // Работа контроллера DMA (включён)
               | (1 << DMA_CFG_CHNL_PROT_CTRL_Pos);  // Защита шины (привилегированная)

  // Конфигурация контроллера NVIC
  NVIC_ClearPendingIRQ(DMA_IRQn);  // Сброс запросов прерываний
  NVIC_SetPriority(DMA_IRQn, 6);   // Приоритет прерываний
  NVIC_EnableIRQ(DMA_IRQn);        // Разрешение обработки прерываний
}

// Обработчик прерывания от DMA
void DMA_IRQHandler(void)
{
  // Инициализация первичной структуры
  if ((DMA_CONTROL_DATA[10].DMA_CHANNEL_CFG & DMA_CHANNEL_CFG_CYCLE_CTRL_Msk) == 0) {
    DMA_CONTROL_DATA[10].DMA_CHANNEL_CFG |= (3 << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)              // Тип цикла (пинг-понг)
                                          | ((DAC_SRM - 1) << DMA_CHANNEL_CFG_N_MINUS_1_Pos);  // Кол-во передач в цикле (по размеру буфера)
  }

  // Инициализация альтернативной структуры
  if ((DMA_CONTROL_DATA[42].DMA_CHANNEL_CFG & DMA_CHANNEL_CFG_CYCLE_CTRL_Msk) == 0) {
    DMA_CONTROL_DATA[42].DMA_CHANNEL_CFG |= (3 << DMA_CHANNEL_CFG_CYCLE_CTRL_Pos)              // Тип цикла (пинг-понг)
                                          | ((DAC_SRM - 1) << DMA_CHANNEL_CFG_N_MINUS_1_Pos);  // Кол-во передач в цикле (по размеру буфера)
  }
}
