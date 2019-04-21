#include<vector>
#include "avail_nums.h"

using namespace std;

avail_nums::avail_nums(int row, int col, vector<int> available_nums) {
    this->row = row;
    this->col = col;
    this->available_nums = available_nums;
    length = this->available_nums.size();
}

void avail_nums::insert(int num) {
    available_nums.push_back(num);
    length++;
}

int avail_nums::len() {
    return length;
}

int avail_nums::get_row() {
    return row;
}

int avail_nums::get_col() {
    return col;
}

int avail_nums::get_index(int i) {
    return available_nums[i];
}
