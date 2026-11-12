#include "ast/statement/RepeatStatement.h"

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

RepeatStatement::RepeatStatement()
    : LoopStatement(NodeType::RepeatStatement)
{
}

void RepeatStatement::onChildAdded(const std::string& role, Node& child)
{
    LoopStatement::onChildAdded(role, child);
    if (role == "condition") {
        condition_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

} // namespace lua::ast
