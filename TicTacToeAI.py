from Strategy import getActions

EMPTY: str = ''
inf: int = 1000
PLAYER_1: str = 'X'
PLAYER_2: str = 'O'
CENTER_POSITION: tuple[int, int] = (1, 1)


def copy(board: list[list[str]]) -> list[list[str]]:
    return [row.copy() for row in board]


def wins(symbol: str, board: list[list[str]]) -> bool:
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

    def is_antiDiagonal_win():
        return all(board[2 - i][i] == symbol for i in range(3))

    return is_horizontal_win() or is_vertical_win() or is_diagonal_win() or is_antiDiagonal_win()


def is_terminal(board: list[list[str]]) -> bool:
    return wins(PLAYER_1, board) or wins(PLAYER_2, board) or all(cell != EMPTY for row in board for cell in row)


def update_state(coordinates: tuple[int, int], board: list[list[str]], symbol):
    [x, y] = coordinates
    updated_board = copy(board)
    updated_board[x][y] = symbol
    return updated_board


class TicTacToeAI:
    def __init__(self, player: str):
        global PLAYER_1, PLAYER_2

        self.ai = player
        self.opponent = 'X' if player == 'O' else 'O'
        PLAYER_1 = self.opponent
        PLAYER_2 = self.ai

    def utility(self, board):
        return 1 if wins(self.ai, board) else -1 if wins(self.opponent, board) else 0

    def getNextMove(self, board):
        def miniMax(state):
            return getMax(state)[1]

        def getMax(state: list[list[str]]):
            if is_terminal(state):
                return self.utility(state), None

            nonlocal alpha, beta
            actions, maxValue, bestAction = getActions(state, self.ai), -inf, None
            for action in actions:
                result, _ = getMin(update_state(action, state, self.ai))
                if result > maxValue:
                    maxValue, bestAction = result, action
                    alpha = max(result, alpha)
                if maxValue > 0 and maxValue > beta:
                    break

            return maxValue, bestAction

        # remove bestAction from min.
        def getMin(state: list[list[str]]):
            if is_terminal(state):
                return self.utility(state), None

            nonlocal alpha, beta
            actions, minValue, bestAction = getActions(state, self.opponent), inf, None

            for action in actions:
                result, _ = getMax(update_state(action, state, self.opponent))
                if result < minValue:
                    minValue, bestAction = result, action
                    beta = min(result, beta)
                if minValue < 0 and minValue < alpha:
                    break

            return minValue, bestAction

        alpha, beta = -inf, inf
        return miniMax(board)

    def rematch(self):
        global PLAYER_1, PLAYER_2
        self.ai = 'X' if self.ai == 'O' else 'O'
        self.opponent = 'X' if self.ai == 'O' else 'O'
        PLAYER_1 = self.opponent
        PLAYER_2 = self.ai

