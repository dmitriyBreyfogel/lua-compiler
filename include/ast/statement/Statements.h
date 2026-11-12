#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class BlockStatement : public StatementNode {
public:
    BlockStatement();

    const Node* statements() const { return child("statements"); }
    const Node* returnStatement() const { return child("return"); }
};

class EmptyStatement : public StatementNode {
public:
    EmptyStatement();
};

class BreakStatement : public StatementNode {
public:
    BreakStatement();
};

class DeclarationStatement : public StatementNode {
public:
    explicit DeclarationStatement(NodeType type);

    const Node* names() const { return child("names"); }
    const Node* values() const { return child("values"); }
};

class AssignmentStatement : public StatementNode {
public:
    AssignmentStatement();

    const Node* targets() const { return child("targets"); }
    const Node* values() const { return child("values"); }
};

class FunctionDeclarationStatement : public StatementNode {
public:
    explicit FunctionDeclarationStatement(std::string scope);

    const Node* name() const { return child("name"); }
    const Node* parameters() const { return child("parameters"); }
    const Node* body() const { return child("body"); }
};

class CallStatement : public StatementNode {
public:
    CallStatement();

    const Node* call() const { return child("call"); }
};

class LabelStatement : public StatementNode {
public:
    explicit LabelStatement(std::string name);
};

class GotoStatement : public StatementNode {
public:
    explicit GotoStatement(std::string name);
};

class BranchStatement : public StatementNode {
public:
    explicit BranchStatement(NodeType type);

    const Node* condition() const { return child("condition"); }
    const Node* thenBlock() const { return child("then"); }
    const Node* elseIfBranches() const { return child("elseif"); }
    const Node* elseBlock() const { return child("else"); }
    const Node* body() const { return child("body"); }
};

class LoopStatement : public StatementNode {
public:
    explicit LoopStatement(NodeType type);

    const Node* variable() const { return child("variable"); }
    const Node* names() const { return child("names"); }
    const Node* start() const { return child("start"); }
    const Node* finish() const { return child("finish"); }
    const Node* step() const { return child("step"); }
    const Node* iterators() const { return child("iterators"); }
    const Node* condition() const { return child("condition"); }
    const Node* body() const { return child("body"); }
};

class ReturnStatement : public StatementNode {
public:
    ReturnStatement();

    const Node* values() const { return child("values"); }
};

} // namespace lua::ast
