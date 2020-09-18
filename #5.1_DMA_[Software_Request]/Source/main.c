/******************************************************************************
 * @File: main.c
 * @Author: Milandr, L.
 * @Project: #5.1_DMA_[Software_Request]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 30.04.2020
 * @Purpose: Основной модуль
 * @Description:
 * В данном проекте производится передача данных между массивами средствами
 * прямого доступа к памяти. Передача инициируется нажатием кнопки. Массив
 * принятых данных отображается на дисплей.
 ******************************************************************************/

// Подключение заголовка
#include "main.h"

// Основная функция
int main(void)
{
  // Общая инициализация
  CPU_Init();   // Система тактирования
  TICK_Init();  // Системный таймер
  BTN_Init();   // Кнопки
  LCD_Init();   // ЖК-дисплей

  // Инициализация DMA
  DMA_Init();

  // Инициализация светодиодов
  LED_Init();

  // Заполнение массива исходных данных
  Randomize((int*)source_array, ARR_S, RND_MAX);

  // Отображение массива принятых данных
  PrintArray(result_array);

  // Основной цикл
  while (true) {

    // Блок работы с передачей данных
    if (Wait(&mark[0], 100) == true) {

      // Обработка кнопки MID
      if (BTN_Handler(BTN_M) == true) {

        // Программный запрос на передачу по DMA
        MDR_DMA->CHNL_SW_REQUEST = (1 << 13);

        // Отображение массива принятых данных
        PrintArray(result_array);
      }
    }

    // Блок работы со светодиодами
    if (Wait(&mark[1], 200) == true) {

      // Переключение светодиодов
      LED_SwitchState();
    }
  }
}