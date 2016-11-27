#ifndef __SUFFIX_TREE_HPP__
#define __SUFFIX_TREE_HPP__

#include <iostream>
#include <string>
#include <vector>
 
enum Character { None = -1, A = 0, C, G, T, $ };
const Character END_CHAR_ = $;
const int ALPHABET_SIZE_ = 5;


/**
 * Alphabet related utility functions
 */

Character char_name(char c, int string_id) {
  switch(c) {
    case 'a':
    case 'A': return A;
    case 'c':
    case 'C': return C;
    case 'g':
    case 'G': return G;
    case 't':
    case 'T': return T;
    case '$': return $;
    default: return None;
  } // switch
} // char_name()


std::string char_value(Character c) {
  //if(c >= $) return std::string("$" + std::to_string(c - $));
  switch(c) {
    case A: return std::string("A");
    case C: return std::string("C");
    case G: return std::string("G");
    case T: return std::string("T");
    case $: return std::string("$");
    default: return std::string("");
  } // switch
} // char_value()


typedef struct SubStringID SubStringID;

struct SubStringID {
  int document_num_;
  int line_num_;
  int character_num_;
  SubStringID(): document_num_(-1), line_num_(-1), character_num_(-1) { }
  SubStringID(int d, int l, int c): document_num_(d), line_num_(l), character_num_(c) { }
}; // struct SubStringID


/**
 * Leaf node: this is attached to the leaves of the suffix tree to store necessary information
 */

typedef struct LeafNode LeafNode;

struct LeafNode {
  int string_id_;   // id of the corresponding string
  int position_;    // position where the corresponding suffix starts in the string
  std::vector<SubStringID> substring_ids_;
  LeafNode *next_;  // pointer to next leaf node in the tree (in sequence)
  LeafNode(): string_id_(-1), position_(-1), next_(NULL) { }
  LeafNode(int d, int s, int p): string_id_(s), position_(p), next_(NULL) {
    substring_ids_.push_back(SubStringID(d, s, p));
  } // LeafNode()
  void add_substring(int d, int s, int p) { substring_ids_.push_back(SubStringID(d, s, p)); }
}; // struct LeafNode


/**
 * Suffix node: node in a suffix tree
 */

class SuffixNode {

  private:

    const int MAX_CHILDREN_;

    int node_id_;                           // temporary just for identification

    int begin_, end_;                       // begin, end positions in the string for its incoming edge
    int string_id_;                         // id of the string begin_ and end_ correspond to

    SuffixNode *children_[ALPHABET_SIZE_];  // downward links
    SuffixNode *parent_, *suffix_link_;     // upward liks
    LeafNode *leaf_;                        // if this is a leaf node, stored related info here

  public:

    SuffixNode(): MAX_CHILDREN_(ALPHABET_SIZE_), begin_(-1), end_(-1), string_id_(-1),
                  parent_(NULL), suffix_link_(NULL), leaf_(NULL) {
      for(auto i = 0; i < MAX_CHILDREN_; ++ i) children_[i] = NULL;
    } // SuffixNode()

    ~SuffixNode() {
      // delete everything
      // ...
    } // ~SuffixNode()
 
    bool find(const std::string& s_curr, const std::string& s_new, int a_begin, int b_begin,
              int& a, int& b) {
      a = a_begin; b = b_begin;
      while(a <= end_ && b < s_new.size() && s_curr.at(a) == s_new.at(b)) { ++ a; ++ b; }
      if(a > end_) return false;
      -- a; -- b;
      return true;
    } // find()

    void print() {
      std::cout << node_id_ << " :: " << this << ": { begin_: (" << string_id_ << ", " << begin_
                << "), end_: (" << string_id_ << ", " << end_ << "), children_: [";
      for(auto i = 0; i < MAX_CHILDREN_; ++ i) std::cout << children_[i] << " ";
      std::cout << "], parent_: " << parent_ << ", suffix_link_: " << suffix_link_
                << ", leaf_: " << leaf_ << " }" << std::endl;
      if(leaf_ != NULL) {
        std::cout << "\tsubstring_ids_: ";
        for(auto i = leaf_->substring_ids_.begin(); i != leaf_->substring_ids_.end(); ++ i) {
          std::cout << "(" << (*i).document_num_ << ", " << (*i).line_num_ << ", "
                    << (*i).character_num_ << ") ";
        } // for
        std::cout << std::endl;
      } // if
    } // print()

