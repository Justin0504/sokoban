#include <vector>
#include <string>
#include <sstream>   
#include <iostream>
#include <cmath>
#include <climits>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <algorithm>  // For std::remove
#include <utility>    // For std::pair

using namespace std;
/**
 * @brief  Read your map from stdin
 * @note   Input format: See project description
 * @param  &grid: You should first resize the grid! Modify it inplace.
 * @retval None
 */
void read_map(std::vector<std::string> &grid){
    unsigned long rows, cols;
    std::cin >> cols >> rows;
    grid.resize(rows);
    std::cin.ignore();   
    for (unsigned long i = 0; i < rows; ++i) {
        std::getline(std::cin, grid[i]);
    }
}
inline unsigned long toul(unsigned long rows, unsigned long cols, unsigned long grid_cols) {
    return rows * grid_cols + cols;
}

inline pair<unsigned long, unsigned long> convertul(unsigned long pos, unsigned long grid_cols) {
    return {pos / grid_cols, pos % grid_cols};
}

struct State {
    unsigned long player_pos;
    std::vector<unsigned long> boxes; //box position
    int f;
    int g;

    bool operator==(const State& other) const {
        return player_pos == other.player_pos && boxes == other.boxes;
    }
    bool operator<(const State& other) const {
        return f > other.f;  
    }

    struct Hash {
        unsigned long operator()(const State& state) const {
            unsigned long hash_val = state.player_pos;
            for (unsigned long box : state.boxes) {
                hash_val ^= hash<unsigned long>()(box) + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2);
            }
            return hash_val;
        }
    };
};

struct step {
    State state;
    string path;
    bool operator<(const step& other) const {
        return state.f > other.state.f;  
    }
};

const std::vector<std::pair<int, int>> directions = {
    {0, -1}, // Left
    {-1, 0}, // Up
    {0, 1},  // Right
    {1, 0}    // Down
};

unsigned long new_pos(int dx,unsigned long pos){
    if(dx>=0){
        return pos+static_cast<unsigned long>(dx);
    }else {
        return pos-static_cast<unsigned long>(-dx);
    }
}

const std::string dir_chars = "LURD"; 

bool is_valid(unsigned long x, unsigned long y, const std::vector<std::string>& grid) {
    return x >= 0 && y >= 0 && x < grid.size() && y < grid[0].size() && grid[x][y] != '#';
}

bool if_push(unsigned long box_x, unsigned long box_y, int direction_x,
 int direction_y, const std::vector<std::string>& grid,
 const std::vector<unsigned long>& boxes,unsigned long grid_cols) {
    unsigned long next_box_x = new_pos(direction_x, box_x);
    unsigned long next_box_y = new_pos(direction_y, box_y);
    
    if (!is_valid(next_box_x, next_box_y, grid)) return false; 
    for (const auto& b : boxes) {
        if ((convertul(b,grid_cols)).first == next_box_x && (convertul(b,grid_cols)).second == next_box_y) {
            return false; 
        }
    }
    return true;
}

int Manhattan(pair<unsigned long,unsigned long> box,pair<unsigned long,unsigned long> target){
    return abs(static_cast<int>(box.first) - static_cast<int>(target.first)) +
           abs(static_cast<int>(box.second) - static_cast<int>(target.second));
}

int Heuristic( const State& state, vector<pair<unsigned long, unsigned long>> targets, unsigned long grid_cols) {
    int heur=0;
    for (unsigned long box : state.boxes) {
            auto box_pos = convertul(box, grid_cols);
            int min_cost = INT_MAX;
            for (const auto& target : targets) {
                int cost = Manhattan(box_pos,target);
                min_cost = min_cost<cost?min_cost:cost;
            }
            heur += min_cost;
        }
        return heur;
}

