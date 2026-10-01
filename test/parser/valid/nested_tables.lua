local tree = {
    root = {children = {{value = 1}, {value = 2}}},
    [1 + 1] = {false, nil, true}
}
return tree.root.children[1].value
