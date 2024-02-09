import unittest
import sys
import os

sys.path.insert(0, os.path.abspath('..'))
from Strategy import can_win


class TestCanWin(unittest.TestCase):
    def setUp(self):
        self.rows_with_threat = [['X', 'X', ''],
                                 ['O', 'O', 'X'],
                                 ['X', 'O', 'O']]

        self.cols_with_threat = [['O', 'O', ''],
                                 ['X', 'X', ''],
                                 ['O', 'X', 'O']]

        self.diagonal_with_threat = ['', 'X', 'X']

        self.anti_diagonal_with_threat = ['X', '', 'X']

        self.rows_without_threat = [['X', 'O', 'X'],
                                    ['O', 'X', 'O'],
                                    ['X', 'O', 'X']]

        self.cols_without_threat = [['X', 'O', 'X'],
                                    ['O', 'X', 'O'],
                                    ['X', 'O', 'X']]

        self.diagonal_without_threat = ['X', 'O', 'X']

        self.anti_diagonal_without_threat = ['O', 'X', 'O']

    def test_can_win_with_threat_in_rows(self):
        result = can_win('X', self.rows_with_threat, self.cols_without_threat, self.diagonal_without_threat,
                         self.anti_diagonal_without_threat)
        self.assertIsNotNone(result)

    def test_can_win_with_threat_in_cols(self):
        result = can_win('X', self.rows_without_threat, self.cols_with_threat, self.diagonal_without_threat,
                         self.anti_diagonal_without_threat)
        self.assertIsNotNone(result)

    def test_can_win_with_threat_in_diagonal(self):
        result = can_win('X', self.rows_without_threat, self.cols_without_threat, self.diagonal_with_threat,
                         self.anti_diagonal_without_threat)
        self.assertIsNotNone(result)

    def test_can_win_with_threat_in_anti_diagonal(self):
        result = can_win('X', self.rows_without_threat, self.cols_without_threat, self.diagonal_without_threat,
                         self.anti_diagonal_with_threat)
        self.assertIsNotNone(result)

    def test_can_win_without_threat(self):
        result = can_win('X', self.rows_without_threat, self.cols_without_threat, self.diagonal_without_threat,
                         self.anti_diagonal_without_threat)
        self.assertIsNone(result)

if __name__ == '__main__':
    unittest.main()