#pragma once

#include "ast/statement/DeclarationStatement.h"

namespace lua::ast {

class GlobalDeclarationStatement : public DeclarationStatement {
public:
    GlobalDeclarationStatement();
};

} // namespace lua::ast
