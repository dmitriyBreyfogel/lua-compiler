#include <cassert>
#include <memory>

#include "ast/Ast.h"

int main()
{
    using namespace lua::ast;

    NodePtr identifier = makeNode(NodeType::Identifier, "total");
    assert(dynamic_cast<IdentifierExpression*>(identifier.get()) != nullptr);

    NodePtr binary = makeNode(NodeType::BinaryExpression, "+");
    assert(dynamic_cast<BinaryExpression*>(binary.get()) != nullptr);

    NodePtr declaration = makeNode(NodeType::LocalDeclaration);
    assert(dynamic_cast<DeclarationStatement*>(declaration.get()) != nullptr);

    NodePtr loop = makeNode(NodeType::NumericForStatement);
    assert(dynamic_cast<LoopStatement*>(loop.get()) != nullptr);

    NodePtr block = makeNode(NodeType::Block);
    assert(dynamic_cast<BlockStatement*>(block.get()) != nullptr);
}
