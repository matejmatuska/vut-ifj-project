
    require                              "ifj21"

function hi() : integer
    return 3
end

function main(a : integer) : integer, string
    local heh : integer = 5 + 7
    local bye : integer = 12 + 3

    a = hi()

    return heh, bye
end
