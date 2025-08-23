#!/bin/bash

# =================== CLEANUP ===================
echo "Cleaning previous binaries and database directories..."
rm -f main_update
rm -rf Server_map1 Server_map2 Server_map3 Sigma_map1

# =================== COMPILATION ===================
echo "Compiling source files..."
g++ -o main_update main_update.cpp \
    ../../../FAST/Setup.cpp \
    ../../../FAST/Update.cpp \
    ../../../FAST/Search.cpp \
    ../src/Setup.cpp ../src/Update.cpp ../src/Search.cpp ../src/Utilities.cpp \
    -lcryptopp -lrocksdb


# =================== AUTOMATED INPUT ===================
echo "Running program with automated input..."

echo "Execution completed. Output saved to main.txt"
