#include "semantic/SemanticAnalyzer.h"
#include "semantic/Scope.h"

namespace lua::semantic {

namespace {

using ast::Node;
using ast::NodeType;

class Analyzer {
public:
    SemanticResult run(const Node& block)
    {
        pushScope();
        visitBlock(static_cast<const ast::BlockStatement&>(block), false);
        return std::move(result_);
    }

private:
    void visitList(const Node* list)
    {
        if (list == nullptr) {
            return;
        }

        for (const ast::Child& item : list->children()) {
            visit(*item.node);
        }
    }

    void visitBlock(const ast::BlockStatement& block, bool createScope)
    {
        if (createScope) {
            pushScope();
        }

        const Node* statements = block.statements();
        declareLabels(statements);
        visitList(statements);
        visitNode(block.returnStatement());

        if (createScope) {
            scopes_.pop_back();
        }
    }

    void declareLabels(const Node* statements)
    {
        if (statements == nullptr) {
            return;
        }

        int localCount = 0;
        for (const ast::Child& item : statements->children()) {
            const Node& statement = *item.node;
            if (statement.type() == NodeType::LabelStatement) {
                if (scopes_.back().hasLabel(statement.value())) {
                    error(statement, "duplicate label: " + statement.value());
                } else {
                    const std::uint64_t id = declare(statement.value(), SymbolType::Label, statement.id(), false);
                    scopes_.back().defineLabel(statement.value(), {id, localCount});
                    bind(statement, id);
                }
            }

            localCount += localDeclarations(statement);
        }
    }

    int localDeclarations(const Node& statement)
    {
        if (statement.type() == NodeType::LocalDeclaration) {
            const auto& declaration = static_cast<const ast::DeclarationStatement&>(statement);
            const Node* names = declaration.names();
            return names == nullptr ? 0 : static_cast<int>(names->children().size());
        }
        if (statement.type() == NodeType::FunctionDeclaration && statement.value() == "local") {
            return 1;
        }
        return 0;
    }

    void visit(const Node& node)
    {
        switch (node.type()) {
        case NodeType::Block:
            visitBlock(static_cast<const ast::BlockStatement&>(node), true);
            break;
        case NodeType::LocalDeclaration:
            visitDeclaration(static_cast<const ast::DeclarationStatement&>(node), SymbolType::Local, true);
            break;
        case NodeType::GlobalDeclaration:
            visitDeclaration(static_cast<const ast::DeclarationStatement&>(node), SymbolType::Global, false);
            break;
        case NodeType::GlobalAllDeclaration:
            break;
        case NodeType::AssignmentStatement:
            visitAssignment(static_cast<const ast::AssignmentStatement&>(node));
            break;
        case NodeType::FunctionDeclaration:
            visitFunctionDeclaration(node);
            break;
        case NodeType::FunctionExpression:
            visitFunction(static_cast<const ast::FunctionExpression&>(node));
            break;
        case NodeType::NumericForStatement:
            visitNumericFor(static_cast<const ast::LoopStatement&>(node));
            break;
        case NodeType::GenericForStatement:
            visitGenericFor(static_cast<const ast::LoopStatement&>(node));
            break;
        case NodeType::WhileStatement:
            visitWhile(static_cast<const ast::LoopStatement&>(node));
            break;
        case NodeType::RepeatStatement:
            visitRepeat(static_cast<const ast::LoopStatement&>(node));
            break;
        case NodeType::IfStatement:
        case NodeType::ElseIfBranch:
            visitBranch(static_cast<const ast::BranchStatement&>(node));
            break;
        case NodeType::BreakStatement:
            if (loopDepth_ == 0) {
                error(node, "break outside loop");
            }
            break;
        case NodeType::GotoStatement:
            resolveLabel(node);
            break;
        case NodeType::FieldExpression:
            visitNode(static_cast<const ast::FieldExpression&>(node).object());
            break;
        case NodeType::Identifier:
            bind(node, resolveName(node));
            break;
        case NodeType::VarargExpression:
            if (varargDepth_ == 0) {
                error(node, "vararg outside function with vararg parameter");
            }
            break;
        default:
            visitChildren(node);
            break;
        }
    }

    void visitChildren(const Node& node)
    {
        for (const ast::Child& item : node.children()) {
            visit(*item.node);
        }
    }

