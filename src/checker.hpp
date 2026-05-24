#pragma once
/**
 * @file checker.hpp
 * @brief Проверка лекарственных взаимодействий (встроенная база ~20 правил).
 */

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

namespace pillio {

/**
 * @brief Уровень серьёзности взаимодействия.
 */
enum class Severity {
    Warning,   ///< Предупреждение (мониторинг)
    Dangerous  ///< Опасная комбинация (избегать)
};

/**
 * @brief Запись о взаимодействии двух веществ.
 */
struct Interaction {
    std::string drug_a;        ///< Первое вещество
    std::string drug_b;        ///< Второе вещество
    Severity severity;         ///< Уровень серьёзности
    std::string description;   ///< Описание взаимодействия
};

/**
 * @brief Сериализует Interaction → JSON.
 */
inline void to_json(nlohmann::json& j, const Interaction& i) {
    j = nlohmann::json{
        {"drug_a", i.drug_a},
        {"drug_b", i.drug_b},
        {"severity", i.severity == Severity::Dangerous ? "dangerous" : "warning"},
        {"description", i.description}
    };
}

/**
 * @brief Проверяет список лекарств на известные взаимодействия.
 *
 * Сравнивает каждую пару из переданных названий с внутренней
 * базой данных взаимодействий. Поиск регистронезависимый.
 *
 * Сложность: O(N² · K), где N — кол-во лекарств, K — размер базы.
 *
 * @param pill_names названия лекарств (могут быть на русском)
 * @return вектор найденных взаимодействий (пустой, если всё безопасно)
 */
std::vector<Interaction> checkInteractions(const std::vector<std::string>& pill_names);

/**
 * @brief Возвращает общее количество правил в базе взаимодействий.
 */
std::size_t interactionDatabaseSize();

}  // namespace pillio
