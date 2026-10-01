local accepted = not false and 2 <= 3 or 4 ~= 5
local bits = (~8 & 15) | (1 << 3) ~ (16 >> 2)
local text = "left" .. "-" .. "right"
return accepted, bits, text
