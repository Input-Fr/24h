export a=4 a=5 b=6
echo $a $b

printenv a
printenv b

export -p a=9
echo $a
printenv a
printenv b

c=14
echo $c
export c=12
echo $c

printenv a
printenv b
printenv c
