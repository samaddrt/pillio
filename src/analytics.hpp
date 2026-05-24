#pragma once
/**
 * @file analytics.hpp
 * @brief Модуль аналитики Pillio — streak-серии, адгезия, задержки.
 */

#include <chrono>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "models.hpp"
#include "tracker.hpp"

namespace pillio {

/**
 * @brief Результат расширенной аналитики.
 */
struct AnalyticsReport {
    int best_streak{0};              ///< Лучшая серия дней (исторический максимум)
    int current_streak{0};           ///< Текущая серия дней
    double weekly_adherence{0.0};    ///< Адгезия за последние 7 дней (0.0–1.0)
    double monthly_adherence{0.0};   ///< Адгезия за последние 30 дней (0.0–1.0)
    double avg_delay_minutes{0.0};   ///< Средняя задержка приёма (минуты)
    int total_taken{0};              ///< Всего принято за всё время
    int total_missed{0};             ///< Всего пропущено за всё время
    std::map<int, int> hourly_distribution; ///< Распределение приёмов по часам (0–23)
};

/**
 * @brief Вычисляет расширенный отчёт аналитики.
 *
 * Использует историю расписаний для расчёта streak-серий,
 * адгезии за различные периоды, средней задержки и распределения.
 *
 * @param all_schedules полная история расписаний
 * @param now текущий момент времени
 * @return заполненный AnalyticsReport
 */
AnalyticsReport computeAnalytics(const std::vector<Schedule>& all_schedules,
                                  const TimePoint& now);

/**
 * @brief Вычисляет адгезию (долю принятых) за диапазон дат.
 *
 * @tparam Pred предикат фильтрации записей
 * @param schedules вектор записей
 * @param predicate фильтр (должен вернуть true для нужных записей)
 * @return доля принятых (0.0–1.0), или 0 если нет записей
 */
template <typename Pred>
double adherenceForPeriod(const std::vector<Schedule>& schedules, Pred predicate) {
    int total = 0, taken = 0;
    for (const auto& s : schedules) {
        if (predicate(s)) {
            ++total;
            if (s.taken) ++taken;
        }
    }
    return total > 0 ? static_cast<double>(taken) / total : 0.0;
}

/**
 * @brief Сериализует AnalyticsReport в JSON.
 */
inline void to_json(nlohmann::json& j, const AnalyticsReport& r) {
    j = nlohmann::json{
        {"best_streak", r.best_streak},
        {"current_streak", r.current_streak},
        {"weekly_adherence", r.weekly_adherence},
        {"monthly_adherence", r.monthly_adherence},
        {"avg_delay_minutes", r.avg_delay_minutes},
        {"total_taken", r.total_taken},
        {"total_missed", r.total_missed},
        {"hourly_distribution", r.hourly_distribution}
    };
}

}  // namespace pillio