bool reach_help(const pair<unsigned long, unsigned long>& begin,
               const pair<unsigned long, unsigned long>& target, 
               const vector<string>& grid){
    bool can_reach=false;
    if(begin==target) return true;
    
    static vector<vector<bool>> record;
    vector<pair<unsigned long, unsigned long>> check;
    record.resize(grid.size(), vector<bool>(grid[0].size(), false));
    for(auto& row : record) {
        fill(row.begin(), row.end(), false);
    }
    record[begin.first][begin.second]=true;
    
    check.push_back(begin);
    while (!check.empty()) {
            pair<unsigned long, unsigned long> current = check.back();
            check.pop_back();
            
            for(unsigned long i=0;i<4;i++){
                int dx=directions[i].first;
                int dy=directions[i].second;
                unsigned long new_x=new_pos(dx,current.first);
                unsigned long new_y=new_pos(dy,current.second);
                if (new_x >= 0 && new_y >= 0 && 
                    new_x < grid.size() && 
                    new_y < grid[0].size()) {
                    
                    if (!record[new_x][new_y] && grid[new_x][new_y] != '#' 
                        && grid[new_x][new_y] != '*') {
                        if (new_x == target.first && new_y == target.second) {
                            can_reach = true;
                            break;
                        }
                        //visit
                        record[new_x][new_y] = true;
                        check.push_back({new_x, new_y});
                    }
                }
            }
            if (can_reach==true) break;
        }
        return can_reach;
}
                
bool if_can_reach(const pair<unsigned long, unsigned long>& box, 
                        const vector<pair<unsigned long, unsigned long>>& Targets,
                        const vector<string>& current_grid) {
    for (const auto& target : Targets) {
        if (reach_help(box, target, current_grid)) {
            return true;
        }
    }
    return false;
}

bool ptob(unsigned long playerRow, unsigned long playerCol, 
                       unsigned long boxRow, unsigned long boxCol,
                       const std::vector<std::string>& grid)  {
    if (playerRow == boxRow && playerCol == boxCol) return true;

    const size_t numRows = grid.size();
    const size_t numCols = grid[0].size();

    // Visited grids for forward and backward searches
    std::vector<std::vector<bool>> visitedFromPlayer(numRows, std::vector<bool>(numCols, false));
    std::vector<std::vector<bool>> visitedFromBox(numRows, std::vector<bool>(numCols, false));

    // Queues for BFS
    std::queue<std::pair<size_t, size_t>> playerQueue, boxQueue;

    // Initialize the BFS
    visitedFromPlayer[playerRow][playerCol] = true;
    visitedFromBox[boxRow][boxCol] = true;
    playerQueue.push({playerRow, playerCol});
    boxQueue.push({boxRow, boxCol});

    // Helper function for jump point search
    auto findJumpPoint = [&](size_t row, size_t col, int dirRow, int dirCol,
                             const std::vector<std::vector<bool>>& otherVisited) -> std::pair<size_t, size_t> {
        size_t currentRow = row, currentCol = col;

        while (true) {
            size_t nextRow = new_pos(dirRow, currentRow);
            size_t nextCol =  new_pos(dirCol, currentCol);

            // Check boundaries and walls
            if (nextRow >= numRows || nextCol >= numCols || grid[nextRow][nextCol] == '#') {
                return {currentRow, currentCol};
            }

            // If the other search visited this point, return it
            if (otherVisited[nextRow][nextCol]) {
                return {nextRow, nextCol};
            }

            // Check for turning points
            bool isTurningPoint = false;
            for (const auto& direction : directions) {
                if (direction.first == dirRow && direction.second == dirCol) continue;

                size_t checkRow = new_pos(direction.first, nextRow);
                size_t checkCol = new_pos(direction.second, nextCol);

                if (checkRow < numRows && checkCol < numCols && grid[checkRow][checkCol] != '#') {
                    isTurningPoint = true;
                    break;
                }
            }

            if (isTurningPoint) {
                return {nextRow, nextCol};
            }

            currentRow = nextRow;
            currentCol = nextCol;
        }
    };

    // Perform bidirectional BFS
    while (!playerQueue.empty() && !boxQueue.empty()) {
        // Expand from the player's side
        size_t playerQueueSize = playerQueue.size();
        while (playerQueueSize--) {
            auto [currentRow, currentCol] = playerQueue.front();
            playerQueue.pop();

            for (const auto& direction : directions) {
                auto [jumpRow, jumpCol] = findJumpPoint(currentRow, currentCol, direction.first, 
                direction.second, visitedFromBox);

                if (jumpRow == currentRow && jumpCol == currentCol) continue;

                if (visitedFromBox[jumpRow][jumpCol]) return true;

                if (!visitedFromPlayer[jumpRow][jumpCol]) {
                    visitedFromPlayer[jumpRow][jumpCol] = true;
                    playerQueue.push({jumpRow, jumpCol});
                }
            }
        }

        // Expand from the box's side
        size_t boxQueueSize = boxQueue.size();
        while (boxQueueSize--) {
            auto [currentRow, currentCol] = boxQueue.front();
            boxQueue.pop();

            for (const auto& direction : directions) {
                auto [jumpRow, jumpCol] = findJumpPoint(currentRow, currentCol, direction.first, 
                direction.second, visitedFromPlayer);

                if (jumpRow == currentRow && jumpCol == currentCol) continue;

                if (visitedFromPlayer[jumpRow][jumpCol]) return true;

                if (!visitedFromBox[jumpRow][jumpCol]) {
                    visitedFromBox[jumpRow][jumpCol] = true;
                    boxQueue.push({jumpRow, jumpCol});
                }
            }
        }
    }

    return false;
}

