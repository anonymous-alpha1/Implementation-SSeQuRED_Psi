#include "RS_Square.h"

// Binary-safe serialization
void serialize_tuple(const tuple<string, int, int> &t, string &value) 
{
    ostringstream oss(std::ios::binary);

    // Get elements
    const string& str = get<0>(t);
    int x = get<1>(t);
    int y = get<2>(t);

    // Write string length and data
    size_t len = str.size();
    oss.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
    oss.write(str.data(), len);

    // Write integers
    oss.write(reinterpret_cast<const char*>(&x), sizeof(int));
    oss.write(reinterpret_cast<const char*>(&y), sizeof(int));

    value = oss.str();
}



// Deserialization
void deserialize_tuple(const std::string& s, tuple<string, int, int> &t) 
{
    std::istringstream iss(s, std::ios::binary);

    string str;
    int x, y;

    // Read string
    size_t len;
    iss.read(reinterpret_cast<char*>(&len), sizeof(size_t));
    str.resize(len);
    iss.read(&str[0], len);

    // Read integers
    iss.read(reinterpret_cast<char*>(&x), sizeof(int));
    iss.read(reinterpret_cast<char*>(&y), sizeof(int));

    t = make_tuple(str, x, y);
}

void Store_tupple_DB(rocksdb::DB *map, const string &keyword, const tuple<string, int, int> &tuple_data)
{
    if(!map)
    {
        cerr << "Error: Database is not initialised !" << endl;
    }

    // Serilising k_tau, c_h, c_u into string value

    string value;
    serialize_tuple(tuple_data, value);

    // Store in RocksDB
    rocksdb::Status status = map->Put(rocksdb::WriteOptions(), keyword, value);
    if (!status.ok()) 
    {
        std::cerr << "Error storing tuple in RocksDB: " << status.ToString() << std::endl;
    }

}

int Retrive_tupple_DB(rocksdb::DB *map, const string &keyword, tuple<string, int, int> &tuple_data)               // Function to retrieve the stored value (st, c) in a Map corresponding to keyword
{

    if(!map)
    {
        cerr << "Error: Database is not initialised !" << endl;
    }

    // Retriving the tupple
    string value;
    rocksdb::Status status  = map->Get(rocksdb::ReadOptions(), keyword, &value);
    if(!status.ok())
    {
        //cout << "Keyword: " << keyword << " & Value:" << status.ToString() << endl;
        return 0;                                                                               // 0: There is no value stored corresponding to Keyword
    }
    else     
    { 
        // Deserilising value into k_tau, c_h, c_u

        deserialize_tuple(value, tuple_data);
        return 1;                                                                      
    }
}

void serialize_tuple_T_c(const std::vector<std::string> &RSet, std::string &value) 
{
    std::ostringstream oss(std::ios::binary);

    // Write vector size
    size_t vec_size = RSet.size();
    oss.write(reinterpret_cast<const char*>(&vec_size), sizeof(size_t));

    // Write each string with its length
    for (const auto &str : RSet) {
        size_t len = str.size();
        oss.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
        oss.write(str.data(), len);
    }

    value = oss.str();
}

void deserialize_tuple_T_c(const std::string &s, std::vector<std::string> &RSet)
{
    std::istringstream iss(s, std::ios::binary);
    size_t vec_size;
    iss.read(reinterpret_cast<char*>(&vec_size), sizeof(size_t));

    RSet.clear();
    for (size_t i = 0; i < vec_size; ++i) {
        size_t len;
        std::string str;

        iss.read(reinterpret_cast<char*>(&len), sizeof(size_t));
        str.resize(len);
        iss.read(&str[0], len);

        RSet.push_back(str);
    }
}


void Store_tupple_T_c_DB(rocksdb::DB *map, const string &keyword, const vector<string> &RSet)
{
    if(!map)
    {
        cerr << "Error: Database is not initialised !" << endl;
    }

    // Serilising RSet values into a string value

    string value;
    serialize_tuple_T_c(RSet, value);

    // Store in RocksDB
    rocksdb::Status status = map->Put(rocksdb::WriteOptions(), keyword, value);
    if (!status.ok()) 
    {
        std::cerr << "Error storing tuple in RocksDB: " << status.ToString() << std::endl;
    }

}

int Retrive_tupple_T_c_DB(rocksdb::DB *map, const string &keyword, vector<string> &RSet)               // Function to retrieve the stored value (st, c) in a Map corresponding to keyword
{
    if(!map)
    {
        cerr << "Error: Database is not initialised !" << endl;
        exit(1);
    }

    // Retriving the RSet values
    string value;
    rocksdb::Status status  = map->Get(rocksdb::ReadOptions(), keyword, &value);
    if(!status.ok())
    {
        //cout << "Keyword: " << keyword << " & Value:" << status.ToString() << endl;
        return 0;                                                                               // 0: There is no value stored corresponding to Keyword
    }
    else     
    { 
        // Deserilising values

        deserialize_tuple_T_c(value, RSet); 
        return 1;                                                                      
    }
}



