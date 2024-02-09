from typing import Any
from random import choice

emptyCell: str = ''
infinity: int = 1000
playerX: str = 'X'
playerO: str = 'O'
centerBoardPosition: tuple[int, int] = (1, 1)
isEmpty: bool = True


def getActions(gameState: list[list[str]], currentPlayer: str):
    opponentPlayer = 'O' if currentPlayer == 'X' else 'X'

    if isInitialState(gameState):
        return selectRandomCorner()

    rowLines, columnLines, diagonalLine, antiDiagonalLine = extractLines(gameState)

    potentialActions = checkWinningMove(currentPlayer, rowLines, columnLines, diagonalLine, antiDiagonalLine)
    if potentialActions is not None:
        return potentialActions

    potentialActions = checkOpponentWinningMove(opponentPlayer, rowLines, columnLines, diagonalLine, antiDiagonalLine)
    if potentialActions is not None:
        return potentialActions

    potentialActions = findForkMove(currentPlayer, gameState)
    if potentialActions is not None:
        return potentialActions

    return calculatePossibleMoves(gameState)


def isInitialState(gameState: list[list[str]]) -> bool:
    for row in gameState:
        for cell in row:
            if cell != emptyCell:
                return False
    return True


def calculatePossibleMoves(gameBoard: list[list[str]]) -> list[tuple[int, int]]:
    possibleMoves = []
    for row in range(3):
        for col in range(3):
            if gameBoard[row][col] == emptyCell:
                possibleMoves.append((row, col))
    return possibleMoves


def selectRandomCorner():
    return [choice([(0, 0), (0, 2), (2, 0), (2, 2)])]


def checkOpponentStartedInCorner(gameBoard: list[list[str]]) -> bool:
    return gameBoard[0][0] == playerX or gameBoard[0][2] == playerX or gameBoard[2][0] == playerX or gameBoard[2][2] == playerX


def extractLines(gameBoard: list[list[str]]) -> tuple[list[list[str]], list[list[str]], list[str], list[str]]:
    rowLines = [row for row in gameBoard]
    columnLines = [[gameBoard[row][col] for row in range(3)] for col in range(3)]
    diagonalLine = [gameBoard[i][i] for i in range(3)]
    antiDiagonalLine = [gameBoard[2 - i][i] for i in range(3)]
    return rowLines, columnLines, diagonalLine, antiDiagonalLine


def checkThreat(symbol: str, line: list[str]) -> bool:
    return line.count(symbol) == 2 and line.count(emptyCell) == 1


def checkWinningMove(symbol: str, rowLines, columnLines, diagonalLine, antiDiagonalLine) -> list[tuple[int | Any, Any]] | None:
    for i in range(3):
        if checkThreat(symbol, rowLines[i]):
            return [(i, rowLines[i].index(emptyCell))]

        if checkThreat(symbol, columnLines[i]):
            return [(columnLines[i].index(emptyCell), i)]

    if checkThreat(symbol, diagonalLine):
        return [(diagonalLine.index(emptyCell), diagonalLine.index(emptyCell))]

    if checkThreat(symbol, antiDiagonalLine):
        return [(2 - antiDiagonalLine.index(emptyCell), antiDiagonalLine.index(emptyCell))]

    return None


def checkOpponentWinningMove(opponent: str, rowLines, columnLines, diagonalLine, antiDiagonalLine) -> list[tuple[int | Any, Any]] | None:
    return checkWinningMove(opponent, rowLines, columnLines, diagonalLine, antiDiagonalLine)


def findForkMove(symbol: str, gameBoard: list[list[str]]) -> list[tuple[int, int]] | None:
    alternativeMoves = []

    def isFork():
        rowLines, columnLines, diagonalLine, antiDiagonalLine = extractLines(gameBoard)
        threats = 0
        for row in rowLines:
            if checkThreat(symbol, row):
                threats += 1
        for col in columnLines:
            if checkThreat(symbol, col):
                threats += 1
        if checkThreat(symbol, diagonalLine):
            threats += 1
        if checkThreat(symbol, antiDiagonalLine):
            threats += 1
        if threats == 1:
            alternativeMoves.append((i, j))
        return threats >= 2

    forkMoves = []
    for i in range(3):
        for j in range(3):
            if gameBoard[i][j] == emptyCell:
                gameBoard[i][j] = symbol
                if isFork():
                    forkMoves.append((i, j))
                gameBoard[i][j] = emptyCell

    return forkMoves if len(forkMoves) > 0 else alternativeMoves if len(alternativeMoves) > 0 else None
