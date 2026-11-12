#include "ast/statement/EmptyStatement.h"

namespace lua::ast {

EmptyStatement::EmptyStatement()
    : StatementNode(NodeType::EmptyStatement)
{
}

} // namespace lua::ast
