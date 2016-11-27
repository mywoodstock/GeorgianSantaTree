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
  vector<string> documents;
  for(int i = 1; i <= num_documents; ++ i) documents.push_back(to_string(i) + ".txt");

  string queries(args[1]);

  GeneralizedSuffixTree st;
  string end_char(char_value(END_CHAR_));
  int doc_num = 0;
  for(auto i = documents.begin(); i != documents.end(); ++ i) {
    ifstream f(*i);
    if(!f.is_open()) {
      cerr << "error: failed to open input document " << *i << endl;
      return -1;
    } // if

    cout << "** pre-processing strings from document " << *i << endl;

    int str_num = 0;
    while(1) {
      string s;
      f >> s;
      if(f.eof()) break;
      st.add_string(doc_num, str_num, s + end_char);
      ++ str_num;
    } // while

    f.close();
    ++ doc_num;
  } // for

  //st.print_tree();

  // perform the queries
  ifstream fq(queries);
  if(!fq.is_open()) {
    cerr << "error: failed to open queries file " << queries << endl;
    return -1;
  } // if
  while(1) {
    string s;
    fq >> s;
    if(fq.eof()) break;
    vector<SubStringID> result;
    cout << s << ": ";
    if(st.search(s, result)) {
      cout << "found." << endl;
      for(auto i = result.begin(); i != result.end(); ++ i) {
        cout << "\t[ " << i->document_num_
             << "\t" << i->line_num_
             << "\t" << i->character_num_
             << "\t]" << endl;
      } // for
    } else {
      cout << " not found." << endl;
    } // if-else
    cout << endl;
  } // while
  fq.close();

  return 0;
} // main()
