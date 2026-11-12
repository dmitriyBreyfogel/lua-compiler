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
    binary->addChild("left", makeNode(NodeType::Identifier, "left"));
    binary->addChild("right", makeNode(NodeType::IntegerLiteral, "1"));
    const auto* binaryExpression = static_cast<const BinaryExpression*>(binary.get());
    assert(binaryExpression->left()->value() == "left");
    assert(binaryExpression->right()->value() == "1");

    NodePtr declaration = makeNode(NodeType::LocalDeclaration);
    assert(dynamic_cast<DeclarationStatement*>(declaration.get()) != nullptr);

    NodePtr loop = makeNode(NodeType::NumericForStatement);
    assert(dynamic_cast<LoopStatement*>(loop.get()) != nullptr);

    NodePtr block = makeNode(NodeType::Block);
    assert(dynamic_cast<BlockStatement*>(block.get()) != nullptr);
    block->addChild("statements", makeNode(NodeType::List));
    const auto* blockStatement = static_cast<const BlockStatement*>(block.get());
    assert(blockStatement->statements()->type() == NodeType::List);
}
