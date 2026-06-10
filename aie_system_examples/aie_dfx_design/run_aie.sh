./double_lab8 task1.xclbin task2.xclbin
return_code=$?
if [ $return_code -ne 0 ]; then
echo "ERROR: host run failed, RC=$return_code"
fi
echo "INFO: host run completed."