bool twoboxes(unsigned long grid_cols,unsigned long box_x, unsigned long box_y,const vector<string> & grid,
             vector<unsigned long> boxes){
    if ( grid[box_x][box_y] == 'R' || grid[box_x][box_y] == 'T') return false;

    bool left_box=find(boxes.begin(),boxes.end(),toul(box_x,box_y-1,grid_cols))!=boxes.end();
    bool right_box=find(boxes.begin(),boxes.end(),toul(box_x,box_y+1,grid_cols))!=boxes.end();
    bool top_box=find(boxes.begin(),boxes.end(),toul(box_x-1,box_y,grid_cols))!=boxes.end();
    bool bot_box=find(boxes.begin(),boxes.end(),toul(box_x+1,box_y,grid_cols))!=boxes.end();

    bool vwall=(grid[box_x+1][box_y]=='#'&&grid[box_x-1][box_y]=='#');
    bool hwall=(grid[box_x][box_y+1]=='#'&&grid[box_x][box_y+1]=='#');
    if(vwall&&((left_box&&grid[box_x][box_y-1]!='T'&&grid[box_x][box_y-1]!='R')||
              (right_box&&grid[box_x][box_y+1]!='T'&&grid[box_x][box_y+1]!='R'))){
                return true;
    }
    if(hwall&&((top_box&&grid[box_x-1][box_y]!='T'&&grid[box_x-1][box_y]!='R')||
              (bot_box&&grid[box_x+1][box_y]!='T'&&grid[box_x+1][box_y]!='R'))){
                return true;
    }
    return false;
}

