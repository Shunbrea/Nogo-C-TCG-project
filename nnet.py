import numpy as np
#import torch

from nogopy.NogoGame import NogoGame
from nogopy.pytorch.NNet import NNetWrapper as NNet

class pynet:
    def __init__(self):
        self.g = NogoGame(9)
        self.n1 = NNet(self.g)
        self.n1.load_checkpoint('./nogopy/Weight/','checkpoint_167.pth.tar')
        self.name = "NogoGame"
        print('class init')

    def predict(self, *args):
        num = np.array(list(args))
        p, v = self.n1.predict(num.reshape(9,-1))

        #print(p)
        #print(v)
        return p, v


if __name__ == '__main__':
    pn = pynet()
    p, v = pn.predict(0,0,0,0,0,0,0,0,0,
                        0,0,0,0,3,0,0,0,0,
                        0,0,0,0,3,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,
                        0,3,3,0,2,0,3,3,0,
                        0,0,0,0,0,0,0,0,0,
                        0,0,0,0,3,0,0,0,0,
                        0,0,0,0,3,0,0,0,0,
                        0,0,0,0,0,0,0,0,0,)
    print(v)
