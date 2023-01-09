#include "node.h"
#include "action.h"
#include "board.h"
#include <algorithm>
#include <random>

class Tree {

public:
    
    // amount of nodes that are created and added to the tree with one mcts run
    vector<Node*> nodes;
    
    // array of pointers to keep track of each created node object
    Tree(){}
    Tree(const Tree& t){
        setroot(t.root->state);
    }

    Node* root = 0;
    board::piece_type who = board::unknown;
    virtual void setroot(const board& root_board){
        root = new Node(root_board);
        who = root_board.info().who_take_turns;
    }

    

    // add a pointer to the tree of nodes
    void addNode(Node *node) {
        nodes.push_back(node);
    }

    // delete all objects the pointers are pointing on
    void deleteNodes() {
        // cerr << amount_nodes << endl;
        for (Node* n : nodes) {
            delete n;
        }
        nodes.clear();
        // cerr << root << endl;
        delete root;
        // root = 0;
    }
};