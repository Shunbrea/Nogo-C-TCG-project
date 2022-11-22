#include "node.h"
#include "action.h"
#include "board.h"
#include <algorithm>
#include <random>

class Tree {

public:
    
    // amount of nodes that are created and added to the tree with one mcts run
    int amount_nodes = 0;
    unsigned int nodes_limit;
    Node** nodes;
    
    // array of pointers to keep track of each created node object
    Tree(int n = 100): nodes_limit(n) {
        nodes = new Node*[nodes_limit];
    }
    Tree(const Tree& t): nodes_limit(t.nodes_limit) {
        nodes = new Node*[nodes_limit];
    }

    Node* root = 0;
    board::piece_type who = board::unknown;
    void setroot(const board& root_board){
        root = new Node(root_board);
        who = root_board.info().who_take_turns;
    }

    // add a pointer to the tree of nodes
    void addNode(Node *node) {
        nodes[amount_nodes] = node;
        amount_nodes++;
    }

    // delete all objects the pointers are pointing on
    void deleteNodes() {
        // cerr << amount_nodes << endl;
        for (int i = 0; i < amount_nodes; i++) {
            delete nodes[i];
            nodes[i] = 0;
        }
        amount_nodes = 0;
        // cerr << root << endl;
        delete root;
        // root = 0;
    }
};