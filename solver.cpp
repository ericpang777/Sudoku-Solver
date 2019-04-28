#include<algorithm>
#include<cstring>
#include<iostream>
#include<set>
#include<vector>

#include "avail_nums.h"

const unsigned short FULL = 511; // 111111111

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

bool is_complete(unsigned short data[3][9]) 
{
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 9; j++) {
            if(data[i][j] != FULL) {
                return false;
            }
        }
    }
    return true;
}

bool solver(int sudoku[9][9], 
            unsigned short data[3][9], 
            std::vector<avail_nums> &possible_nums, int index) 
{
    if(is_complete(data)) {
        return true;
    }
    int row = possible_nums[index].get_row();
    int col = possible_nums[index].get_col();
    int length = possible_nums[index].len();

    for(int i = 0; i < length; i++) {
        int num = possible_nums[index].get_index(i);
        unsigned short power_of_2 = 1 << (num-1);
        if((data[0][row] | power_of_2) != data[0][row]
           && (data[1][col] | power_of_2) != data[1][col]
           && (data[2][row/3*3 + col/3] | power_of_2) != data[2][row/3*3 + col/3])
        {
            sudoku[row][col] = num;
            data[0][row] |= power_of_2;
            data[1][col] |= power_of_2;
            data[2][row/3*3 + col/3] |= power_of_2;

            if((unsigned int)(index + 1) == possible_nums.size()) {
                return true;
            } else if(solver(sudoku, data, possible_nums, index + 1)) {
                return true;
            }

            sudoku[row][col] = 0;
            data[0][row] ^= power_of_2;
            data[1][col] ^= power_of_2;
            data[2][row/3*3 + col/3] ^= power_of_2;
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

    // Store numbers of each row/col/subgrid as 101010010, meaning whether 987654321 are in
    unsigned short data[3][9];
    for(int i = 0; i < 9; i++) {
        int row_data = 0;
        int col_data = 0;
        int subgrid_data = 0;
        for(int j = 0; j < 9; j++) {
            row_data |= (1 << sudoku[i][j]) >> 1;
            col_data |= (1 << sudoku[j][i]) >> 1;
            subgrid_data |= (1 << sudoku[i/3*3 + j/3][i%3*3 + j%3]) >> 1;
        }
        data[0][i] = row_data;
        data[1][i] = col_data;
        data[2][i] = subgrid_data;
    }

    // Get list of possible numbers for each blank
    std::vector<avail_nums> possible_nums;
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            int num = sudoku[i][j];
            if(num == 0) {
                std::vector<int> possible;
                for(int k = 1; k < 10; k++) {
                    unsigned short power_of_2 = 1 << (k-1);
                    if((data[0][i] | power_of_2) != data[0][i] 
                       && (data[1][j] | power_of_2) != data[1][j]
                       && (data[2][i/3*3 + j/3] | power_of_2) != data[2][i/3*3 + j/3])
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
    if(solver(copy, data, possible_nums, 0)) {
        memcpy(sudoku, copy, sizeof(int) * 81);
    }
}

int main() 
{
    int sudoku[9][9]={{0, 2, 0, 0, 0, 0, 0, 0, 0},
		     {0, 0, 0, 6, 0, 0, 0, 0, 3},
		     {0, 7, 4, 0, 8, 0, 0, 0, 0},
		     {0, 0, 0, 0, 0, 3, 0, 0, 2},
		     {0, 8, 0, 0, 4, 0, 0, 1, 0},
		     {6, 0, 0, 5, 0, 0, 0, 0, 0},
		     {0, 0, 0, 0, 1, 0, 7, 8, 0},
		     {5, 0, 0, 0, 0, 9, 0, 0, 0},
		     {0, 0, 0, 0, 0, 0, 0, 4, 0}};
    print_sudoku(sudoku);
    solve_sudoku(sudoku);
    print_sudoku(sudoku);
    return 0;
}