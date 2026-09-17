function process(...)
    for i = 1, 3 do
        if i ~= 2 then
            result = result // i
        else
            break
        end
    end

    return ...
end
