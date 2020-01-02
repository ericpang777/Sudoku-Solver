#include<algorithm>
#include<bitset>
#include<chrono>
#include<cstring>
#include<fstream>
#include<iostream>
#include<set>
#include<string>
#include<vector>

int LOGPOWER[513]; // Lookup table fof 2^x, LOGPOWER[i] = log_2(i)
int avail[81]; // Global so the sort can access it

void print_sudoku(int sudoku[9][9], int output) 
{
    if(!output) {
        return;
    }
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

int validate_sudoku(int sudoku[9][9]) {
    for(int i = 0; i < 9; i++) {
        int row = 0;
        int col = 0;
        int subgrid = 0;

        for(int j = 0; j < 9; j++) {
            row |= 1 << sudoku[i][j];
            col |= 1 << sudoku[j][i];
            subgrid |= 1 << sudoku[i/3*3 + j/3][i%3*3 + j%3];
        }
        // 1022 is 1s from the 1th to 9th bit, i.e. 0b00000000000000000000001111111110
        if(row != 1022 || col != 1022 || subgrid != 1022) {
            return 0;
        }
    }
    return 1;
}

bool has_duplicate(int sudoku[9][9]) 
{
    for(int i = 0; i < 9; i++) {
        int row_seen = 0;
        int col_seen = 0;
        int subgrid_seen = 0;

        for(int j = 0; j < 9; j++) {
            if(sudoku[i][j] != 0) {
                // If AND is > 0, this means two bits are the same at some point
                if((row_seen & (1 << sudoku[i][j])) > 0) 
                    return true;
                row_seen |= 1 << sudoku[i][j];
            }

            if(sudoku[j][i] != 0) {
                if((col_seen & (1 << sudoku[j][i])) > 0) 
                    return true;
                col_seen |= 1 << sudoku[j][i];
            }

            if(sudoku[i/3*3 + j/3][i%3*3 + j%3] != 0) {
                if((subgrid_seen & (1 << sudoku[i/3*3 + j/3][i%3*3 + j%3])) > 0) 
                    return true;
                subgrid_seen |= 1 << sudoku[i/3*3 + j/3][i%3*3 + j%3];
            }
        }
    }
    return false;
}

bool solver(int sudoku[9][9], int constraint[3][9], std::vector<int> &indices, int idx) 
{
    int power_of_2;
    int sq_num = indices[idx];
    int to_check = constraint[0][sq_num/9] & constraint[1][sq_num%9] & constraint[2][sq_num/3%3 + sq_num/27*3];
    
    while(to_check != 0) {
        power_of_2 = to_check & -to_check; // Get lowest set bit
        to_check ^= power_of_2;
        
        sudoku[sq_num/9][sq_num%9] = LOGPOWER[power_of_2];
        constraint[0][sq_num/9] ^= power_of_2;
        constraint[1][sq_num%9] ^= power_of_2;
        constraint[2][sq_num/3%3 + sq_num/27*3] ^= power_of_2;

        if((unsigned)(idx+1) == indices.size() || solver(sudoku, constraint, indices, idx + 1)) {
            return true;
        }
        
        sudoku[sq_num/9][sq_num%9] = 0;
        constraint[0][sq_num/9] |= power_of_2;
        constraint[1][sq_num%9] |= power_of_2;
        constraint[2][sq_num/3%3 + sq_num/27*3] |= power_of_2;
    }
    return false;
}

bool avail_compare(int a, int b) 
{
    a = avail[a];
    b = avail[b];
    return __builtin_popcount(a) < __builtin_popcount(b);
}

int set_up(int sudoku[9][9], int constraint[3][9], std::vector<int> &indices) 
{
    //memset(sudoku, 0, sizeof(sudoku)); Not needed since complete array gets rewritten if needed
    // Set all values in constraint to 1022
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 9; j++) {
            constraint[i][j] = 1022; // 1022 is 1s from the 1th to 9th bit, i.e. 0b00000000000000000000001111111110
        }
    }
    indices.clear();
    // Set constraints
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            // Values on index 1 to 9 inclusive will be updated
            // AND the current value with number with all 1s except at the sudoku[i][j]th index
            constraint[0][i] &= ~(1 << sudoku[i][j]); 
            constraint[1][i] &= ~(1 << sudoku[j][i]);
            constraint[2][i] &= ~(1 << sudoku[i/3*3 + j/3][i%3*3 + j%3]);
        }
    }

    // Fill indices with combined constraints of empty squares
    for(int i = 0; i < 81; i++) {
        if(sudoku[i/9][i%9] == 0) {
            indices.push_back(i);
            avail[i] = constraint[0][i/9] & constraint[1][i%9] & constraint[2][i/3%3 + i/27*3];
        }
    }
    // Would indicate either completed sudoku or unsolvable sudoku
    if(indices.size() == 0)
        return 0;
    
    // Sort indices by increasing number of 1s, so squares with least number of possible numbers are first
    std::sort(indices.begin(), indices.end(), avail_compare);
    return 1;
}

