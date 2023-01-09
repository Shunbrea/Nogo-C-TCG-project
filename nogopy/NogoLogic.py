import numpy as np

EMPTY = 0
BLACK = 1
WHITE = 2
BORDER = 3

def where1d(condition):
    return np.where(condition)[0]

class Board():

    def __init__(self, n):
        "Set up initial board configuration."

        """
        Creates a start state, an empty board with the given size
        The board is stored as a one-dimensional array
        See GoBoardUtil.coord_to_point for explanations of the array encoding
        """
        self.size = n
        self.NS = n + 1
        self.WE = 1
        self.current_player = BLACK
        self.maxpoint = n * n + 3 * (n + 1)
        
        # Create the empty board array.
        self.pieces = [BORDER]*self.maxpoint

        self._initialize_empty_points()
        if n == 9:
            self._initialize_hollow_points()
        self._initialize_neighbors()
    
    def row_start(self, row):
        #assert row >= 1
        #assert row <= self.size
        return row * self.NS + 1

    def _initialize_empty_points(self):
        """
        Fills points on the board with EMPTY
        """
        for row in range(1, self.size + 1):
            start = self.row_start(row)
            self.pieces[start : start + self.size] = [EMPTY] * self.size
        
    def _initialize_hollow_points(self):
        """
        Fills points on the board with hollow
        *   A B C D E F G H J
        * 9 + + + + + + + + + 9
        * 8 + + + +   + + + + 8
        * 7 + + + +   + + + + 7
        * 6 + + + + + + + + + 6
        * 5 +     + + +     + 5
        * 4 + + + + + + + + + 4
        * 3 + + + +   + + + + 3
        * 2 + + + +   + + + + 2
        * 1 + + + + + + + + + 1
        *   A B C D E F G H J

        100 101 102 103 104 105 106 107 108 109 110
		90  91  92  93  94  95  96  97  98  99
		80  81  82  83  84 *85  86  87  88  89
		70  71  72  73  74 *75  76  77  78  79
        60	61	62	63	64	65	66	67	68	69
        50	51 *52 *53	54	55	56 *57 *58	59
        40	41	42	43	44	45	46	47	48	49
        30	31	32	33	34 *35	36	37	38	39
        20	21	22	23	24 *25	26	27	28	29
        10	11	12	13	14	15	16	17	18	19
        0   1   2   3   4   5   6   7   8   9
        """
        self.pieces[25] = BORDER
        self.pieces[35] = BORDER
        self.pieces[52] = BORDER
        self.pieces[53] = BORDER
        self.pieces[57] = BORDER
        self.pieces[58] = BORDER
        self.pieces[75] = BORDER
        self.pieces[85] = BORDER
	
    def _initialize_neighbors(self):
        """
        precompute neighbor array.
        For each point on the board, store its list of on-the-board neighbors
        """
        self.neighbors = []
        for point in range(self.maxpoint):
            if self.pieces[point] == BORDER:
                self.neighbors.append([])
            else:
                self.neighbors.append(self._on_board_neighbors(point))

    def _on_board_neighbors(self, point):
        nbs = []
        for nb in self._neighbors(point):
            if self.pieces[nb] != BORDER:
                nbs.append(nb)
        return nbs
    
    def _neighbors(self, point):
        """ List of all four neighbors of the point """
        return [point - 1, point + 1, point - self.NS, point + self.NS]

    def __getitem__(self, index):
        #point = y * (self.size + 1) + x
        return self.pieces[index]

    def play_move(self, point, color, show_error=False):
        """
        Play a move of color on point
        Returns boolean: whether move was legal
        """
        # Special cases
        if point == None:
            if show_error: print("action ilegel : none action")
            return False
        elif self.pieces[point] != EMPTY:
            if show_error: print("action ilegel : occupied")
            return False
        elif self.pieces[point] == BORDER:
            if show_error: print("action ilegel : is border")
            return False
        elif point > self.maxpoint or point < 0:
            if show_error: print("action ilegel : out of range")
            return False
        
        # General case: deal with captures, suicide, and next ko point
        opp_color = self._get_opponent_color(color)
        in_enemy_eye = self._is_surrounded(point, opp_color)
        self.pieces[point] = color
        
        # check suicide of whole block
        if not self._stone_has_liberty(point):
            block = self._block_of(point)
            if not self._has_liberty(block): 
                if show_error: print("action ilegel : suicide")
                return False

        neighbors = self.neighbors[point]
        for nb in neighbors:
            if self.pieces[nb] == opp_color:
                if self._detect_and_process_capture(nb) == True:
                    if show_error: print("action ilegel : capture")
                    return False
        
        self.current_player = self._get_opponent_color(color)
        return True

    def set_pieces(self, board):
        #assert (len(board) == self.maxpoint) or (len(board) == self.size**2)
        #if len(board) == self.maxpoint:
        #    self.pieces = board
        #else:
            for row in range(1, self.size + 1):
                start = self.row_start(row)
                start_2d = (row-1)*self.size
                self.pieces[start : start + self.size] = board[start_2d : start_2d + self.size]

    @staticmethod
    def _is_black_white(color):
        return color == BLACK or color == WHITE

    @staticmethod
    def _get_opponent_color(color):
        return WHITE + BLACK - color

    def _stone_has_liberty(self, stone):
        return len(self.neighbors_of_color(stone, EMPTY)) != 0

    def _has_liberty(self, block):
        """
        Check if the given block has any liberty.
        block is a numpy boolean array
        """
        for stone in where1d(block):
            if self._stone_has_liberty(stone):
                return True
        return False

    def _block_of(self, stone):
        """
        Find the block of given stone
        Returns a board of boolean markers which are set for
        all the points in the block 
        """
        marker = [False]*self.maxpoint
        pointstack = [stone]
        color = self.pieces[stone]
        assert self._is_black_white(color)
        marker[stone] = True
        while pointstack:
            p = pointstack.pop()
            neighbors = self.neighbors_of_color(p, color)
            for nb in neighbors:
                if not marker[nb]:
                    marker[nb] = True
                    pointstack.append(nb)
        return marker

    def neighbors_of_color(self, point, color):
        """ List of neighbors of point of given color """
        nbc = []
        for nb in self.neighbors[point]:
            if self.pieces[nb] == color:
                nbc.append(nb)
        return nbc

    def _detect_and_process_capture(self, nb_point):
        """
        Check whether opponent block on nb_point is captured.
        If yes, remove the stones.
        Returns the stone if only a single stone was captured,
            and returns None otherwise.
        This result is used in play_move to check for possible ko
        """
        opp_block = self._block_of(nb_point)
        if not self._has_liberty(opp_block):
            return True
        return False
    
    def _is_surrounded(self, point, color):
        """
        check whether empty point is surrounded by stones of color.
        """
        for nb in self.neighbors[point]:
            nb_color = self.pieces[nb]
            if nb_color != color:
                return False
        return True

    def __repr__(self):
        repr_str = ''
        for row in range(self.size, 0, -1):
            start = self.row_start(row)
            repr_str += str(self.pieces[start : start + self.size])
            repr_str += '\n'
        return repr_str

    def get_real_board_pieces(self):
        board = []
        for row in range(1, self.size+1):
            start = self.row_start(row)
            board += self.pieces[start : start + self.size]
        return board

    def get_legal_moves(self, color):
        """Returns all the legal moves for the given color.
        """


    def has_legal_moves(self, color):
        pass
