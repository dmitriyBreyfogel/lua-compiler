#include "semantic/SemanticAnalyzer.h"
#include "semantic/Scope.h"

namespace lua::semantic {

using ast::Node;
using ast::NodeType;

class Analyzer {
public:
    SemanticResult run(const Node& block)
    {
        pushScope();
        visitBlock(block, false);
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

    void visitBlock(const Node& block, bool createScope)
    {
        if (createScope) {
            pushScope();
        }

        const Node* statements = block.child("statements");
        declareLabels(statements);
        visitList(statements);
        visitNode(block.child("return"));

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
                if (hasVisibleLabel(statement.value())) {
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
            const Node* names = statement.child("names");
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
            visitBlock(node, true);
            break;
        case NodeType::LocalDeclaration:
            visitDeclaration(node, SymbolType::Local, true);
            break;
        case NodeType::GlobalDeclaration:
            visitDeclaration(node, SymbolType::Global, false);
            break;
        case NodeType::GlobalAllDeclaration:
            visitGlobalAll(node);
            break;
        case NodeType::AssignmentStatement:
            visitAssignment(node);
            break;
        case NodeType::FunctionDeclaration:
            visitFunctionDeclaration(node);
            break;
        case NodeType::FunctionExpression:
            visitFunction(node.child("parameters"), node.child("body"), nullptr);
            break;
        case NodeType::NumericForStatement:
            visitNumericFor(node);
            break;
        case NodeType::GenericForStatement:
            visitGenericFor(node);
            break;
        case NodeType::WhileStatement:
            visitWhile(node);
            break;
        case NodeType::RepeatStatement:
            visitRepeat(node);
            break;
        case NodeType::IfStatement:
        case NodeType::ElseIfBranch:
            visitBranch(node);
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
            visitNode(node.child("object"));
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

    void visitDeclaration(const Node& declaration, SymbolType type, bool valuesBeforeNames)
    {
        if (type == SymbolType::Global) {
            visitList(declaration.child("values"));
            scopes_.back().setFreeGlobalPolicy(FreeGlobalPolicy::ExplicitOnly);
            declareNames(declaration.child("names"), type);
            return;
        }

        if (valuesBeforeNames) {
            visitList(declaration.child("values"));
            declareNames(declaration.child("names"), type);
            return;
        }

        declareNames(declaration.child("names"), type);
        visitList(declaration.child("values"));
    }

    void visitGlobalAll(const Node& declaration)
    {
        const Node* attribute = declaration.child("attribute");
        if (attribute == nullptr) {
            scopes_.back().setFreeGlobalPolicy(FreeGlobalPolicy::ExplicitWritable);
            return;
        }

        if (attribute->value() == "const") {
            scopes_.back().setFreeGlobalPolicy(FreeGlobalPolicy::ExplicitReadOnly);
        } else if (attribute->value() == "close") {
            error(*attribute, "close attribute is only allowed for local declarations");
            scopes_.back().setFreeGlobalPolicy(FreeGlobalPolicy::ExplicitOnly);
        } else {
            error(*attribute, "unknown local attribute: " + attribute->value());
            scopes_.back().setFreeGlobalPolicy(FreeGlobalPolicy::ExplicitOnly);
        }
    }

    void visitAssignment(const Node& assignment)
    {
        visitList(assignment.child("values"));
        visitAssignmentTargets(assignment.child("targets"));
    }

    void visitNumericFor(const Node& loop)
    {
        visitNode(loop.child("start"));
        visitNode(loop.child("finish"));
        visitNode(loop.child("step"));
        pushScope();
        declareNode(*loop.child("variable"), SymbolType::LoopVariable, true);
        ++loopDepth_;
        visitNode(loop.child("body"));
        --loopDepth_;
        scopes_.pop_back();
    }

    void visitGenericFor(const Node& loop)
    {
        visitList(loop.child("iterators"));
        pushScope();
        declareNames(loop.child("names"), SymbolType::LoopVariable);
        ++loopDepth_;
        visitNode(loop.child("body"));
        --loopDepth_;
        scopes_.pop_back();
    }

    void visitWhile(const Node& loop)
    {
        visitNode(loop.child("condition"));
        ++loopDepth_;
        visitNode(loop.child("body"));
        --loopDepth_;
    }

    void visitRepeat(const Node& loop)
    {
        ++loopDepth_;
        visitRepeatBody(*loop.child("body"));
        visitNode(loop.child("condition"));
        finishRepeatBody();
        --loopDepth_;
    }

    void visitBranch(const Node& branch)
    {
        if (branch.type() == NodeType::IfStatement) {
            visitNode(branch.child("condition"));
            visitNode(branch.child("then"));
            visitList(branch.child("elseif"));
            visitNode(branch.child("else"));
            return;
        }

        visitNode(branch.child("condition"));
        visitNode(branch.child("body"));
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
        const Node* statements = block.child("statements");
        declareLabels(statements);
        visitList(statements);
        visitNode(block.child("return"));
    }

    void finishRepeatBody()
    {
        scopes_.pop_back();
    }

    void visitFunctionDeclaration(const Node& node)
    {
        const Node* name = node.child("name");
        if (name != nullptr && name->type() == NodeType::Identifier) {
            const SymbolType type = node.value() == "local" ? SymbolType::Local : SymbolType::Global;
            if (node.value() == "global") {
                scopes_.back().setFreeGlobalPolicy(FreeGlobalPolicy::ExplicitOnly);
            }
            declareNode(*name, type, false);
        } else {
            visitNode(name);
        }
        visitFunction(node.child("parameters"), node.child("body"), name);
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
                        declareNode(*item.node, SymbolType::Parameter, false);
                    }
                } else {
                    declareNode(*item.node, SymbolType::Parameter, false);
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

        const Node* commonAttribute = nullptr;
        if (!names->children().empty()) {
            commonAttribute = names->children().front().node->child("before");
        }

        int closeCount = 0;
        for (const ast::Child& item : names->children()) {
            bool readOnly = type == SymbolType::LoopVariable;
            readOnly = validateAttribute(commonAttribute, type, closeCount) || readOnly;
            readOnly = validateAttribute(item.node->child("after"), type, closeCount) || readOnly;
            declareNode(*item.node, type, readOnly);
        }
    }

    bool validateAttribute(const Node* attribute, SymbolType type, int& closeCount)
    {
        if (attribute == nullptr) {
            return false;
        }
        if (attribute->value() == "const") {
            return true;
        }
        if (attribute->value() == "close") {
            if (type != SymbolType::Local) {
                error(*attribute, "close attribute is only allowed for local declarations");
            }
            if (++closeCount > 1) {
                error(*attribute, "only one close attribute is allowed in a declaration");
            }
            return true;
        }
        error(*attribute, "unknown local attribute: " + attribute->value());
        return false;
    }

    void declareNode(const Node& node, SymbolType type, bool readOnly)
    {
        const std::string& name = node.value();
        Scope& scope = scopes_.back();
        if ((type == SymbolType::Parameter || type == SymbolType::LoopVariable) && scope.hasName(name)) {
            error(node, "duplicate declaration: " + name);
            return;
        }
        const std::uint64_t id = declare(name, type, node.id(), readOnly);
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

        Scope& scope = scopes_.back();
        const FreeGlobalPolicy policy = scope.freeGlobalPolicy();
        if (policy == FreeGlobalPolicy::ExplicitOnly) {
            error(node, "unknown global: " + name);
            return 0;
        }

        const bool readOnly = policy == FreeGlobalPolicy::ExplicitReadOnly;
        const std::uint64_t id = declare(name, SymbolType::Global, node.id(), readOnly);
        scope.defineName(name, id);
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
        const FreeGlobalPolicy policy = scopes_.empty()
            ? FreeGlobalPolicy::ImplicitWritable
            : scopes_.back().freeGlobalPolicy();
        scopes_.emplace_back(functionDepth_, policy);
    }

    bool hasVisibleLabel(const std::string& name) const
    {
        for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
            if (scope->functionDepth() < functionDepth_) {
                break;
            }
            if (scope->hasLabel(name)) {
                return true;
            }
        }
        return false;
    }

    SemanticResult result_;
    std::vector<Scope> scopes_;
    int loopDepth_ = 0;
    int functionDepth_ = 0;
    int varargDepth_ = 0;
};

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
