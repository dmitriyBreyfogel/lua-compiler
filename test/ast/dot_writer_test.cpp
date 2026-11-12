#include <cassert>
#include <sstream>

#include "ast/Ast.h"
#include "ast/DotWriter.h"

int main()
{
    auto expression = lua::ast::makeNode(lua::ast::NodeKind::BinaryExpression, "+");
    expression->addChild("left", lua::ast::makeNode(lua::ast::NodeKind::Identifier, "a"));
    expression->addChild("right", lua::ast::makeNode(lua::ast::NodeKind::StringLiteral, "x\\\"y"));

    std::ostringstream dot;
    lua::ast::DotWriter::writeExpression(dot, *expression, "expression");
    const std::string actual = dot.str();

    assert(actual.find("digraph \"expression\"") != std::string::npos);
    assert(actual.find("BinaryExpression\\n+") != std::string::npos);
    assert(actual.find("label=\"left\"") != std::string::npos);
    assert(actual.find("StringLiteral\\nx\\\\\\\"y") != std::string::npos);
}
