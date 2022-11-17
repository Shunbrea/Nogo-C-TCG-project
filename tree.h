#include "node.h"
#include "action.h"
#include "board.h"
#include <algorithm>
#include <random>

class MctsTree {

public:
    
    // amount of nodes that are created and added to the tree with one mcts run
    int amount_nodes = 0;
    const unsigned int nodes_limit;
    Node** nodes;
    
    // array of pointers to keep track of each created node object
    MctsTree(const int nodes_limit): nodes_limit(nodes_limit) {
        nodes = new Node*[nodes_limit];
    };

    Node* root = 0;
    void setroot(const board& root_board){
        root = new Node(root_board);
    }
    
    action::place run() {
        for  (size_t i = 0; i < (nodes_limit); i++) {
            Node* select_node = slection();
            // cout << "The select node depth = " << select_node->depth << endl;

            Node* expan_node = expansion(select_node);
            // cout << "The expansion node depth = " << expan_node->depth << endl;

            bool win = rollout(expan_node);
            // cout << "The rollout result = " << win << endl;

            // cout << "before backpropagation, root visits = " << root->visits << endl;
            // cout << "before backpropagation, root wins = " << root->wins << endl;
            backpropagation(expan_node, win);
            // cout << "after backpropagation, root visits = " << root->visits << endl;
            // cout << "after backpropagation, root wins = " << root->wins << endl;
            // cout << "after backpropagation, expansion node visits = " << expan_node->visits << endl;
            // cout << "after backpropagation, expansion node wins = " << expan_node->wins << endl;
        } 
        std::vector<double> vec;
        for (const Node* c : root->child_nodes){
            double r = double(c->wins) / double(c->visits) ;
            // cout << c->takemove << ":" << c->wins << "/" << c->visits << "=" << r << endl;
            vec.push_back(r);
        }
        int max_idx = arg_max(vec);
        action::place move = root->child_nodes[max_idx]->takemove;
        return move;
    }


    Node* slection() {
        // cout << "here is slection" << endl;
        int level;
        Node* select_node;

        level = root->depth;
        select_node = slection_one(root);
        while (select_node->depth != level){
            level = select_node->depth;
            select_node = slection_one(select_node);
        }
        // cout << "here is slection end" << endl;
        return select_node;
    }

    template <typename T, typename A>
    int arg_max(std::vector<T, A> const& vec) {
        return static_cast<int>(std::distance(vec.begin(), max_element(vec.begin(), vec.end())));
    }

    Node* slection_one(Node* here_node){
        // cout << "here is slection_one" << endl;
        if (here_node->Numpossiblemoves() > 0) {
            return here_node;
        } else {
            std::vector<double> vec;
            for (const Node* n : here_node->child_nodes){
                double v = (n->wins/n->visits)+sqrt(2*log10(here_node->visits)/n->visits);
                vec.push_back(v);
            }
            int max_idx = arg_max(vec);
            return here_node->child_nodes[max_idx];
        }
        // cout << "here is slection end" << endl;
    }
    
    Node* expansion(Node* select_node) {
        action::place* move = select_node->getRandomMove();
        Node* child = select_node->addChild(*move);
        addNode(child);
        return child;
    }

    bool rollout(Node* expan_node) {
        // cout << nodes[amount_nodes-1]->state << endl;
        board::piece_type losser = expan_node->random_rollout();
        bool win = (losser != root->state.info().who_take_turns);
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
        cerr << amount_nodes << endl;
        for (int i = 0; i < amount_nodes; i++) {
            delete nodes[i];
            nodes[i] = 0;
        }
        amount_nodes = 0;
        cerr << root << endl;
        delete root;
        // root = 0;
    }

};