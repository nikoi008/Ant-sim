#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef enum{
    UP,
    RIGHT,
    DOWN,
    LEFT
} direction;

typedef struct{
    int x;
    int y;
    direction dir;
} Ant;

Ant Ants[8];

void defaultAnt(){ //starts a 200x200 grid sized up to 5px in a 1000x1000 display
    //char *ask;
    //printf("How many ants do you want?");
    //scanf("%s",ask);

    Ants[0].x = 50;
    Ants[0].y = 50;
    Ants[0].dir = DOWN;

    int grid[200][200];
    for(int i = 0; i < 200; i++){
        for(int j = 0; j < 200; j++){
            grid[i][j] = 0;
        }
    }

    bool AntDead = false;
    InitWindow(1000,1000,"simulation");
    RenderTexture2D RenderGrid = LoadRenderTexture(1000,1000);
    SetTargetFPS(10000);
    ClearBackground(RAYWHITE);
    double iteration = 0;
    while (!AntDead && !WindowShouldClose()) {
        
        int x = Ants[0].x;
        int y = Ants[0].y;

        if (x < 0 || x >= 200 || y < 0 || y >= 200) {
            AntDead = true;
            break;
        }

        
        BeginTextureMode(RenderGrid);

        if (grid[y][x] == 0) {

            grid[y][x] = 1;
            DrawRectangle(x * 5, y * 5, 5, 5, BLACK);
            Ants[0].dir = (Ants[0].dir + 1) % 4;
        } else {
  
            grid[y][x] = 0;
            DrawRectangle(x * 5, y * 5, 5, 5, RAYWHITE);
            Ants[0].dir = (Ants[0].dir + 3) % 4; 
        }

        switch (Ants[0].dir) {
            case UP:    Ants[0].y--; break;
            case RIGHT: Ants[0].x++; break;
            case DOWN:  Ants[0].y++; break;
            case LEFT:  Ants[0].x--; break;
        }
        EndTextureMode();

        if (Ants[0].x < 0 || Ants[0].x >= 200 ||
            Ants[0].y < 0 || Ants[0].y >= 200) {
            AntDead = true;
            break;
        }
        iteration++;
         BeginDrawing();
         ClearBackground(RAYWHITE);
         DrawTexture(RenderGrid.texture, 0, 0, RAYWHITE);
         EndDrawing();
    }


}

void CLI(){
    while(1){
        char ask[20];
        printf("Type 'default' for the classic simulation \nType 'random' for randomly generated simulation with different rules\nType 'custom' for setting your own rules\nType 'symmetry' for some preset symmetrical grids");
        scanf("%s",ask);
        if(strcmp(ask,"default") == 0) defaultAnt(); //ask how many ants
        //else if(strcmp(ask,"random") == 0) randomAnt();
        //else if(strcmp(ask,"custom") == 0) customAnt();
        //else if(strcmp(ask,"symmetry") == 0) symmetricalAnt; 
        else printf("invalid choice, try again");
    }
}

int main(void) {
    CLI();
    return 0;
}
