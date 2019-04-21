from typing import List
from copy import deepcopy

PRIME = [1, 2, 3, 5, 7, 11, 13, 17, 19, 23] # 1 not prime
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


def check_valid(sudoku: List[List[int]]) -> bool:
    # Check for duplicates
    for i in range(9):
        row_seen = set()
        col_seen = set()
        subgrid_seen = set()

        for j in range(9):
            row_num = sudoku[i][j]
            if row_num != 0:
                if row_num in row_seen:
                    return False
                row_seen.add(row_num)

            col_num = sudoku[j][i]
            if col_num != 0:
                if col_num in col_seen:
                    return False
                col_seen.add(col_num)
                
            subgrid_num = sudoku[i//3*3 + j//3][i%3*3 + j%3]
            if subgrid_num != 0:
                if subgrid_num in subgrid_seen:
                    return False
                subgrid_seen.add(subgrid_num)
    return True


def product_check(products: List[List[int]]) -> bool:
    # Change to only look at affected row,col area if needed
    # Check for completed sudoku
    for sublist in products:
        for product in sublist:
            if product != PRIME_PRODUCT:
                return False
    return True


def solver(sudoku: List[List[int]], products: List[List[int]],
           possible_nums: List[List[int]], row: int, col: int) -> bool:

    if product_check(products):
        return True

    if sudoku[row][col] == 0:
        for num in possible_nums[row*9 + col]:
            if (products[ROW_PRODUCTS][row] % PRIME[num] != 0
                and products[COL_PRODUCTS][col] % PRIME[num] != 0
                and products[SUBGRID_PRODUCTS][row//3*3 + col//3] % PRIME[num] != 0):

                sudoku[row][col] = num
                products[ROW_PRODUCTS][row] *= PRIME[num]
                products[COL_PRODUCTS][col] *= PRIME[num]
                products[SUBGRID_PRODUCTS][row//3*3 + col//3] *= PRIME[num]

                if col+1 == 9:
                    if row+1 == 9:
                        return True
                    elif solver(sudoku, products, possible_nums, row+1, 0):
                        return True
                elif solver(sudoku, products, possible_nums, row, col+1):
                    return True

                sudoku[row][col] = 0
                products[ROW_PRODUCTS][row] /= PRIME[num]
                products[COL_PRODUCTS][col] /= PRIME[num]
                products[SUBGRID_PRODUCTS][row//3*3 + col//3] /= PRIME[num]
    else:
        if col + 1 == 9:
            if row + 1 == 9:
                return True
            elif solver(sudoku, products, possible_nums, row+1, 0):
                return True
        elif solver(sudoku, products, possible_nums, row, col+1):
            return True
    return False


def solve_sudoku(sudoku: List[List[int]]) -> None:
    # Check for nums not in 0-9
    for sublist in sudoku:
        for num in sublist:
            if num > 9 or num < 0:
                print("Not valid sudoku")
                return
    # Check for duplicates
    if not check_valid(sudoku):
        print("Not valid sudoku")
        return

    # Compute products of each row/col/subgrid
    products = [[],[],[]]
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
                for k in range(1,10):
                    if (products[ROW_PRODUCTS][i] % PRIME[k] != 0
                        and products[COL_PRODUCTS][j] % PRIME[k] != 0
                        and products[SUBGRID_PRODUCTS][i//3*3 + j//3]) % PRIME[k] != 0:

                        possible.append(k)
                if len(possible) == 0:
                    print("No valid solution possible")
                    return
            possible_nums.append(possible)

    copy = deepcopy(sudoku)
    if solver(copy, products, possible_nums, 0, 0, 0):
        for i in range(9):
            for j in range(9):
                sudoku[i][j] = copy[i][j]
        return
    print("No valid solution possible")

if __name__ == "__main__":
    # Change to file read later
    sudoku = [[0, 0, 0, 0, 0, 0, 0, 0, 0],
		     [0, 0, 0, 0, 0, 3, 0, 8, 5],
		     [0, 0, 1, 0, 2, 0, 0, 0, 0],
		     [0, 0, 0, 5, 0, 7, 0, 0, 0],
		     [0, 0, 4, 0, 0, 0, 1, 0, 0],
		     [0, 9, 0, 0, 0, 0, 0, 0, 0],
		     [5, 0, 0, 0, 0, 0, 0, 7, 3],
		     [0, 0, 2, 0, 1, 0, 0, 0, 0],
		     [0, 0, 0, 0, 4, 0, 0, 0, 9]]
    print_sudoku(sudoku)
    import time
    start = time.time()
    solve_sudoku(sudoku)
    end = time.time()
    print(end-start)
    print_sudoku(sudoku)
