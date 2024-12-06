#include <bits/stdc++.h>

std::vector<int> encoding(std::string s1)
{
    std::cout << "Encoding\n";
    std::unordered_map<std::string, int> table;
    for (int i = 0; i <= 255; i++)
    {
        std::string ch = "";
        ch += char(i);
        table[ch] = i;
    }

    std::string p = "", c = "";
    p += s1[0];
    int code = 256;
    std::vector<int> output_code;
    std::cout << "String\tOutput_Code\tAddition\n";
    for (size_t i = 0; i < s1.length(); i++)
    {
        if (i != s1.length() - 1)
            c += s1[i + 1];
        if (table.find(p + c) != table.end())
        {
            p = p + c;
        }
        else
        {
            std::cout << p << "\t" << table[p] << "\t\t"
                      << p + c << "\t" << code << '\n';
            output_code.push_back(table[p]);
            table[p + c] = code;
            code++;
            p = c;
        }
        c = "";
    }
    std::cout << p << "\t" << table[p] << '\n';
    output_code.push_back(table[p]);
    return output_code;
}

void decoding(std::vector<int> op)
{
    std::cout << "\nDecoding\n";
    std::unordered_map<int, std::string> table;
    for (int i = 0; i <= 255; i++)
    {
        std::string ch = "";
        ch += char(i);
        table[i] = ch;
    }
    int old = op[0], n;
    std::string s = table[old];
    std::string c = "";
    c += s[0];
    std::cout << s;
    int count = 256;
    for (size_t i = 0; i < op.size() - 1; i++)
    {
        n = op[i + 1];
        if (table.find(n) == table.end())
        {
            s = table[old];
            s = s + c;
        }
        else
        {
            s = table[n];
        }
        std::cout << s;
        c = "";
        c += s[0];
        table[count] = table[old] + c;
        count++;
        old = n;
    }
}
int main()
{

    std::string s = "WYS*WYGWYS*WYSWYSG";
    std::vector<int> output_code = encoding(s);
    std::cout << "Output Codes are: ";
    for (size_t i = 0; i < output_code.size(); i++)
    {
        std::cout << output_code[i] << " ";
    }
    std::cout << '\n';
    decoding(output_code);
    std::cout << '\n';
}
