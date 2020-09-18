/******************************************************************************
 * @File: main.c
 * @Author: Milandr, L.
 * @Project: #5.2_DMA_[Hardware_Request]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 07.05.2020
 * @Purpose: Основной модуль
 * @Description:
 * В данном проекте производится передача данных между массивами средствами
 * прямого доступа к памяти. Передача инициируется по запросам таймера.
 * Массив принятых данных отображается на дисплей.
 ******************************************************************************/

// Подключение заголовка
#include "main.h"

// Основная функция
int main(void)
{
  // Общая инициализация
  CPU_Init();   // Система тактирования
  TICK_Init();  // Системный таймер
  LCD_Init();   // ЖК-дисплей

  // Инициализация DMA
  DMA_Init();

  // Инициализация таймеров
  TIMER_Init();

  // Заполнение массива исходных данных
  Randomize((int*)source_array, ARR_S, RND_MAX);

  // Отображение массива принятых данных
  PrintArray(result_array);

  // Основной цикл
  while (true) {

    // Блок работы с дисплеем
    if (Wait(&mark[0], 100) == true) {

      // Отображение массива принятых данных
      PrintArray(result_array);
    }
  }
}
