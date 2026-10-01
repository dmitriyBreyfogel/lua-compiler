-- перед кодом / before code
global result

local value = -0xA + .5 -- после числа
result = [=[long value]=] .. "!"
data = { [1] = "one", key = false }

::start::
if value >= 0 then
    print(result, true, nil)
end
