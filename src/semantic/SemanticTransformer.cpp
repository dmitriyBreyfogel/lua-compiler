#include "semantic/SemanticTransformer.h"

namespace lua::semantic {

namespace {

bool hasChild(const ast::Node& node, const char* role)
{
    for (const ast::Child& child : node.children()) {
        if (child.role == role) {
            return true;
        }
    }
    return false;
}

void normalizeNode(ast::Node& node)
{
    if (node.type() == ast::NodeType::NumericForStatement && !hasChild(node, "step")) {
        node.addChild("step", ast::makeNode(ast::NodeType::IntegerLiteral, "1"));
    }

    for (const ast::Child& child : node.children()) {
        normalizeNode(*child.node);
    }
}

}

void SemanticTransformer::normalize(ast::Node& block)
{
    normalizeNode(block);
}

} // namespace lua::semantic
