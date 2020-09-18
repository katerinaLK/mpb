﻿/******************************************************************************
 * @File: main.c
 * @Author: Milandr, L.
 * @Project: #10.1_RTOS_[Integration]
 * @Version: 1.0.1
 * @Compiler: ARM Compiler V5.06 (build 750)
 * @Microcontroller: К1986ВЕ92QI
 * @Device: Отладочная плата «МилКиТЭС»
 * @Date: 04.06.2020
 * @Purpose: Основной модуль
 * @Description:
 * Данный проект представляет собой заготовку для интеграции ОСРВ.
 ******************************************************************************/

// Подключение заголовка
#include "main.h"

// Основная функция
int main(void)
{
  // Общая инициализация
  CPU_Init();   // Система тактирования
  BTN_Init();   // Кнопки

  // Инициализация светодиодов
  LED_Init();

  // Инициализация ОСРВ
  RTOS_Init();

  // Основной цикл
  while (true) {
  }
}