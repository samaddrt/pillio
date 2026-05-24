// checker.cpp — встроенная база взаимодействий + поиск пар.

#include "checker.hpp"

#include <algorithm>
#include <cctype>

namespace pillio {

namespace {

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

bool containsIgnoreCase(const std::string& haystack, const std::string& needle) {
    auto h = toLower(haystack);
    auto n = toLower(needle);
    return h.find(n) != std::string::npos;
}

const std::vector<Interaction>& getDatabase() {
    static const std::vector<Interaction> db = {
        {"warfarin", "aspirin", Severity::Dangerous,
         "Повышенный риск кровотечения при совместном приёме"},
        {"warfarin", "ibuprofen", Severity::Dangerous,
         "НПВС усиливают антикоагулянтный эффект варфарина"},
        {"aspirin", "ibuprofen", Severity::Warning,
         "Ибупрофен снижает кардиопротективный эффект аспирина"},
        {"metformin", "alcohol", Severity::Dangerous,
         "Риск лактоацидоза при сочетании с алкоголем"},
        {"simvastatin", "erythromycin", Severity::Dangerous,
         "Повышенный риск рабдомиолиза"},
        {"lisinopril", "potassium", Severity::Warning,
         "Риск гиперкалиемии при совместном приёме"},
        {"methotrexate", "ibuprofen", Severity::Dangerous,
         "НПВС снижают выведение метотрексата — токсичность"},
        {"fluoxetine", "tramadol", Severity::Dangerous,
         "Риск серотонинового синдрома"},
        {"omeprazole", "clopidogrel", Severity::Warning,
         "Омепразол снижает эффективность клопидогрела"},
        {"ciprofloxacin", "antacid", Severity::Warning,
         "Антациды снижают всасывание ципрофлоксацина"},
        {"amoxicillin", "methotrexate", Severity::Warning,
         "Амоксициллин повышает концентрацию метотрексата"},
        {"digoxin", "amiodarone", Severity::Dangerous,
         "Амиодарон повышает уровень дигоксина — токсичность"},
        {"lithium", "ibuprofen", Severity::Dangerous,
         "НПВС повышают концентрацию лития в крови"},
        {"ssri", "maoi", Severity::Dangerous,
         "Крайне опасная комбинация — серотониновый синдром"},
        {"sildenafil", "nitrate", Severity::Dangerous,
         "Критическое падение артериального давления"},
        {"tetracycline", "calcium", Severity::Warning,
         "Кальций снижает всасывание тетрациклина"},
        {"iron", "tetracycline", Severity::Warning,
         "Железо снижает всасывание тетрациклина"},
        {"theophylline", "ciprofloxacin", Severity::Dangerous,
         "Ципрофлоксацин повышает уровень теофиллина"},
        {"carbamazepine", "erythromycin", Severity::Dangerous,
         "Эритромицин повышает концентрацию карбамазепина"},
        {"phenytoin", "fluconazole", Severity::Dangerous,
         "Флуконазол повышает уровень фенитоина"},
    };
    return db;
}

}  // namespace

std::vector<Interaction> checkInteractions(const std::vector<std::string>& pill_names) {
    std::vector<Interaction> found;
    const auto& db = getDatabase();

    for (std::size_t i = 0; i < pill_names.size(); ++i) {
        for (std::size_t j = i + 1; j < pill_names.size(); ++j) {
            for (const auto& rule : db) {
                bool match_ab = (containsIgnoreCase(pill_names[i], rule.drug_a) &&
                                 containsIgnoreCase(pill_names[j], rule.drug_b));
                bool match_ba = (containsIgnoreCase(pill_names[i], rule.drug_b) &&
                                 containsIgnoreCase(pill_names[j], rule.drug_a));
                if (match_ab || match_ba) {
                    Interaction result = rule;
                    result.drug_a = pill_names[i];
                    result.drug_b = pill_names[j];
                    found.push_back(std::move(result));
                }
            }
        }
    }

    std::sort(found.begin(), found.end(),
              [](const Interaction& a, const Interaction& b) {
                  return a.severity > b.severity;
              });

    return found;
}

std::size_t interactionDatabaseSize() {
    return getDatabase().size();
}

}  // namespace pillio
