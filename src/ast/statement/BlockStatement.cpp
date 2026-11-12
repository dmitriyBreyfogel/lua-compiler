#include "ast/statement/BlockStatement.h"

namespace lua::ast {

BlockStatement::BlockStatement()
    : StatementNode(NodeType::Block)
{
}

} // namespace lua::ast
