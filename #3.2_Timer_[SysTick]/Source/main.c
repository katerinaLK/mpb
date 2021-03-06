/******************************************************************************
 * @File: main.c
 * @Author: Milandr, L.
 * @Project: #3.2_Timer_[SysTick]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 12.03.2020
 * @Purpose: Основной модуль
 * @Description:
 * В данном проекте реализована временная база на основе системного таймера.
 * База используется в основном цикле для управления частотой мигания светодиодов.
 ******************************************************************************/

// Подключение заголовка
#include "main.h"

// Основная функция
int main(void)
{
  // Общая инициализация
  CPU_Init();   // Система тактирования
  TICK_Init();  // Системный таймер

  // Инициализация светодиодов
  LED_Init();

  // Основной цикл
  while (true) {

    // Блок работы с первой парой светодиодов
    if (Wait(&mark[0], 500) == true) {

      // Переключение состояния линий PA0, PA1
      MDR_PORTA->RXTX ^= 0x3;
    }

    // Блок работы со второй парой светодиодов
    if (Wait(&mark[1], 200) == true) {

      // Переключение состояния линий PA2, PA3
      MDR_PORTA->RXTX ^= 0xC;
    }
  }
}
