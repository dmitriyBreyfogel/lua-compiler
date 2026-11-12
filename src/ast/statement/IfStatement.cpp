#include "ast/statement/IfStatement.h"

namespace lua::ast {

IfStatement::IfStatement()
    : ConditionalStatement(NodeType::IfStatement)
{
}

} // namespace lua::ast
