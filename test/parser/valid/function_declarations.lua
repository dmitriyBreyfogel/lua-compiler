function toolkit.math.add(left, right)
    return left + right
end

local function identity(value)
    return value
end

function record:update(value)
    self.value = value
end

return identity(toolkit.math.add(1, 2))
