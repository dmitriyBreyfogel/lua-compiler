#include <cassert>

#include "semantic/Scope.h"

int main()
{
    lua::semantic::Scope scope(2, lua::semantic::FreeGlobalPolicy::ImplicitWritable);
    assert(scope.functionDepth() == 2);
    assert(scope.localCount() == 0);
    assert(scope.freeGlobalPolicy() == lua::semantic::FreeGlobalPolicy::ImplicitWritable);

    scope.setFreeGlobalPolicy(lua::semantic::FreeGlobalPolicy::ExplicitReadOnly);
    assert(scope.freeGlobalPolicy() == lua::semantic::FreeGlobalPolicy::ExplicitReadOnly);

    scope.defineName("value", 4);
    scope.addLocal();
    assert(scope.hasName("value"));
    assert(scope.findName("value") == 4);
    assert(scope.localCount() == 1);

    scope.defineLabel("again", {7, scope.localCount()});
    const lua::semantic::LabelTarget* label = scope.findLabel("again");
    assert(label != nullptr);
    assert(label->symbolId == 7);
    assert(label->localCount == 1);
}
