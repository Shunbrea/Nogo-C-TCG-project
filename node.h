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
        // TO.DO
    };

    // parent node of the node
    Node *parent_node = 0;

    // child nodes of the node
    vector<Node*> child_nodes;

    // amount of times that this node has been used for simulations either as leaf node or as node in the branch of the leaf node
    int visits = 0;

    // amount of wins that occurred by simulations having this node in its branch
    int wins = 0;

    // the tree depth of this node
    int depth = 0;

    // the state of this node
    const board state;

    // set the array with possible moves
    vector<action::place> possiblemoves;
    
    void setPossibleMoves() {
        for (size_t i = 0; i < board::size_x * board::size_y; i++){
            board after = state;
            action::place move(i, state.info().who_take_turns);
            if (move.apply(after) == board::legal){
                possiblemoves.push_back(action::place(i, state.info().who_take_turns));
            }
        }
    }

    int Numpossiblemoves() {
        return possiblemoves.size();
    }

    // expansion of a child node
    Node* addChild(action::place move) {
        // create new node with its game state and possible moves
        Node *child;
        board after = state;
        move.apply(after);
        // cout << after << endl;
        
        child = new Node(after);

        child->setPossibleMoves();
        child->depth++;

        // assign the new child node as a child node of the actual node
        child_nodes.push_back(child);

        // assign actual node as parent node
        child->parent_node = this;
        // cout << child->state << endl;

        return child;
    }

    // get the parent node
    Node* getParent() {
        return parent_node;
    }

    std::default_random_engine engine;
    // return randomly a possible move and thereupon erase it from the list of possible moves
    action::place* getRandomMove() {       
        std::shuffle(std::begin(possiblemoves), std::end(possiblemoves), engine); 
        return &possiblemoves[0];
    }

    board::piece_type random_rollout() {
        size_t i;
        board temp;
        board after;
        after = state;
        std::array<int, board::size_x * board::size_y> pos;
        std::iota(pos.begin(), pos.end(), 1);
        std::shuffle(pos.begin(), pos.end(), engine);
        while (true){
            for (i = 0; i < board::size_x * board::size_y; i++){
                temp = after;
                action::place move(pos[i], after.info().who_take_turns);
                if (move.apply(temp) == board::legal){
                    after = temp;
                    break;
                }
            }
            if (i == board::size_x * board::size_y){
                break;
            }
        }
        // cout << after << endl;
        return after.info().who_take_turns;
    }

    void backpropagation(bool win){
        visits++;
        if (win) {
            wins++;
        };
        parent_node->backpropagation(win);
    }
};
