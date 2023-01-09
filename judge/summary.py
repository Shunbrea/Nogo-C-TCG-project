import pandas as pd
import numpy as np
import os 

file_name = "gogui-twogtp-20221124153707/P1B_P2W-0.sgf"

def read_sgf(file_name):
    with open(file_name, 'r') as f:
        #f = open(file_name, 'r')
        f.readline()        
        string = f.readline()
        winer = string[(string.find('RE')+3):(string.find('RE')+4)]
        for _ in range(8) : f.readline()
        string = f.readline()
        moves = []
        while (string != "") :
            moves += string.replace('\n','').replace(')','').split(';')[1:]
            string = f.readline()
        f.close()
    
    return moves, winer

file_list = os.listdir("gogui-twogtp-20221124153707/")

black_win_array = np.zeros((9,9))
black_loss_array = np.zeros((9,9))
white_win_array = np.zeros((9,9))
white_loss_array = np.zeros((9,9))

for file_name in file_list:
    if file_name.endswith('.sgf'):
        moves, winer = read_sgf(os.path.join('gogui-twogtp-20221124153707', file_name))
        for m in moves:
            cord = ord(m[2]) - ord('a'), ord(m[3]) - ord('a')
            if m[0] == 'B':
                if winer == 'B':
                    black_win_array[cord] += 1
                else:
                    black_loss_array[cord] += 1
            else:
                if winer == 'W':
                    white_win_array[cord] += 1
                else:
                    white_loss_array[cord] += 1

q_value = (black_win_array + white_win_array) / (black_win_array + white_win_array + black_loss_array + white_loss_array)

win = (black_win_array + white_win_array)
np.flip(win)

visits = (black_win_array + white_win_array + black_loss_array + white_loss_array)
visits[-1:0:-1].T
