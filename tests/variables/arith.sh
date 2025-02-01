test=4
echo "$test$(echo toto; echo tata)test$test"
echo $test$(echo toto; echo tata)test$test

echo "$test $(echo toto; echo tata)test $test $(echo toto; echo tata)"
