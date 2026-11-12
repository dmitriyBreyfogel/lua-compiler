local function factorial(value)
    if value == 0 then
        return 1
    end

    return value * factorial(value - 1)
end

return factorial(3)
