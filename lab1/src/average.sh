sum=0

for n in $@
do
    sum=$((sum+n))
done

echo "input numbers count: $#"
echo "avg.: $((sum/$#))"