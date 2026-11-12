#include "ast/statement/GlobalDeclarationStatement.h"

namespace lua::ast {

GlobalDeclarationStatement::GlobalDeclarationStatement()
    : DeclarationStatement(NodeType::GlobalDeclaration)
{
}

} // namespace lua::ast
