from typing import Any
from random import choice

empty_cell: str = ''
infinity: int = 10000
player_x: str = 'X'
player_o: str = 'O'
center_board_position: tuple[int, int] = (1, 1)
is_empty: bool = True


def get_actions(game_state: list[list[str]], current_player: str):
    opponent_player = 'O' if current_player == 'X' else 'X'

    if is_initial_state(game_state):
        return select_random_corner()

    row_lines, column_lines, diagonal_line, anti_diagonal_line = extract_lines(game_state)

    potential_actions = check_winning_move(current_player, row_lines, column_lines, diagonal_line, anti_diagonal_line)
    if potential_actions is not None:
        return potential_actions

    potential_actions = check_opponent_winning_move(opponent_player, row_lines, column_lines,
                                                    diagonal_line, anti_diagonal_line)
    if potential_actions is not None:
        return potential_actions

    potential_actions = find_fork_move(current_player, game_state)
    if potential_actions is not None:
        return potential_actions

    return calculate_possible_moves(game_state)


def is_initial_state(game_state: list[list[str]]) -> bool:
    for row in game_state:
        for cell in row:
            if cell != empty_cell:
                return False
    return True


def calculate_possible_moves(game_board: list[list[str]]) -> list[tuple[int, int]]:
    possible_moves = []
    for row in range(3):
        for col in range(3):
            if game_board[row][col] == empty_cell:
                possible_moves.append((row, col))
    return possible_moves


def select_random_corner():
    return [choice([(0, 0), (0, 2), (2, 0), (2, 2)])]


def check_opponent_started_in_corner(game_board: list[list[str]]) -> bool:
    return game_board[0][0] == player_x or game_board[0][2] == player_x or game_board[2][0] == player_x or \
        game_board[2][2] == player_x


def extract_lines(game_board: list[list[str]]) -> tuple[list[list[str]], list[list[str]], list[str], list[str]]:
    row_lines = [row for row in game_board]
    column_lines = [[game_board[row][col] for row in range(3)] for col in range(3)]
    diagonal_line = [game_board[i][i] for i in range(3)]
    anti_diagonal_line = [game_board[2 - i][i] for i in range(3)]
    return row_lines, column_lines, diagonal_line, anti_diagonal_line


def check_threat(symbol: str, line: list[str]) -> bool:
    return line.count(symbol) == 2 and line.count(empty_cell) == 1


def check_winning_move(symbol: str, row_lines, column_lines, diagonal_line, anti_diagonal_line) -> \
                                                                                    list[tuple[int | Any, Any]] | None:
    for i in range(3):
        if check_threat(symbol, row_lines[i]):
            return [(i, row_lines[i].index(empty_cell))]

        if check_threat(symbol, column_lines[i]):
            return [(column_lines[i].index(empty_cell), i)]

    if check_threat(symbol, diagonal_line):
        return [(diagonal_line.index(empty_cell), diagonal_line.index(empty_cell))]

    if check_threat(symbol, anti_diagonal_line):
        return [(2 - anti_diagonal_line.index(empty_cell), anti_diagonal_line.index(empty_cell))]

    return None


def check_opponent_winning_move(opponent: str, row_lines, column_lines, diagonal_line, anti_diagonal_line) -> \
                                                                                    list[tuple[int | Any, Any]] | None:
    return check_winning_move(opponent, row_lines, column_lines, diagonal_line, anti_diagonal_line)


def find_fork_move(symbol: str, game_board: list[list[str]]) -> list[tuple[int, int]] | None:
    alternative_moves = []

    def is_fork():
        row_lines, column_lines, diagonal_line, anti_diagonal_line = extract_lines(game_board)
        threats = 0
        for row in row_lines:
            if check_threat(symbol, row):
                threats += 1
        for col in column_lines:
            if check_threat(symbol, col):
                threats += 1
        if check_threat(symbol, diagonal_line):
            threats += 1
        if check_threat(symbol, anti_diagonal_line):
            threats += 1
        if threats == 1:
            alternative_moves.append((i, j))
        return threats >= 2

    fork_moves = []
    for i in range(3):
        for j in range(3):
            if game_board[i][j] == empty_cell:
                game_board[i][j] = symbol
                if is_fork():
                    fork_moves.append((i, j))
                game_board[i][j] = empty_cell

    return fork_moves if len(fork_moves) > 0 else alternative_moves if len(alternative_moves) > 0 else None
