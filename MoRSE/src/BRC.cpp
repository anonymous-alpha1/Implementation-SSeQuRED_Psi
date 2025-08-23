#include "MoRSE.h"

vector<string> GetBRCm(const tuple<int, int> &range_query, const int &height) 
{
    vector<string> BRC;
    int i = 0;
    int a = get<0>(range_query);
    int b = get<1>(range_query);

    constexpr int max_bits = 32; // ensure this is large enough
    int m  = height;
    

    while (true) 
    {

            // Convert to (m+1)-bit binary strings
        bitset<max_bits> bin_a(a);
        bitset<max_bits> bin_b(b);
        string str_a = bin_a.to_string().substr(max_bits - (m + 1), m + 1);
        string str_b = bin_b.to_string().substr(max_bits - (m + 1), m + 1);

        string prefix_a = str_a.substr(0, m - i + 1);
        string prefix_b = str_b.substr(0, m - i + 1);

        if (prefix_a < prefix_b) 
        {
            // If a's current bit is 1, include a's prefix
            if (str_a[m - i] == '1') 
            {
                BRC.push_back(prefix_a);
            }
            // If b's current bit is 0, include b's prefix
            if (str_b[m - i] == '0') 
            {
                BRC.push_back(prefix_b);
            }

            // Increment and decrement a and b accordingly
            a += (1 << i);
            b -= (1 << i);
            i++;
        }
        else 
        {
            // If the remaining prefixes match, include the common prefix
            if (prefix_a == prefix_b) 
            {
                BRC.push_back(prefix_a);
            }
            break;
        }
    }
    // cout << "Printing BRC" << endl;
    // for (const auto& code : BRC)
    // {
    //     cout << code << endl;
    // }

    return BRC;
}
