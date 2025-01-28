a=5
b=6

a=$((a + b))
echo $a
echo "$(( $b + 6 * a ))"
a="$(( 1 + b * $a ))"
echo "$(( 1 + c * ${a} ))"
echo "$(( 1 + 6 * "${a}" ))"
