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
    assert(dynamic_cast<LocalDeclarationStatement*>(declaration.get()) != nullptr);

    NodePtr globalDeclaration = makeNode(NodeType::GlobalDeclaration);
    assert(dynamic_cast<GlobalDeclarationStatement*>(globalDeclaration.get()) != nullptr);

    NodePtr globalAllDeclaration = makeNode(NodeType::GlobalAllDeclaration);
    assert(dynamic_cast<GlobalAllDeclarationStatement*>(globalAllDeclaration.get()) != nullptr);

    NodePtr loop = makeNode(NodeType::NumericForStatement);
    assert(dynamic_cast<NumericForStatement*>(loop.get()) != nullptr);

    NodePtr genericLoop = makeNode(NodeType::GenericForStatement);
    assert(dynamic_cast<GenericForStatement*>(genericLoop.get()) != nullptr);

    NodePtr whileLoop = makeNode(NodeType::WhileStatement);
    assert(dynamic_cast<WhileStatement*>(whileLoop.get()) != nullptr);

    NodePtr repeatLoop = makeNode(NodeType::RepeatStatement);
    assert(dynamic_cast<RepeatStatement*>(repeatLoop.get()) != nullptr);

    NodePtr ifStatement = makeNode(NodeType::IfStatement);
    assert(dynamic_cast<IfStatement*>(ifStatement.get()) != nullptr);

    NodePtr elseIfStatement = makeNode(NodeType::ElseIfBranch);
    assert(dynamic_cast<ElseIfStatement*>(elseIfStatement.get()) != nullptr);

    NodePtr callStatement = makeNode(NodeType::CallStatement);
    assert(dynamic_cast<FunctionCallStatement*>(callStatement.get()) != nullptr);

    NodePtr block = makeNode(NodeType::Block);
    assert(dynamic_cast<BlockStatement*>(block.get()) != nullptr);
    block->addChild("statements", makeNode(NodeType::List));
    const auto* blockStatement = static_cast<const BlockStatement*>(block.get());
    assert(blockStatement->statements()->type() == NodeType::List);
}
