#!/bin/sh
#!/bin/bash

a=0

b=0

run_test()
{
    #if [ "$COVERAGE" == "yes" ]; then
    #    echo yes
    #fi

    local args=$1

    bash --posix -c "$args" > tests/ex.txt 2>/dev/null
    local bash_status=$?

    "$BIN_PATH" -c "$args" > tests/get.txt 2>/dev/null
    local bin_path_status=$?

    if diff -q tests/ex.txt tests/get.txt > /dev/null; then
        a=$((a+1))
        if [ $bash_status -eq $bin_path_status ]; then
            echo "$args : passed"
        else
            echo "exit status :"
            echo "expected : $bash_status, get : $bin_path_status."
        fi
        echo "OK"
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
        echo "$file : passed"
        a=$((a+1))
    else
        echo "'$file': error"
        if ! diff -q tests/ex.txt tests/get.txt > /dev/null ; then
            echo "get :"
            cat tests/get.txt
            echo "expected :"
            cat tests/ex.txt
            echo ""
        fi

        if [ ! $bash_status -eq $bin_path_status ] ; then
            echo "exit status :"
            echo "expected : $bash_status, get : $bin_path_status."
        fi
    fi

    b=$((b+1))
}


run_test "echo hello"
run_test "echo hello;"
run_test "echo hello : "
# run_test "echo #hello"
run_test "echo if true; then echo world"
run_test "if true; then echo 'if'; fi"
run_test "if true; then echo oui; else echo oui; fi"
run_test "if false; then echo oui; else echo oui; fi"
run_test "if false; then echo 'if'; fi"
run_test "if true; then echo a///ad/fag/a//fd/a oui else echo oui; fi"
run_test "if false
true
then
echo a
echo b; echo c
fi"

run_file "script.sh"

if [ -f "tests/script.sh" ]; then
    run_file "tests/script.sh"
else
    echo "No file specified"
    exit 1
fi

for test_file in tests/tests_basic/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done

for test_file in tests/tests_var/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done
for test_file in tests/tests_facile/*; do
    if [ -f "$test_file" ]; then  # Ensure it's a file, not a directory
        run_file "$test_file"
    fi
done



b=$((b*10))
a=$((a*1000))

if [ "$COVERAGE" != "yes" ]; then
    echo "$a / $b" | bc > "$OUTPUT_FILE"
fi
