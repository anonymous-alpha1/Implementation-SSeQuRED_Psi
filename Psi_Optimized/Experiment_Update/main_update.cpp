// We are implementing our proposed DSSE scheme supporting range query.
#include "../src/Lambda.h"

#include <bitset>
const int Size_ID = 10000;                                                

void ind_Depadding_FAST(std::string &ind)
{
    size_t pos = ind.find_last_not_of('_');
    if (pos != std::string::npos)
        ind.erase(pos + 1);  // Keep characters up to the last non-'_'
    else
        ind.clear();  // All characters were '_'
}


bitset<Size_ID> build_bitmap(vector<string> inds)
{
    bitset<Size_ID> bitmap;                                                     // All bits are initialized to 0

    for(auto id : inds)
    {
        ind_Depadding_FAST(id);
        int index  = stoi(id);
        bitmap.set(Size_ID - 1 - index);                                        // Set the corresponding bit to 1. Note: most-significant bit need to be set 1
    }
    return bitmap;
}


void Lambda_Setup(DSSE &FAST_)
{
    cout << "============================= PSI: Setup =============================" << endl; 
    
    Setup(FAST_);
    
}

void Lambda_Update(DSSE &FAST_, vector<int> &Keyword_Space, const int &Size_KS)
{
    cout << "============================= PSI: Update =============================" << endl; 
    
    long double avg_uc = 0;
    long double avg_us = 0;


    // Use a random device to seed the random number engine
    std::random_device rd; 
    std::mt19937 gen(rd());                             // Mersenne Twister engine

    // Create a uniform distribution in the given range
    std::uniform_int_distribution<> distrib(0, Size_KS-1);

    for(int i = 0; i < Size_ID; i++)
    {   
        string ind = "ID" + to_string(i);
        int w = distrib(gen);

        vector<tuple<string, string>> U_list;                                                       // Lambda Update Token List

        auto u_c0 = chrono::high_resolution_clock::now();
        Update_client(FAST_, true, ind, i, w, Keyword_Space, U_list);
        auto u_c1 = chrono::high_resolution_clock::now();
        long double u_cd = chrono::duration_cast<chrono::microseconds>(u_c1 - u_c0).count();
        

        auto u_s0 = chrono::high_resolution_clock::now();
        Update_server(FAST_, U_list);
        auto u_s1 = chrono::high_resolution_clock::now();
        long double u_sd = chrono::duration_cast<chrono::microseconds>(u_s1 - u_s0).count();

        avg_uc = avg_uc + u_cd;
        avg_us = avg_us + u_sd; 
    }

    long double avg_client_time = avg_uc / Size_ID;
    long double avg_server_time = avg_us / Size_ID;

    cout << "Average client-side update time (μs): " << avg_client_time << endl;
    cout << "Average server-side update time (μs): " << avg_server_time << endl;

}   





int main()
{
    DSSE FAST_;

    Lambda_Setup(FAST_);

    int Size_KS;
    vector<int> Keyword_Space;

    Size_KS = 100000;                                                                       //[10, 100, 1000, 10000, 20000, 50000, 100000]                                        

    //============================= PSI: Keyword Space =============================    
    for(int i = 0; i < Size_KS; i++)
    {
        Keyword_Space.push_back(i);
    }

    Lambda_Update(FAST_, Keyword_Space, Size_KS);
   
    return 0;
}
