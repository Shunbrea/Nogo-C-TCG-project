#include "action.h"
#include "board.h"
#include "tree.h"
#include <algorithm>

class MctsTree : public Tree {

public:    
    // constructor
    MctsTree(int n = 100): Tree(n) {}
    MctsTree(const MctsTree& t): Tree(t.nodes_limit) {}

    void setseed(int seed) {
        engine.seed(seed);
    }

    // procedure UctSearch(s0)
    action::place uctsearch() {
        if (root->Numpossiblemoves() == 0) return action();
        for (size_t i = 0; i < nodes_limit; i++){
            simulate();
        }        
        double c=0;
        action::place move = slection_one(root, c)->takemove;
        // cerr << "Simulate " << i << " times, Move = " << move << endl;
        return move;
    }

    // procedure Simulate(board, s0)
    void simulate() {
        Node* select_node = slection();
        // cout << "The select node depth = " << select_node->depth << endl;

        Node* expan_node = expansion(select_node);
        // cout << "The expansion node depth = " << expan_node->depth << endl;

        bool win = rollout(expan_node);
        // cout << "The rollout result = " << win << endl;

        // cout << "before backpropagation, root= " << root->wins << "/" << root->visits << endl;
        backpropagation(expan_node, win);
        // cout << "after backpropagation, root= " << root->wins << "/" << root->visits << endl;
    }

    // procedure SimTree(board)
    Node* slection() {
        // cerr << "here is slection" << endl;
        int level;
        double c=sqrt(2);
        Node* select_node;

        level = root->depth;
        select_node = slection_one(root, c);
        // cerr << level << "|" << select_node->depth << endl;
        while (select_node->depth != level){
            level = select_node->depth;
            select_node = slection_one(select_node, c);
            
            // cerr << level << "|" << select_node->depth << endl;
        }
        // cerr << "here is slection end" << endl;
        return select_node;
    }

    // procedure SelectMove(board, s, c)
    Node* slection_one(Node* here_node, double c){
        // cout << "here is slection_one" << endl;
        int max_idx;
        if (here_node->Numpossiblemoves() > 0) {
            return here_node;
        } else {
            if (here_node->child_nodes.size() == 0) {
                return here_node;
            } else {
                std::vector<double> vec;
                double v;
                for (const Node* n : here_node->child_nodes) {
                    double q = double(n->wins) / double(n->visits);
                    double p = sqrt(log10(here_node->visits) / double(n->visits));
                    if (who == here_node->state.info().who_take_turns){
                        v = q + c * p;
                    } else {
                        v = q - c * p;
                    }
                    vec.push_back(v);
                }
                if (who == here_node->state.info().who_take_turns){
                    max_idx = arg_max(vec);
                } else {
                    max_idx = arg_min(vec);
                }
                return here_node->child_nodes[max_idx];
            }
        }
        // cout << "here is slection end" << endl;
    }

    template <typename T, typename A>
    int arg_max(std::vector<T, A> const& vec) {
        return static_cast<int>(std::distance(vec.begin(), max_element(vec.begin(), vec.end())));
    }
    template <typename T, typename A>
    int arg_min(std::vector<T, A> const& vec) {
        return static_cast<int>(std::distance(vec.begin(), min_element(vec.begin(), vec.end())));
    }
    
    // procedure NewNode(s)
    Node* expansion(Node* select_node) {
        if (select_node->Numpossiblemoves() == 0) return select_node;
        action::place move = getRandomMove(select_node);
        Node* child = select_node->addChild(move);
        addNode(child);
        return child;
    }
    
    action::place getRandomMove(Node* select_node) {       
        std::shuffle(std::begin(select_node->possiblemoves),
                     std::end(select_node->possiblemoves),
                     engine); 
        action::place mv = select_node->possiblemoves.back();
        select_node->possiblemoves.pop_back();
        return mv;
    }

    // procedure SimDefault(board)
    bool rollout(Node* expan_node) {
        // cout << nodes[amount_nodes-1]->state << endl;
        board::piece_type losser = random_rollout(expan_node);
        bool win = (losser != who);
        return win;
    }

    board::piece_type random_rollout(Node* expan_node) {
        size_t i;
        board temp;
        board after;
        after = expan_node->state;
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
    
    // procedure Backup([s0,..., sT ], z)
    void backpropagation(Node* expan_node, bool win) {
        nodepropagation(expan_node, win);
    }

    void nodepropagation(Node* node, bool win){
        node->visits++;
        if (win) {
            node->wins++;
        };
        if (node->depth > 0) {
            backpropagation(node->parent_node, win);
        }
    }

protected:
    std::default_random_engine engine;
};
