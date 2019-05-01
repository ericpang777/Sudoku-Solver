from typing import List
from copy import deepcopy

POWEROF2 = [1, 2, 4, 8, 16, 32, 64, 128, 256] 


def print_sudoku(sudoku: List[List[int]]) -> None:
    for sublist in sudoku:
        for num in sublist:
            if num == 0:
                print("-  ", end="")
            else:
                print(str(num) + "  ", end="")
        print()


def has_duplicate(sudoku: List[List[int]]) -> bool:
    # Check for duplicates
    for i in range(9):
        row_seen = set()
        col_seen = set()
        subgrid_seen = set()

        for j in range(9):
            row_num = sudoku[i][j]
            if row_num != 0:
                if row_num in row_seen:
                    return True
                row_seen.add(row_num)

            col_num = sudoku[j][i]
            if col_num != 0:
                if col_num in col_seen:
                    return True
                col_seen.add(col_num)
                                    
            subgrid_num = sudoku[i//3*3 + j//3][i%3*3 + j%3]
            if subgrid_num != 0:
                if subgrid_num in subgrid_seen:
                    return True
                subgrid_seen.add(subgrid_num)
    return False


def solver(sudoku: List[List[int]], data: List[List[int]],
           possible_nums: List, index: int) -> bool:

    row = possible_nums[index][0]
    col = possible_nums[index][1]
    possible = possible_nums[index][2]

    for num in possible:
        power_of_2 = POWEROF2[num-1]
        if ((data[0][row] | power_of_2) != data[0][row]
            and (data[1][col] | power_of_2) != data[1][col]
            and (data[2][row//3*3 + col//3] | power_of_2) != data[2][row//3*3 + col//3]):

            sudoku[row][col] = num
            data[0][row] |= power_of_2
            data[1][col] |= power_of_2
            data[2][row//3*3 + col//3] |= power_of_2

            if index+1 == len(possible_nums) or solver(sudoku, data, possible_nums, index+1):
                return True

            sudoku[row][col] = 0
            data[0][row] ^= power_of_2
            data[1][col] ^= power_of_2
            data[2][row//3*3 + col//3] ^= power_of_2

    return False


def solve_sudoku(sudoku: List[List[int]]) -> None:
    # Check for nums not in 0-9
    for sublist in sudoku:
        for num in sublist:
            if num < 0 or 9 < num:
                return
    # Check for duplicates
    if has_duplicate(sudoku):
        return

    # Compute data of each row/col/subgrid
    data = [[], [], []]
    for i in range(9):
        row_data = 0
        col_data = 0
        subgrid_data = 0
        for j in range(9):
            row_data |= (1 << sudoku[i][j]) >> 1
            col_data |= (1 << sudoku[j][i]) >> 1
            subgrid_data |= (1 << sudoku[i//3*3 + j//3][i%3*3 + j%3]) >> 1
        data[0].append(row_data)
        data[1].append(col_data)
        data[2].append(subgrid_data)

    # Get list of possible numbers for each blank
    possible_nums = []
    for i in range(9):
        for j in range(9):
            num = sudoku[i][j]
            if num == 0:
                possible = []
                for k in range(1, 10):
                    power_of_2 = POWEROF2[k-1]
                    if ((data[0][i] | power_of_2) != data[0][i]
                        and (data[1][j] | power_of_2) != data[1][j]
                        and (data[2][i//3*3 + j//3] | power_of_2) != data[2][i//3*3 + j//3]):

                        possible.append(k)
                if len(possible) == 0:
                    return
                possible_nums.append([i, j, possible])
    if len(possible_nums) == 0:
        return
    
    possible_nums.sort(key=lambda x: len(x[2]))  # Sort by least number of possible values
    copy = deepcopy(sudoku)
    if not solver(copy, data, possible_nums, 0):
        return
    # Change the 'not' later
    for i in range(9):
        for j in range(9):
            sudoku[i][j] = copy[i][j]


def read_file(sudoku: List[List[int]], file_name: str) -> None:
    file = open("sudoku.txt", "r")
    for line in file:
        split = line.rstrip()[:-1].split(",")
        sudoku.append(list(map(int, split)))


if __name__ == "__main__":
    # Change to file read later

    sudoku = []
    read_file(sudoku, "sudoku.txt")

    print_sudoku(sudoku)
    #import time
    #start = time.time()
    solve_sudoku(sudoku)
    #end = time.time()
    #print(end-start)
    print_sudoku(sudoku)