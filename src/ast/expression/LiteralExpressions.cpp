#include "ast/expression/LiteralExpressions.h"

#include <utility>

namespace lua::ast {

LiteralExpression::LiteralExpression(NodeType type, std::string value)
    : ExpressionNode(type, std::move(value))
{
}

IntegerLiteralExpression::IntegerLiteralExpression(long long value)
    : LiteralExpression(NodeType::IntegerLiteral, std::to_string(value)), integerValue_(value)
{
}

long long IntegerLiteralExpression::integerValue() const
{
    return integerValue_;
}

FloatLiteralExpression::FloatLiteralExpression(double value)
    : LiteralExpression(NodeType::FloatLiteral, std::to_string(value)), floatValue_(value)
{
}

double FloatLiteralExpression::floatValue() const
{
    return floatValue_;
}

StringLiteralExpression::StringLiteralExpression(std::string value)
    : LiteralExpression(NodeType::StringLiteral, std::move(value))
{
}

const std::string& StringLiteralExpression::text() const
{
    return value();
}

BooleanLiteralExpression::BooleanLiteralExpression(bool value)
    : LiteralExpression(NodeType::BooleanLiteral, value ? "true" : "false"), booleanValue_(value)
{
}

bool BooleanLiteralExpression::booleanValue() const
{
    return booleanValue_;
}

NilLiteralExpression::NilLiteralExpression()
    : LiteralExpression(NodeType::NilLiteral)
{
}

} // namespace lua::ast
