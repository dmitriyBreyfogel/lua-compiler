#include "ast/statement/ElseIfStatement.h"

namespace lua::ast {

ElseIfStatement::ElseIfStatement()
    : ConditionalStatement(NodeType::ElseIfBranch)
{
}

} // namespace lua::ast
