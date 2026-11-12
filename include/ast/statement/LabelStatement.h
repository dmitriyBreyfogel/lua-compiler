#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class LabelStatement : public StatementNode {
public:
    explicit LabelStatement(std::string name);
};

} // namespace lua::ast
