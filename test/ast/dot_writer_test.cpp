#include <cassert>
#include <sstream>

#include "ast/Ast.h"
#include "ast/DotWriter.h"

int main()
{
    auto expression = lua::ast::makeNode(lua::ast::NodeType::BinaryExpression, "+");
    expression->addChild("left", lua::ast::makeNode(lua::ast::NodeType::Identifier, "a"));
    expression->addChild("right", lua::ast::makeNode(lua::ast::NodeType::StringLiteral, "x\\\"y"));

    std::ostringstream dot;
    auto root = lua::ast::makeNode(lua::ast::NodeType::Block);
    root->addChild("statement[0]", std::move(expression));

    lua::ast::DotWriter::writeTree(dot, *root, "expression");
    const std::string actual = dot.str();

    assert(actual.find("digraph \"expression\"") != std::string::npos);
    assert(actual.find("BinaryExpression\\n+") != std::string::npos);
    assert(actual.find("Block") != std::string::npos);
    assert(actual.find("statement[0]") != std::string::npos);
    assert(actual.find("label=\"left\"") != std::string::npos);
    assert(actual.find("StringLiteral\\nx\\\\\\\"y") != std::string::npos);
}
