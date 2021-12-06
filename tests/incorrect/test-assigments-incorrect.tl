require "ifj21"

global main : function(integer) : integer, string

function main(a : integer) : integer, string
    local b : integer = 5 - 9
    a = a + b

    return a, b, 12, 3, "hehe"
end
