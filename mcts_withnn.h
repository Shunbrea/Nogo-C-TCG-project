#include "action.h"
#include "board.h"
#include "tree.h"
#include <ctime>
#include <algorithm>
#include <limits>
#include <map>
#include "pynet.h"

class AlphaZeroTree : public Tree {

public:    
    // constructor
    AlphaZeroTree(): Tree() {}
    AlphaZeroTree(const AlphaZeroTree& t): Tree() {
        setroot(t.root->state);
    }

    void setseed(int seed) {
        engine.seed(seed);
    }

    std::map<board, float> Ps_map;
    std::map<board, size_t> Ns_map;
    typedef std::array<float, board::size_x*board::size_y> action_array;
    std::map<board, action_array> Qsa_map;
    std::map<board, action_array> Nsa_map;
    
    // procedure Search(s0)
    action::place search(int time_limit) {
        clock_t timer1, timer2;
        timer1 = clock();
        if (root->Numpossiblemoves() == 0) return action();
        timer2 = clock();
        // cout << (timer2 - timer1) << endl;
        while ((timer2 - timer1) < time_limit){
            simulate();
            timer2 = clock();
            // cout << (timer2 - timer1) << endl;
        }
        double c=0;
        action::place move = selectmove(root, c);
        // cerr << "Simulate " << i << " times, Move = " << move << endl;
        return move;
    }

    // procedure Simulate(board, s0)
    void simulate() {
        Node* expan_node = slection();
        // cout << "The select move = " << expan_node->takemove << endl;

        vector<action::place> rollout_play = rollout(expan_node);

        board::piece_type winer;
        if (rollout_play.size() > 0){
            winer = rollout_play.back().color();
        } else {
            winer = expan_node->takemove.color();
        }
        bool win = (who == winer);
        // cout << endl;
        // cout << "The rollout_play last = " << winer << endl;
        // cout << "The rollout result = " << win << endl;

        // cout << "before backpropagation, root= " << root->wins << "/" << root->visits << endl;
        backpropagation(expan_node, win);
        // cout << "after backpropagation, root= " << root->wins << "/" << root->visits << endl;
    }

    // procedure SimTree(board)
    Node* slection() {
        // cerr << "here is slection" << endl;
        // int level;
        // level = root->depth;
        Node* select_node = root;
        double c=sqrt(2);
        while (select_node->legal_count > 0){
            action::place selected_move = selectmove(select_node, c);
            // cerr << select_node->legal_count << endl;
            bool node_exist = false;
            for (Node* cn_ptr : select_node->child_nodes){
                if (cn_ptr->takemove == selected_move){
                    node_exist = true;
                    select_node = cn_ptr;
                    break;
                }
            }
            if (node_exist == false){
                // cerr << select_node->state << endl;
                // cerr << selected_move << endl;
                return expansion(select_node, selected_move);
            }
        }
        return select_node;
    }

    // procedure SelectMove(board, s, c)
    action::place selectmove(Node* here_node, double c){
        // cout << "here is slection_one | c = " << c << endl;
        int max_idx;

        std::vector<double> vec;
        double v;
        int n = std::accumulate(here_node->visits.begin(), here_node->visits.end(), 0);
        for (size_t i = 0; i < here_node->legal_count; i++) {
            // cerr << here_node->legalmoves[i] << here_node->wins[i] << "|";
            // cerr << here_node->visits[i] << "\t";
            if (here_node->visits[i] == 0){
                if (who == here_node->state.info().who_take_turns){
                    v = std::numeric_limits<double>::infinity();
                } else {
                    v = -1 * std::numeric_limits<double>::infinity();
                }
                // cerr << v << "|*|*\t";
                vec.emplace_back(v);
            } else {
                double q = double(here_node->wins[i]) / double(here_node->visits[i]);
                double p = sqrt(log10(n) / double(here_node->visits[i]));
                if (who == here_node->state.info().who_take_turns){
                    v = q + c * p;
                } else {
                    v = q - c * p;
                }
                // cerr << v << "|" << q << "|" << p << "\t";
            }
            vec.emplace_back(v);
        }
        if (who == here_node->state.info().who_take_turns){
            max_idx = arg_max(vec);
        } else {
            max_idx = arg_min(vec);
        }
        return here_node->legalmoves[max_idx];
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
    Node* expansion(Node* select_node, action::place move) {
        // cerr << "here is expansion" << endl;
        Node* child = select_node->addChild(move);
        addNode(child);
        return child;
    }
    
    // action::place getRandomMove(Node* select_node) {       
    //     std::shuffle(std::begin(select_node->possiblemoves),
    //                  std::end(select_node->possiblemoves),
    //                  engine); 
    //     action::place mv = select_node->possiblemoves.back();
    //     select_node->possiblemoves.pop_back();
    //     return mv;
    // }

    PyNNet pynet;

    // procedure SimDefault(board)
    vector<action::place> rollout(Node* expan_node) {
        // cerr << "here is rollout" << endl;
        // cout << nodes[amount_nodes-1]->state << endl;
        if (expan_node->legal_count > 0){
            vector<action::place> rollout_play = pynet.predict(expan_node->state);
            return rollout_play;
        } else {
            vector<action::place> rollout_play(0);
            return rollout_play;
        }
    }

    // procedure Backup([s0,..., sT ], z)
    void backpropagation(Node* expan_node, bool win) {
        Node* node = expan_node;
        action::place move;
        while (node->depth > 0){
            move = node->takemove;
            node = node->parent_node;
            for (size_t i = 0; i < node->legal_count; i++){
                if (node->legalmoves[i] == move){
                    node->visits[i]++;
                    if (win) node->wins[i]++;
                }
            }
        }
    }

protected:
    std::default_random_engine engine;
};
