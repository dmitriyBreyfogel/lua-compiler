local collect = function(first, ...)
    return first, ...
end

local function outer(seed)
    return function(delta)
        return seed + delta
    end
end

return collect(outer(4)(5), "extra")
