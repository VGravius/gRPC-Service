// =============================================================================
//  @file validator.cpp
//  @brief Реализация класса для валидации входных данных.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  Данный файл содержит реализацию методов класса Validator,
//  который проверяет корректность входных данных (имя, значение, request_id)
//  и обеспечивает уникальность request_id с помощью потокобезопасного хранилища.
// =============================================================================

#include "validator.h"

// =============================================================================
//  Реализация методов валидации
// =============================================================================

/**
 * @brief Проверяет, что имя параметра не пустое.
 *
 * @param name Имя параметра.
 * @return true  — имя не пустое.
 * @return false — имя пустое.
 */
bool Validator::ValidateName(const std::string& name) const {
    return !name.empty();
}

/**
 * @brief Проверяет, что значение параметра не пустое.
 *
 * @param value Значение параметра.
 * @return true  — значение не пустое.
 * @return false — значение пустое.
 */
bool Validator::ValidateValue(const std::string& value) const {
    return !value.empty();
}

/**
 * @brief Проверяет, что request_id не пустой.
 *
 * @param request_id Уникальный идентификатор запроса.
 * @return true  — request_id не пустой.
 * @return false — request_id пустой.
 */
bool Validator::ValidateRequestId(const std::string& request_id) const {
    return !request_id.empty();
}

// =============================================================================
//  Реализация методов для работы с уникальностью request_id
// =============================================================================

/**
 * @brief Проверяет, использовался ли указанный request_id ранее.
 *
 * Метод блокирует мьютекс для безопасного доступа к хранилищу used_ids_.
 * Поиск выполняется за O(1) благодаря использованию std::unordered_set.
 *
 * @param request_id Уникальный идентификатор запроса.
 * @return true  — request_id уже использовался.
 * @return false — request_id новый (не использовался).
 *
 * @note Потокобезопасен благодаря std::lock_guard.
 */
bool Validator::IsRequestIdUsed(const std::string& request_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return used_ids_.find(request_id) != used_ids_.end();
}

/**
 * @brief Отмечает request_id как использованный.
 *
 * Метод блокирует мьютекс и добавляет request_id в хранилище used_ids_.
 * После этого любой вызов IsRequestIdUsed() для этого ID вернёт true.
 *
 * @param request_id Уникальный идентификатор запроса.
 *
 * @note Потокобезопасен благодаря std::lock_guard.
 * @note Дублирующиеся вставки игнорируются (множество гарантирует уникальность).
 */
void Validator::MarkRequestIdUsed(const std::string& request_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    used_ids_.insert(request_id);
}