    friend class GeneralizedSuffixTree;

}; // class SuffixNode


/**
 * Generalized Suffix tree: the main definition of a suffix tree
 */

class GeneralizedSuffixTree {

  private:

    int num_nodes_;         // temporary to set node id

    SuffixNode* root_;      // root of the generalized suffix tree
    LeafNode* leaves_head_; // head pointer to the linked list of leaves in sequence

    std::vector<std::string> strings_;
    int num_strings_;

    SuffixNode* curr_node_;
    SuffixNode* last_node_;

  public:

    GeneralizedSuffixTree(): root_(NULL), leaves_head_(NULL), num_nodes_(0),
                             curr_node_(NULL), last_node_(NULL),
                             num_strings_(0) { }

    ~GeneralizedSuffixTree() {
      // delete all nodes in the tree and the leaves
      // ...
    } // ~GeneralizedSuffixTree()

    /**
     * construct a suffix tree with a single string
     * process is to insert all suffixes of given string into the tree
     */
    void add_string(const int doc_num, const int str_num, const std::string& s) {
      strings_.push_back(s);
      int string_id = strings_.size() - 1;
      for(auto i = 0; i < s.size() - 1; ++ i) insert_suffix(doc_num, str_num, string_id, s, i);
    } // add_string()


    /**
     * insert the suffix starting at 'begin' into the tree
     */
    void insert_suffix(const int doc_num, const int string_num,
                       const int string_id, const std::string& s, const int begin) {

      //std::cout << " ** inserting suffix " << begin << ": " << s.substr(begin) << std::endl;

      if(root_ == NULL) {
        root_ = new SuffixNode();
        root_->suffix_link_ = root_;    // root has self loop suffix link
        root_->begin_ = -1;
        root_->end_ = -1;
        root_->string_id_ = string_id;
        root_->node_id_ = num_nodes_ ++;
        curr_node_ = root_;
        last_node_ = root_;
      } // if

      curr_node_ = curr_node_->suffix_link_;
      curr_node_ = root_;
      int c = char_name(s.at(begin), string_id);

      if(curr_node_->children_[c] == NULL) {

        // insert a new child node (leaf)
        curr_node_->children_[c] = new SuffixNode();
        curr_node_->children_[c]->begin_ = begin;
        curr_node_->children_[c]->end_ = s.size() - 1;
        curr_node_->children_[c]->string_id_ = string_id;
        curr_node_->children_[c]->parent_ = curr_node_;
        curr_node_->children_[c]->leaf_ = new LeafNode(doc_num, string_num, begin);
        curr_node_->children_[c]->node_id_ = num_nodes_ ++;
        last_node_ = curr_node_;

        //std::cout << "** adding leaf: ";
        //curr_node_->children_[c]->print();

      } else {

        // when 'begin' is between curr_node_ and curr_node_->children_[c]
        // split this edge by inserting a new node and a child
        // find a and b
        // where, a is position w.r.t. the suffix corresponding to the original child
        // and, b is the position w.r.t. the new suffix, where a + 1 != b + 1
        int a = curr_node_->children_[c]->begin_, b = begin;
        bool added = false;
        while(!curr_node_->children_[c]->find(strings_[curr_node_->children_[c]->string_id_], s,
                                              a, b, a, b)) {
          if(a >= s.size() - 1 || b >= s.size() - 1) {
            added = true;
            break;
          } // if
          // follow the corresponding child node
          curr_node_ = curr_node_->children_[c];
          c = char_name(s.at(b), string_id);
          if(curr_node_->children_[c] == NULL) {
            // this is the case where the curr_node_ does not have a c child
            // as it might have been constructed from another previous string
            // hence, insert a new child node (leaf)
            curr_node_->children_[c] = new SuffixNode();
            curr_node_->children_[c]->begin_ = b;
            curr_node_->children_[c]->end_ = s.size() - 1;
            curr_node_->children_[c]->string_id_ = string_id;
            curr_node_->children_[c]->parent_ = curr_node_;
            curr_node_->children_[c]->leaf_ = new LeafNode(doc_num, string_num, begin);
            curr_node_->children_[c]->node_id_ = num_nodes_ ++;
            last_node_ = curr_node_;
            added = true;

            //std::cout << "** adding another leaf: ";
            //curr_node_->children_[c]->print();

            break;
          } // if

          a = curr_node_->children_[c]->begin_;
          if(a >= s.size() - 1) {
            std::cout << " ++ ++ " << a << " " << s.size() - 1 << " " << c << std::endl;
            if(curr_node_->children_[c]->children_[$] == NULL) {
              curr_node_->children_[c]->children_[$] = new SuffixNode();
              curr_node_->children_[c]->children_[$]->begin_ = b;
              curr_node_->children_[c]->children_[$]->end_ = s.size() - 1;
              curr_node_->children_[c]->children_[$]->string_id_ = string_id;
              curr_node_->children_[c]->children_[$]->parent_ = curr_node_->children_[c];
              curr_node_->children_[c]->children_[$]->leaf_ = new LeafNode(doc_num, string_num, begin);
              curr_node_->children_[c]->children_[$]->node_id_ = num_nodes_ ++;
              last_node_ = curr_node_->children_[c];
            } else {
              curr_node_->children_[c]->leaf_->add_substring(doc_num, string_num, begin);
            } // if
            added = true;
            break;
          } // if

        } // while

        if(!added) {
          // create a new internal node and a new child node
          SuffixNode *new_node = new SuffixNode();
          SuffixNode *new_child = new SuffixNode();

          new_node->begin_ = curr_node_->children_[c]->begin_;
          new_node->end_ = a;
          new_node->string_id_ = curr_node_->children_[c]->string_id_;
          new_node->parent_ = curr_node_;
          new_node->children_[char_name(strings_[curr_node_->children_[c]->string_id_].at(a + 1),
                                        curr_node_->children_[c]->string_id_)]
            = curr_node_->children_[c];
          new_node->children_[char_name(s.at(b + 1), string_id)] = new_child;
          new_node->suffix_link_ = root_;
          new_node->node_id_ = num_nodes_ ++;
        
          new_child->parent_ = new_node;
          new_child->begin_ = b + 1;
          new_child->end_ = s.size() - 1;
          new_child->string_id_ = string_id;
          new_child->leaf_ = new LeafNode(doc_num, string_num, begin);
          new_child->node_id_ = num_nodes_ ++;

          curr_node_->children_[c]->parent_ = new_node;
          curr_node_->children_[c]->begin_ = a + 1;
          curr_node_->children_[c] = new_node;
        
          if(last_node_ != NULL) last_node_->suffix_link_ = new_node;
          last_node_ = new_node;
          curr_node_ = new_node;

          //std::cout << "** adding node: ";
          //new_node->print();
          //std::cout << "** and a leaf: ";
          //new_child->print();
        } // if
      } // if-else

    } // insert_suffix()


    /**
     * This function is to make sure that the ending character of each string is unique in
     * order to maintain the generalized suffix tree.
     * It traverses the tree and converts any END_CHAR_ to the given string id.
     */
    void finalize_string(int string_id) {
      finalize_dfs(root_, string_id);
    } // finalize_string()


    void finalize_dfs(SuffixNode* n, int sid) {
      if(n->begin_ > -1 && n->end_ > -1) {
        // this is the root
      } // if
    } // finalize_dfs()


    void print_preorder(SuffixNode* n) {
      n->print();
      for(auto i = 0; i < n->MAX_CHILDREN_; ++ i)
        if(n->children_[i] != NULL) print_preorder(n->children_[i]);
    } // print_preorder()

    void print_tree() {
      print_preorder(root_);
    } // print_tree()

}; // class GeneralizedSuffixTree

#endif // __SUFFIX_TREE_HPP__
