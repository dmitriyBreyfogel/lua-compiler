#include "semantic/SemanticDotWriter.h"

#include <ostream>

namespace lua::semantic {

namespace {

std::string escapeDot(const std::string& value)
{
    std::string escaped;
    for (char character : value) {
        if (character == '\\') {
            escaped += "\\\\";
        } else if (character == '"') {
            escaped += "\\\"";
        } else if (character == '\n') {
            escaped += "\\n";
        } else {
            escaped += character;
        }
    }
    return escaped;
}

void writeNode(std::ostream& output, const ast::Node& node, const SemanticResult& result)
{
    output << "  node_" << node.id() << " [label=\"" << ast::nodeTypeName(node.type());
    if (!node.value().empty()) {
        output << "\\n" << escapeDot(node.value());
    }
    if (const Symbol* symbol = result.bindingFor(node)) {
        output << "\\n" << symbolTypeName(symbol->type) << ": " << escapeDot(symbol->name);
        if (result.isUpvalue(node)) {
            output << " (upvalue)";
        }
    }
    output << "\"];\n";

    for (const ast::Child& child : node.children()) {
        output << "  node_" << node.id() << " -> node_" << child.node->id()
               << " [label=\"" << escapeDot(child.role) << "\"];\n";
        writeNode(output, *child.node, result);
    }

    if (const Symbol* symbol = result.bindingFor(node)) {
        output << "  node_" << node.id() << " -> symbol_" << symbol->id
               << " [style=dashed,color=gray];\n";
    }
}

}

void SemanticDotWriter::writeTree(std::ostream& output, const ast::Node& block, const SemanticResult& result)
{
    output << "digraph \"semantic\" {\n"
           << "  rankdir=TB;\n"
           << "  node [shape=box];\n";
    writeNode(output, block, result);

    for (const Symbol& symbol : result.symbols()) {
        output << "  symbol_" << symbol.id << " [shape=ellipse,style=dashed,label=\""
               << symbolTypeName(symbol.type) << ": " << escapeDot(symbol.name) << "\"];\n";
    }
    output << "}\n";
}

} // namespace lua::semantic
