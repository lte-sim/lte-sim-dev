file="mob_sinrplot"
echo "postSim for ${file}"
grep "UserPosition X" output/${file}_manh | awk '{print $5}' > output/${file}_manh_X
grep "UserPosition Y" output/${file}_manh | awk '{print $5}' > output/${file}_manh_Y
grep "sinr:" output/${file}_manh | awk {'print $2'} > output/${file}_manh_sinr
grep "Created Cell" output/${file}_manh | awk {'print $6 " " $7'} > output/${file}_manh_enbs
grep " ** 	 time:" output/${file}_manh | awk {'print $5'} > output/${file}_manh_hos

grep "UserPosition X" output/${file}_randdir | awk '{print $5}' > output/${file}_randdir_X
grep "UserPosition Y" output/${file}_randdir | awk '{print $5}' > output/${file}_randdir_Y
grep "sinr:" output/${file}_randdir | awk {'print $2'} > output/${file}_randdir_sinr
grep " ** 	 time:" output/${file}_randdir | awk {'print $5'} > output/${file}_randdir_hos
