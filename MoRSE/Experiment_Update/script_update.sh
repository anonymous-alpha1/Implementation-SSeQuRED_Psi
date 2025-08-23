rm -f main_update


rm -rf Server_map1 Server_map2 Server_map3 Sigma_map1 main


g++ -o main_update main_update.cpp \
                    ../../../FAST/Setup.cpp \
                    ../../../FAST/Update.cpp \
                    ../../../FAST/Search.cpp \
                    ../src/Utilities.cpp \
                    ../src/BRC.cpp \
                    ../src/Setup.cpp \
                    ../src/Update.cpp \
                    ../src/Search.cpp \
                    -lcryptopp -lrocksdb

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

echo "Execution completed."