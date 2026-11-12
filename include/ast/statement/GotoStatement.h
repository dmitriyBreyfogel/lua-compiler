#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class GotoStatement : public StatementNode {
public:
    explicit GotoStatement(std::string name);
};

} // namespace lua::ast
