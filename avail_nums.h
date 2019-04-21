#include<vector>

using namespace std;

class avail_nums {
    public:
        avail_nums(int, int, vector<int>);
        void insert(int);
        int len();
    private: 
        int row;
        int col;
        vector<int> available_nums;
        int length;
};