# make load-parameters.h

rm src/load-parameters.h
cat CONFIG/load-parameters-start > src/load-parameters.h
PWD=$(pwd)
echo "static std::string path (\"$PWD/\");" >> src/load-parameters.h
cat CONFIG/global_config >> src/load-parameters.h

