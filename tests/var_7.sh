abc=4 echo test1${abc}

echo "${abc}"
abc=2; echo "test1     ${abc}" ;echo "${abc} test2"
abc=2;
echo "test1     ${abc} test2"
echo 'test1     ${abc} test2'
echo test$abc
abc=4;
echo $abc?
echo "$abc"
echo "test1     $abc"
echo "$abc test2"
abc=3;
echo "test1     $abc test2"
echo 'test1     $abc test2'
abc=3;
echo "${abc}"
abc=2;
echo "test1     ${abc}"
abc=1;
echo "${abc} test2"
echo "test1     ${abc} test2"
