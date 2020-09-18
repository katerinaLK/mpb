/******************************************************************************
 * @File: main.c
 * @Author: Milandr, L.
 * @Project: #3.3_Timer_[Watchdog]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 08.04.2020
 * @Purpose: Основной модуль
 * @Description:
 * В данном проекте выполнен запуск сторожевого таймера. Период его работы
 * задаётся константой. Основной цикл программы отсчитывает время до перезагрузки
 * системы. Перезагрузка таймера осуществляется кнопкой. Обратите внимание, что
 * работающий сторожевой таймер может прерывать процесс загрузки новой программы
 * в микроконтроллер. По этой причине в основном цикле реализован блок отключения
 * таймера по нажатию сенсорной кнопки.
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

  // Инициализация сторожевого таймера
  IWDG_Init();

  // Квантовая метка перезагрузки системы
  uint32_t reset_mark = system_ticks + IWDG_RLD_TIME;

  // Запуск ожидания для калибровки генератора LSI
  Wait(&mark[0], IWDG_RLD_TIME);

  // Основной цикл
  while (true) {

    // Блок калибровки генератора LSI
    if (Wait(&mark[0], IWDG_RLD_TIME) == true) {
   
      // Отображение метки
      LCD_PrintSymbol('*', 0, 7);
    }

    // Блок перезагрузки сторожевого таймера
    if (Wait(&mark[1], 100) == true) {

      // Обработка кнопки MID
      if (BTN_Handler(BTN_M) == true) {

        // Перезагрузка таймера
        MDR_IWDG->KR = IWDG_KR_RELOAD;

        // Обновление метки
        reset_mark = system_ticks + IWDG_RLD_TIME;
      }
    }

    // Блок работы с дисплеем
    if (Wait(&mark[2], 100) == true) {

      // Защита от переполнения
      if (reset_mark < system_ticks) {
        reset_mark = system_ticks;
      }

      // Отображение времени до перезагрузки (в секундах)
      LCD_PrintString("  T = %03u [%02u] sec.", 3, system_ticks / 1000, (reset_mark - system_ticks) / 1000);
    }

    // Блок отключения сторожевого таймера
    if (Wait(&mark[3], 100) == true) {

      // Обработка кнопки SENSE
      if (BTN_Handler(BTN_S) == true) {

        // Отключение генератора LSI
        MDR_BKP->REG_0F &= ~BKP_REG_0F_LSI_ON;

        // Отображение диагностического сообщения
        LCD_PrintString("    WDG DISABLED!", 5);
      }
    }
  }
}