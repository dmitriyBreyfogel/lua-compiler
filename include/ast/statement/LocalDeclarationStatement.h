#pragma once

#include "ast/statement/DeclarationStatement.h"

namespace lua::ast {

class LocalDeclarationStatement : public DeclarationStatement {
public:
    LocalDeclarationStatement();
};

} // namespace lua::ast
