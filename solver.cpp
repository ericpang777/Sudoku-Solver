#include<algorithm>
#include<bitset>
#include<chrono>
#include<cstring>
#include<fstream>
#include<iostream>
#include<set>
#include<string>
#include<vector>

int avail[81]; // Global so the sort can access it
int LOGPOWER[513]; // Lookup table fof 2^x, LOGPOWER[i] = log_2(i)

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

bool solver(int sudoku[9][9], int data[3][9], std::vector<int> &indices, int idx) 
{
    int power_of_2;
    int sq_num = indices[idx];
    int avail = (~(data[0][sq_num/9] | data[1][sq_num%9] | data[2][sq_num/3%3 + sq_num/27*3]) >> 1 << 1) - 4294966272; // 4294966272 is sum of 2^i from i = 10 to 31
    while(avail != 0) {
        power_of_2 = avail & -avail;
        avail ^= power_of_2;
        if(    (data[0][sq_num/9] & power_of_2)                 == 0
            && (data[1][sq_num%9] & power_of_2)                 == 0
            && (data[2][sq_num/3%3 + sq_num/27*3] & power_of_2) == 0)
        {
            sudoku[sq_num/9][sq_num%9] = LOGPOWER[power_of_2];
            data[0][sq_num/9] |= power_of_2;
            data[1][sq_num%9] |= power_of_2;
            data[2][sq_num/3%3 + sq_num/27*3] |= power_of_2;
            if((unsigned)(idx+1) == indices.size() || solver(sudoku, data, indices, idx + 1)) {
                return true;
            }
            sudoku[sq_num/9][sq_num%9] = 0;
            data[0][sq_num/9] ^= power_of_2;
            data[1][sq_num%9] ^= power_of_2;
            data[2][sq_num/3%3 + sq_num/27*3] ^= power_of_2;
        }
    }
    return false;
}

bool avail_compare(int a, int b) 
{
    a = avail[a];
    b = avail[b];
    return __builtin_popcount(a) > __builtin_popcount(b);
}

void solve_sudoku(int sudoku[9][9]) 
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

    // Store numbers of each row/col/subgrid as 101010010, meaning whether 987654321 are in, on [9-1], avoid [8-0] to not have 1 << n-1
    // Data stored as row, col, subgrid as [0], [1], [2] respectively
    int data[3][9];
    memset(data, 0, sizeof(data));
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            data[0][i] |= (1 << sudoku[i][j]); 
            data[1][i] |= (1 << sudoku[j][i]);
            data[2][i] |= (1 << sudoku[i/3*3 + j/3][i%3*3 + j%3]);
        }
    }
    for(int i = 0; i < 3; i++) 
        for(int j = 0; j  < 9; j++) 
            data[i][j] = data[i][j] >> 1 << 1;
    
    int powers[9] = {2, 4, 8, 16, 32, 64, 128, 256, 512};
    for(int i = 0; i < 9; i++) {
        LOGPOWER[powers[i]] = i+1;
    }

    std::vector<int> indices;
    for(int i = 0; i < 81; i++) {
        if(sudoku[i/9][i%9] == 0) {
            indices.push_back(i);
            avail[i] = data[0][i/9] | data[1][i%9] | data[2][i/3%3 + i/27*3];
        }
    }
    if(indices.size() == 0)
        return;

    std::sort(indices.begin(), indices.end(), avail_compare);
    int copy[9][9];
    memcpy(copy, sudoku, sizeof(int) * 81);
    if(solver(copy, data, indices, 0)) {
        memcpy(sudoku, copy, sizeof(int) * 81);
    }
}

void read_file(int sudoku[9][9], std::string file_name) {
    std::ifstream inFile(file_name);
    std::string line;
    int i = 0;
    while(std::getline(inFile, line, ',')) {
        sudoku[i/9][i%9] = std::stoi(line);    
        i++;
    }
}

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
    //outFile.close();
}

int main() 
{
    int sudoku[9][9];
    //test_big_file(sudoku);
    read_file(sudoku, "sudoku.txt");
    print_sudoku(sudoku);
    solve_sudoku(sudoku);
    print_sudoku(sudoku);
    return 0;
}