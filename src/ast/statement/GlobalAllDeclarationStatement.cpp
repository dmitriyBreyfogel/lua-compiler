#include "ast/statement/GlobalAllDeclarationStatement.h"

#include "ast/SupportNodes.h"

namespace lua::ast {

GlobalAllDeclarationStatement::GlobalAllDeclarationStatement()
    : DeclarationStatement(NodeType::GlobalAllDeclaration)
{
}

void GlobalAllDeclarationStatement::onChildAdded(const std::string& role, Node& child)
{
    DeclarationStatement::onChildAdded(role, child);
    if (role == "attribute") {
        attribute_ = dynamic_cast<AttributeNode*>(&child);
    }
}

} // namespace lua::ast