bool stickwall(const vector<unsigned long>& boxes, const vector<pair<unsigned long, unsigned long>>& Targets,
                 unsigned long box_x, unsigned long box_y,const vector<string>& grid){
    int numB=0,numT=0;
    if(box_x==1||box_x==grid.size()-2){
        for(auto target:Targets){
            if(target.first==box_x){
                numT++;
            }
        }
        for(auto box:boxes){
            if(convertul(box,grid[0].size()).first==box_x){
                numB++;
            }
        }
        if(numB>numT) return true;
    }
     
    if(box_y==1||box_y==grid[0].size()-2){
        numB=0;
        numT=0;
        for(auto target:Targets){
            if(target.second==box_y){
                numT++;
            }
        }
        for(auto box:boxes){
            if(convertul(box,grid[0].size()).second==box_y){
                numB++;
            }
        }
        if(numB>numT) return true;
    }
    return false;

}
bool deadlock(unsigned long box_x, unsigned long box_y, unsigned long ply_x, unsigned long ply_y,
                const vector<string>& grid,const vector<pair<unsigned long, unsigned long>>& Targets, 
                const vector<string>& new_grid, vector<unsigned long> boxes){

    if(stickwall(boxes,Targets,box_x,box_y,grid)) return true;

    pair<unsigned long,unsigned long> box={box_x,box_y};
    if (!if_can_reach(box, Targets, new_grid)) {
        return true;
    }
    
    if(grid[box_x][box_y]!='T'){

        //corner
        bool top = (grid[box_x-1][box_y] == '#');
        bool bot = (grid[box_x+1][box_y] == '#');
        bool left = (grid[box_x][box_y-1] == '#');
        bool right = (grid[box_x][box_y+1] == '#');
        if(twoboxes(grid[0].size(),box_x,box_y, grid ,boxes)) return true;

        if ((left && top) || (right && top) || (right && bot) || (bot && left)) {
            return true;
        }

        //surround
        if(left||right){
            vector<unsigned long> exits;
            bool if_target=false;
            for(unsigned long i=0;i<grid.size();i++){
                if(grid[i][box_y]=='T') {
                    if_target=true;
                    break;
                }
                if(right&&grid[i][box_y+1]!='#'){
                    exits.push_back(i);
                }
                if(left&&grid[i][box_y-1]!='#'){
                    exits.push_back(i);
                }
            }
            if(if_target==false){
                if(exits.empty()) return true;
                else{
                    bool can_exit=false;

                    for(auto exit:exits){
                        bool to_exit=false;
                        if(exit>box_x){
                            for(unsigned long k=box_x;k<exit;k++){
                                if(!ptob(ply_x,ply_y,k,box_y,grid)){
                                    to_exit=false;
                                    break;
                                }                           
                            }
                            to_exit=true;
                        } else{
                            for(unsigned long k=exit+1;k<box_x;k++){
                                if(!ptob(ply_x,ply_y,k,box_y,grid)){
                                    to_exit=false;
                                    break;
                                }
                            }
                            to_exit=true;
                        }
                        if(to_exit){
                            vector<string> temp=grid;
                            temp[box_x][box_y]='.';
                            temp[exit][box_y]='B';
                            if(ptob(ply_x,ply_y,exit,box_y,grid)&&(grid[exit][box_y-1] != '#'||grid[exit][box_y+1] != '#')){
                                can_exit=true;
                                break;
                            }
                            
                        }
                    }
                    if(can_exit==false) return true;
                    
                }
            }
        }

        if(bot||top){
            vector<unsigned long> exits;
            bool if_target=false;
            for(unsigned long j=0;j<grid[0].size();j++){
                if(grid[box_x][j]=='T') {
                    if_target=true;
                    break;
                }
                if(top&&grid[box_x-1][j]!='#'){
                    exits.push_back(j);
                }
                if(bot&&grid[box_x+1][j]!='#'){
                    exits.push_back(j);
                }
            }
            if(if_target==false){
                if(exits.empty()) return true;
                else{
                    bool can_exit=false;
                    for(auto exit:exits){
                        bool to_exit=false;
                        if(exit>box_y){
                            for(unsigned long k=box_y;k<exit;k++){
                                if(!ptob(ply_x,ply_y,box_x,k,grid)){
                                    to_exit=false;
                                    break;
                                }                           
                            }
                            to_exit=true;
                        } else{
                            for(unsigned long k=exit+1;k<box_y;k++){
                                if(!ptob(ply_x,ply_y,box_x,k,grid)){
                                    to_exit=false;
                                    break;
                                }
                            }
                            to_exit=true;
                        }
                        if(to_exit){
                            vector<string> temp=grid;
                            temp[box_x][exit]='B';
                            temp[box_x][box_y]='.';
                            if(ptob(ply_x,ply_y,box_x,exit,grid)&&(grid[box_x-1][exit] != '#'||grid[box_x+1][exit] != '#')){
                                can_exit=true;
                                break;
                            }
                           
                        }
                    }
                    if(can_exit==false) return true;
                    
                }
            }
        }
    }
    return false;
}




/**
 * @brief  Solve the sokoban described by the grid
 * @note   Output string format: See project description
 * @param  &grid: The grid you read by `read_map'
 * @retval 
 */
bool isterminante(const State& state, const vector<std::string> &grid, unsigned long grid_cols) {
        for (const auto& box : state.boxes) {
            pair temp = convertul(box, grid_cols);
            if (grid[temp.first][temp.second] != 'T' && grid[temp.first][temp.second] != 'R') {  
                return false;
            }
        }
        return true;
}

