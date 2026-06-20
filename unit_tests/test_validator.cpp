// =============================================================================
//  @file test_validator.cpp
//  @brief Unit-тесты для класса Validator.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  Данный файл содержит набор тестов для проверки функциональности
//  валидатора Validator.
//  Тесты покрывают: проверку имени, значения, request_id и уникальности.
// =============================================================================

#include <gtest/gtest.h>
#include "../src/validator.h"

// =============================================================================
//  Тест: ValidateName
// =============================================================================

/**
 * @brief Проверяет валидацию имени параметра.
 *
 * Сценарий:
 * 1. Проверяется, что непустое имя возвращает true.
 * 2. Проверяется, что пустое имя возвращает false.
 *
 * @test Проверяет, что Validator::ValidateName работает корректно.
 */
TEST(ValidatorTest, ValidateName) {
    Validator validator;

    EXPECT_TRUE(validator.ValidateName("temperature"));  // Непустое имя → true
    EXPECT_FALSE(validator.ValidateName(""));           // Пустое имя → false
}

// =============================================================================
//  Тест: ValidateValue
// =============================================================================

/**
 * @brief Проверяет валидацию значения параметра.
 *
 * Сценарий:
 * 1. Проверяется, что непустое значение возвращает true.
 * 2. Проверяется, что пустое значение возвращает false.
 *
 * @test Проверяет, что Validator::ValidateValue работает корректно.
 */
TEST(ValidatorTest, ValidateValue) {
    Validator validator;

    EXPECT_TRUE(validator.ValidateValue("23.5"));  // Непустое значение → true
    EXPECT_FALSE(validator.ValidateValue(""));     // Пустое значение → false
}

// =============================================================================
//  Тест: ValidateRequestId
// =============================================================================

/**
 * @brief Проверяет валидацию request_id.
 *
 * Сценарий:
 * 1. Проверяется, что непустой request_id возвращает true.
 * 2. Проверяется, что пустой request_id возвращает false.
 *
 * @test Проверяет, что Validator::ValidateRequestId работает корректно.
 */
TEST(ValidatorTest, ValidateRequestId) {
    Validator validator;

    EXPECT_TRUE(validator.ValidateRequestId("id1"));  // Непустой ID → true
    EXPECT_FALSE(validator.ValidateRequestId(""));    // Пустой ID → false
}

// =============================================================================
//  Тест: RequestIdUniqueness
// =============================================================================

/**
 * @brief Проверяет уникальность request_id.
 *
 * Сценарий:
 * 1. Проверяется, что новый ID не используется.
 * 2. ID отмечается как использованный.
 * 3. Проверяется, что ID теперь считается использованным.
 *
 * @test Проверяет, что Validator::IsRequestIdUsed и MarkRequestIdUsed работают.
 */
TEST(ValidatorTest, RequestIdUniqueness) {
    Validator validator;

    // Шаг 1: новый ID не должен быть использован
    EXPECT_FALSE(validator.IsRequestIdUsed("id1"));

    // Шаг 2: отмечаем ID как использованный
    validator.MarkRequestIdUsed("id1");

    // Шаг 3: теперь ID должен считаться использованным
    EXPECT_TRUE(validator.IsRequestIdUsed("id1"));
}

// =============================================================================
//  Тест: RequestIdMultiple
// =============================================================================

/**
 * @brief Проверяет работу с несколькими request_id.
 *
 * Сценарий:
 * 1. Отмечаются два разных ID как использованные.
 * 2. Проверяется, что оба ID считаются использованными.
 * 3. Проверяется, что новый ID не считается использованным.
 *
 * @test Проверяет, что хранилище used_ids_ работает корректно.
 */
TEST(ValidatorTest, RequestIdMultiple) {
    Validator validator;

    // Шаг 1: отмечаем два ID
    validator.MarkRequestIdUsed("id1");
    validator.MarkRequestIdUsed("id2");

    // Шаг 2: оба должны быть использованы
    EXPECT_TRUE(validator.IsRequestIdUsed("id1"));
    EXPECT_TRUE(validator.IsRequestIdUsed("id2"));

    // Шаг 3: новый ID не должен быть использован
    EXPECT_FALSE(validator.IsRequestIdUsed("id3"));
}