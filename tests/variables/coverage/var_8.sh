abc=4 echo test1${abc}

echo "${abc}"
abc=2; echo "test1     ${abc}" ;echo "${abc} test2"
abc=2;
echo "test1     ${abc} test2"
echo 'test1     ${abc} test2'
echo test$abc
