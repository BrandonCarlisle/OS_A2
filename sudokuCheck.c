
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>


typedef struct {
    int row;
    int col;
} Block;

pthread_mutex_t lock; 
bool solved = true;

int* GetGrid() {
    static int sudokuGrid[9][9] = {
        {6, 2, 4, 5, 3, 9, 1, 8, 7},
        {5, 1, 9, 7, 2, 8, 6, 3, 4},
        {8, 3, 7, 6, 1, 4, 2, 9, 5},
        {1, 4, 3, 8, 6, 5, 7, 2, 9},
        {9, 5, 8, 2, 4, 7, 3, 6, 1},
        {7, 6, 2, 3, 9, 1, 4, 5, 8},
        {3, 7, 1, 9, 5, 6, 8, 4, 2},
        {4, 9, 6, 1, 8, 2, 5, 7, 3},
        {2, 8, 5, 4, 7, 3, 9, 1, 6}
    };
    return sudokuGrid;
}


bool checkRow(int row, int grid[9][9]) {
    int y;
    int checkArr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (y = 0; y < 9; y++) {
        int chkNum = grid[row][y];
        if (checkArr[chkNum-1] == chkNum) {
            checkArr[chkNum-1] = 0;
        }
        else {
            return false;
        }
    }
    return true;
}


bool checkCol(int col, int grid[9][9]) {
    int x;
    int checkArr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (x = 0; x < 9; x++) {
        int chkNum = grid[x][col];
        if (checkArr[chkNum-1] == chkNum) {
            checkArr[chkNum-1] = 0;
        }
        else {
            return false;
        }
    }
    return true;
}


bool checkGridSection(Block* startBlock, int grid[9][9]) {
    if (startBlock->row < 7 && startBlock->col < 7) {
        int checkArr[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        int x = startBlock->row;
        int y = startBlock->col;
        int endx = startBlock->row + 3;
        int endy = startBlock->col + 3;
        for (x; x < endx; x++) {
            for (y; y < endy; y++) {
                int chkNum = grid[x][y];
                if (checkArr[chkNum-1] == chkNum) {
                    checkArr[chkNum-1] = 0;
                }
                else {
                    return false;
                }
            }
            y = y-3;
        }
        return true;
    }
    else {
        
        return false;
    }
}


void printGrid(int grid[9][9]) {
    int x, y;
    for (x = 0; x < 9; x++) {
        for (y = 0; y < 9; y++) {
            printf("%d ", grid[x][y]);
        }
        printf("\n");
    }
}


void* multCheckGridSection(void* vargp) 
{ 
    Block* startBlock = (Block*)vargp; 
    
    bool check = checkGridSection(startBlock, GetGrid());
    
    pthread_mutex_lock(&lock); 
    if (check == 0) {
        solved = false;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
} 


void* multCheckRows(void* vargp) 
{ 
    int* tid = (int*)vargp; 
    
    bool check = true;
    int i;
    for (i = 0; i < 10; i++) {
        bool rowCheck = checkRow(i, GetGrid());
        if (rowCheck == false) {
            check = false;
        }
    }
    
    pthread_mutex_lock(&lock);  
    if (check == false) {
        solved = false;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
} 


void* multCheckCols(void* vargp) 
{ 
    int* tid = (int*)vargp; 
    
    bool check = true;
    int i;
    for (i = 0; i < 10; i++) {
        bool colCheck = checkCol(i, GetGrid());
        if (colCheck == false) {
            check = false;
        }
    }
    
    pthread_mutex_lock(&lock);  
    if (check == false) {
        solved = false;
    }
    pthread_mutex_unlock(&lock);
    return NULL;
} 


bool checkValidGrid(int grid[9][9]) {
    int i; 
    pthread_t tid[11];
    
    int row = 0;
    int col = 0;
    for (i = 0; i < 9; i++) {
        Block* data = (Block*)malloc(sizeof(Block));
        data->row = row;
        data->col = col;
        pthread_create(&tid[i], NULL, multCheckGridSection, (Block*)data);
        row = row + 3;
        if (row == 9) {
            row = 0;
            col = col + 3;
        }
    }
    pthread_create(&tid[i], NULL, multCheckRows, (void*)&tid);
    i += 1;
    pthread_create(&tid[i], NULL, multCheckCols, (void*)&tid);
    
    for (i = 0; i < 11; i++)
        pthread_join(tid[i], NULL);
    
    if (solved)
        printf("Grid Solved\n");
    else
        printf("Grid Not Solved\n");
    pthread_exit(NULL); 
}


int main()
{
    checkValidGrid(GetGrid());
    return 0;
}


