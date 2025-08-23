#include "../src/MoRSE.h"

#define Size_ID 10000
#define Size_KS 100000                                          // 10, 100, 1000, 10000, 20000, 50000, 100000


/* Range parameter: keyword space calW = {0, 1, 2, ... , 2^m - 1}. It is infact the size of the keyword Space
 * m = 17 if |calW| = 10^5, 
 * m = 14 if |calW| = 10^4,  
 * m = 10 if |calW| = 10^3, 
 * m = 7 if |calW| = 10^2,
 * m = 4 if |calW| = 10            
*/

#define m 17

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
    std::uniform_int_distribution<> distrib(0, Size_KS-1);


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


int main()
{
    DSSE FAST_;

    cout << "============================= MoRSE(FAST): Setup Begins =============================" << endl; 
    MoRSE_Setup(FAST_);
    cout << "============================= MoRSE(FAST): Setup Ends =============================" << endl; 

    MoRSE_Update(FAST_);
    
    return 0;
}