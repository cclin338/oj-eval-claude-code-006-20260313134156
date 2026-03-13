#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Global variables for client state
char client_map[35][35];  // Current known state of the map

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // Initialize all global variables
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      client_map[i][j] = '?';
    }
  }

  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  for (int i = 0; i < rows; i++) {
    std::string line;
    std::cin >> line;
    for (int j = 0; j < columns; j++) {
      client_map[i][j] = line[j];
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
  int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

  // Strategy 1: Mark obvious mines (where remaining mines = unknown neighbors)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] >= '0' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        int marked = 0;
        int unknown = 0;
        int first_unknown_r = -1, first_unknown_c = -1;

        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] == '@') {
              marked++;
            } else if (client_map[ni][nj] == '?') {
              unknown++;
              if (first_unknown_r == -1) {
                first_unknown_r = ni;
                first_unknown_c = nj;
              }
            }
          }
        }

        // If all remaining unknowns must be mines
        if (unknown > 0 && mine_count - marked == unknown) {
          Execute(first_unknown_r, first_unknown_c, 1);  // Mark as mine
          return;
        }
      }
    }
  }

  // Strategy 2: Auto-explore where all mines are marked
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        int marked = 0;
        bool has_unknown = false;

        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] == '@') {
              marked++;
            } else if (client_map[ni][nj] == '?') {
              has_unknown = true;
            }
          }
        }

        if (has_unknown && marked == mine_count) {
          Execute(i, j, 2);  // Auto-explore
          return;
        }
      }
    }
  }

  // Strategy 3: Find safest cell to visit
  // Pick unvisited cell with max visited neighbors and min marked neighbors
  int best_r = -1, best_c = -1;
  int best_score = -1000;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] == '?') {
        int visited_count = 0;
        int marked_count = 0;

        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] >= '0' && client_map[ni][nj] <= '8') {
              visited_count++;
            } else if (client_map[ni][nj] == '@') {
              marked_count++;
            }
          }
        }

        int score = visited_count * 10 - marked_count * 5;
        if (score > best_score) {
          best_score = score;
          best_r = i;
          best_c = j;
        }
      }
    }
  }

  if (best_r != -1) {
    Execute(best_r, best_c, 0);
    return;
  }
}

#endif
