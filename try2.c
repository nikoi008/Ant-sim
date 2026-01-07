#include "raylib.h"
#include <stdio.h>
#include <string.h>

typedef enum { 
    UP, 
    RIGHT, 
    DOWN, 
    LEFT 
} direction;
typedef struct { 
    int x; 
    int y; 
    direction dir; 
} Ant;


int grid[200][200]; 

void defaultAnt() {

    for(int i = 0; i < 200; i ++){
        for(int j = 0; j< 200; j++){
            grid[i][j] = 0;
        }
    }
    Ant ant = { 100, 100, UP };


    InitWindow(1000, 1000, "Simulation");
    SetTargetFPS(500); 


    ClearBackground(RAYWHITE);
   

    while (!WindowShouldClose()) {
        if (ant.x < 0 || ant.x >= 200 || ant.y < 0 || ant.y >= 200) break;

        BeginDrawing();
        if (grid[ant.x][ant.y] == 0) {
            grid[ant.x][ant.y] = 1;
            ant.dir = (ant.dir + 1) % 4; 
            DrawRectangle(ant.x * 5, ant.y * 5, 5, 5, BLACK);
        } else {
            grid[ant.x][ant.y] = 0;
            ant.dir = (ant.dir + 3) % 4; 
            DrawRectangle(ant.x * 5, ant.y * 5, 5, 5, RAYWHITE);
        }

        switch (ant.dir) {
            case UP:    ant.y--; break;
            case RIGHT: ant.x++; break;
            case DOWN:  ant.y++; break;
            case LEFT:  ant.x--; break;
        }
        EndDrawing();
    }


}

void CLI() {
    char ask[20];
    while(1) {
        printf("Type 'default' for classic simulation\n");
        printf("Type 'quit' to exit\n");

        
        if (scanf("%19s", ask) != 1) break;

        if (strcmp(ask, "default") == 0) {
            defaultAnt();
        } else if (strcmp(ask, "quit") == 0) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

int main(void) {
    CLI();
    return 0;
}