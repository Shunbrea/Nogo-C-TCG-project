#include <string>
#include <iostream>
#include "tree.h"
#include "board.h"
#include "episode.h"

using namespace std;

int main()
{
    episode ep;
    cout << ep.state() << endl;
    // cout << ep.state().info().who_take_turns << endl;

    McstTree thetree(100, ep.state());
    // cout << thetree.nodes_limit << endl;

    // cout << thetree.root.Numpossiblemoves() << endl;
    
    Node* select_node = &thetree.root;

    // Node* select_node = thetree.slection();
    Node* expan_node = thetree.expansion(select_node);
    bool win = thetree.rollout(expan_node);
    thetree.backpropagation(expan_node, win);
}