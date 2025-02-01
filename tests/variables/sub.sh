IFS=''
test=4
echo "$test$(echo toto; echo tata)test$test"
echo $(echo a; echo b)$test
