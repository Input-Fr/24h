test=4
a=1
b=2
echo "$b$test$(echo toto; echo tata)test$test"
echo $test${a}${a}${a}$(echo toto; echo tata)test$test

echo "$test $(echo toto; echo tata)test $test $b$b$a${a}$(echo toto; echo tata)$a"
