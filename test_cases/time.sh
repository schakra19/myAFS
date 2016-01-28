START=$(date +%s.%N)
ls
END=$(date +%s.%N)
DIFF=$(echo "$END - $START" | bc)
echo "Time taken is" $DIFF
 

