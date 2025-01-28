a=5
b=6

echo "$((a + b))"
echo "$(( $b + 6 * a ))"
echo "$(( 1 + b * $a ))"
echo "$(( 1 + c * ${a} ))"
echo "$(( 1 + 6 * "${a}" ))"
