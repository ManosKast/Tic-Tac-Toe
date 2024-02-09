import pygame
import sys
from TicTacToeAI import TicTacToeAI, wins  # Make sure to import your AI class
from model import TicTacToeView  # Import your View class


def get_winning_line(board, player):
    # Check rows
    for row in range(3):
        if all(board[row][col] == player for col in range(3)):
            return (row, 0), (row, 2)
    # Check columns
    for col in range(3):
        if all(board[row][col] == player for row in range(3)):
            return (0, col), (2, col)
    # Check diagonals
    if all(board[i][i] == player for i in range(3)):
        return (0, 0), (2, 2)
    if all(board[i][2 - i] == player for i in range(3)):
        return (0, 2), (2, 0)
    return None


class TicTacToeController:
    def __init__(self):
        self.view = TicTacToeView()
        self.ai = None
        self.board = [['' for _ in range(3)] for _ in range(3)]
        self.current_player = ''  # Will be set after symbol choice
        self.game_over = False
        self.turn = 'X'  # 'X' always starts first

    def run(self):
        self.choose_symbol()  # Start with symbol choice screen
        while True:
            if self.is_terminal():
                self.wait_for_rematch()

            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if self.is_players_turn():
                        self.handle_mouse_click(event.pos)

            if not self.is_players_turn():
                self.ai_move()
            self.view.draw_board(self.board)  # Draw the board one last time when the game is over
            pygame.time.wait(50)  # Reduce the wait time to 50 milliseconds

    def choose_symbol(self):
        self.view.display_symbol_choice()
        symbol_chosen = False
        while not symbol_chosen:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    symbol = self.view.check_symbol_choice(event.pos)
                    if symbol:
                        self.current_player = symbol
                        self.ai = TicTacToeAI('X' if symbol == 'O' else 'O')
                        symbol_chosen = True
                        # If player chooses 'O', AI makes the first move as 'X'
                        if symbol == 'O':
                            self.ai_move()

    def ai_move(self):
        ai_move = self.ai.getNextMove(self.board)
        if ai_move:
            self.board[ai_move[0]][ai_move[1]] = self.ai.ai
            self.switch_turn()

    def handle_mouse_click(self, position):
        row, col = position[1] // self.view.cell_size, position[0] // self.view.cell_size
        if self.board[row][col] == '' and not self.game_over:
            self.make_move(row, col, self.current_player)

    def make_move(self, row, col, player):
        self.board[row][col] = player
        self.switch_turn()

    def switch_turn(self):
        self.turn = 'X' if self.turn == 'O' else 'O'

    def is_terminal(self):
        def check_win():
            # Assuming is_winner is a function that checks if a player has won
            if wins(self.current_player, self.board):
                self.view.display_game_result(f'You Win!')
                # Assuming get_winning_line is a function that returns the start and end of the winning line
                start, end = get_winning_line(self.board, self.current_player)
                self.view.draw_winning_line(start, end, self.current_player)
                self.game_over = True
                return True
            return False

        def check_loss():
            # Assuming is_winner is a function that checks if a player has won
            if wins(self.ai.ai, self.board):
                # Assuming get_winning_line is a function that returns the start and end of the winning line
                start, end = get_winning_line(self.board, self.ai.ai)
                self.view.draw_winning_line(start, end, self.ai.ai)
                self.view.display_game_result(f'You Lose!')
                self.game_over = True
                return True
            return False

        def check_draw():
            # Check if all cells on the board are filled
            if all(self.board[row][col] != '' for row in range(3) for col in range(3)):
                self.view.display_game_result('Draw!')
                self.game_over = True
                return True
            return False

        return check_win() or check_loss() or check_draw()

    def is_players_turn(self):
        return self.current_player == self.turn

    def rematch(self):
        # Reset the game state
        self.board = [['' for _ in range(3)] for _ in range(3)]
        self.game_over = False
        # Switch the player's symbol
        self.current_player = 'X' if self.current_player == 'O' else 'O'
        self.ai = TicTacToeAI('X' if self.current_player == 'O' else 'O')
        self.turn = 'X'

    def wait_for_rematch(self):
        rematch: bool = False
        while not rematch:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    if self.view.check_for_rematch(event.pos):
                        self.rematch()
                        rematch = True
            pygame.time.wait(50)


if __name__ == "__main__":
    controller = TicTacToeController()
    controller.run()
