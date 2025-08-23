#Remove the compiled binaries

rm -f main_update

# Clean the RocksDB database instance
rm -rf map1_W map2_T_e map3_T_c

# Remove RocksDB database directory if it exists

# Added -lrocksdb to link the RocksDB library
g++ -o main_update main_update.cpp \
                    ../src/BRC.cpp \
                    ../src/Utilities.cpp \
                    ../src/Setup.cpp \
                    ../src/Update.cpp \
                    ../src/Search.cpp \
                    -lcryptopp -lrocksdb 

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

echo "Execution completed"