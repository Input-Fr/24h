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
    "$BIN_PATH" -c "$args" > tests/get.txt 2>/dev/null

    if diff -q tests/ex.txt tests/get.txt > /dev/null; then
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

run_test "echo hello"
run_test "echo hello;"
run_test "echo hello : "
run_test "echo #hello"
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


b=$((b*10))
a=$((a*1000))

if [ "$COVERAGE" != "yes" ]; then
    echo "$a / $b" | bc > "$OUTPUT_FILE"
fi

