#include "../src/MoRSE.h"

#define Size_ID 100000
#define Size_KS 10000

/* Range parameter: keyword space calW = {0, 1, 2, ... , 2^m - 1}. It is infact the size of the keyword Space
 * m = 17 if |calW| = 10^5, 
 * m = 14 if |calW| = 10^4,  
 * m = 10 if |calW| = 10^3, 
 * m = 7 if |calW| = 10^2,
 * m = 4 if |calW| = 10            
*/


#define m 14

void MoRSE_Update(DSSE &FAST_)
{
    cout << "============================= MoRSE: Update Begins =============================" << endl; 

    bool op = true;

    long double avg_uc = 0;
    long double avg_us = 0;
    

    // Use a random device to seed the random number engine
    std::random_device rd; 
    std::mt19937 gen(rd());                             // Mersenne Twister engine

    // Create a uniform distribution in the given range
    std::uniform_int_distribution<> distrib(0, 10000-1);


    for(int i = 0; i < Size_ID; i++)
    {
        string ind = to_string(i);
        int w = distrib(gen);

        vector<tuple<string, string>> u_tokens;
        
        auto u_c0 = chrono::high_resolution_clock::now();
        MoRSE_Update_client(FAST_, op, w, ind, m, u_tokens);
        auto u_c1 = chrono::high_resolution_clock::now();

        auto u_cd = chrono::duration_cast<chrono::microseconds>(u_c1 - u_c0).count();

        auto u_s0 = chrono::high_resolution_clock::now();
        MoRSE_Update_server(FAST_, u_tokens);
        auto u_s1 = chrono::high_resolution_clock::now();

        auto u_sd = chrono::duration_cast<chrono::microseconds>(u_s1 - u_s0).count();

        avg_uc = avg_uc + u_cd;
        avg_us = avg_us + u_sd; 
    }                                      

    long double avg_client_time = avg_uc / Size_ID;
    long double avg_server_time = avg_us / Size_ID;

    cout << "Average client-side update time (μs): " << avg_client_time << endl;
    cout << "Average server-side update time (μs): " << avg_server_time << endl;


    cout << "============================= MoRSE: Update Ends =============================" << endl; 
    
}


tuple<int, int> generate_range_query(int &range_size) 
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

    cout << "============================= MoRSE: Setup Begins =============================" << endl; 

    MoRSE_Setup(FAST_);
    
    cout << "============================= MoRSE: Setup Ends =============================" << endl; 

    


    MoRSE_Update(FAST_);

    tuple <int, int> range_query; 
    vector<int> Sizes_Q = {10, 100, 500, 1000, 2000};

    for (auto &ks: Sizes_Q)
    {
        long double avg_sc = 0;
        long double avg_ss = 0;

        double avg_stk = 0;

        cout << "============================= MoRSE(FAST): Search for query of size" << ks << "=============================" << endl;

        for (int i = 0; i < 100; i++)
        {
            range_query = generate_range_query(ks);
            
            cout << "q = [" << get<0>(range_query) << ", " << get<1>(range_query) << "]" << endl;

            
            int stk_num = 0;
            vector<tuple<string, string, int>> s_tokens;
            vector<string> search_result;

            auto s_c0 = chrono::high_resolution_clock::now();
            MoRSE_Search_client(FAST_, range_query, m, s_tokens);
            auto s_c1 = chrono::high_resolution_clock::now();
            auto s_cd = chrono::duration_cast<chrono::microseconds>(s_c1 - s_c0).count();

            stk_num = s_tokens.size();

            auto s_s0 = chrono::high_resolution_clock::now();
            MoRSE_Search_server(FAST_, s_tokens, search_result);
            auto s_s1 = chrono::high_resolution_clock::now();
            auto s_sd = chrono::duration_cast<chrono::microseconds>(s_s1 - s_s0).count();


            avg_stk = avg_stk + stk_num;

            avg_sc = avg_sc + s_cd;
            avg_ss = avg_ss + s_sd; 
        }

        long double avg_client_time = avg_sc / 100;
        long double avg_server_time = avg_ss / 100;

        cout << "Average number of search tokens: " << avg_stk / 100 << endl;
        
        cout << "Average client-side Search Time (μs): " << avg_client_time << endl;
        cout << "Average server-side Search Time (ms): " << avg_server_time << endl;

        
    }
    
    return 0;
}