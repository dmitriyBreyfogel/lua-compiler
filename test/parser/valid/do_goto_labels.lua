local value = 0

::again::
do
    value = value + 1
end

if value < 2 then
    goto again
end
