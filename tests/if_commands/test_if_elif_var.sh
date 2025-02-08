input="hello"

if [ "$input" = "hello" ]; then
    echo "The input is hello."
elif [ "$input" = "world" ]; then
    echo "The input is world."
else
    echo "The input is something else."
fi
