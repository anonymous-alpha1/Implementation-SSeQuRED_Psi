#Remove the compiled binaries

rm -f main main.txt

# Clean the RocksDB database instance
rm -rf map1_W map2_T_e map3_T_c

# Remove RocksDB database directory if it exists

# Added -lrocksdb to link the RocksDB library
g++ -o main main.cpp \
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

# =================== AUTOMATED INPUT ===================
echo "Running program with automated input..."

# Inputs:
# 1   → Update
# y   → Continue
# 2   → Search
# 0   → a = 0
# 99999   → b = 99999
# n   → Exit
echo -e "1\ny\n2\n0\n99999\nn" | ./main > main.txt

echo "Execution completed. Output saved to main.txt"