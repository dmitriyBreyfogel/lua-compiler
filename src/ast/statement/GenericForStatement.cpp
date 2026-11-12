#include "ast/statement/GenericForStatement.h"

namespace lua::ast {

GenericForStatement::GenericForStatement()
    : LoopStatement(NodeType::GenericForStatement)
{
}

} // namespace lua::ast
