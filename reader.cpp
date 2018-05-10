/*
 * Function: read a CNF file, return a SFDD
 * Input(parameter): pro_file - name of CNF file
 * Output: fml - object SFDD
 */

SFDD cnf_to_sfdd(const char* pro_file) {
    /* parser cnf format */
    ifstream infile(pro_file, ios::in);  // 打开要写入的文本文件
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

    /* order Vtree */
    Vtree v;
    vector<int> vars_order;
    for (int i = 1; i <= var_no; ++i) vars_order.push_back(i);
    v = *new Vtree(1, var_no*2-1, vars_order, TRIVIAL_TREE);


    /* manager */
    Manager m(v);  // initial manager with Vtree

    /* add literal to SFDD one by one */
    SDD fml = m.sddOne();  // initial formula with True
    for(int line = 0; line < col_no; ++line)  //read every line number, and save as a clause
    {
        SDD clause = m.sfddZero();  // initial clause with False
        while (true) {
            int literal;
            infile >> literal;  // 从文件中读取一个literal
            if (literal == 0) break;
            SDD tmp_literal = m.sfddVar(literal);
            clause = m.Or(clause, tmp_literal);
        }
        fml = m.And(fml, clause);
    }

	p_f.close();
    return fml;
}
