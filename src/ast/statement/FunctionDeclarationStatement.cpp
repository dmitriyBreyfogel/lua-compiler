#include "ast/statement/FunctionDeclarationStatement.h"

#include <utility>

#include "ast/SupportNodes.h"
#include "ast/expression/ExpressionNode.h"
#include "ast/statement/BlockStatement.h"

namespace lua::ast {

FunctionDeclarationStatement::FunctionDeclarationStatement(std::string scope)
    : StatementNode(NodeType::FunctionDeclaration, std::move(scope))
{
}

void FunctionDeclarationStatement::onChildAdded(const std::string& role, Node& child)
{
    if (role == "name") {
        name_ = dynamic_cast<ExpressionNode*>(&child);
    } else if (role == "parameters") {
        parameters_ = dynamic_cast<ListNode*>(&child);
    } else if (role == "body") {
        body_ = dynamic_cast<BlockStatement*>(&child);
    }
}

} // namespace lua::ast
