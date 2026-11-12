#include "ast/statement/ReturnStatement.h"

namespace lua::ast {

ReturnStatement::ReturnStatement()
    : StatementNode(NodeType::ReturnStatement)
{
}

} // namespace lua::ast
