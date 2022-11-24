#include "action.h"
#include "board.h"
#include "mctstree.h"
#include <ctime>
#include <algorithm>
#include <limits>

class McraveTree : public Tree {

public:    
    // constructor
    McraveTree(): Tree() {}
    McraveTree(const McraveTree& t): Tree() {
        setroot(t.root->state);
    }

    void setseed(int seed) {
        engine.seed(seed);
    }

    // procedure Mc–Rave(s0)
    action::place mcravesearch(int time_limit) {
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
        backpropagation(expan_node, rollout_play, win);
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
        // int n = std::accumulate(here_node->visits.begin(), here_node->visits.end(), 0);
        for (size_t i = 0; i < here_node->legal_count; i++) {
            // cerr << here_node->legalmoves[i] << here_node->wins[i] << "|";
            // cerr << here_node->visits[i] << "\t";
            if (here_node->amaf_visits[i] == 0){
                if (who == here_node->state.info().who_take_turns){
                    v = std::numeric_limits<double>::infinity();
                } else {
                    v = -1 * std::numeric_limits<double>::infinity();
                }
                // cerr << v << "|*|*\t";
                vec.emplace_back(v);
            } else {
                v = expval(here_node, i);
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

    // procedure SimDefault(board)
    vector<action::place> rollout(Node* expan_node) {
        // cerr << "here is rollout" << endl;
        // cout << nodes[amount_nodes-1]->state << endl;
        if (expan_node->legal_count > 0){
            vector<action::place> rollout_play = random_rollout(expan_node);
            return rollout_play;
        } else {
            vector<action::place> rollout_play(0);
            return rollout_play;
        }
    }

    vector<action::place> random_rollout(Node* expan_node) {
        size_t i;
        board temp;
        board after;
        after = expan_node->state;
        std::array<int, board::size_x * board::size_y> pos;
        std::iota(pos.begin(), pos.end(), 1);
        std::shuffle(pos.begin(), pos.end(), engine);
        vector<action::place> rollout_play;
        while (true){
            for (i = 0; i < board::size_x * board::size_y; i++){
                temp = after;
                action::place move(pos[i], after.info().who_take_turns);
                if (move.apply(temp) == board::legal){
                    after = temp;
                    // cerr << "get move" << endl;
                    rollout_play.push_back(move);
                    break;
                }
            }
            if (i == board::size_x * board::size_y){
                break;
            }
        }
        // cout << after << endl;
        return rollout_play;
    }
    
    // procedure Backup([s0,..., sT ],[a0,...,aD], z)
    void backpropagation(Node* expan_node, vector<action::place> rollout_play, bool win) {
        Node* node = expan_node;
        action::place move;

        // for (action::place amaf_move : rollout_play){
        //     cout << amaf_move << "=>";
        // }
        // cout << endl;

        std::reverse(rollout_play.begin(), rollout_play.end());
        while (node->depth > 0){
            move = node->takemove;
            rollout_play.push_back(move);
            node = node->parent_node;
        }
        std::reverse(rollout_play.begin(), rollout_play.end());

        // for (action::place amaf_move : rollout_play){
        //     cout << amaf_move << "=>";
        // }
        // cout << endl;

        node = expan_node;
        while (node->depth > 0){
            move = node->takemove;
            node = node->parent_node;

            // cerr << "b4" << endl;
            // for (size_t i = 0; i < node->legal_count; i++){
            //     cerr << node->legalmoves[i] << ":";
            //     cerr << node->visits[i] << "|";
            //     cerr << node->wins[i] << "|";
            //     cerr << node->amaf_visits[i] << "|";
            //     cerr << node->amaf_wins[i] << " = ";
            //     cerr << expval(node,i) << endl;
            // }

            for (size_t i = 0; i < node->legal_count; i++){
                if (node->legalmoves[i] == move){
                    node->visits[i]++;
                    if (win) node->wins[i]++;
                }
                for (action::place amaf_move : rollout_play){
                    if (node->legalmoves[i] == amaf_move){
                        node->amaf_visits[i]++;
                        if (win) node->amaf_wins[i]++;
                    }
                }
            }
            
            // cerr << "aft" << endl;
            // for (size_t i = 0; i < node->legal_count; i++){
            //     cerr << node->legalmoves[i] << ":";
            //     cerr << node->visits[i] << "|";
            //     cerr << node->wins[i] << "|";
            //     cerr << node->amaf_visits[i] << "|";
            //     cerr << node->amaf_wins[i] << " = ";
            //     cerr << expval(node,i) << endl;
            // }

        }
    }

    // procedure Eval(s,a)
    double b = 0.025;
    double expval(Node* node, size_t i){
        double ne = double(node->amaf_visits[i]);
        double n = double(node->visits[i]);
        double beta = ne/(n + ne + 4*n*ne*b*b);
        double qe = double(node->amaf_wins[i])/double(node->amaf_visits[i]);
        double q = double(node->wins[i])/double(node->visits[i]);
        if (beta == 1.0) {
            return qe;
        } else {
            return (1-beta)*q+beta*qe;
        }
        
    }


protected:
    std::default_random_engine engine;
};
