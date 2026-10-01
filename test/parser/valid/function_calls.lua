print()
print(1, "message")
logger:write("ready")
emit {kind = "event"}
emit "done"
factory()()
local first = factory().items[1]
return first
