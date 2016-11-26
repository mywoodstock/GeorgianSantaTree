#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "suffix_tree.hpp"


using namespace std;


int main(int narg, char** args) {

  if(narg != 2) {
    cout << "usage: ./find_string <filename>"<< endl;
    return 0;
  } // if

  int num_documents = 28;
  //int num_documents = 1;
  vector<string> documents;
  for(int i = 1; i <= num_documents; ++ i) documents.push_back(to_string(i) + ".txt");
  //documents.push_back("hehe.txt");

  string queries(args[1]);

  GeneralizedSuffixTree st;
  string end_char(char_value(END_CHAR_));
  for(auto i = documents.begin(); i != documents.end(); ++ i) {
    ifstream f(*i);
    if(!f.is_open()) {
      cerr << "error: failed to open input document " << *i << endl;
      return -1;
    } // if

    cout << "** inserting strings from document " << *i << endl;

    while(1) {
      string s;
      f >> s;
      if(f.eof()) break;
      st.add_string(s + end_char);
    } // while

    f.close();
  } // for

  st.print_tree();

  return 0;
} // main()
