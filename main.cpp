 #include "sokoban.hpp"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <queue>
#include <unordered_set>
#include <algorithm>  // For std::remove
#include <utility>    // For std::pair
using namespace std;


int main(){
    string result;
    vector<string> grid;
    read_map(grid);
    result=solve(grid);
    cout<<result;
}