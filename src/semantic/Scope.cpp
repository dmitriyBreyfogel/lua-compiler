#include "semantic/Scope.h"

#include <utility>

namespace lua::semantic {

Scope::Scope(int functionDepth, FreeGlobalPolicy freeGlobalPolicy)
    : functionDepth_(functionDepth)
    , freeGlobalPolicy_(freeGlobalPolicy)
{
}

int Scope::functionDepth() const
{
    return functionDepth_;
}

int Scope::localCount() const
{
    return localCount_;
}

FreeGlobalPolicy Scope::freeGlobalPolicy() const
{
    return freeGlobalPolicy_;
}

void Scope::addLocal()
{
    ++localCount_;
}

void Scope::setFreeGlobalPolicy(FreeGlobalPolicy policy)
{
    freeGlobalPolicy_ = policy;
}

bool Scope::hasName(const std::string& name) const
{
    return names_.count(name) != 0;
}

void Scope::defineName(std::string name, std::uint64_t symbolId)
{
    names_[std::move(name)] = symbolId;
}

std::uint64_t Scope::findName(const std::string& name) const
{
    const auto found = names_.find(name);
    return found == names_.end() ? 0 : found->second;
}

bool Scope::hasLabel(const std::string& name) const
{
    return labels_.count(name) != 0;
}

void Scope::defineLabel(std::string name, LabelTarget target)
{
    labels_[std::move(name)] = target;
}

const LabelTarget* Scope::findLabel(const std::string& name) const
{
    const auto found = labels_.find(name);
    return found == labels_.end() ? nullptr : &found->second;
}

} // namespace lua::semantic
