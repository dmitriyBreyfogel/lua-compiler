#pragma once

#include "ast/statement/StatementNode.h"

namespace lua::ast {

class BlockStatement : public StatementNode {
public:
    BlockStatement();
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
};

class AssignmentStatement : public StatementNode {
public:
    AssignmentStatement();
};

class FunctionDeclarationStatement : public StatementNode {
public:
    explicit FunctionDeclarationStatement(std::string scope);
};

class CallStatement : public StatementNode {
public:
    CallStatement();
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
};

class LoopStatement : public StatementNode {
public:
    explicit LoopStatement(NodeType type);
};

class ReturnStatement : public StatementNode {
public:
    ReturnStatement();
};

} // namespace lua::ast
