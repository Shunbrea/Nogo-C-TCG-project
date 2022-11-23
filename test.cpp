#include <string>
#include <iostream>
#include "board.h"
#include "episode.h"

using namespace std;

int main()
{
    episode ep;
    cout << ep.state() << endl;
    
    // cout << ep.state().info().who_take_turns << endl;

    MctsTree thetree;
    action::place move;

    thetree.deleteNodes();
    thetree.setroot(ep.state());
    cout << thetree.who << endl;
    cout << thetree.root->state << endl;
    
    for (size_t i=0; i<50; i++){
        thetree.deleteNodes();
        thetree.setroot(ep.state());
        cout << thetree.who << endl;
        cout << thetree.root->state << endl;

        move = thetree.uctsearch(1000000);
        cout << move << endl;
        ep.apply_action(move);
        cout << ep.state() << endl;
    }
    cout << CLOCKS_PER_SEC << endl;
        

    // cout << i << endl;;
        // thetree.simulate();
        // cout << endl;
        // cout << endl;
    // MctsTree thetree(1000, ep.state());
    // action::place move = thetree.run();

    // cout << move << endl;
    // ep.apply_action(move);
    // cout << ep.state() << endl;

    return 0;
}