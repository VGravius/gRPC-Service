// =============================================================================
//  @file logger.cpp
//  @brief Реализация класса для логирования операций.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  Данный файл содержит реализацию методов класса Logger,
//  который обеспечивает вывод сообщений в консоль с временными метками.
//  Поддерживает логирование операций (Log) и ошибок (LogError).
// =============================================================================

#include "logger.h"
#include <iostream>      // Для вывода в консоль
#include <chrono>        // Для работы со временем
#include <iomanip>       // Для форматирования времени
#include <sstream>       // Для построения строки с временем

// =============================================================================
//  Реализация метода GetCurrentTime
// =============================================================================

/**
 * @brief Возвращает текущее время в формате "YYYY-MM-DD HH:MM:SS.mmm".
 *
 * Алгоритм:
 * 1. Получает текущее время из системных часов.
 * 2. Преобразует в структуру tm (локальное время).
 * 3. Извлекает миллисекунды из временной метки.
 * 4. Форматирует всё в строку.
 *
 * @return std::string Строка с текущим временем (локальный часовой пояс).
 *
 * @note Использует localtime_s (безопасная версия localtime).
 * @note Миллисекунды выводятся с тремя знаками (дополнение нулями).
 */
std::string Logger::GetCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm;
    localtime_s(&tm, &time_t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

// =============================================================================
//  Реализация метода Log
// =============================================================================

/**
 * @brief Записывает информацию об операции в консоль.
 *
 * Формат вывода:
 * [YYYY-MM-DD HH:MM:SS.mmm] operation: name - status (details)
 *
 * Если поле details не пустое, оно добавляется в скобках.
 *
 * @param operation Название операции (например, "GetParameter").
 * @param name      Имя параметра (или другая сущность).
 * @param status    Статус операции (например, "OK", "NOT_FOUND").
 * @param details   Дополнительная информация (опционально).
 *
 * @example
 * Logger().Log("GetParameter", "temperature", "OK", "value: 23.5");
 * // Вывод: [2026-06-20 15:30:00.123] GetParameter: temperature - OK (value: 23.5)
 */
void Logger::Log(const std::string& operation,
    const std::string& name,
    const std::string& status,
    const std::string& details) {
    std::cout << "[" << GetCurrentTime() << "] "
        << operation << ": " << name
        << " - " << status;
    if (!details.empty()) {
        std::cout << " (" << details << ")";
    }
    std::cout << std::endl;
}

// =============================================================================
//  Реализация метода LogError
// =============================================================================

/**
 * @brief Записывает сообщение об ошибке в консоль.
 *
 * Формат вывода:
 * [YYYY-MM-DD HH:MM:SS.mmm] ОШИБКА: message
 *
 * Используется для логирования критических ситуаций:
 * - Невалидные входные данные
 * - Внутренние ошибки сервера
 *
 * @param message Текст сообщения об ошибке.
 *
 * @example
 * Logger().LogError("SetParameter: invalid name (empty)");
 * // Вывод: [2026-06-20 15:30:05.456] ОШИБКА: SetParameter: invalid name (empty)
 */
void Logger::LogError(const std::string& message) {
    std::cout << "[" << GetCurrentTime() << "] "
        << "ОШИБКА: " << message << std::endl;
}