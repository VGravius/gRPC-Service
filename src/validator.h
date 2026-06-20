// =============================================================================
//  @file validator.h
//  @brief ќбъ€вление класса дл€ валидации входных данных.
//  @version 1.0
//  @date 2026-06-20
//  @author vladcrim
//
//  ƒанный файл содержит объ€вление класса Validator, который провер€ет
//  корректность входных данных (им€, значение, request_id) и обеспечивает
//  уникальность request_id с помощью потокобезопасного хранилища.
// =============================================================================

#pragma once

#include <string>          // ƒл€ работы со строками
#include <unordered_set>   // ƒл€ хранени€ использованных request_id
#include <mutex>           // ƒл€ потокобезопасности

/**
 * @brief  ласс дл€ валидации входных данных и контрол€ уникальности request_id.
 *
 * ѕредоставл€ет методы дл€ проверки:
 * - »м€ параметра Ч не пустое.
 * - «начение параметра Ч не пустое.
 * - request_id Ч не пустой и уникальный.
 *
 * ¬се методы, работающие с хранилищем used_ids_, потокобезопасны благодар€
 * использованию std::mutex.
 *
 * @note »спользует std::unordered_set дл€ хранени€ использованных request_id.
 * @note ѕоиск и вставка выполн€ютс€ за O(1) в среднем.
 */
class Validator {
public:
    /**
     * @brief ѕровер€ет, что им€ параметра не пустое.
     *
     * @param name »м€ параметра.
     * @return true  Ч им€ не пустое.
     * @return false Ч им€ пустое.
     */
    bool ValidateName(const std::string& name) const;

    /**
     * @brief ѕровер€ет, что значение параметра не пустое.
     *
     * @param value «начение параметра.
     * @return true  Ч значение не пустое.
     * @return false Ч значение пустое.
     */
    bool ValidateValue(const std::string& value) const;

    /**
     * @brief ѕровер€ет, что request_id не пустой.
     *
     * @param request_id ”никальный идентификатор запроса.
     * @return true  Ч request_id не пустой.
     * @return false Ч request_id пустой.
     */
    bool ValidateRequestId(const std::string& request_id) const;

    /**
     * @brief ѕровер€ет, использовалс€ ли указанный request_id ранее.
     *
     * ћетод потокобезопасен.
     *
     * @param request_id ”никальный идентификатор запроса.
     * @return true  Ч request_id уже использовалс€.
     * @return false Ч request_id новый (не использовалс€).
     */
    bool IsRequestIdUsed(const std::string& request_id) const;

    /**
     * @brief ќтмечает request_id как использованный.
     *
     * ћетод потокобезопасен.
     * ѕосле вызова этого метода IsRequestIdUsed() дл€ данного ID вернЄт true.
     *
     * @param request_id ”никальный идентификатор запроса.
     */
    void MarkRequestIdUsed(const std::string& request_id);

private:
    std::unordered_set<std::string> used_ids_;  // ’ранилище использованных request_id.
    mutable std::mutex mutex_;                  // ћьютекс дл€ потокобезопасности.
};