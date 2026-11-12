#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace lua::ast {

enum class NodeType {
    Block,
    List,
    DeclarationName,
    Attribute,
    EmptyStatement,
    BreakStatement,
    FunctionDeclaration,
    LocalDeclaration,
    GlobalDeclaration,
    GlobalAllDeclaration,
    AssignmentStatement,
    CallStatement,
    LabelStatement,
    GotoStatement,
    IfStatement,
    ElseIfBranch,
    NumericForStatement,
    GenericForStatement,
    WhileStatement,
    RepeatStatement,
    ReturnStatement,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    BooleanLiteral,
    NilLiteral,
    VarargExpression,
    Identifier,
    UnaryExpression,
    BinaryExpression,
    IndexExpression,
    FieldExpression,
    CallExpression,
    MethodCallExpression,
    TableConstructor,
    TableField,
    FunctionExpression,
};

class Node;
using NodePtr = std::unique_ptr<Node>;

struct Child {
    std::string role;
    NodePtr node;
};

class Node {
public:
    virtual ~Node() = default;

    std::uint64_t id() const;
    NodeType type() const;
    const std::string& value() const;
    const std::vector<Child>& children() const;

    Node& addChild(std::string role, NodePtr child);
    Node& addChild(std::string role, Node* child);

protected:
    Node(NodeType type, std::string value = {});

private:
    std::uint64_t id_;
    NodeType type_;
    std::string value_;
    std::vector<Child> children_;
};

class ExpressionNode : public Node {
public:
    ExpressionNode(NodeType type, std::string value = {});
};

class StatementNode : public Node {
public:
    StatementNode(NodeType type, std::string value = {});
};

class BlockNode : public StatementNode {
public:
    BlockNode();
};

class ListNode : public Node {
public:
    ListNode();
};

class DeclarationNameNode : public Node {
public:
    explicit DeclarationNameNode(std::string value);
};

class AttributeNode : public Node {
public:
    explicit AttributeNode(std::string value);
};

NodePtr makeNode(NodeType type, std::string value = {});
const char* nodeTypeName(NodeType type);

} // namespace lua::ast
