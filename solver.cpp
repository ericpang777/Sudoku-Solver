#include<array>
#include<iostream>
#include<set>
#include<vector>

using namespace std;

const int PRIME[10] = {1, 2, 3, 5, 7, 11, 13, 17, 19, 23}; // 1 not prime
const int ROW_PRODUCTS = 0;
const int COL_PRODUCTS = 1;
const int SUBGRID_PRODUCTS = 2;
const int PRIME_PRODUCT = 223092870;

void print_sudoku(array<array<int, 9>, 9> sudoku) {
    for(array<int, 9> row : sudoku) {
        for(int num : row) {
            cout << num << "  ";
        }
        cout << "\n";
    }
}

bool has_duplicate(array<array<int, 9>, 9> sudoku) {
    for(int i = 0; i < 9; i++) {
        set<int> row_seen;
        set<int> col_seen;
        set<int> subgrid_seen;

        for(int j = 0; j < 9; j++) {
            int row_num = sudoku[i][j];
            if(row_num != 0) {
                if(row_seen.find(row_num) != row_seen.end()) {
                    return true;
                }
                row_seen.insert(row_num);
            }

            int col_num = sudoku[j][i];
            if(col_num != 0) {
                if(col_seen.find(col_num) != col_seen.end()) {
                    return true;
                }
                col_seen.insert(col_num);
            }

            int subgrid_num = sudoku[i/3*3 + j/3][i%3*3 + j%3];
            if(subgrid_num != 0) {
                if(subgrid_seen.find(subgrid_num) != subgrid_seen.end()) {
                    return true;
                }
                subgrid_seen.insert(subgrid_num);
            }
        }
    }
    return false;
}

bool product_check() {

}

bool solver() {

}

void solve_sudoku(array<array<int, 9>, 9> sudoku) {
    // Check nums not in 0-9
    for(array<int, 9> row : sudoku) {
        for(int num : row) {
            if(num < 0 || 9 < num) {
                return;
            }
        }
    }
    // Check dupliates
    if(has_duplicate(sudoku)) {
        return;
    }

    // Compute products of each row/col/subgrid
    array<array<int, 9>, 3> products;
    for(int i = 0; i < 9; i++) {
        int row_product = 1;
        int col_product = 1;
        int subgrid_product = 1;
        for(int j = 0; j < 9; j++) {
            row_product *= PRIME[sudoku[i][j]];
            col_product *= PRIME[sudoku[j][i]];
            subgrid_product *= PRIME[sudoku[i/3*3 + j/3][i%3*3 + j%3]];
        }
        products[ROW_PRODUCTS][i] = row_product;
        products[COL_PRODUCTS][i] = col_product;
        products[SUBGRID_PRODUCTS][i] = subgrid_product;
    }

    // Get list of possible numbers for each blank
    
    for i in range(9):
        for j in range(9):
            num = sudoku[i][j]
            possible = []
            if num == 0:
                for k in range(1, 10):
                    if (products[ROW_PRODUCTS][i] % PRIME[k] != 0
                        and products[COL_PRODUCTS][j] % PRIME[k] != 0
                        and products[SUBGRID_PRODUCTS][i//3*3 + j//3]) % PRIME[k] != 0:

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

    for i in range(9):
        for j in range(9):
            sudoku[i][j] = copy[i][j]
}

int main() {
    array<array<int, 9>, 9> sudoku = {{{5, 3, 0, 0, 7, 0, 0, 0, 0},
                                       {6, 0, 0, 1, 9, 5, 0, 0, 0},
                                       {0, 9, 8, 0, 0, 0, 0, 6, 0},
                                       {8, 0, 0, 0, 6, 0, 0, 0, 3},
                                       {4, 0, 0, 8, 0, 3, 0, 0, 1},
                                       {7, 0, 0, 0, 2, 0, 0, 0, 6},
                                       {0, 6, 0, 0, 0, 0, 2, 8, 0},
                                       {0, 0, 0, 4, 1, 9, 0, 0, 5},
                                       {0, 0, 0, 0, 8, 0, 0, 7, 9}}};
    print_sudoku(sudoku);
    return 0;
}