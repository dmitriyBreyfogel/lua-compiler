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

    NodePtr integer = makeNode(NodeType::IntegerLiteral, "42");
    const auto* integerLiteral = dynamic_cast<IntegerLiteralExpression*>(integer.get());
    assert(integerLiteral != nullptr);
    assert(integerLiteral->integerValue() == 42);

    NodePtr floating = makeNode(NodeType::FloatLiteral, "3.5");
    const auto* floatLiteral = dynamic_cast<FloatLiteralExpression*>(floating.get());
    assert(floatLiteral != nullptr);
    assert(floatLiteral->floatValue() == 3.5);

    NodePtr string = makeNode(NodeType::StringLiteral, "hello");
    const auto* stringLiteral = dynamic_cast<StringLiteralExpression*>(string.get());
    assert(stringLiteral != nullptr);
    assert(stringLiteral->text() == "hello");

    NodePtr boolean = makeNode(NodeType::BooleanLiteral, "true");
    const auto* booleanLiteral = dynamic_cast<BooleanLiteralExpression*>(boolean.get());
    assert(booleanLiteral != nullptr);
    assert(booleanLiteral->booleanValue());

    NodePtr nil = makeNode(NodeType::NilLiteral);
    assert(dynamic_cast<NilLiteralExpression*>(nil.get()) != nullptr);

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
