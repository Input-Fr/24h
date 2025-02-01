test=4
a=1
b=2
d=4
c=6
IFS=''
echo "$test $(echo toto; echo tata)test $test $b$b$a$d$(echo toto; echo tata)$c"
