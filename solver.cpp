#include<algorithm>
#include<cstring>
#include<iostream>
#include<set>
#include<vector>

#include "avail_nums.h"

const int PRIME[10] = {1, 2, 3, 5, 7, 11, 13, 17, 19, 23}; // 1 not prime
const int ROW_PRODUCTS = 0;
const int COL_PRODUCTS = 1;
const int SUBGRID_PRODUCTS = 2;
const int PRIME_PRODUCT = 223092870;

void print_sudoku(int sudoku[9][9]) 
{
    std::cout << "\n";
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(sudoku[i][j] == 0) {
                std::cout << "-  ";
            } else {
                std::cout << sudoku[i][j] << "  ";
            }
        }
        std::cout << "\n";
    }
}

bool has_duplicate(int sudoku[9][9]) 
{
    for(int i = 0; i < 9; i++) {
        std::set<int> row_seen;
        std::set<int> col_seen;
        std::set<int> subgrid_seen;

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

bool product_check(int products[3][9]) 
{
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 9; j++) {
            if(products[i][j] != PRIME_PRODUCT) {
                return false;
            }
        }
    }
    return true;
}

bool solver(int sudoku[9][9], 
            int products[3][9], 
            std::vector<avail_nums> &possible_nums, int index) 
{
    if(product_check(products)) {
        return true;
    }
    int row = possible_nums[index].get_row();
    int col = possible_nums[index].get_col();
    int length = possible_nums[index].len();

    for(int i = 0; i < length; i++) {
        int num = possible_nums[index].get_index(i);
        if(products[ROW_PRODUCTS][row] % PRIME[num] != 0
           && products[COL_PRODUCTS][col] % PRIME[num] != 0
           && products[SUBGRID_PRODUCTS][row/3*3 + col/3] % PRIME[num] != 0)
        {
            sudoku[row][col] = num;
            products[ROW_PRODUCTS][row] *= PRIME[num];
            products[COL_PRODUCTS][col] *= PRIME[num];
            products[SUBGRID_PRODUCTS][row/3*3 + col/3] *= PRIME[num];

            if((unsigned)(index + 1) == possible_nums.size()) {
                return true;
            } else if(solver(sudoku, products, possible_nums, index + 1)) {
                return true;
            }

            sudoku[row][col] = 0;
            products[ROW_PRODUCTS][row] /= PRIME[num];
            products[COL_PRODUCTS][col] /= PRIME[num];
            products[SUBGRID_PRODUCTS][row/3*3 + col/3] /= PRIME[num];
        }
    }
    return false;
}

bool avail_nums_compare(avail_nums a, avail_nums b) 
{
    return a.len() < b.len();
}

void solve_sudoku(int sudoku[9][9]) 
{
    // Check nums not in 0-9
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(sudoku[i][j] < 0 || 9 < sudoku[i][j]) {
                return;
            }
        }
    }
    // Check dupliates
    if(has_duplicate(sudoku)) {
        return;
    }

    // Compute products of each row/col/subgrid
    int products[3][9];
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
    std::vector<avail_nums> possible_nums;
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            int num = sudoku[i][j];
            if(num == 0) {
                std::vector<int> possible;
                for(int k = 1; k < 10; k++) {
                    if(products[ROW_PRODUCTS][i] % PRIME[k] != 0
                       && products[COL_PRODUCTS][j] % PRIME[k] != 0
                       && products[SUBGRID_PRODUCTS][i/3*3 + j/3] % PRIME[k] != 0)
                    {
                        possible.push_back(k);
                    }
                }
                if(possible.size() == 0) { 
                    return; 
                }
                possible_nums.push_back(avail_nums(i, j, possible));
            }
        }
    }

    if(possible_nums.size() == 0) {
        return;
    }
    std::sort(possible_nums.begin(), possible_nums.end(), avail_nums_compare);
    int copy[9][9];
    memcpy(copy, sudoku, sizeof(int) * 81);
    if(solver(copy, products, possible_nums, 0)) {
        memcpy(sudoku, copy, sizeof(int) * 81);
    }
}

int main() 
{
    int sudoku[9][9]={{0, 0, 0, 0, 0, 0, 0, 0, 0},
		     {0, 0, 0, 0, 0, 3, 0, 8, 5},
		     {0, 0, 1, 0, 2, 0, 0, 0, 0},
		     {0, 0, 0, 5, 0, 7, 0, 0, 0},
		     {0, 0, 4, 0, 0, 0, 1, 0, 0},
		     {0, 9, 0, 0, 0, 0, 0, 0, 0},
		     {5, 0, 0, 0, 0, 0, 0, 7, 3},
		     {0, 0, 2, 0, 1, 0, 0, 0, 0},
		     {0, 0, 0, 0, 4, 0, 0, 0, 9}};
    print_sudoku(sudoku);
    solve_sudoku(sudoku);
    print_sudoku(sudoku);
    return 0;
}