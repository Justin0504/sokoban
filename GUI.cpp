#include "sokoban.hpp"
#include <fstream>
#include <conio.h>
#include <sstream>
#include <stdlib.h>
#include <graphics.h>
#include <easyx.h>
#include <Windows.h>
#include <stack>  

#define CELL_SIZE 100

enum _PROBS {
    WALL,
    Player,
    BOX,
    TARGET,
    ONTARGET,
    ALL
};
IMAGE images[ALL];

// 绘制地图
void draw_grid(const std::vector<std::string>& Grid) {
    cleardevice();
    for (size_t i = 0; i < Grid.size(); i++) {
        for (size_t j = 0; j < Grid[i].size(); j++) {
            int x = (int)j * CELL_SIZE;
            int y = (int)i * CELL_SIZE;
            switch (Grid[i][j]) {
            case '#':
                putimage(x, y, &images[WALL]);
                break;
            case 'S':
                putimage(x, y, &images[Player]);
                break;
            case 'B':
                putimage(x, y, &images[BOX]);
                break;
            case 'T':
                putimage(x, y, &images[TARGET]);
                break;
            case 'R':
                putimage(x, y, &images[ONTARGET]);
                break;
            case 'X':
                putimage(x, y, &images[TARGET]);
                putimage(x, y, &images[Player]);
                break;
            default:
                break;
            }
        }
    }
    FlushBatchDraw();
}

bool check_victory(std::vector<std::string>& Grid) {
    for (const auto& row : Grid) {
        for (char cha : row) {
            if (cha == 'T') {
                return false;
            }
        }
    }
    return true;
}

void moves(std::vector<std::string>& Grid, char move, int& prev_move) {
    int m = -1, n = -1;
    int rows = Grid.size();
    int cols = Grid[0].size();
    bool isPlayer = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (Grid[i][j] == 'S' || Grid[i][j] == 'X') {
                m = i;
                n = j;
                isPlayer = 1;
                break;
            }
        }
    }

    if (!isPlayer) return;

    int dx = 0, dy = 0;
    switch (move) {
    case 'U': dy = -1; break;
    case 'D': dy = 1; break;
    case 'L': dx = -1; break;
    case 'R': dx = 1; break;
    default: return;
    }

    int row_next = m + dy;
    int col_next = n + dx;

    int box_row = m + 2 * dy;
    int box_col = n + 2 * dx;

    if (row_next < 0 || row_next >= rows || col_next < 0 || col_next >= cols)
        return;
    if (Grid[row_next][col_next] == '.' && Grid[m][n] == 'S') {
        Grid[m][n] = '.';
        Grid[row_next][col_next] = 'S';
    }
    else if (Grid[row_next][col_next] == 'T' && Grid[m][n] == 'S') {
        Grid[m][n] = '.';
        Grid[row_next][col_next] = 'X';
    }
    else if ((Grid[row_next][col_next] == 'B' || Grid[row_next][col_next] == 'R') && Grid[m][n] == 'S') {
        if (Grid[box_row][box_col] == '.') {
            Grid[m][n] = '.';
            Grid[row_next][col_next] = 'S';
            Grid[box_row][box_col] = 'B';
        }
        else if (Grid[box_row][box_col] == 'T') {
            Grid[m][n] = '.';
            Grid[row_next][col_next] = 'S';
            Grid[box_row][box_col] = 'R';
        }
    }

    if (Grid[row_next][col_next] == '.' && Grid[m][n] == 'X') {
        Grid[m][n] = 'T';
        Grid[row_next][col_next] = 'S';
    }
    else if (Grid[row_next][col_next] == 'T' && Grid[m][n] == 'X') {
        Grid[m][n] = 'T';
        Grid[row_next][col_next] = 'X';
    }
    else if ((Grid[row_next][col_next] == 'B' || Grid[row_next][col_next] == 'R') && Grid[m][n] == 'X') {
        if (Grid[box_row][box_col] == '.') {
            Grid[m][n] = 'T';
            Grid[row_next][col_next] = 'S';
            Grid[box_row][box_col] = 'B';
        }
        else if (Grid[box_row][box_col] == 'T') {
            Grid[m][n] = 'T';
            Grid[row_next][col_next] = 'S';
            Grid[box_row][box_col] = 'R';
        }
    }

    if ((prev_move == 'U' && move == 'D') || (prev_move == 'D' && move == 'U') ||
        (prev_move == 'L' && move == 'R') || (prev_move == 'R' && move == 'L')) {
        prev_move = move;
        return;
    }

    prev_move = move;
}

int main() {
    IMAGE bg_img;

    std::vector<std::string> Grid;
    read_map(Grid);

    int window_row = Grid[0].size() * CELL_SIZE;
    int window_col = Grid.size() * CELL_SIZE;
    initgraph(window_row, window_col);

    BeginBatchDraw();
    loadimage(&bg_img, L"bg.jpg", window_row * 2, window_col * 2, false);
    putimage(1, 0, &bg_img);
    loadimage(&images[WALL], L"wall.png", CELL_SIZE, CELL_SIZE, true);  // Wall
    loadimage(&images[Player], L"PLAYER.jpg", CELL_SIZE, CELL_SIZE, true);  // Player
    loadimage(&images[BOX], L"box.jpg", CELL_SIZE, CELL_SIZE, true);      // Box
    loadimage(&images[TARGET], L"target.png", CELL_SIZE, CELL_SIZE, true);   // Target
    loadimage(&images[ONTARGET], L"box.jpg", CELL_SIZE, CELL_SIZE, true);  // Box on Target

    std::string steps = solve(Grid); 
    if (steps == "No solution!") {
        std::cout << "No solution!";
        return 0;
    }

    int prev_move = '\0';
    std::stack<std::vector<std::string>> history; // 用于保存历史状态
    history.push(Grid);

    for (size_t i = 0; i < steps.length(); ++i) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 'p') { // 暂停
                Sleep(200);

                while (true) {
                    if (_kbhit() && _getch() == 'r') { // 恢复
                        Sleep(200);

                        break;
                    }
                }
            }
            else if (key == 'b') { // 返回上一步
                if (history.size() > 1) {
                    Sleep(200);

                    history.pop();
                    Grid = history.top();
                    draw_grid(Grid);
                }
                continue; // 跳过本次循环
            }
        }

        draw_grid(Grid);
        moves(Grid, steps[i], prev_move);
        history.push(Grid); // 保存当前状态
        Sleep(400); 
    }

    draw_grid(Grid);

    EndBatchDraw();
    closegraph();
    return 0;
}