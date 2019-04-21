from typing import List
from copy import deepcopy

PRIME = [1, 2, 3, 5, 7, 11, 13, 17, 19, 23]  # 1 not prime
ROW_PRODUCTS = 0
COL_PRODUCTS = 1
SUBGRID_PRODUCTS = 2
PRIME_PRODUCT = 223092870


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


def product_check(products: List[List[int]]) -> bool:
    # Change to only look at affected row,col area if needed
    # Check for completed sudoku
    for sublist in products:
        for product in sublist:
            if product != PRIME_PRODUCT:
                return False
    return True


def solver(sudoku: List[List[int]], products: List[List[int]],
           possible_nums: List, index: int) -> bool:

    if product_check(products):
        return True
    row = possible_nums[index][0]
    col = possible_nums[index][1]
    possible = possible_nums[index][2]

    for num in possible:
        if (products[ROW_PRODUCTS][row] % PRIME[num] != 0
            and products[COL_PRODUCTS][col] % PRIME[num] != 0
            and products[SUBGRID_PRODUCTS][row//3*3 + col//3] % PRIME[num] != 0):

            sudoku[row][col] = num
            products[ROW_PRODUCTS][row] *= PRIME[num]
            products[COL_PRODUCTS][col] *= PRIME[num]
            products[SUBGRID_PRODUCTS][row//3*3 + col//3] *= PRIME[num]

            if index+1 == len(possible_nums):
                return True
            elif solver(sudoku, products, possible_nums, index+1):
                return True

            sudoku[row][col] = 0
            products[ROW_PRODUCTS][row] /= PRIME[num]
            products[COL_PRODUCTS][col] /= PRIME[num]
            products[SUBGRID_PRODUCTS][row//3*3 + col//3] /= PRIME[num]

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

    # Compute products of each row/col/subgrid
    products = [[], [], []]
    for i in range(9):
        row_product = 1
        col_product = 1
        subgrid_product = 1
        for j in range(9):
            row_product *= PRIME[sudoku[i][j]]
            col_product *= PRIME[sudoku[j][i]]
            subgrid_product *= PRIME[sudoku[i//3*3 + j//3][i%3*3 + j%3]]
        products[ROW_PRODUCTS].append(row_product)
        products[COL_PRODUCTS].append(col_product)
        products[SUBGRID_PRODUCTS].append(subgrid_product)

    # Get list of possible numbers for each blank
    possible_nums = []
    for i in range(9):
        for j in range(9):
            num = sudoku[i][j]
            possible = []
            if num == 0:
                for k in range(1, 10):
                    if (products[ROW_PRODUCTS][i] % PRIME[k] != 0
                        and products[COL_PRODUCTS][j] % PRIME[k] != 0
                        and products[SUBGRID_PRODUCTS][i//3*3 + j//3] % PRIME[k] != 0):

                        possible.append(k)
                if len(possible) == 0:
                    return
                possible_nums.append([i, j, possible])
    if len(possible_nums) == 0:
        return
    
    possible_nums.sort(key=lambda x: len(x[2]))  # Sort by least number of possible values
    copy = deepcopy(sudoku)
    if not solver(copy, products, possible_nums, 0):
        return
    # Change the 'not' later
    for i in range(9):
        for j in range(9):
            sudoku[i][j] = copy[i][j]


if __name__ == "__main__":
    # Change to file read later

    sudoku = [[5, 3, 0, 0, 7, 0, 0, 0, 0],
              [6, 0, 0, 1, 9, 5, 0, 0, 0],
              [0, 9, 8, 0, 0, 0, 0, 6, 0],
              [8, 0, 0, 0, 6, 0, 0, 0, 3],
              [4, 0, 0, 8, 0, 3, 0, 0, 1],
              [7, 0, 0, 0, 2, 0, 0, 0, 6],
              [0, 6, 0, 0, 0, 0, 2, 8, 0],
              [0, 0, 0, 4, 1, 9, 0, 0, 5],
              [0, 0, 0, 0, 8, 0, 0, 7, 9]]

    print_sudoku(sudoku)
    #import time
    #start = time.time()
    solve_sudoku(sudoku)
    #end = time.time()
    #print(end-start)
    print_sudoku(sudoku)
