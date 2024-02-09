import pygame
import sys


class TicTacToeView:
    def __init__(self, size=300):
        pygame.init()
        self.size = size
        self.cell_size = size // 3
        self.screen = pygame.display.set_mode((size, size))
        pygame.display.set_caption('Tic Tac Toe')
        self.line_color = (0, 0, 0)  # Black color for lines
        self.x_color = (0, 0, 255)  # Blue color for X
        self.o_color = (255, 0, 0)  # Red color for O
        self.line_width = 4

    def draw_board(self, board):
        self.screen.fill((255, 255, 255))  # White background

        # Draw grid lines
        for i in range(1, 3):
            pygame.draw.line(self.screen, self.line_color, (0, i * self.cell_size), (self.size, i * self.cell_size),
                             self.line_width)
            pygame.draw.line(self.screen, self.line_color, (i * self.cell_size, 0), (i * self.cell_size, self.size),
                             self.line_width)

        # Draw X and O
        for row in range(3):
            for col in range(3):
                if board[row][col] == 'X':
                    self.draw_x(row, col)
                elif board[row][col] == 'O':
                    self.draw_o(row, col)

        pygame.display.flip()

    def draw_x(self, row, col):
        start_pos1 = (col * self.cell_size + 15, row * self.cell_size + 15)
        end_pos1 = (start_pos1[0] + self.cell_size - 30, start_pos1[1] + self.cell_size - 30)
        pygame.draw.line(self.screen, self.x_color, start_pos1, end_pos1, self.line_width)

        start_pos2 = (col * self.cell_size + 15, row * self.cell_size + self.cell_size - 15)
        end_pos2 = (col * self.cell_size + self.cell_size - 15, row * self.cell_size + 15)
        pygame.draw.line(self.screen, self.x_color, start_pos2, end_pos2, self.line_width)

    def draw_o(self, row, col):
        center = (col * self.cell_size + self.cell_size // 2, row * self.cell_size + self.cell_size // 2)
        radius = self.cell_size // 2 - 15
        pygame.draw.circle(self.screen, self.o_color, center, radius, self.line_width)

    # Rematch related methods
    def display_game_result(self, result):
        font = pygame.font.Font(None, 36)
        text = font.render(result, True, (0, 128, 0))  # Using green color for the text
        text_rect = text.get_rect(center=(self.size/2, self.size/2 - 20))
        self.screen.blit(text, text_rect)

        rematch_text = font.render("Click for Rematch", True, (0, 128, 0))
        rematch_rect = rematch_text.get_rect(center=(self.size/2, self.size/2 + 20))
        self.screen.blit(rematch_text, rematch_rect)

        pygame.display.update()

    def check_for_rematch(self, mouse_pos):
        # Assuming the rematch option is in the middle of the screen
        center_x, center_y = self.size / 2, self.size / 2 + 20
        if (center_x - 50 < mouse_pos[0] < center_x + 50) and \
           (center_y - 10 < mouse_pos[1] < center_y + 10):
            return True
        return False

    # Symbol choice related methods
    def display_symbol_choice(self):
        font = pygame.font.Font(None, 36)
        x_text = font.render("X", True, (0, 128, 0))
        o_text = font.render("O", True, (0, 128, 0))

        x_rect = x_text.get_rect(center=(self.size/2 - 50, self.size/2))
        o_rect = o_text.get_rect(center=(self.size/2 + 50, self.size/2))

        self.screen.blit(x_text, x_rect)
        self.screen.blit(o_text, o_rect)

        pygame.display.update()

    def check_symbol_choice(self, mouse_pos):
        # Check if the click is on the "Play as X" or "Play as O" text
        # Assuming the "Play as X" is on the left and "Play as O" is on the right
        if self.size/2 - 100 < mouse_pos[0] < self.size/2:
            return 'X'
        elif self.size/2 < mouse_pos[0] < self.size/2 + 100:
            return 'O'
        return None

    def draw_winning_line(self, start, end, winner):
        start_y, start_x = start
        end_y, end_x = end
        # Convert grid coordinates to pixel coordinates
        start_x, start_y = start_x * self.cell_size + self.cell_size // 2, start_y * self.cell_size + self.cell_size // 2
        end_x, end_y = end_x * self.cell_size + self.cell_size // 2, end_y * self.cell_size + self.cell_size // 2
        # Set color based on the winner
        color = self.x_color if winner == 'X' else self.o_color
        pygame.draw.line(self.screen, color, (start_x, start_y), (end_x, end_y), self.line_width)