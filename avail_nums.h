#include<vector>

class avail_nums {
    public:
        avail_nums(int, int, std::vector<int>);
        void insert(int);
        int len();
        int get_row();
        int get_col();
        int get_index(int);

    private: 
        int row;
        int col;
        std::vector<int> available_nums;
        int length;
};