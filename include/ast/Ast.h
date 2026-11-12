#pragma once

#include <string>
#include "ast/Node.h"

namespace lua::ast {

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
} // namespace lua::ast
