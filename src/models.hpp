#pragma once
/**
 * @file models.hpp
 * @brief Доменные модели: Pill, Schedule + JSON-сериализация.
 */

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace pillio {

// Кастомные исключения
class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

class StorageError : public std::runtime_error {
public:
    explicit StorageError(const std::string& msg) : std::runtime_error(msg) {}
};

class NotFoundError : public std::runtime_error {
public:
    explicit NotFoundError(const std::string& msg) : std::runtime_error(msg) {}
};

// ── Pill — описание одного лекарства ──────────────────────────────

struct Pill {
    std::uint64_t id{0};               ///< Уникальный идентификатор
    std::string name;                  ///< Название лекарства (UTF-8)
    double dosage{0.0};                ///< Числовое значение дозировки
    std::string unit{"мг"};            ///< Единица измерения (мг, мл, шт.)
    int interval_hours{8};             ///< Интервал между приёмами (часы)
    int start_hour{8};                 ///< Час первого приёма (0–23)
    int start_minute{0};               ///< Минута первого приёма (0–59)
    std::string meal_relation{"none"}; ///< Связь с едой: none/before/during/after
    int course_days{0};                ///< Длительность курса (0 = бессрочно)

    /// Бросает ValidationError при некорректных значениях.
    void validate() const {
        if (name.empty()) {
            throw ValidationError("Pill name must not be empty");
        }
        if (dosage <= 0.0) {
            throw ValidationError("Dosage must be positive, got " + std::to_string(dosage));
        }
        if (interval_hours <= 0 || interval_hours > 24) {
            throw ValidationError("Interval must be 1..24 h, got " +
                                  std::to_string(interval_hours));
        }
        if (start_hour < 0 || start_hour > 23) {
            throw ValidationError("start_hour must be 0..23, got " +
                                  std::to_string(start_hour));
        }
        if (start_minute < 0 || start_minute > 59) {
            throw ValidationError("start_minute must be 0..59, got " +
                                  std::to_string(start_minute));
        }
        if (meal_relation != "none" && meal_relation != "before" &&
            meal_relation != "during" && meal_relation != "after") {
            throw ValidationError("meal_relation must be none/before/during/after");
        }
        if (course_days < 0) {
            throw ValidationError("course_days must be >= 0");
        }
    }
};

inline void to_json(nlohmann::json& j, const Pill& p) {
    j = nlohmann::json{{"id", p.id},
                       {"name", p.name},
                       {"dosage", p.dosage},
                       {"unit", p.unit},
                       {"interval_hours", p.interval_hours},
                       {"start_hour", p.start_hour},
                       {"start_minute", p.start_minute},
                       {"meal_relation", p.meal_relation},
                       {"course_days", p.course_days}};
}

inline void from_json(const nlohmann::json& j, Pill& p) {
    j.at("id").get_to(p.id);
    j.at("name").get_to(p.name);
    j.at("dosage").get_to(p.dosage);
    j.at("unit").get_to(p.unit);
    j.at("interval_hours").get_to(p.interval_hours);
    j.at("start_hour").get_to(p.start_hour);
    j.at("start_minute").get_to(p.start_minute);
    // Backwards-compatible: new fields use defaults if missing
    p.meal_relation = j.value("meal_relation", std::string{"none"});
    p.course_days = j.value("course_days", 0);
}

// ── Schedule — отметка о конкретном приёме ────────────────────────

struct Schedule {
    std::uint64_t pill_id{0};          ///< ID связанного Pill
    std::string scheduled_time;        ///< Запланированное время (ISO 8601 строка)
    bool taken{false};                 ///< Было ли лекарство принято
    std::string taken_at;              ///< Фактическое время приёма (или пусто)

    void validate() const {
        if (pill_id == 0) {
            throw ValidationError("Schedule must reference a valid pill_id");
        }
        if (scheduled_time.empty()) {
            throw ValidationError("scheduled_time must not be empty");
        }
    }
};

inline void to_json(nlohmann::json& j, const Schedule& s) {
    j = nlohmann::json{{"pill_id", s.pill_id},
                       {"scheduled_time", s.scheduled_time},
                       {"taken", s.taken},
                       {"taken_at", s.taken_at}};
}

inline void from_json(const nlohmann::json& j, Schedule& s) {
    j.at("pill_id").get_to(s.pill_id);
    j.at("scheduled_time").get_to(s.scheduled_time);
    j.at("taken").get_to(s.taken);
    j.at("taken_at").get_to(s.taken_at);
}

}  // namespace pillio
