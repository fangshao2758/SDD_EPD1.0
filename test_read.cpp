#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<stdlib.h>
using namespace std;
int main(int argc, char const *argv[])
{
    ifstream infile("big-swap.cnf",ios::in);  // 打开要写入的文本文件
    if(!infile)
    {
        cerr << "open infile error!" << endl;
        exit(1);
    }
    string line;
    int var_no = 0, col_no = 0;  // Number of variables and number of clauses
    while (!infile.eof()) {
        getline(infile, line);
        if (line.length() == 0 || line[0] == 'c')
            ; //cout << "IGNORE LINE\n";
        else {
            var_no = stoi(line.substr(6, line.find_last_of(' ')-6));
            col_no = stoi(line.substr(line.find_last_of(' ')+1, line.length()-line.find_last_of(' ')));
            break;
        }
    }
    cout << var_no << endl << col_no;
   return 0;
}
