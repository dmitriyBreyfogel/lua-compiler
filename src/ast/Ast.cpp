#include "ast/Ast.h"

#include <utility>

namespace lua::ast {

NodePtr makeNode(NodeType type, std::string value)
{
    switch (type) {
    case NodeType::Block:
        return std::make_unique<BlockStatement>();
    case NodeType::List:
        return std::make_unique<ListNode>();
    case NodeType::DeclarationName:
        return std::make_unique<DeclarationNameNode>(std::move(value));
    case NodeType::Attribute:
        return std::make_unique<AttributeNode>(std::move(value));
    case NodeType::IntegerLiteral:
    case NodeType::FloatLiteral:
    case NodeType::StringLiteral:
    case NodeType::BooleanLiteral:
    case NodeType::NilLiteral:
        return std::make_unique<LiteralExpression>(type, std::move(value));
    case NodeType::VarargExpression:
        return std::make_unique<VarargExpression>(std::move(value));
    case NodeType::Identifier:
        return std::make_unique<IdentifierExpression>(std::move(value));
    case NodeType::UnaryExpression:
        return std::make_unique<UnaryExpression>(std::move(value));
    case NodeType::BinaryExpression:
        return std::make_unique<BinaryExpression>(std::move(value));
    case NodeType::IndexExpression:
        return std::make_unique<IndexExpression>();
    case NodeType::FieldExpression:
        return std::make_unique<FieldExpression>(std::move(value));
    case NodeType::CallExpression:
        return std::make_unique<CallExpression>();
    case NodeType::MethodCallExpression:
        return std::make_unique<MethodCallExpression>(std::move(value));
    case NodeType::TableConstructor:
        return std::make_unique<TableConstructorExpression>();
    case NodeType::TableField:
        return std::make_unique<TableFieldExpression>();
    case NodeType::FunctionExpression:
        return std::make_unique<FunctionExpression>();
    case NodeType::EmptyStatement:
        return std::make_unique<EmptyStatement>();
    case NodeType::BreakStatement:
        return std::make_unique<BreakStatement>();
    case NodeType::FunctionDeclaration:
        return std::make_unique<FunctionDeclarationStatement>(std::move(value));
    case NodeType::LocalDeclaration:
    case NodeType::GlobalDeclaration:
    case NodeType::GlobalAllDeclaration:
        return std::make_unique<DeclarationStatement>(type);
    case NodeType::AssignmentStatement:
        return std::make_unique<AssignmentStatement>();
    case NodeType::CallStatement:
        return std::make_unique<CallStatement>();
    case NodeType::LabelStatement:
        return std::make_unique<LabelStatement>(std::move(value));
    case NodeType::GotoStatement:
        return std::make_unique<GotoStatement>(std::move(value));
    case NodeType::IfStatement:
    case NodeType::ElseIfBranch:
        return std::make_unique<BranchStatement>(type);
    case NodeType::NumericForStatement:
    case NodeType::GenericForStatement:
    case NodeType::WhileStatement:
    case NodeType::RepeatStatement:
        return std::make_unique<LoopStatement>(type);
    case NodeType::ReturnStatement:
        return std::make_unique<ReturnStatement>();
    }

    return nullptr;
}

} // namespace lua::ast
