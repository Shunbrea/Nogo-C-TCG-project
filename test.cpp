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

    MctsTree thetree(1000);
    action::place move;

    for (size_t i=0; i<5; i++){
        thetree.deleteNodes();
        thetree.setroot(ep.state());
        cout << thetree.who << endl;
        cout << thetree.root->state << endl;

        move = thetree.uctsearch();
        cout << move << endl;
        ep.apply_action(move);
        cout << ep.state() << endl;
    }
    // MctsTree thetree(1000, ep.state());
    // action::place move = thetree.run();

    // cout << move << endl;
    // ep.apply_action(move);
    // cout << ep.state() << endl;

    return 0;
}