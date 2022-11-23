#include <random>
#include <algorithm>
#include <vector>
#include "action.h"
#include "board.h"

using namespace std;

class Node {

public:
    // set the state of this node
    // void setState(board& state) {
    //     board& state = state;
    // }
    Node(board state) : state(state) {
        setlegalMoves();
        visits.resize(legal_count);
        amaf_visits.resize(legal_count);
        wins.resize(legal_count);
        amaf_wins.resize(legal_count);
    };

    // parent node of the node
    Node *parent_node = 0;

    // child nodes of the node
    vector<Node*> child_nodes;

    // amount of times that this node has been used for simulations either as leaf node or as node in the branch of the leaf node
    vector<int> visits;
    vector<int> amaf_visits;

    // amount of wins that occurred by simulations having this node in its branch
    vector<int> wins;
    vector<int> amaf_wins;

    // the tree depth of this node
    int depth = 0;

    // the state of this node
    const board state;
    
    // set the array with possible moves
    vector<action::place> legalmoves;
    vector<action::place> possiblemoves;
    size_t legal_count;
    void setlegalMoves() {
        for (size_t i = 0; i < board::size_x * board::size_y; i++){
            board after = state;
            action::place move(i, state.info().who_take_turns);
            if (move.apply(after) == board::legal){
                legalmoves.emplace_back(action::place(i, state.info().who_take_turns));
            }
        }
        legal_count = legalmoves.size();
        possiblemoves = legalmoves;
    }

    int Numpossiblemoves() {
        return possiblemoves.size();
    }

    // expansion of a child node
    action::place takemove = action();

    Node* addChild(action::place move) {
        // create new node with its game state and possible moves
        Node *child;
        board after = state;
        move.apply(after);
        // cout << after << endl;
        
        child = new Node(after);

        child->depth++;

        // assign the new child node as a child node of the actual node
        child_nodes.emplace_back(child);

        // assign actual node as parent node
        child->parent_node = this;
        child->takemove = move;
        // cout << child->state << endl;

        return child;
    }

    // get the parent node
    Node* getParent() {
        return parent_node;
    }

};

