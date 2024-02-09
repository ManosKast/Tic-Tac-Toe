import unittest
import sys
import os

sys.path.insert(0, os.path.abspath('..'))
from Strategy import get_lines


class TestGetLines(unittest.TestCase):
    def setUp(self):
        self.empty_board = [['', '', ''], ['', '', ''], ['', '', '']]
        self.filled_board = [['X', 'O', 'X'], ['O', 'X', 'O'], ['X', 'O', 'X']]

    def test_diagonal_filled_with_same_symbol(self):
        board = [['X', 'O', 'O'], ['O', 'X', 'O'], ['O', 'O', 'X']]
        rows, cols, diagonal, antiDiagonal = get_lines(board)
        self.assertEqual(diagonal, ['X', 'X', 'X'])

    def test_anti_diagonal_filled_with_same_symbol(self):
        board = [['O', 'O', 'X'], ['O', 'X', 'O'], ['X', 'O', 'O']]
        rows, cols, diagonal, antiDiagonal = get_lines(board)
        self.assertEqual(antiDiagonal, ['X', 'X', 'X'])

    def test_rows_filled_with_different_symbols(self):
        rows, cols, diagonal, antiDiagonal = get_lines(self.filled_board)
        self.assertEqual(rows, [['X', 'O', 'X'], ['O', 'X', 'O'], ['X', 'O', 'X']])

    def test_cols_filled_with_different_symbols(self):
        rows, cols, diagonal, antiDiagonal = get_lines(self.filled_board)
        self.assertEqual(cols, [['X', 'O', 'X'], ['O', 'X', 'O'], ['X', 'O', 'X']])

    def test_empty_board_returns_empty_lines(self):
        rows, cols, diagonal, antiDiagonal = get_lines(self.empty_board)
        self.assertEqual(rows, [['', '', ''], ['', '', ''], ['', '', '']])
        self.assertEqual(cols, [['', '', ''], ['', '', ''], ['', '', '']])
        self.assertEqual(diagonal, ['', '', ''])
        self.assertEqual(antiDiagonal, ['', '', ''])


if __name__ == '__main__':
    unittest.main()
