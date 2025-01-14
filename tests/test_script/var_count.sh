base_var="count"
count1=5
count2=10
count3=15

for i in 1 2 3
do
  new_value=$(( ${!base_var$i} + 5 ))
  eval "$base_var$i=$new_value"
  echo "New value of count$i: ${!base_var$i}"
done
