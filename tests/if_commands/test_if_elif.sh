#!/bin/bash --posix

input="hello"

if [ "$input" = "hella" ]; then
    echo "The input is hello."
elif [ "$input" = "world" ]; then
    echo "The input is world."
elif [ "$input" = "toto" ]; then
    echo "The input is world."
elif [ "$input" = "tata" ]; then
    echo "The input is world."
elif [ "$input" = "hello" ]; then
    echo "The input is hello"
else
    echo "The input is something else."
fi

