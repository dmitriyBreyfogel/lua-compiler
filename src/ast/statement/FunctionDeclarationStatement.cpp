#include "ast/statement/FunctionDeclarationStatement.h"

#include <utility>

namespace lua::ast {

FunctionDeclarationStatement::FunctionDeclarationStatement(std::string scope)
    : StatementNode(NodeType::FunctionDeclaration, std::move(scope))
{
}

} // namespace lua::ast