void parse_dash_line(int sudoku[9][9], std::string line) {
    int i = 0; // Index in sudoku
    int j = 0; // Index in line
    while(i < 81) {
        if(line[j] == ' ' || line[j] == '\r' || line[j] == '\n') {
            j++;
            continue;
        }
        if(line[j] == '-') {
            sudoku[i/9][i%9] = 0;
        } else {
            sudoku[i/9][i%9] = (int)line[j] - 48; // Convert char representation to int
        }
        i++;
        j++;
    }
}

void solve_sudoku(int time, int output, std::string filename, std::string format) 
{
    // Check nums not in 0-9
    //for(int i = 0; i < 9; i++) {
    //    for(int j = 0; j < 9; j++) {
    //       if(sudoku[i][j] < 0 || 9 < sudoku[i][j]) {
    //            return;
    //        }
    //    }
    //}

    // Check dupliates
    /*
    if(has_duplicate(sudoku)) {
        return;
    }
    */
    // Set up table for log_2(x)
    int powers[9] = {2, 4, 8, 16, 32, 64, 128, 256, 512};
    for(int i = 0; i < 9; i++) {
        LOGPOWER[powers[i]] = i+1;
    }

    int sudoku[9][9];
    // Store numbers of each row/col/subgrid as 101010010, meaning whether 987654321 are in the region, on [9 to 1], and not [8 to 0] to not have to use 1 << n-1
    // constraint stored as row, col, subgrid as [0], [1], [2] respectively
    int constraint[3][9];
    // List of all indices in the sudoku that are empty
    std::vector<int> indices;
    int copy[9][9];

    std::ifstream inFile(filename);
    std::string line;
    //int count = 0;
    if(format.compare("dash") == 0) {
        while(std::getline(inFile, line, ',')) {
            parse_dash_line(sudoku, line);
            if(!set_up(sudoku, constraint, indices)) {
                print_sudoku(sudoku, output);
            } else {
                for(int i = 0; i < 9; i++) {
                    memcpy(copy[i], sudoku[i], sizeof(int)*9);
                }
                print_sudoku(sudoku, output);
                if(solver(sudoku, constraint, indices, 0)) {
                    print_sudoku(sudoku, output);
                    //std::cout << validate_sudoku(sudoku) << "\n";
                } else {
                    print_sudoku(copy, output);
                    //std::cout << validate_sudoku(sudoku) << "\n";

                }
                
            }
        }
    } else if(format.compare("dot") == 0) {

    }    
}

/*
void test_big_file(int sudoku[9][9]) {
    std::ifstream inFile("test_sudoku/sudoku_big.txt");
    std::ofstream outFile("test_sudoku/result.txt");
    std::string line;
    int count = 0;
    while(std::getline(inFile, line)) {
        line = line.substr(0, line.length()-1);
        for(int i = 0; i < 81; i++) {
            if(line[i] == '.') 
                sudoku[i/9][i%9] = 0;
            else       
                sudoku[i/9][i%9] = (int)line[i] - 48;
        }
        
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        solve_sudoku(sudoku);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        char s[81];
        for(int i = 0; i < 81; i++) {
            s[i] = (char)(sudoku[i/9][i%9] + 48);
        }
        outFile << s << ", time=" << std::chrono::duration_cast<std::chrono::milliseconds> (end - begin).count() << "ms" << std::endl;
        count += std::chrono::duration_cast<std::chrono::milliseconds>(end-begin).count();
        
    }  
    std::cout << count << "ms";
    inFile.close();
    outFile.close();
}*/

/*
 * ./solver [-time] <output> <filename> <format>
 */ 
int main(int argc, char** argv) 
{
    if(argc == 4) {
        int time = 0;
        int output = std::stoi(argv[1]);
        std::string filename = argv[2];
        std::string format = argv[3];
        solve_sudoku(time, output, filename, format);
    } else if(argc == 5) {
        int time = 1;
        int output = std::stoi(argv[2]);
        std::string filename = argv[3];
        std::string format = argv[4];
        solve_sudoku(time, output, filename, format);
    } else {
        std::cout << "Usage: solver [-time] <output> <filename> <format>";
    }
    return 0;
}