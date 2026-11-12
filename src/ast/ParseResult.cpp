#include "ast/ParseResult.h"

namespace lua::ast {

namespace {

Node* parsedBlock = nullptr;

}

void setParsedBlock(Node* block)
{
    parsedBlock = block;
}

NodePtr takeParsedBlock()
{
    NodePtr block(parsedBlock);
    parsedBlock = nullptr;
    return block;
}

void clearParsedBlock()
{
    parsedBlock = nullptr;
}

} // namespace lua::ast