    void visitDeclaration(const ast::DeclarationStatement& declaration, SymbolType type, bool valuesBeforeNames)
    {
        if (valuesBeforeNames) {
            visitList(declaration.values());
            declareNames(declaration.names(), type);
            return;
        }

        declareNames(declaration.names(), type);
        visitList(declaration.values());
    }

    void visitAssignment(const ast::AssignmentStatement& assignment)
    {
        visitList(assignment.values());
        visitAssignmentTargets(assignment.targets());
    }

    void visitNumericFor(const ast::LoopStatement& loop)
    {
        visitNode(loop.start());
        visitNode(loop.finish());
        visitNode(loop.step());
        pushScope();
        declareNode(*loop.variable(), SymbolType::LoopVariable);
        ++loopDepth_;
        visitNode(loop.body());
        --loopDepth_;
        scopes_.pop_back();
    }

    void visitGenericFor(const ast::LoopStatement& loop)
    {
        visitList(loop.iterators());
        pushScope();
        declareNames(loop.names(), SymbolType::LoopVariable);
        ++loopDepth_;
        visitNode(loop.body());
        --loopDepth_;
        scopes_.pop_back();
    }

    void visitWhile(const ast::LoopStatement& loop)
    {
        visitNode(loop.condition());
        ++loopDepth_;
        visitNode(loop.body());
        --loopDepth_;
    }

    void visitRepeat(const ast::LoopStatement& loop)
    {
        ++loopDepth_;
        visitRepeatBody(*loop.body());
        visitNode(loop.condition());
        finishRepeatBody();
        --loopDepth_;
    }

    void visitBranch(const ast::BranchStatement& branch)
    {
        if (branch.type() == NodeType::IfStatement) {
            visitNode(branch.condition());
            visitNode(branch.thenBlock());
            visitList(branch.elseIfBranches());
            visitNode(branch.elseBlock());
            return;
        }

        visitNode(branch.condition());
        visitNode(branch.body());
    }

    void visitNode(const Node* node)
    {
        if (node != nullptr) {
            visit(*node);
        }
    }

    void visitRepeatBody(const Node& block)
    {
        pushScope();
        const auto& repeatBlock = static_cast<const ast::BlockStatement&>(block);
        const Node* statements = repeatBlock.statements();
        declareLabels(statements);
        visitList(statements);
        visitNode(repeatBlock.returnStatement());
    }

    void finishRepeatBody()
    {
        scopes_.pop_back();
    }

    void visitFunctionDeclaration(const Node& node)
    {
        const auto& declaration = static_cast<const ast::FunctionDeclarationStatement&>(node);
        const Node* name = declaration.name();
        if (name != nullptr && name->type() == NodeType::Identifier) {
            declareNode(*name, node.value() == "local" ? SymbolType::Local : SymbolType::Global);
        } else {
            visitNode(name);
        }
        visitFunction(declaration.parameters(), declaration.body(), name);
    }

    void visitFunction(const ast::FunctionExpression& function)
    {
        visitFunction(function.parameters(), function.body(), nullptr);
    }

    void visitFunction(const Node* parameters, const Node* body, const Node* name)
    {
        ++functionDepth_;
        const int previousLoopDepth = loopDepth_;
        const int previousVarargDepth = varargDepth_;
        loopDepth_ = 0;
        varargDepth_ = 0;
        pushScope();
        if (name != nullptr && name->type() == NodeType::FieldExpression && name->value() == "method") {
            declare("self", SymbolType::Parameter, name->id(), false);
            scopes_.back().defineName("self", result_.symbols_.back().id);
        }
        bool acceptsVararg = false;
        if (parameters != nullptr) {
            for (const ast::Child& item : parameters->children()) {
                if (item.node->type() == NodeType::VarargExpression) {
                    acceptsVararg = true;
                    if (!item.node->value().empty()) {
                        declareNode(*item.node, SymbolType::Parameter);
                    }
                } else {
                    declareNode(*item.node, SymbolType::Parameter);
                }
            }
        }

        if (acceptsVararg) {
            ++varargDepth_;
        }
        visitNode(body);
        if (acceptsVararg) {
            --varargDepth_;
        }
        --functionDepth_;
        loopDepth_ = previousLoopDepth;
        varargDepth_ = previousVarargDepth;
        scopes_.pop_back();
    }

    void declareNames(const Node* names, SymbolType type)
    {
        if (names == nullptr) {
            return;
        }
        for (const ast::Child& item : names->children()) {
            declareNode(*item.node, type);
        }
    }

