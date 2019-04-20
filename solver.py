from typing import List
from copy import deepcopy

prime = [1,2,3,5,7,11,13,17,19,23]


def print_sudoku(sudoku: List[List[int]]) -> None:
    for sublist in sudoku:
        for num in sublist:
            if num == 0:
                print("-  ", end="")
            else:
                print(str(num) + "  ", end="")
        print()


def solve_sudoku(sudoku: List[List[int]]) -> None:
    copy = deepcopy(sudoku)


if __name__ == "__main__":
    sudoku = [[5, 3, 0, 0, 7, 0, 0, 0, 0],
              [6, 0, 0, 1, 9, 5, 0, 0, 0],
              [0, 9, 8, 0, 0, 0, 0, 6, 0],
              [8, 0, 0, 0, 6, 0, 0, 0, 3],
              [4, 0, 0, 8, 0, 3, 0, 0, 1],
              [7, 0, 0, 0, 2, 0, 0, 0, 6],
              [0, 6, 0, 0, 0, 0, 2, 8, 0],
              [0, 0, 0, 4, 1, 9, 0, 0, 5],
              [0, 0, 0, 0, 8, 0, 3, 7, 9]]
    print_sudoku(sudoku)
