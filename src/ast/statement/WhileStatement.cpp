#include "ast/statement/WhileStatement.h"

namespace lua::ast {

WhileStatement::WhileStatement()
    : LoopStatement(NodeType::WhileStatement)
{
}

} // namespace lua::ast
