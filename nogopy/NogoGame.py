from __future__ import print_function
import sys
sys.path.append('..')
from .Game import Game
from .NogoLogic import Board, BLACK, WHITE, where1d
#from .simple_board import SimpleGoBoard as Board
#from .board_util import GoBoardUtil, coord_to_point

import numpy as np

class NogoGame(Game):
    square_content = {
        +0: "-",
        +1: "b",
        +2: "W",
        +3: "*",
    }

    @staticmethod
    def getSquarePiece(piece):
        return NogoGame.square_content[piece]

    @staticmethod
    def display(board):
        n = int(board.shape[0] ** 0.5)
        b = Board(n)
        b.set_pieces(board)
        print("    ", end="")
        for y in range(n):
            print(y, end=" ")
        print("")
        print("  ---------------------")
        for row in range(n, 0, -1):
            print(row-1, end=" ")
            print("|", end=" ")
            start = row * (n + 1) + 1
            for point in range(start, start+n):
                print(NogoGame.square_content[b[point]], end=" ")
            print("|")
        print("  ---------------------")

    def __init__(self, n):
        self.n = n

    def getInitBoard(self):
        # return initial board (numpy board)
        b = Board(self.n)
        return np.array(b.get_real_board_pieces())

    def getBoardSize(self):
        # (a,b) tuple
        return (self.n, self.n)

    def getActionSize(self):
        # return number of actions
        return self.n*self.n

    def getCanonicalForm(self, board, player):
        # return state if player==1, else return -state if player==-1
        b = Board(self.n)
        b.set_pieces(board)
        if self.convert_color(player) == BLACK:
            return np.array(b.get_real_board_pieces())
        elif self.convert_color(player) == WHITE:
            inv_board = np.array(b.get_real_board_pieces())
            inv_board[board == BLACK] = WHITE
            inv_board[board == WHITE] = BLACK
            return inv_board
        else:
            return np.array()

    def getValidMoves(self, board, player, search=False):
        # return a fixed size binary vector
        color = self.convert_color(player)
        b = Board(self.n)
        b.set_pieces(board)
        points = where1d(np.array(b.pieces) == 0)
        if search:
            legal = points
        else:
            legal = []
            for point in points:
                #b = Board(self.n)
                b.set_pieces(board)
                if b.play_move(point, color):
                    legal.append(point)

        valids = [0]*self.getActionSize()
        for action in [self.point_to_action(p) for p in legal]:
            valids[action] = 1
        return np.array(valids)

    def point_to_action(self, point):
        return (point // (self.n + 1) - 1) * self.n + point % (self.n + 1) - 1

    def action_to_point(self, action):
        return (action // (self.n) + 1) * (self.n + 1) + action % (self.n) + 1

    def getGameEnded(self, board, player, search=False):
        # return 0 if not ended, 1 if player 1 won, -1 if player 1 lost
        # player = 1
        if search:
            return 0
        legal_moves = self.getValidMoves(board, player)
        if legal_moves.max() == 1:
            return 0
        else:
            return -1 * player

    def getNextState(self, board, player, action, fast=False):
        # if player takes action on board, return next (board,player)
        # action must be a valid move
        color = self.convert_color(player)
        if fast:
            board[action] = color
            return board, -player
        b = Board(self.n)
        b.set_pieces(board)
        point = self.action_to_point(action)
        b.play_move(point, color)
        return np.array(b.get_real_board_pieces()), self.color_to_player(b.current_player)

    def convert_color(self, player):
        if player == 1:
            return BLACK
        elif player == -1:
            return WHITE
        else: return None

    def color_to_player(self, color):
        if color == WHITE:
            return -1
        if color == BLACK:
            return 1
        else: return None

    def getSymmetries(self, board, pi):
        # mirror, rotational
        assert(len(pi) == self.n**2)
        board = board.reshape(self.n, self.n)
        pi_board = np.reshape(pi, (self.n, self.n))
        l = []
        for i in range(1, 5):
            for j in [True, False]:
                newB = np.rot90(board, i)
                newPi = np.rot90(pi_board, i)
                if j:
                    newB = np.fliplr(newB)
                    newPi = np.fliplr(newPi)
                l += [(newB, list(newPi.ravel()))]
        return l

    def stringRepresentation(self, board):
        return board.tostring()

    def stringRepresentationReadable(self, board):
        board_s = "".join(self.square_content[square] for row in board for square in row)
        return board_s

    
