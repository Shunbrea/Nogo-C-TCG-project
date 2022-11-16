#include "node.h"
#include "board.h"
#include <algorithm>
#include <random>

class McstTree {

public:
    
    // amount of nodes that are created and added to the tree with one mcts run
    int amount_nodes = 0;
    const int nodes_limit;
    Node **nodes;
    board& root_board;
    Node root;
    
    
    // array of pointers to keep track of each created node object
    McstTree(const int nodes_limit, board& root_board) :
    nodes_limit(nodes_limit), root_board(root_board), root(root_board) {
        // Node *nodes[nodes_limit];
        nodes = new Node*[nodes_limit];
        addNode(&root);
        root.setPossibleMoves();
    };

    
    Node* expansion(Node* select_node) {
        action::place* move = select_node->getRandomMove();
        Node* child = select_node->addChild(*move);
        addNode(child);
        return child;
    }

    bool rollout(Node* expan_node) {
        // cout << nodes[amount_nodes-1]->state << endl;
        board::piece_type losser = expan_node->random_rollout();
        bool win = (losser != root.state.info().who_take_turns);
        return win;
    }
    
    void backpropagation(Node* expan_node, bool win) {
        expan_node->backpropagation(win);
    }

    // add a pointer to the tree of nodes
    void addNode(Node *node) {
        nodes[amount_nodes] = node;
        amount_nodes++;
    }

    // delete all objects the pointers are pointing on
    void deleteNodes() {
        for (int i = 0; i < amount_nodes; i++) {
            delete nodes[i];
            nodes[i] = 0;
        }
    }

};