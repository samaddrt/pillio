// analytics.cpp — реализация модуля аналитики.

#include "analytics.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>

namespace pillio {

namespace {

/**
 * @brief Извлекает дату (YYYY-MM-DD) из ISO-строки.
 */
std::string extractDate(const std::string& iso) {
    return iso.size() >= 10 ? iso.substr(0, 10) : "";
}

/**
 * @brief Извлекает час из ISO-строки.
 * @return час (0–23) или -1 при ошибке
 */
int extractHour(const std::string& iso) {
    if (iso.size() >= 13 && iso[10] == 'T') {
        try {
            return std::stoi(iso.substr(11, 2));
        } catch (...) {}
    }
    return -1;
}

}  // namespace

AnalyticsReport computeAnalytics(const std::vector<Schedule>& all_schedules,
                                  const TimePoint& now) {
    AnalyticsReport report;

    if (all_schedules.empty()) return report;

    // ── Собираем уникальные даты ──────────────────────────────
    std::set<std::string> all_dates;
    std::transform(all_schedules.begin(), all_schedules.end(),
                   std::inserter(all_dates, all_dates.end()),
                   [](const Schedule& s) { return extractDate(s.scheduled_time); });

    // ── Считаем общую статистику ──────────────────────────────
    report.total_taken = static_cast<int>(
        std::count_if(all_schedules.begin(), all_schedules.end(),
                      [](const Schedule& s) { return s.taken; }));
    report.total_missed = static_cast<int>(all_schedules.size()) - report.total_taken;

    // ── Streak-серии (текущая и лучшая) ──────────────────────
    // Собираем даты в отсортированный вектор
    std::vector<std::string> sorted_dates(all_dates.begin(), all_dates.end());
    std::sort(sorted_dates.begin(), sorted_dates.end(), std::greater<>());

    // Для каждой даты считаем, все ли приёмы выполнены
    auto isFullDay = [&](const std::string& date) -> bool {
        return std::all_of(all_schedules.begin(), all_schedules.end(),
                          [&date](const Schedule& s) {
                              return extractDate(s.scheduled_time) != date || s.taken;
                          });
    };

    // Текущий streak (с последней даты назад)
    for (const auto& date : sorted_dates) {
        if (isFullDay(date)) ++report.current_streak;
        else break;
    }

    // Лучший streak (проходим все даты)
    int running = 0;
    for (const auto& date : sorted_dates) {
        if (isFullDay(date)) {
            ++running;
            report.best_streak = std::max(report.best_streak, running);
        } else {
            running = 0;
        }
    }

    // ── Адгезия за 7 и 30 дней ──────────────────────────────
    auto now_str = formatTimePoint(now).substr(0, 10);
    auto daysAgo = [&](int n) -> std::string {
        auto tp = now - std::chrono::hours(24 * n);
        return formatTimePoint(tp).substr(0, 10);
    };

    std::string week_start = daysAgo(7);
    std::string month_start = daysAgo(30);

    report.weekly_adherence = adherenceForPeriod(
        all_schedules,
        [&](const Schedule& s) {
            auto d = extractDate(s.scheduled_time);
            return d >= week_start && d <= now_str;
        });

    report.monthly_adherence = adherenceForPeriod(
        all_schedules,
        [&](const Schedule& s) {
            auto d = extractDate(s.scheduled_time);
            return d >= month_start && d <= now_str;
        });

    // ── Средняя задержка приёма (в минутах) ──────────────────
    std::vector<double> delays;
    for (const auto& s : all_schedules) {
        if (s.taken && !s.taken_at.empty() && !s.scheduled_time.empty()) {
            try {
                auto sched_tp = parseTimePoint(s.scheduled_time);
                auto taken_tp = parseTimePoint(s.taken_at);
                auto diff = std::chrono::duration_cast<std::chrono::minutes>(
                    taken_tp - sched_tp).count();
                delays.push_back(std::abs(static_cast<double>(diff)));
            } catch (...) {}
        }
    }

    if (!delays.empty()) {
        report.avg_delay_minutes = std::accumulate(
            delays.begin(), delays.end(), 0.0) / static_cast<double>(delays.size());
    }

    // ── Распределение по часам ────────────────────────────────
    for (const auto& s : all_schedules) {
        if (s.taken && !s.taken_at.empty()) {
            int h = extractHour(s.taken_at);
            if (h >= 0 && h <= 23) {
                report.hourly_distribution[h]++;
            }
        }
    }

    return report;
}

}  // namespace pillio
