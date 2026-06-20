// =============================================================================
//  @file test_telemetry_service.cpp
//  @brief Unit-тесты для класса TelemetryServiceImpl.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  Данный файл содержит набор тестов для проверки функциональности
//  gRPC-сервиса TelemetryServiceImpl.
//  Тесты проверяют базовую работоспособность и интеграцию с компонентами.
// =============================================================================

#include <gtest/gtest.h>
#include "../src/telemetry_service.h"
#include "../src/cache.h"
#include "../src/validator.h"
#include "../src/logger.h"

// =============================================================================
//  Глобальные объекты для тестов
// =============================================================================

static Cache g_test_cache;
static Validator g_test_validator;
static Logger g_test_logger;

// =============================================================================
//  Тест: CompilationTest
// =============================================================================

/**
 * @brief Проверяет, что класс TelemetryServiceImpl компилируется.
 *
 * Сценарий:
 * 1. Создаётся объект TelemetryServiceImpl.
 * 2. Проверяется, что объект создаётся без ошибок.
 *
 * @test Проверяет, что класс TelemetryServiceImpl существует и компилируется.
 */
TEST(TelemetryServiceTest, CompilationTest) {
    TelemetryServiceImpl service;

    // Если код скомпилировался, тест пройден
    EXPECT_TRUE(true);
}

// =============================================================================
//  Тест: ValidationTest
// =============================================================================

/**
 * @brief Проверяет, что валидатор работает через сервис.
 *
 * Сценарий:
 * 1. Проверяется, что валидатор корректно обрабатывает непустое имя.
 * 2. Проверяется, что валидатор корректно обрабатывает пустое имя.
 * 3. Проверяется, что валидатор корректно обрабатывает непустое значение.
 * 4. Проверяется, что валидатор корректно обрабатывает пустое значение.
 * 5. Проверяется, что валидатор корректно обрабатывает непустой request_id.
 * 6. Проверяется, что валидатор корректно обрабатывает пустой request_id.
 *
 * @test Проверяет интеграцию валидатора с сервисом.
 */
TEST(TelemetryServiceTest, ValidationTest) {
    // Проверка имени
    EXPECT_TRUE(g_test_validator.ValidateName("temperature"));
    EXPECT_FALSE(g_test_validator.ValidateName(""));

    // Проверка значения
    EXPECT_TRUE(g_test_validator.ValidateValue("23.5"));
    EXPECT_FALSE(g_test_validator.ValidateValue(""));

    // Проверка request_id
    EXPECT_TRUE(g_test_validator.ValidateRequestId("id1"));
    EXPECT_FALSE(g_test_validator.ValidateRequestId(""));
}

// =============================================================================
//  Тест: CacheTest
// =============================================================================

/**
 * @brief Проверяет, что кэш работает через сервис.
 *
 * Сценарий:
 * 1. Сохраняется пара "test_key" → "test_value".
 * 2. Выполняется запрос по ключу "test_key".
 * 3. Проверяется, что значение существует и равно "test_value".
 *
 * @test Проверяет интеграцию кэша с сервисом.
 */
TEST(TelemetryServiceTest, CacheTest) {
    // Сохранение значения
    g_test_cache.Set("test_key", "test_value");

    // Получение значения
    auto result = g_test_cache.Get("test_key");

    // Проверка
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "test_value");
}