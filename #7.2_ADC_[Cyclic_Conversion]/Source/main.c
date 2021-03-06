/******************************************************************************
 * @File: main.c
 * @Author: Milandr, L.
 * @Project: #7.2_ADC_[Cyclic_Conversion]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 18.03.2020
 * @Purpose: Основной модуль
 * @Description:
 * В данном проекте формируется выборка аналого-цифровых преобразований сигнала
 * с датчика температуры. Выборка собирается с использованием DMA и усредняется
 * в целях стабилизации результата. Результат отображается на дисплей.
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

  // Инициализация АЦП
  ADC_Init();

  // Инициализация нагревателя
  HEAT_Init();

  // Инициализация переменных
  uint32_t adc_result = 0;  // Результат преобразования
  double temp = 0.00;       // Расчётная температура

  // Запуск АЦП
  MDR_ADC->ADC1_CFG |= ADC1_CFG_REG_ADON;

  // Основной цикл
  while (true) {

    // Блок работы с АЦП
    if (Wait(&mark[0], 100) == true) {

      // Опрос флага завершения цикла преобразований
      if (flg_adc_eoc == true) {

        // Сброс флага
        flg_adc_eoc = false;

        // Вычисление среднего арифметического
        adc_result = ADC_GetAverage(adc_sample);

        // Вычисление температуры (в градусах Цельсия)
        temp = ADC_TO_TMP(adc_result);

        // Отображение результатов
        LCD_PrintString("    T = %.1f °C", 3, temp);   // Температура
        LCD_PrintString("    D = %u", 5, adc_result);  // Цифровое значение

        // Запуск цикла преобразований
        MDR_ADC->ADC1_CFG |= ADC1_CFG_REG_ADON;
      }
    }

    // Блок работы с нагревателем
    if (Wait(&mark[1], 100) == true) {

      // Обработка кнопки MID
      if (BTN_Handler(BTN_M) == true) {

        // Переключение состояния нагревателя
        MDR_PORTE->RXTX ^= PORT_RXTX0_Msk;
      }
    }
  }
}
