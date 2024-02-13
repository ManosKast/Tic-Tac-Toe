from Strategy import get_actions

EMPTY_CELL: str = ''
INFINITY: int = 1000
PLAYER_X: str = 'X'
PLAYER_O: str = 'O'
CENTER_POSITION: tuple[int, int] = (1, 1)


def copy_board(board: list[list[str]]) -> list[list[str]]:
    return [row.copy() for row in board]


def check_wins(symbol: str, board: list[list[str]]) -> bool:
    def is_horizontal_win():
        for row in board:
            if all(cell == symbol for cell in row):
                return True
        return False

    def is_vertical_win():
        for i in range(3):
            if all(board[j][i] == symbol for j in range(3)):
                return True
        return False

    def is_diagonal_win():
        return all(board[i][i] == symbol for i in range(3))

    def is_anti_diagonal_win():
        return all(board[2 - i][i] == symbol for i in range(3))

    return is_horizontal_win() or is_vertical_win() or is_diagonal_win() or is_anti_diagonal_win()


def is_terminal_state(board: list[list[str]]) -> bool:
    return check_wins(PLAYER_X, board) or check_wins(PLAYER_O, board) or all(cell != EMPTY_CELL for row in board for cell in row)


def update_game_state(coordinates: tuple[int, int], board: list[list[str]], symbol):
    [x, y] = coordinates
    updated_board = copy_board(board)
    updated_board[x][y] = symbol
    return updated_board


class TicTacToeAI:
    def __init__(self, player: str):
        global PLAYER_X, PLAYER_O

        self.ai_symbol = player
        self.opponent_player = 'X' if player == 'O' else 'O'
        PLAYER_X = self.opponent_player
        PLAYER_O = self.ai_symbol

    def calculate_utility(self, board):
        return 1 if check_wins(self.ai_symbol, board) else -1 if check_wins(self.opponent_player, board) else 0

    def get_next_move(self, board):
        def mini_max(state):
            return get_max(state)[1]

        def get_max(state: list[list[str]]):
            if is_terminal_state(state):
                return self.calculate_utility(state), None

            nonlocal alpha, beta
            actions, max_value, best_action = get_actions(state, self.ai_symbol), -INFINITY, None
            for action in actions:
                result, _ = get_min(update_game_state(action, state, self.ai_symbol))
                if result > max_value:
                    max_value, best_action = result, action
                    alpha = max(result, alpha)
                if max_value > 0 and max_value > beta:
                    break

            return max_value, best_action

        def get_min(state: list[list[str]]):
            if is_terminal_state(state):
                return self.calculate_utility(state), None

            nonlocal alpha, beta
            actions, min_value, best_action = get_actions(state, self.opponent_player), INFINITY, None

            for action in actions:
                result, _ = get_max(update_game_state(action, state, self.opponent_player))
                if result < min_value:
                    min_value, best_action = result, action
                    beta = min(result, beta)
                if min_value < 0 and min_value < alpha:
                    break

            return min_value, best_action

        alpha, beta = -INFINITY, INFINITY
        return mini_max(board)

    def rematch(self):
        global PLAYER_X, PLAYER_O
        self.ai_symbol = 'X' if self.ai_symbol == 'O' else 'O'
        self.opponent_player = 'X' if self.ai_symbol == 'O' else 'O'
        PLAYER_X = self.opponent_player
        PLAYER_O = self.ai_symbol
