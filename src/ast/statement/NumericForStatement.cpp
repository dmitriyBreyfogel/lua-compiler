#include "ast/statement/NumericForStatement.h"

#include "ast/expression/ExpressionNode.h"

namespace lua::ast {

NumericForStatement::NumericForStatement()
    : LoopStatement(NodeType::NumericForStatement)
{
}

void NumericForStatement::onChildAdded(const std::string& role, Node& child)
{
    LoopStatement::onChildAdded(role, child);
    if (role == "variable") {
        variable_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "start") {
        start_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "finish") {
        finish_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "step") {
        step_ = dynamic_cast<ExpressionNode*>(&child);
    }
}

} // namespace lua::ast
