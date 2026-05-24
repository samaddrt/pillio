#pragma once
/**
 * @file family.hpp
 * @brief Семейный доступ — профили, share-коды, связи подписок.
 *
 * Позволяет делиться статусом приёма лекарств с близкими:
 * сын подписывается по 6-символьному коду и видит, приняла ли
 * мама лекарство. Данные в отдельном family.json.
 */

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "models.hpp"  // ValidationError, NotFoundError, StorageError

namespace pillio {

// ── Profile ──────────────────────────────────────────────────────

struct Profile {
    std::int64_t chat_id{0};   ///< Telegram chat_id владельца профиля
    std::string name;          ///< Отображаемое имя (UTF-8)
    std::string share_code;    ///< Уникальный код для подписки (6 символов)

    void validate() const {
        if (chat_id == 0) {
            throw ValidationError("Profile chat_id must not be zero");
        }
        if (name.empty()) {
            throw ValidationError("Profile name must not be empty");
        }
    }
};

inline void to_json(nlohmann::json& j, const Profile& p) {
    j = nlohmann::json{{"chat_id", p.chat_id},
                       {"name", p.name},
                       {"share_code", p.share_code}};
}

inline void from_json(const nlohmann::json& j, Profile& p) {
    j.at("chat_id").get_to(p.chat_id);
    j.at("name").get_to(p.name);
    p.share_code = j.value("share_code", std::string{});
}

// ── FamilyLink — подписка «кто → за кем» ─────────────────────────

struct FamilyLink {
    std::int64_t follower{0};  ///< chat_id того, кто следит
    std::int64_t target{0};    ///< chat_id того, за кем следят
    std::string relation;      ///< Метка отношения, заданная подписчиком
    std::string created_at;    ///< Момент создания связи (ISO 8601)

    void validate() const {
        if (follower == 0 || target == 0) {
            throw ValidationError("FamilyLink requires non-zero follower/target");
        }
        if (follower == target) {
            throw ValidationError("Cannot follow yourself");
        }
    }
};

inline void to_json(nlohmann::json& j, const FamilyLink& l) {
    j = nlohmann::json{{"follower", l.follower},
                       {"target", l.target},
                       {"relation", l.relation},
                       {"created_at", l.created_at}};
}

inline void from_json(const nlohmann::json& j, FamilyLink& l) {
    j.at("follower").get_to(l.follower);
    j.at("target").get_to(l.target);
    l.relation = j.value("relation", std::string{});
    l.created_at = j.value("created_at", std::string{});
}

// ── FamilyStore — JSON-хранилище профилей и связей ───────────────

class FamilyStore {
public:
    explicit FamilyStore(std::filesystem::path path);

    /// Создаёт профиль (если нет) и возвращает его с share_code.
    Profile ensureProfile(std::int64_t chat_id, const std::string& name);
    /// Поиск по 6-символьному коду (регистронезависимый).
    std::optional<Profile> profileByCode(const std::string& code) const;
    std::optional<Profile> profileByChatId(std::int64_t chat_id) const;

    /// Подписка follower → target (идемпотентно: повтор обновляет relation).
    void link(std::int64_t follower, std::int64_t target,
              const std::string& relation);
    void unlink(std::int64_t follower, std::int64_t target);

    std::vector<FamilyLink> following(std::int64_t follower) const;
    std::vector<FamilyLink> followers(std::int64_t target) const;

private:
    std::filesystem::path path_;
    nlohmann::json load() const;
    void save(const nlohmann::json& data) const;
    std::string generateCode(const nlohmann::json& data) const;
};

}  // namespace pillio
