require "ifj21"

global main : function() : integer, string

function main() : integer, string
    local a : integer = 16 // 4
    local b : integer = 5 - 9
    local c : number = 12.5
    local d : string = "lalala"
    local e : string = "lelele"
    c = a + b / c - #(d..e..e) * 2

    return a, b, 12, 3, "hehe"
end

main()