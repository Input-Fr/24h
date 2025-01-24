#!/bin/sh

a=0
b=0

run_test()
{
    local args=$1

    bash --posix -c "$args" > tests/ex.txt # 2>/dev/null
    local bash_status=$?

    "$BIN_PATH" -c "$args" > tests/get.txt #2>/dev/null
    local bin_path_status=$?

    if diff -q tests/ex.txt tests/get.txt > /dev/null; then
        if [ $bash_status -eq $bin_path_status ]; then
            echo -e "\e[1m$args: \e[0m\e[1;32mpassed\e[0m"

        else
            echo "exit status :"
            echo "expected : $bash_status, get : $bin_path_status."
        fi
        a=$((a+1))
    else
        echo ""
        echo "Test avec '$args': ERREUR"
        echo "get :"
        cat tests/get.txt
        echo "expected :"
        cat tests/ex.txt
        echo ""

    fi
    b=$((b+1))
}


run_file()
{
    local file="$1"

    bash --posix "$file" > tests/ex.txt 2>/dev/null
    local bash_status=$?

    "$BIN_PATH" "$file" > tests/get.txt 2>/dev/null
    local bin_path_status=$?

    if diff -q tests/ex.txt tests/get.txt > /dev/null && [ $bash_status -eq $bin_path_status ] ; then
        echo -e "\e[1m$file: \e[0m\e[1;32mpassed\e[0m"

        a=$((a+1))
    else
        if ! diff -q tests/ex.txt tests/get.txt > /dev/null ; then
            echo -e "\e[1m$file: \e[0m\e[1;31merror\e[0m"
            cat $file 
            echo ""
            echo -e "\e[1;36m *** get: ***\e[0m"
            cat tests/get.txt -e
            echo ""
            echo -e "\e[1;36m *** expected: *** \e[0m"
            cat tests/ex.txt -e
            echo ""
        fi

        if [ ! $bash_status -eq $bin_path_status ] ; then
            echo -e "\e[1m$file : \e[0m\e[1;31mwrong exit status\e[0m"
            cat $file
            echo -e "\e[1;36mget:\e[0m : \e[1m$bin_path_status\e[0m"
            echo -e "\e[1;36mexpected:\e[0m : \e[1m$bash_status\e[0m"
            echo ""
        fi
    fi

    b=$((b+1))
}

echo -e "\e[1;33m ---- tests string ----\e[0m"
echo ""
run_test "echo hello"
run_test "echo hello;"
run_test "echo hello : "
run_test "echo #hello"
run_test "echo if true; then echo world"
run_test "if true; then echo 'if'; fi"
run_test "if true; then echo oui; else echo oui; fi"
run_test "if false; then echo oui; else echo oui; fi"
run_test "if false; then echo 'if'; fi"
#run_test "if true; then echo a///ad/fag/a//fd/a oui else echo oui; fi"

#echo ""
#echo ""
#echo -e "\e[1;33m ------ tests folder tests_facile ------\e[0m"
#echo ""
#echo ""
#for test_file in tests/tests_facile/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done
#
#echo ""
#echo ""
#echo -e "\e[1;33m ---- tests folder tests_basic ----\e[0m"
#echo ""
#echo ""

for test_file in tests/tests_basic/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done

#echo ""
#echo ""
#echo -e "\e[1;33m ------ tests folder while_until ------\e[0m"
#echo ""
#echo ""
#for test_file in tests/while_until/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done

echo ""
echo ""
echo -e "\e[1;33m ---- tests folder quotes ----\e[0m"
echo ""
echo ""

for test_file in tests/quotes/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done


#echo ""
#echo ""
#echo -e "\e[1;33m ---- tests folder for ----\e[0m"
#echo ""
#echo ""
#
#for test_file in tests/for/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done


echo ""
echo ""
echo -e "\e[1;33m ------ tests folder tests_variables ------\e[0m"
echo ""
echo ""
for test_file in tests/variables/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done

#echo ""
#echo ""
#echo -e "\e[1;33m ------ tests folder if_commands ------\e[0m"
#echo ""
#echo ""
#for test_file in tests/if_commands/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done

#echo ""
#echo ""
#echo -e "\e[1;33m ------ tests folder simple_commands ------\e[0m"
#echo ""
#echo ""
#for test_file in tests/simple_commands/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done

echo ""
echo ""
echo -e "\e[1;33m ------ tests folder builtins ------\e[0m"
echo ""
echo ""
for test_file in tests/builtins/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done


#echo ""
#echo ""
#echo -e "\e[1;33m ------ tests folder current_debug ------\e[0m"
#echo ""
#echo ""
#for test_file in tests/current_debug/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done

echo ""
echo ""
echo -e "\e[1;33m ------ tests folder current_debug ------\e[0m"
echo ""
echo ""
for test_file in tests/current_debug/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done
echo
echo -e "\e[1;33m------------------------------------\e[0m"

#echo ""
#echo ""
#echo -e "\e[1;33m ------ tests folder command_block ------\e[0m"
#echo ""
#echo ""
#for test_file in tests/command_block/*; do
#    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
#        run_file "$test_file"
#    fi
#done






nb=$a
b=$((b*10))
a=$((a*1000))

result=$(echo "$a / $b" | bc)
echo""
echo""
echo""
echo -e "\e[1;35m~~~~~~~~~~~~~ RESULT ~~~~~~~~~~~~\e[0m"
echo -e "\e[1;35m|                               |\e[0m"
echo -e "|\e[1;32m         $nb tests passed       \e[0m|"
echo -e "|\e[1;32m         $result% tests passed      \e[0m|"
echo -e "\e[1;35m|                               |\e[0m"
echo -e "\e[1;35m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\e[0m"


if [ "$COVERAGE" != "yes" ]; then
    echo "$a / $b" | bc > "$OUTPUT_FILE"
fi


