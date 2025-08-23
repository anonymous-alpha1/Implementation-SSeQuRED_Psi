#pragma once
#ifndef RS_Square
#define RS_Square

#include <iostream>

#include <iomanip>

#include <vector>
#include <sstream>
#include <string>
#include <random>  
#include <bitset>
#include <tuple>  

#include <cryptopp/osrng.h>                                                         // For AutoSeededRandomPool
#include <cryptopp/sha.h>
#include <cryptopp/shake.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/secblock.h>

#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/iterator.h>                                                      // to iterate the db over the keys



using namespace std;
using namespace CryptoPP;

vector<string> GetBRCm(tuple<int, int>& range_query, const int& height);
int histUptCnt(rocksdb::DB* map, const int& m, const string& tau);

// void displayAllPairs(rocksdb::DB* map, ofstream& f);


int Retrive_tupple_DB(rocksdb::DB* map, const string& keyword, tuple<string, int, int>& tuple_data);               // Function to retrieve the stored value (st, c) in a Map corresponding to keyword
void Store_tupple_DB(rocksdb::DB* map, const string& keyword, const tuple<string, int, int>& tuple_data);

void Store_tupple_T_c_DB(rocksdb::DB* map, const string& keyword, const vector<string>& RSet);
int Retrive_tupple_T_c_DB(rocksdb::DB* map, const string& keyword, vector<string>& RSet);                          // Function to retrieve the stored value (st, c) in a Map corresponding to keyword

string xorStrings(const string& str1, const string& str2);
//string toHex(const string& input);                                                                               // to be removed


string generate128BitString();
CryptoPP::SecByteBlock generateRandom128BitKey();                                                                  // Function to generate a random key of length 128 bit

string sha256(const std::string& input);
string hashSHAKE(const std::string& input, size_t outputLength);

void encryptAES(const SecByteBlock& key, const string& plaintext, string& ciphertext);
void decryptAES(const SecByteBlock& key, const string& ciphertext, string& plaintext);


class DSSE
{
private:

    SecByteBlock secret_key1;
    SecByteBlock secret_key2;

public:
    struct SetupResult
    {
        SecByteBlock client_sk1;
        SecByteBlock client_sk2;
        rocksdb::DB* map1_W;
        rocksdb::DB* map2_T_e;
        rocksdb::DB* map3_T_c;
    };

    SetupResult Data;                                                                       // A variable within the class which can be accessed/modified by any class function in their definition

    // Constructor function
    DSSE()
    {
        cout << "########### DSSE RS^2 Begins #####################" << endl;
        secret_key1 = generateRandom128BitKey();                                            // Initialising the secret key 
        secret_key2 = generateRandom128BitKey();
    }
    //Destructor function
    ~DSSE()
    {
        cout << "########### DSSE RS^2 Ends #####################" << endl;
    }

    tuple<SecByteBlock, SecByteBlock> Get_Client_sk()                                        // If client can access the secret_key using this function
    {
        return make_tuple(secret_key1, secret_key2);
    }

    void Setup();                                                                            // Declaration of function for Setup Algorithm

    void Update_client(const int& m, const bool& op, const string& ind, const int& keyword, vector<tuple<string, string>>& t_u);
    void Update_server(vector<tuple<string, string>>& t_u);

    void Search_client(const int& m, tuple<int, int>& range_query, vector<tuple<string, string, int>>& t_q);
    void Search_server(vector<tuple<string, string, int>>& t_q, vector<string>& R);

};

#endif