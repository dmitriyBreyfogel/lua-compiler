#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace lua::semantic {

enum class FreeGlobalPolicy {
    ImplicitWritable,
    ExplicitOnly,
    ExplicitWritable,
    ExplicitReadOnly,
};

struct LabelTarget {
    std::uint64_t symbolId;
    int localCount;
};

class Scope {
public:
    Scope(int functionDepth, FreeGlobalPolicy freeGlobalPolicy);

    int functionDepth() const;
    int localCount() const;
    FreeGlobalPolicy freeGlobalPolicy() const;
    void addLocal();
    void setFreeGlobalPolicy(FreeGlobalPolicy policy);

    bool hasName(const std::string& name) const;
    void defineName(std::string name, std::uint64_t symbolId);
    std::uint64_t findName(const std::string& name) const;

    bool hasLabel(const std::string& name) const;
    void defineLabel(std::string name, LabelTarget target);
    const LabelTarget* findLabel(const std::string& name) const;

private:
    std::unordered_map<std::string, std::uint64_t> names_;
    std::unordered_map<std::string, LabelTarget> labels_;
    int localCount_ = 0;
    int functionDepth_;
    FreeGlobalPolicy freeGlobalPolicy_;
};

} // namespace lua::semantic
