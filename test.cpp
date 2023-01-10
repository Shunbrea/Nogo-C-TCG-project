#include <string>
#include <iostream>
#include "board.h"
#include "episode.h"
//#include "pynet.h"
#include <tuple>
#include <iostream>
// #include "mcrave.h"


using namespace std;

int main()
{

    episode ep;
    PyNNet pynet;


    

    // cout << thetree.root->state << endl;
    
    for (size_t i=0; i<50; i++){
        cout << ep.state() << endl;
        cout << ep.state().info().who_take_turns << endl;

        size_t action = pynet.GetAction(ep.state(), ep.state().info().who_take_turns);
        cout << action << endl;
        action::place move(action, ep.state().info().who_take_turns);

        cout << move << endl;
        ep.apply_action(move);
        cout << ep.state() << endl;
    
    }
    // cout << CLOCKS_PER_SEC << endl;
        
    // for (size_t i=0; i<1; i++){
    //     cout << i << endl;;

    //     Node* expan_node = thetree.slection();

    //     cout << "The board : " << endl;
    //     cout << expan_node->state << endl;
    //     cout << "The value = " << value << endl;


    //     vector<action::place> rollout_play = thetree.rollout(expan_node);
    //     board::piece_type winer;
    //     if (rollout_play.size() > 0){
    //         winer = rollout_play.back().color();
    //     } else {
    //         winer = expan_node->takemove.color();
    //     }
    //     bool win = (thetree.who == winer);


    //     thetree.backpropagation(expan_node, rollout_play, win);

    //     cout << endl;
    //     cout << endl;
    // }
    // MctsTree thetree(1000, ep.state());
    // action::place move = thetree.run();

    // cout << move << endl;
    // ep.apply_action(move);
    // cout << ep.state() << endl;

    return 0;
}