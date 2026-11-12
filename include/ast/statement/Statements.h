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
    const Node* names() const { return child("names"); }
    const Node* values() const { return child("values"); }

protected:
    explicit DeclarationStatement(NodeType type);
};

class LocalDeclarationStatement : public DeclarationStatement {
public:
    LocalDeclarationStatement();
};

class GlobalDeclarationStatement : public DeclarationStatement {
public:
    GlobalDeclarationStatement();
};

class GlobalAllDeclarationStatement : public DeclarationStatement {
public:
    GlobalAllDeclarationStatement();
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

class FunctionCallStatement : public StatementNode {
public:
    FunctionCallStatement();

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

class ConditionalStatement : public StatementNode {
public:
    const Node* condition() const { return child("condition"); }

protected:
    explicit ConditionalStatement(NodeType type);
};

class IfStatement : public ConditionalStatement {
public:
    IfStatement();

    const Node* thenBlock() const { return child("then"); }
    const Node* elseIfBranches() const { return child("elseif"); }
    const Node* elseBlock() const { return child("else"); }
};

class ElseIfStatement : public ConditionalStatement {
public:
    ElseIfStatement();

    const Node* body() const { return child("body"); }
};

class LoopStatement : public StatementNode {
public:
    const Node* body() const { return child("body"); }

protected:
    explicit LoopStatement(NodeType type);
};

class NumericForStatement : public LoopStatement {
public:
    NumericForStatement();

    const Node* variable() const { return child("variable"); }
    const Node* start() const { return child("start"); }
    const Node* finish() const { return child("finish"); }
    const Node* step() const { return child("step"); }
};

class GenericForStatement : public LoopStatement {
public:
    GenericForStatement();

    const Node* names() const { return child("names"); }
    const Node* iterators() const { return child("iterators"); }
};

class WhileStatement : public LoopStatement {
public:
    WhileStatement();

    const Node* condition() const { return child("condition"); }
};

class RepeatStatement : public LoopStatement {
public:
    RepeatStatement();

    const Node* condition() const { return child("condition"); }
};

class ReturnStatement : public StatementNode {
public:
    ReturnStatement();

    const Node* values() const { return child("values"); }
};

} // namespace lua::ast
