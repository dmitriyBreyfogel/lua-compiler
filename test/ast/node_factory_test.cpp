#include <cassert>
#include <memory>

#include "ast/Ast.h"

int main()
{
    using namespace lua::ast;

    NodePtr identifier = createNode(NodeType::Identifier, "total");
    assert(identifier->type() == NodeType::Identifier);

    NodePtr binary = createNode(NodeType::BinaryExpression, "+");
    binary->addChild("left", createNode(NodeType::Identifier, "left"));
    binary->addChild("right", createNode(NodeType::IntegerLiteral, "1"));
    assert(binary->child("left")->value() == "left");
    assert(binary->child("right")->value() == "1");

    NodePtr integer = createNode(NodeType::IntegerLiteral, "42");
    assert(integer->value() == "42");

    NodePtr floating = createNode(NodeType::FloatLiteral, "3.5");
    assert(floating->value() == "3.5");

    NodePtr string = createNode(NodeType::StringLiteral, "hello");
    assert(string->value() == "hello");

    NodePtr boolean = createNode(NodeType::BooleanLiteral, "true");
    assert(boolean->value() == "true");

    NodePtr nil = createNode(NodeType::NilLiteral);
    assert(nil->type() == NodeType::NilLiteral);

    NodePtr declaration = createNode(NodeType::LocalDeclaration);
    assert(declaration->type() == NodeType::LocalDeclaration);

    NodePtr globalDeclaration = createNode(NodeType::GlobalDeclaration);
    assert(globalDeclaration->type() == NodeType::GlobalDeclaration);

    NodePtr globalAllDeclaration = createNode(NodeType::GlobalAllDeclaration);
    assert(globalAllDeclaration->type() == NodeType::GlobalAllDeclaration);

    NodePtr loop = createNode(NodeType::NumericForStatement);
    loop->addChild("variable", createNode(NodeType::Identifier, "index"));
    loop->addChild("start", createNode(NodeType::IntegerLiteral, "1"));
    loop->addChild("finish", createNode(NodeType::IntegerLiteral, "3"));
    loop->addChild("body", createNode(NodeType::Block));
    assert(loop->child("variable")->value() == "index");
    assert(loop->child("start")->value() == "1");
    assert(loop->child("body") != nullptr);

    NodePtr genericLoop = createNode(NodeType::GenericForStatement);
    assert(genericLoop->type() == NodeType::GenericForStatement);

    NodePtr block = createNode(NodeType::Block);
    block->addChild("statements", createNode(NodeType::List));
    assert(block->child("statements")->type() == NodeType::List);
}
