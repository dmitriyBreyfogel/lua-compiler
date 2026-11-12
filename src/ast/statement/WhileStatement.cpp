#include "ast/statement/WhileStatement.h"

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

WhileStatement::WhileStatement()
    : LoopStatement(NodeType::WhileStatement)
{
}

void WhileStatement::onChildAdded(const std::string& role, Node& child)
{
    LoopStatement::onChildAdded(role, child);
    if (role == "condition") {
        condition_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

} // namespace lua::ast
