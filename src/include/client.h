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
bool client_visited[35][35];  // True if cell has been visited
bool client_marked[35][35];   // True if cell has been marked as mine
int unknown_neighbors[35][35]; // Count of unknown neighbors for each cell

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
      client_visited[i][j] = false;
      client_marked[i][j] = false;
      unknown_neighbors[i][j] = 0;
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

      if (line[j] >= '0' && line[j] <= '8') {
        client_visited[i][j] = true;
      } else if (line[j] == 'X') {
        // Hit a mine, game over
        client_visited[i][j] = true;
      } else if (line[j] == '@') {
        client_marked[i][j] = true;
      }
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

  // Update unknown neighbors count
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_visited[i][j] && client_map[i][j] >= '0' && client_map[i][j] <= '8') {
        int unknown = 0;
        int marked = 0;
        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_map[ni][nj] == '?') {
              unknown++;
            } else if (client_marked[ni][nj]) {
              marked++;
            }
          }
        }
        unknown_neighbors[i][j] = unknown;
      }
    }
  }

  // Strategy 1: Find cells where all unknown neighbors must be mines
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_visited[i][j] && client_map[i][j] >= '0' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        int marked = 0;
        int unknown = 0;

        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_marked[ni][nj]) {
              marked++;
            } else if (client_map[ni][nj] == '?') {
              unknown++;
            }
          }
        }

        // If remaining mines equals unknown cells, mark all unknowns
        if (unknown > 0 && mine_count - marked == unknown) {
          for (int d = 0; d < 8; d++) {
            int ni = i + dx[d];
            int nj = j + dy[d];
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (client_map[ni][nj] == '?') {
                Execute(ni, nj, 1);  // Mark as mine
                return;
              }
            }
          }
        }
      }
    }
  }

  // Strategy 2: Find cells where all mines are marked, auto-explore
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_visited[i][j] && client_map[i][j] >= '1' && client_map[i][j] <= '8') {
        int mine_count = client_map[i][j] - '0';
        int marked = 0;

        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_marked[ni][nj]) {
              marked++;
            }
          }
        }

        if (marked == mine_count) {
          Execute(i, j, 2);  // Auto-explore
          return;
        }
      }
    }
  }

  // Strategy 3: Find a safe cell to visit (advanced logic)
  // Look for cells that can be deduced as safe through constraint satisfaction
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] == '?') {
        // Check if this cell can be proven safe by analyzing constraints
        bool might_be_safe = true;
        bool proven_safe = false;

        // Check all visited neighbors
        for (int d1 = 0; d1 < 8; d1++) {
          int ni1 = i + dx[d1];
          int nj1 = j + dy[d1];
          if (ni1 >= 0 && ni1 < rows && nj1 >= 0 && nj1 < columns) {
            if (client_visited[ni1][nj1] && client_map[ni1][nj1] >= '0' && client_map[ni1][nj1] <= '8') {
              int mine_count1 = client_map[ni1][nj1] - '0';
              int marked1 = 0;
              int unknown1 = 0;

              for (int d2 = 0; d2 < 8; d2++) {
                int ni2 = ni1 + dx[d2];
                int nj2 = nj1 + dy[d2];
                if (ni2 >= 0 && ni2 < rows && nj2 >= 0 && nj2 < columns) {
                  if (client_marked[ni2][nj2]) {
                    marked1++;
                  } else if (client_map[ni2][nj2] == '?') {
                    unknown1++;
                  }
                }
              }

              // If all mines for this neighbor are already marked, this cell is safe
              if (marked1 == mine_count1) {
                proven_safe = true;
              }
            }
          }
        }

        if (proven_safe) {
          Execute(i, j, 0);  // Visit this safe cell
          return;
        }
      }
    }
  }

  // Strategy 4: If no safe move found, pick the cell with lowest risk
  // Find unvisited cells with most visited neighbors (likely safer)
  int best_i = -1, best_j = -1;
  int max_visited_neighbors = -1;

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] == '?') {
        int visited_neighbors = 0;
        for (int d = 0; d < 8; d++) {
          int ni = i + dx[d];
          int nj = j + dy[d];
          if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
            if (client_visited[ni][nj]) {
              visited_neighbors++;
            }
          }
        }

        if (visited_neighbors > max_visited_neighbors) {
          max_visited_neighbors = visited_neighbors;
          best_i = i;
          best_j = j;
        }
      }
    }
  }

  if (best_i != -1) {
    Execute(best_i, best_j, 0);  // Visit best guess
    return;
  }

  // Fallback: visit any unvisited cell
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (client_map[i][j] == '?') {
        Execute(i, j, 0);
        return;
      }
    }
  }
}

#endif