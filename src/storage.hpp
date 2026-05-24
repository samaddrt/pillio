#pragma once
/// @file storage.hpp — CRUD-хранилище (pills + schedules) в одном JSON-файле.

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "models.hpp"

namespace pillio {

/// Каждый метод делает полный load → modify → save (без внешней БД).
class Storage {
public:
    explicit Storage(const std::filesystem::path& db_path);

    std::vector<Pill> getAllPills() const;
    Pill getPillById(std::uint64_t id) const;
    Pill addPill(Pill pill);
    void removePill(std::uint64_t id);

    std::vector<Schedule> getAllSchedules() const;
    std::vector<Schedule> getSchedulesForDate(const std::string& date_prefix) const;
    void addSchedule(Schedule entry);
    void markTaken(std::uint64_t pill_id,
                   const std::string& scheduled_time,
                   const std::string& taken_at);

private:
    std::filesystem::path db_path_;
    nlohmann::json load() const;
    void save(const nlohmann::json& data) const;
    std::uint64_t nextPillId(const nlohmann::json& data) const;
};

}  // namespace pillio
