for index = 1, 10 do
    if index == 4 then
        break
    end
end

function choose(flag)
    if flag then return true end
    return false;
end

return choose(true)
