#pragma once

#include "ast/statement/DeclarationStatement.h"

namespace lua::ast {

class GlobalAllDeclarationStatement : public DeclarationStatement {
public:
    GlobalAllDeclarationStatement();
};

} // namespace lua::ast
