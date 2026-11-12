#include "ast/statement/LocalDeclarationStatement.h"

namespace lua::ast {

LocalDeclarationStatement::LocalDeclarationStatement()
    : DeclarationStatement(NodeType::LocalDeclaration)
{
}

} // namespace lua::ast
