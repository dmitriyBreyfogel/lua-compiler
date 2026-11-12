#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace lua::ast {

enum class NodeKind {
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

class Node final {
public:
    Node(NodeKind kind, std::string value = {});

    std::uint64_t id() const noexcept;
    NodeKind kind() const noexcept;
    const std::string& value() const noexcept;
    const std::vector<Child>& children() const noexcept;

    Node& addChild(std::string role, NodePtr child);
    Node& addChild(std::string role, Node* child);

private:
    std::uint64_t id_;
    NodeKind kind_;
    std::string value_;
    std::vector<Child> children_;
};

NodePtr makeNode(NodeKind kind, std::string value = {});
const char* nodeKindName(NodeKind kind) noexcept;

} // namespace lua::ast
