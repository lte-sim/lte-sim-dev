file_in=output/test_mob
file_out_x=output/test_mob_POS_X
file_out_y=output/test_mob_POS_Y

echo "GREP on ${file_in}"

grep "UserPosition X" ${file_in} | awk '{print $5 " " $6 " " $7}' > ${file_out_x}
grep "UserPosition Y" ${file_in} | awk '{print $5 " " $6 " " $7}' > ${file_out_y}
              




 
 

  

