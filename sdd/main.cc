#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <assert.h>
#include <chrono>
#include "sdd.h"
using namespace std;
using namespace sdd;


Sdd cnf_to_sdd(const char* file_name, string vtree_file_name)
{
    ifstream infile(file_name, ios::in);  // 打开要写入的文本文件
    if(!infile)
    {
        cerr << "open cnf_file error!" << endl;
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

    // VTree* vtree = new VTree( VTree::construct_right_linear_vtree(var_no)); //初始化vtree
    // assert(vtree!=NULL);
    // cout << "vtree not NULL"<<endl;
    // cerr << "creating vtree (right-linear)..." << endl;

    // if (vtree_file_name != "") {
        VTree* vtree = new VTree(VTree::import_from_sdd_vtree_file(vtree_file_name));
        cerr << "loading vtree..." << endl;
        if(vtree!=NULL) cout << "vtree not NULL"<<endl;
    // } else {
    //     VTree* vtree = new VTree( VTree::construct_right_linear_vtree(var_no));
    //     cerr << "creating vtree (right-linear)..." << endl;
    // }


    SddManager* mgr = new SddManager(*vtree,  1U<<16);
        assert(mgr!=NULL);

    cerr << "compiling..." << endl;
    auto compile_start = chrono::system_clock::now(); //开始转换
    cout << "test"<<endl;
    //Sdd result = NULL;

    cout << "mgr not null"<<endl;
    //mgr->make_sdd_true();
    Sdd result = mgr->make_sdd_true();//初始化为true
    mgr->inc_sddnode_refcount_at(result.addr());

    for(int line = 0;line < col_no;line++)
    {
        Sdd clause = mgr->make_sdd_false();//初始化一个clause为false
        while(true)
        {
            int literal;
            infile >> literal;
            if(literal == 0) break;

            cout << "literal:  "<<literal<<endl;
            Sdd new_literal = mgr->make_sdd_literal(literal);
            //cout << "new_literal_addr:"<<new_literal.addr()<<"  size "<<mgr->size(new_literal.addr())<<endl;
            mgr->inc_sddnode_refcount_at(new_literal.addr());
            Sdd new_clause = mgr->sdd_Or(clause,new_literal);
            //cout << "new_clause_addr:"<<new_clause.addr()<<"  size: "<<mgr->size(new_clause.addr())<<endl;
            //cout << "new_clause:"<<new_clause.size()<<endl;
            mgr->inc_sddnode_refcount_at(new_clause.addr());
            mgr->dec_sddnode_refcount_at(clause.addr());
            clause = new_clause;
            
        }

        cout << "clause_addr:"<<clause.addr()<<"  size: "<<mgr->size(clause.addr())<<endl;
        //cout << clause.size()<<endl;
        Sdd intFunc = mgr->sdd_And(result,clause);
        mgr->inc_sddnode_refcount_at(intFunc.addr());
        mgr->dec_sddnode_refcount_at(result.addr());
        result = intFunc;
    }

    auto compile_end = chrono::system_clock::now();
    cerr << "compilation time: "
    << chrono::duration_cast<chrono::milliseconds>(compile_end - compile_start).count() //转换结束
    << " msec" << endl;

    cerr << "sdd node count: " << mgr->size(result.addr()) << endl;
    cerr << "sdd model count: " << result.count_solution() << endl;

    infile.close();
    return result;
}


int main(int argc, char** argv)
{
     const char* cnf_input_file_name = "big-swap.cnf";
     string vtree_file_name = "big-swap.vtree";
    //const char* cnf_input_file_name = "s208.1.scan.cnf";
    //string vtree_file_name = "s208.1.scan.vtree";
    VTree* vtree = nullptr;

    Sdd sdd = cnf_to_sdd(cnf_input_file_name,vtree_file_name);
    cout << "test ok!"<<endl;

    
    return 0;
}