    void declareNode(const Node& node, SymbolType type)
    {
        const std::string& name = node.value();
        Scope& scope = type == SymbolType::Global ? scopes_.front() : scopes_.back();
        if ((type == SymbolType::Parameter || type == SymbolType::LoopVariable) && scope.hasName(name)) {
            error(node, "duplicate declaration: " + name);
            return;
        }
        const std::uint64_t id = declare(name, type, node.id(), isReadOnly(node));
        scope.defineName(name, id);
        if (type == SymbolType::Local) {
            scope.addLocal();
        }
        bind(node, id);
    }

    std::uint64_t declare(const std::string& name, SymbolType type, std::uint64_t declarationId, bool readOnly)
    {
        const std::uint64_t id = result_.symbols_.size() + 1;
        result_.symbols_.push_back({id, name, type, declarationId, readOnly});
        return id;
    }

    bool isReadOnly(const Node& node)
    {
        for (const ast::Child& child : node.children()) {
            if (child.node->type() != NodeType::Attribute) {
                continue;
            }
            if (child.node->value() == "const" || child.node->value() == "close") {
                return true;
            }
            error(*child.node, "unknown local attribute: " + child.node->value());
        }
        return false;
    }

    void visitAssignmentTargets(const Node* targets)
    {
        if (targets == nullptr) {
            return;
        }
        for (const ast::Child& item : targets->children()) {
            visit(*item.node);
            if (item.node->type() != NodeType::Identifier) {
                continue;
            }
            const Symbol* symbol = result_.bindingFor(*item.node);
            if (symbol != nullptr && symbol->readOnly) {
                error(*item.node, "assignment to read-only variable: " + symbol->name);
            }
        }
    }

    std::uint64_t resolveName(const Node& node)
    {
        const std::string& name = node.value();
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            const std::uint64_t symbolId = scope->findName(name);
            if (symbolId != 0) {
                const Symbol& symbol = result_.symbols_[symbolId - 1];
                if (scope->functionDepth() < functionDepth_ && symbol.type != SymbolType::Global) {
                    result_.upvalues_.insert(node.id());
                }
                return symbolId;
            }
        }

        Scope& global = scopes_.front();
        const std::uint64_t symbolId = global.findName(name);
        if (symbolId != 0) {
            return symbolId;
        }

        const std::uint64_t id = declare(name, SymbolType::Global, node.id(), false);
        global.defineName(name, id);
        return id;
    }

    void resolveLabel(const Node& node)
    {
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            if (scope->functionDepth() < functionDepth_) {
                break;
            }
            const LabelTarget* label = scope->findLabel(node.value());
            if (label != nullptr) {
                if (label->localCount > scope->localCount()) {
                    error(node, "goto jumps into scope of local variable: " + node.value());
                    return;
                }
                bind(node, label->symbolId);
                return;
            }
        }
        error(node, "unknown label: " + node.value());
    }

    void bind(const Node& node, std::uint64_t symbolId)
    {
        result_.bindings_[node.id()] = symbolId;
    }

    void error(const Node& node, const std::string& message)
    {
        result_.diagnostics_.push_back({node.id(), message});
    }

    void pushScope()
    {
        scopes_.emplace_back(functionDepth_);
    }

    SemanticResult result_;
    std::vector<Scope> scopes_;
    int loopDepth_ = 0;
    int functionDepth_ = 0;
    int varargDepth_ = 0;
};

}

const std::vector<Symbol>& SemanticResult::symbols() const
{
    return symbols_;
}

const std::vector<Diagnostic>& SemanticResult::diagnostics() const
{
    return diagnostics_;
}

const Symbol* SemanticResult::bindingFor(const ast::Node& node) const
{
    const auto found = bindings_.find(node.id());
    if (found == bindings_.end()) {
        return nullptr;
    }
    return &symbols_[found->second - 1];
}

bool SemanticResult::isUpvalue(const ast::Node& node) const
{
    return upvalues_.count(node.id()) != 0;
}

SemanticResult SemanticAnalyzer::analyze(const ast::Node& block)
{
    Analyzer analyzer;
    return analyzer.run(block);
}

const char* symbolTypeName(SymbolType type)
{
    switch (type) {
    case SymbolType::Local: return "local";
    case SymbolType::Global: return "global";
    case SymbolType::Parameter: return "parameter";
    case SymbolType::LoopVariable: return "loop variable";
    case SymbolType::Label: return "label";
    }
    return "unknown";
}

} // namespace lua::semantic
