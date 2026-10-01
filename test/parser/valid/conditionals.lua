if score >= 90 then
    grade = "A"
elseif score >= 70 then
    grade = "B"
elseif score >= 50 then
    grade = "C"
else
    grade = "D"
end

if enabled then
    if verbose then print(grade) end
end
