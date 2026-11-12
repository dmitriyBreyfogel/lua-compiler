#include "ast/statement/RepeatStatement.h"

namespace lua::ast {

RepeatStatement::RepeatStatement()
    : LoopStatement(NodeType::RepeatStatement)
{
}

} // namespace lua::ast
