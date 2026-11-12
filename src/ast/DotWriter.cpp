#include "ast/DotWriter.h"

#include <ostream>

namespace lua::ast {

namespace {

std::string escapeDot(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size());

    for (const char character : value) {
        switch (character) {
        case '\\': escaped += "\\\\"; break;
        case '"': escaped += "\\\""; break;
        case '\n': escaped += "\\n"; break;
        default: escaped += character; break;
        }
    }

    return escaped;
}

void writeNode(std::ostream& output, const Node& node)
{
    output << "  node_" << node.id() << " [label=\"" << nodeKindName(node.kind());
    if (!node.value().empty()) {
        output << "\\n" << escapeDot(node.value());
    }
    output << "\"];\n";

    for (const Child& child : node.children()) {
        output << "  node_" << node.id() << " -> node_" << child.node->id()
               << " [label=\"" << escapeDot(child.role) << "\"];\n";
        writeNode(output, *child.node);
    }
}

}

void DotWriter::writeExpression(std::ostream& output, const Node& root, const std::string& graphName)
{
    output << "digraph \"" << escapeDot(graphName) << "\" {\n"
           << "  rankdir=TB;\n"
           << "  node [shape=box];\n";
    writeNode(output, root);
    output << "}\n";
}

} // namespace lua::ast
