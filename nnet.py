import logging
import math
import numpy as np
import time
#import torch

from nogopy.NogoGame import NogoGame
from nogopy.pytorch.NNet import NNetWrapper as NNet

EPS = 1e-8

class pymcts:
    def __init__(self):
        self.game = NogoGame(9)
        self.nnet = NNet(self.game)
        self.nnet.load_checkpoint('./nogopy/Weight/','best.pth.tar')
        self.cpuct = 1
        self.mctssimcnt = 200
        self.totaltime = 270
        self.timeleft = 270
        self.step = 0

        self.name = "NogoGamer"
        #print('pymcts init')
        self.Qsa = {}  # stores Q values for s,a (as defined in the paper)
        self.Nsa = {}  # stores #times edge s,a was visited
        self.Ns = {}  # stores #times board s was visited
        self.Ps = {}  # stores initial policy (returned by neural net)

        self.Es = {}  # stores game.getGameEnded ended for board s
        self.Vs = {}  # stores game.getValidMoves for board s
        

    def predict(self, *args):
        time1 = time.time()
        input_args = list(args)
        board = np.array(input_args, dtype=float)

        self.step += 1
        mexp = sum(self.game.getValidMoves(board, 1))
        if mexp == 0:
            return 0
        if self.step <= 10:
            timeout = 5
        else:
            timeout = self.timeleft/mexp
        #print('Step {:d} : think time = {:.2f}'.format(self.step, timeout))
        probs = np.argmax(self.getActionProb(board, temp=0, timeout=timeout))
        time2 = time.time()
        self.timeleft -= (time2-time1)
        return probs

    def getActionProb(self, canonicalBoard, temp=1, timeout=None):
        """
        This function performs numMCTSSims simulations of MCTS starting from
        canonicalBoard.
        Returns:
            probs: a policy vector where the probability of the ith action is
                   proportional to Nsa[(s,a)]**(1./temp)
        """
        time1 = time.time()
        if (timeout == None):
            for i in range(self.mctssimcnt):
                self.search(canonicalBoard)
        else:
            time2 = time.time()
            while (time2-time1 <= timeout):
                self.search(canonicalBoard)
                time2 = time.time()

        s = self.game.stringRepresentation(canonicalBoard)
        counts = [self.Nsa[(s, a)] if (s, a) in self.Nsa else 0 for a in range(self.game.getActionSize())]
        
        if temp == 0:
            bestAs = np.array(np.argwhere(counts == np.max(counts))).flatten()
            bestA = np.random.choice(bestAs)
            probs = [0] * len(counts)
            probs[bestA] = 1
            return probs

        counts = [x ** (1. / temp) for x in counts]
        counts_sum = float(sum(counts))
        probs = [x / counts_sum for x in counts]
        return probs

    def search(self, canonicalBoard):
        """
        This function performs one iteration of MCTS. It is recursively called
        till a leaf node is found. The action chosen at each node is one that
        has the maximum upper confidence bound as in the paper.
        Once a leaf node is found, the neural network is called to return an
        initial policy P and a value v for the state. This value is propagated
        up the search path. In case the leaf node is a terminal state, the
        outcome is propagated up the search path. The values of Ns, Nsa, Qsa are
        updated.
        NOTE: the return values are the negative of the value of the current
        state. This is done since v is in [-1,1] and if v is the value of a
        state for the current player, then its value is -v for the other player.
        Returns:
            v: the negative of the value of the current canonicalBoard
        """

        s = self.game.stringRepresentation(canonicalBoard)

        if s not in self.Es:
            self.Es[s] = self.game.getGameEnded(canonicalBoard, 1)
        if self.Es[s] != 0:
            # terminal node
            return -self.Es[s]

        if s not in self.Ps:
            # leaf node
            self.Ps[s], v = self.nnet.predict(canonicalBoard)
            valids = self.game.getValidMoves(canonicalBoard, 1)
            self.Ps[s] = self.Ps[s] * valids  # masking invalid moves
            sum_Ps_s = np.sum(self.Ps[s])
            if sum_Ps_s > 0:
                self.Ps[s] /= sum_Ps_s  # renormalize
            else:
                # if all valid moves were masked make all valid moves equally probable

                # NB! All valid moves may be masked if either your NNet architecture is insufficient or you've get overfitting or something else.
                # If you have got dozens or hundreds of these messages you should pay attention to your NNet and/or training process.   
                self.Ps[s] = self.Ps[s] + valids
                self.Ps[s] /= np.sum(self.Ps[s])

            self.Vs[s] = valids
            self.Ns[s] = 0
            return -v

        valids = self.Vs[s]
        cur_best = -float('inf')
        best_act = -1

        # pick the action with the highest upper confidence bound
        for a in range(self.game.getActionSize()):
            if valids[a]:
                if (s, a) in self.Qsa:
                    u = self.Qsa[(s, a)] + self.cpuct * self.Ps[s][a] * math.sqrt(self.Ns[s]) / (
                            1 + self.Nsa[(s, a)])
                else:
                    u = self.cpuct * self.Ps[s][a] * math.sqrt(self.Ns[s] + EPS)  # Q = 0 ?

                if u > cur_best:
                    cur_best = u
                    best_act = a

        a = best_act
        next_s, next_player = self.game.getNextState(canonicalBoard, 1, a)
        next_s = self.game.getCanonicalForm(next_s, next_player)

        v = self.search(next_s)

        if (s, a) in self.Qsa:
            self.Qsa[(s, a)] = (self.Nsa[(s, a)] * self.Qsa[(s, a)] + v) / (self.Nsa[(s, a)] + 1)
            self.Nsa[(s, a)] += 1

        else:
            self.Qsa[(s, a)] = v
            self.Nsa[(s, a)] = 1

        self.Ns[s] += 1
        return -v


if __name__ == '__main__':
    pn = pymcts()
    p = pn.predict(0,0,0,0,0,0,0,0,0,
                    0,0,0,0,3,0,0,0,0,
                    0,0,0,0,3,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,
                    0,3,3,0,2,0,3,3,0,
                    0,0,0,0,0,0,0,0,0,
                    0,0,0,0,3,0,0,0,0,
                    0,0,0,0,3,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,)
    print(p)




    