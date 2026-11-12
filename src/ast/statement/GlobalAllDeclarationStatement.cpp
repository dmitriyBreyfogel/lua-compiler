#include "ast/statement/GlobalAllDeclarationStatement.h"

namespace lua::ast {

GlobalAllDeclarationStatement::GlobalAllDeclarationStatement()
    : DeclarationStatement(NodeType::GlobalAllDeclaration)
{
}

} // namespace lua::ast