std::string solve(std::vector<std::string> &grid){
    
    unsigned long rows=grid.size();
    unsigned long cols=grid[0].size();
    unsigned long start; // player start position
    std::vector<pair<unsigned long, unsigned long>> Targets; //targets position
    vector<unsigned long> boxes;

     //grid with corner
     vector<string> new_grid = grid;
        
    //find S and B
    int num_S=0;
    int num_B=0;
    int num_T=0;
    for (unsigned long i = 0; i < rows; ++i) {
        for (unsigned long j = 0; j < cols; ++j) {
            if (grid[i][j] == 'S') {
                start=toul(i,j,cols);
                num_S++;
            } 
            if (grid[i][j] == 'B'||grid[i][j]=='R') {
                boxes.push_back(toul(i, j,cols));
                num_B++;
            }
            if (grid[i][j] == 'T'||grid[i][j]=='R'){
                num_T++;
                Targets.emplace_back(i,j);
            }
            if (grid[i][j] != '#' && grid[i][j] != 'T' && grid[i][j] != 'R') {
                    bool top =  (grid[i-1][j] == '#');
                    bool bot =  (grid[i+1][j] == '#');
                    bool left =  (grid[i][j-1] == '#');
                    bool right =  (grid[i][j+1] == '#');
                    
                    if ((top && left) || (top && right) || 
                        (bot && left) || (bot && right)) {
                        new_grid[i][j] = '*';
                    }
            }
        }
    }
    //1.If no S or more than 1 s
    if(num_S!=1){
        return "No solution!";
    }

    //2.check if B>T
    if(num_B>num_T){
        return "No solution!";
    }
    
    for(auto box:boxes){
        if(!if_can_reach(convertul(box,cols),Targets,new_grid)){
            return "No solution!";
        }
    }

    sort(boxes.begin(),boxes.end());

    //initialization
    priority_queue<step> pq;
    unordered_set<State, State::Hash> record;

    State begin={start,boxes,0,0};
    begin.f=Heuristic(begin,Targets,cols);
    pq.push({begin, ""});
    record.insert(begin);

    
    while (!pq.empty()) {
        step current = pq.top();
        pq.pop();
        //termination check
        if(isterminante(current.state,grid,cols)){
            return current.path;
        }
        

        for(unsigned long i=0;i<4;i++){
            int dx=directions[i].first;
            int dy=directions[i].second;
            char move_char = dir_chars[i];

            pair<unsigned long,unsigned long> curply=convertul(current.state.player_pos,cols);
            pair<unsigned long,unsigned long> newplayer={new_pos(dx,curply.first),new_pos(dy,curply.second)};
            unsigned long new_p=toul(newplayer.first,newplayer.second,cols);
            vector<unsigned long> new_boxes=current.state.boxes;

            //if next position is valid
            if(is_valid(newplayer.first,newplayer.second,grid)){
                bool pushing=false;
                unsigned long box_x=0,box_y=0,new_boxx=0,new_boxy=0;

                //if push
                for (unsigned long i=0;i<new_boxes.size();i++) {
                    if (new_p==new_boxes[i]) { 
                        box_x = (convertul(new_boxes[i],cols)).first;
                        box_y = (convertul(new_boxes[i],cols)).second;
                        new_boxx=new_pos(dx,box_x);
                        new_boxy=new_pos(dy,box_y);
                        new_boxes[i]=toul(new_boxx,new_boxy,cols);
                        pushing=true; 
                        break;
                    }
                }

                //push
                if (pushing && if_push(box_x, box_y, dx, dy, grid, current.state.boxes,cols)&&
                    !deadlock(new_boxx,new_boxy,curply.first,curply.second,grid, Targets,new_grid,new_boxes)) {
                    State new_state={toul(newplayer.first,newplayer.second,cols),new_boxes,0,current.state.g+1};
                    new_state.f=new_state.g+static_cast<int>(Heuristic(new_state,Targets,cols));
                    sort(new_boxes.begin(),new_boxes.end());
                    
                    if(record.find(new_state)==record.end()){
                        pq.push({new_state,current.path+move_char});
                        record.insert(new_state);
                    }
                 
                }  else if(!pushing){
                    
                    State newstate={toul(newplayer.first,newplayer.second,cols),current.state.boxes,
                    0,current.state.g+1};
                    newstate.f=newstate.g+static_cast<int>(Heuristic(newstate,Targets,cols));
                    
                    if(record.find(newstate)==record.end()){
                        pq.push({newstate,current.path+move_char});
                        record.insert(newstate);
                    }
                }
                
            }

        }
    }
    //unsolvable path
    
    return "No solution!";
    
}

// For big cases, paste your answers here (i.e. replace "ans for big 1" with your answer)
// Do not remove the first element, it should be left blank.
// NOTE: Ensure the order is correct!
// Your answer should loelse ifok like "UUDDLLRR..."
const std::vector<std::string> answers = {
    "__leave_this_blank__", 


};

// Don't modify this.
std::string print_answer(int index) {
    if (index < 1 || (unsigned long)index >= (unsigned long)answers.size()) {
        return "invalid";
    }
    return answers[(size_t)index];
}



