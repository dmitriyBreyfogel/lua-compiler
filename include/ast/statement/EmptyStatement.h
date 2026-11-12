#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class EmptyStatement : public StatementNode {
public:
    EmptyStatement();
};

} // namespace lua::ast
