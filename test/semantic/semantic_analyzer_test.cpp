#include <cassert>

#include "ast/Ast.h"
#include "semantic/SemanticAnalyzer.h"
#include "semantic/SemanticTransformer.h"

namespace {

lua::ast::NodePtr list()
{
    return lua::ast::createNode(lua::ast::NodeType::List);
}

lua::ast::NodePtr block(lua::ast::NodePtr statements)
{
    auto result = lua::ast::createNode(lua::ast::NodeType::Block);
    result->addChild("statements", std::move(statements));
    return result;
}

}

int main()
{
    auto forLoop = lua::ast::createNode(lua::ast::NodeType::NumericForStatement);
    lua::semantic::SemanticTransformer transformer;
    transformer.normalize(*forLoop);
    assert(forLoop->children().size() == 1);
    assert(forLoop->children()[0].role == "step");
    assert(forLoop->children()[0].node->value() == "1");

    auto statements = list();

    auto declaration = lua::ast::createNode(lua::ast::NodeType::LocalDeclaration);
    auto names = list();
    auto declaredName = lua::ast::createNode(lua::ast::NodeType::DeclarationName, "value");
    names->addChild("item[0]", std::move(declaredName));
    declaration->addChild("names", std::move(names));
    auto values = list();
    auto initialValue = lua::ast::createNode(lua::ast::NodeType::Identifier, "value");
    const lua::ast::Node& initialValueReference = *initialValue;
    values->addChild("item[0]", std::move(initialValue));
    declaration->addChild("values", std::move(values));
    statements->addChild("item[0]", std::move(declaration));

    auto assignment = lua::ast::createNode(lua::ast::NodeType::AssignmentStatement);
    auto targets = list();
    auto target = lua::ast::createNode(lua::ast::NodeType::Identifier, "value");
    const lua::ast::Node& targetReference = *target;
    targets->addChild("item[0]", std::move(target));
    assignment->addChild("targets", std::move(targets));
    statements->addChild("item[1]", std::move(assignment));

    auto function = lua::ast::createNode(lua::ast::NodeType::FunctionExpression);
    function->addChild("parameters", list());
    auto functionStatements = list();
    auto capturedValue = lua::ast::createNode(lua::ast::NodeType::Identifier, "value");
    const lua::ast::Node& capturedReference = *capturedValue;
    functionStatements->addChild("item[0]", std::move(capturedValue));
    function->addChild("body", block(std::move(functionStatements)));
    statements->addChild("item[2]", std::move(function));

    auto invalidBreak = lua::ast::createNode(lua::ast::NodeType::BreakStatement);
    statements->addChild("item[3]", std::move(invalidBreak));

    auto program = block(std::move(statements));
    lua::semantic::SemanticAnalyzer analyzer;
    const lua::semantic::SemanticResult result = analyzer.analyze(*program);

    const lua::semantic::Symbol* targetSymbol = result.bindingFor(targetReference);
    assert(targetSymbol != nullptr);
    assert(targetSymbol->name == "value");
    assert(targetSymbol->type == lua::semantic::SymbolType::Local);
    const lua::semantic::Symbol* initialValueSymbol = result.bindingFor(initialValueReference);
    assert(initialValueSymbol != nullptr);
    assert(initialValueSymbol->type == lua::semantic::SymbolType::Global);
    assert(result.isUpvalue(capturedReference));
    assert(result.diagnostics().size() == 1);
    assert(result.diagnostics()[0].message == "break outside loop");

    auto globalNames = list();
    globalNames->addChild("item[0]", lua::ast::createNode(lua::ast::NodeType::DeclarationName, "known"));
    auto globalDeclaration = lua::ast::createNode(lua::ast::NodeType::GlobalDeclaration);
    globalDeclaration->addChild("names", std::move(globalNames));

    auto unknownReference = lua::ast::createNode(lua::ast::NodeType::Identifier, "unknown");
    const lua::ast::Node& unknownReferenceNode = *unknownReference;
    auto unknownTargets = list();
    unknownTargets->addChild("item[0]", std::move(unknownReference));
    auto unknownValues = list();
    unknownValues->addChild("item[0]", lua::ast::createNode(lua::ast::NodeType::IntegerLiteral, "1"));
    auto unknownAssignment = lua::ast::createNode(lua::ast::NodeType::AssignmentStatement);
    unknownAssignment->addChild("targets", std::move(unknownTargets));
    unknownAssignment->addChild("values", std::move(unknownValues));

    auto unknownStatements = list();
    unknownStatements->addChild("item[0]", std::move(globalDeclaration));
    unknownStatements->addChild("item[1]", std::move(unknownAssignment));
    auto unknownProgram = block(std::move(unknownStatements));
    const lua::semantic::SemanticResult unknownResult = analyzer.analyze(*unknownProgram);

    assert(unknownResult.bindingFor(unknownReferenceNode) == nullptr);
    assert(unknownResult.diagnostics().size() == 1);
    assert(unknownResult.diagnostics()[0].message == "unknown global: unknown");
}