void encryptAES(const SecByteBlock &key, const string &plaintext, string &ciphertext) 
{
    ciphertext = "";
    ECB_Mode<AES>::Encryption encryptor;
    encryptor.SetKey(key, key.size());
    StringSource(plaintext, true, new StreamTransformationFilter(encryptor, new StringSink(ciphertext)));
}

void decryptAES(const SecByteBlock &key, const string &ciphertext, string &plaintext) 
{
    plaintext = "";
    ECB_Mode<AES>::Decryption decryptor;
    decryptor.SetKey(key, key.size());
    StringSource(ciphertext, true, new StreamTransformationFilter(decryptor, new StringSink(plaintext)));
}

string generate128BitString() 
{
    std::random_device rd;                                                                 // Seed generator
    std::mt19937 gen(rd());                                                                // Mersenne Twister engine
    std::uniform_int_distribution<unsigned char> dis(0, 255);

    std::string result;
    for (int i = 0; i < 16; ++i) 
    {
        unsigned char random_char = dis(gen);
        result.push_back(static_cast<unsigned char>(random_char));                                  // Append raw byte to result
    }
    return result;
}



CryptoPP::SecByteBlock generateRandom128BitKey()                                       // Function to generate a random key of length 128 bit
{
    CryptoPP::AutoSeededRandomPool prng;                                               // Random number generator

    CryptoPP::SecByteBlock key(16);                                                    // 128-bit key (16 bytes)
    prng.GenerateBlock(key, key.size());                                               // Generate random bytes

    return key;                                                                        // Return raw bytes
}

std::string xorStrings(const std::string &str1, const std::string &str2) 
{
    if (str1.size() != str2.size()) 
    {
        throw std::invalid_argument("Strings must be of equal length for XOR operation.");
    }

    std::string result;
    result.reserve(str1.size());                                                            // Reserve space to avoid reallocations

    for (size_t i = 0; i < str1.size(); ++i) 
    {
        result += static_cast<char>(str1[i] ^ str2[i]);                                     // XOR corresponding characters
    }
    return result;
}

std::string sha256(const std::string &input)                                            // Function to compute SHA-256 hash and return 32 bytes as std::string
{
    SHA256 hash;
    std::string digest;                                                                 // Leave it uninitialized

    // Compute the hash
    StringSource ss(input, true, new HashFilter(hash, new StringSink(digest)));

    return digest;
}

string hashSHAKE(const std::string& input, size_t outputLength)                       // Function to compute SHAKE-128 hash of desired length (32 bytes) as string
{
    SHAKE128 shake;                                                                   // For SHAKE256, use SHAKE256 shake;
    string output;

    shake.Update(reinterpret_cast<const unsigned char*>(input.data()), input.size());
    output.resize(outputLength);
    shake.TruncatedFinal(reinterpret_cast<unsigned char*>(&output[0]), outputLength);

    return output;
}


int histUptCnt(rocksdb::DB *map, const int &m, const string &tau)
{
    int decision;
    tuple<string, int, int> map_value;

    // if tau is leaf node
    if(tau.length() == m+1)
    {   
        decision = Retrive_tupple_DB(map, tau, map_value);
        if(decision == 0)
        {
            return 0;
        }
        else
        {
            return get<1>(map_value);
        }
    }

    // Else, tau is internal node → get left/right children
    string left_child = tau + "0";
    string right_child = tau + "1";

    int ch_left = histUptCnt(map, m, left_child);
    int ch_right = histUptCnt(map, m, right_child);

    return ch_left + ch_right;
}

// string toHex(const string &input)
// {
//     ostringstream ss;
//     ss << std::hex << std::setfill('0');
//     for (unsigned char c : input)
//     {
//         ss << setw(2) << static_cast<int>(c) << "";
//     }
//     return ss.str();
// }

// void displayAllPairs(rocksdb::DB* map, ofstream &f) 
// {
//     if (map == nullptr) {
//         std::cerr << "Error: DB pointer is null." << std::endl;
//         return;
//     }

//     rocksdb::ReadOptions read_options;
//     std::unique_ptr<rocksdb::Iterator> it(map->NewIterator(read_options));

//     f << "Key-Value Pairs in RocksDB:\n";
//     for (it->SeekToFirst(); it->Valid(); it->Next()) {
//         f << toHex(it->key().ToString()) << " : " << toHex(it->value().ToString()) << std::endl;
//     }

//     if (!it->status().ok()) {
//         std::cerr << "Iterator error: " << it->status().ToString() << std::endl;
//     }
// }

