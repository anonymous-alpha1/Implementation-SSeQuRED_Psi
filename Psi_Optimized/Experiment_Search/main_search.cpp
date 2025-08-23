// We are implementing our proposed DSSE scheme supporting range query.
#include "../src/Lambda.h"

#include <bitset>
const int Size_ID = 100000;
const int Size_KS = 10000;

void Lambda_Setup(DSSE &FAST_)
{
    cout << "============================= PSI: Setup =============================" << endl; 
    
    Setup(FAST_);
    
}

void Lambda_Update(DSSE &FAST_, vector<int> &Keyword_Space)
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
        long double u_cd = chrono::duration_cast<chrono::milliseconds>(u_c1 - u_c0).count();
        

        auto u_s0 = chrono::high_resolution_clock::now();
        Update_server(FAST_, U_list);
        auto u_s1 = chrono::high_resolution_clock::now();
        long double u_sd = chrono::duration_cast<chrono::milliseconds>(u_s1 - u_s0).count();

        avg_uc = avg_uc + u_cd;
        avg_us = avg_us + u_sd; 
    }

    long double avg_client_time = avg_uc / Size_ID;
    long double avg_server_time = avg_us / Size_ID;

    cout << "Average client-side update time (ms): " << avg_client_time << endl;
    cout << "Average server-side update time (ms): " << avg_server_time << endl;

}   


tuple<int, int> generate_range_query(const int &Size_KS, int &range_size) 
{

    if (range_size >= Size_KS) 
    {
        throw std::invalid_argument("Range size too large for given domain.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, Size_KS - range_size - 1);

    int a = dist(gen);
    int b = a + range_size;

    return make_tuple(a, b);
}


int main()
{
    DSSE FAST_;

    Lambda_Setup(FAST_);

    //============================= PSI: Keyword Space =============================    

    vector<int> Keyword_Space;
    for(int i = 0; i < Size_KS; i++)
    {
        Keyword_Space.push_back(i);
    }

    Lambda_Update(FAST_, Keyword_Space);

    tuple<int, int> range_query;
    vector<int> Sizes_KS = {10, 100, 500, 1000, 2000};
    for (auto &ks: Sizes_KS)
    {
        long double avg_sc = 0;
        long double avg_ss = 0;

        cout << "============================= PSI: Search for query of size" << ks << "=============================" << endl;

        for (int i = 0; i < 100; i++)
        {
            range_query = generate_range_query(Size_KS, ks);
            
            cout << "q = [" << get<0>(range_query) << ", " << get<1>(range_query) << "]" << endl;

            vector<string> res1_l, res1_e, res2_l, res2_e;
            tuple<string, string, int> stk1_l, stk1_e, stk2_l, stk_2_e;                                                   // Lambda's Search Tokens

            auto s_c0 = chrono::high_resolution_clock::now();
            Search_client(FAST_, range_query, Keyword_Space, stk1_l, stk1_e, stk2_l, stk_2_e);
            auto s_c1 = chrono::high_resolution_clock::now();
            auto s_cd = chrono::duration_cast<chrono::microseconds>(s_c1 - s_c0).count();


            auto s_s0 = chrono::high_resolution_clock::now();
            Search_server(FAST_, stk1_l, stk1_e, stk2_l, stk_2_e, res1_l, res1_e, res2_l, res2_e);
            auto s_s1 = chrono::high_resolution_clock::now();
            auto s_sd = chrono::duration_cast<chrono::milliseconds>(s_s1 - s_s0).count();

            avg_sc = avg_sc + s_cd;
            avg_ss = avg_ss + s_sd; 
        }

        long double avg_client_time = avg_sc / 100;
        long double avg_server_time = avg_ss / 100;

        cout << "Average client-side Search Time (μs): " << avg_client_time << endl;
        cout << "Average server-side Search Time (ms): " << avg_server_time << endl;
        
    }
    
    return 0;
}