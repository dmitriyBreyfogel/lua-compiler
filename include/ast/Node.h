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
    Node* child(const std::string& role);
    const Node* child(const std::string& role) const;

    Node& addChild(std::string role, NodePtr child);
    Node& addChild(std::string role, Node* child);

protected:
    Node(NodeType type, std::string value = {});
    virtual void onChildAdded(const std::string& role, Node& child);

private:
    std::uint64_t id_;
    NodeType type_;
    std::string value_;
    std::vector<Child> children_;
};

const char* nodeTypeName(NodeType type);

} // namespace lua::ast
