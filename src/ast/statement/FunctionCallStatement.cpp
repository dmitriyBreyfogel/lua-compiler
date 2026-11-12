#include "ast/statement/FunctionCallStatement.h"

namespace lua::ast {

FunctionCallStatement::FunctionCallStatement()
    : StatementNode(NodeType::CallStatement)
{
}

} // namespace lua::ast
