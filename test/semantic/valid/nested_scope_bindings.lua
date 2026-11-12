local value <const> = 1

do
    local value = 2
    value = 3
end

global known
do
    known = value
end

goto done
do
    local skipped = 1
end

::done::
return known
