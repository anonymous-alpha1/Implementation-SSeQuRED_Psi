#include "RS_Square.h"

void DSSE::Setup()
{

    // Step 1: Get the 128-bit random secret keys using the DSSE public function Get_Client_sk()
   
    tuple<SecByteBlock, SecByteBlock> keys = Get_Client_sk();
    this->Data.client_sk1 = get<0>(keys);
    this->Data.client_sk2 = get<1>(keys);

    // Step 2: Initialize three RocksDB database instances (map1, map2, map3)
    rocksdb::Options options;
    options.create_if_missing = true;

    rocksdb::DB* db1;
    rocksdb::Status status1 = rocksdb::DB::Open(options, "map1_W", &db1);
    if (!status1.ok()) 
    {
        std::cerr << "Error opening RocksDB for map1: " << status1.ToString() << std::endl;
        exit(1);
    }

    rocksdb::DB* db2;
    rocksdb::Status status2 = rocksdb::DB::Open(options, "map2_T_e", &db2);
    if (!status2.ok()) {
        std::cerr << "Error opening RocksDB for map2: " << status2.ToString() << std::endl;
        exit(1);
    }
    rocksdb::DB* db3;
    rocksdb::Status status3 = rocksdb::DB::Open(options, "map3_T_c", &db3);
    if (!status3.ok()) {
        std::cerr << "Error opening RocksDB for map3: " << status3.ToString() << std::endl;
        exit(1);
    }
    this->Data.map1_W = db1;
    this->Data.map2_T_e = db2;
    this->Data.map3_T_c = db3;

};