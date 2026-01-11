#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#define RAYLIB_NUKLEAR_IMPLEMENTATION
#define RAYLIB_NUKLEAR_INCLUDE_DEFAULT_FONT
#include "raylib-nuklear.h"
#include <string.h>
#include <stdbool.h>
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef enum { UP, RIGHT, DOWN, LEFT } Direction;
typedef enum { R, L, U, C } Rule;

typedef struct {
    int antX;
    int antY;
    Direction antDir;
    bool antDead;
} Ant;

typedef struct {
    int r;
    int g;
    int b;
    Rule colourRule;
} tileColour;

Ant ants[8];
tileColour Colours[20];
static int grid[200][200];
int colourCount = 2;
int antcount = 1;
int antlen[8][3] = {0};
int colourlen[20][3] = {0};
int selected_dirs[8] = {0};
int selected_rules[20] = {0};
char dirs[4][7] = {"UP", "RIGHT", "DOWN", "LEFT"};
const char *dir_pointers[] = { dirs[0], dirs[1], dirs[2], dirs[3] };
char rules[4][4] = {"R", "L", "U", "C"};
const char *rule_pointers[] = { rules[0], rules[1], rules[2], rules[3] };

void InitColourStruct() {
    for (int i = 0; i < 20; i++) {
        Colours[i].r = rand() % 255;
        Colours[i].g = rand() % 255;
        Colours[i].b = rand() % 255;
        selected_rules[i] = rand() % 4;
    }
}

void InitAntStruct(){
    for(int i = 0; i < 8; i++){
        ants[i].antX = rand() % 200;
        ants[i].antY = rand() % 200;
        selected_dirs[i] = rand() % 4;


    }
}
void setDefaultRules(){
    antcount = 1;
    ants[0].antX = 100;
    ants[0].antY = 100;
    selected_dirs[0] = rand() % 4;
    Colours[0].r = 0;
    Colours[0].g = 0;
    Colours[0].b = 0;
    colourCount = 2;
    selected_rules[0] = R;
    selected_rules[1] = L;
    Colours[0].colourRule = R;
    Colours[1].r = 255;
    Colours[1].g = 255;
    Colours[1].b = 255;
    Colours[1].colourRule = L;
}

void setSymmetryRules(){
    antcount = 4;
    ants[0].antX = 107; ants[0].antY = 107; selected_dirs[0] = UP;
    ants[1].antX = 107; ants[1].antY = 93; selected_dirs[1] = LEFT; 
    ants[2].antX = 93; ants[2].antY = 93; selected_dirs[2] = DOWN;
    ants[3].antX = 93; ants[3].antY = 107; selected_dirs[3] = RIGHT;
    colourCount = 2;
    Colours[0].r = rand() % 255;
    Colours[0].g = rand() % 255;
    Colours[0].b = rand() % 255;
    selected_rules[0] = R;
    selected_rules[1] = L;
    Colours[0].colourRule = R;
    Colours[1].r = rand() % 255;
    Colours[1].g = rand() % 255;
    Colours[1].b = rand() % 255;
    Colours[1].colourRule = L;
}  
void setRandomRules(){
    antcount = rand() % 8;
    colourCount = rand() % 20;
    for(int i = 0; i < antcount; i++){
        ants[i].antX = rand() % 200;
        ants[i].antY = rand() % 200;
        selected_dirs[i] = rand() % 4;
    }
    for(int i = 0; i < colourCount; i++){
            Colours[i].r = rand() % 255;
            Colours[i].g = rand() % 255;
            Colours[i].b = rand() % 255;
            selected_rules[i] = rand() % 4;
    }
}
bool RunLauncherFrame(struct nk_context *ctx) {
    bool startGame = false;
    UpdateNuklear(ctx);
    float ratio_two[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
    if (nk_begin(ctx, "Settings", nk_rect(50, 50, 800, 800), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
        struct nk_rect bounds = nk_widget_bounds(ctx);
        nk_layout_row(ctx, NK_DYNAMIC, 30, 5, ratio_two);
        if (nk_button_label(ctx, "START SIM")) {
            startGame = true;
        }
        if (nk_button_label(ctx, "ADD ANT")) {
            antcount++; 
            if (antcount > 8) antcount = 8;
        }
        if (nk_button_label(ctx, "REMOVE ANT")) {
            antcount--;
            if (antcount < 1) antcount = 1;
        }   
        if (nk_button_label(ctx, "ADD COLOUR")) {
            colourCount++;
            if (colourCount > 20) colourCount = 20;
        }
        if (nk_button_label(ctx, "REMOVE COLOUR")) {
            colourCount--;
            if (colourCount < 2) colourCount = 2;
        }
        float ratio_three[] = {0.33333333333333f,0.33333333333333f,0.33333333333333f}; // just to be sure :)
        nk_layout_row(ctx, NK_DYNAMIC, 30, 3, ratio_three);
        if(nk_button_label(ctx,"DEFAULT RULES")){
            setDefaultRules();
            startGame = true;
        }
        if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds))
            nk_tooltip(ctx, "Set symmetry rules");
        if(nk_button_label(ctx,"SYMMETRY")){
            setSymmetryRules();
            startGame = true;
        }



        if(nk_button_label(ctx,"RANDOM RULES")){
            setRandomRules();
            startGame = true;
        }
       
        nk_layout_row_dynamic(ctx, 30, 3);
        for (int i = 0; i < antcount; i++) {
            struct nk_rect bounds = nk_widget_bounds(ctx);
            char bufferX[25] = "";
            char bufferY[25] = "";
            char butterDir[25] = "";
            sprintf(bufferX, "Ant %d X coordinate", i + 1);
            sprintf(bufferY, "Ant %d y coordinate", i + 1);
            sprintf(butterDir, "Ant %d direction", i + 1);

            nk_label(ctx, bufferX, NK_TEXT_LEFT);
            
            nk_label(ctx, bufferY, NK_TEXT_LEFT);
            nk_label(ctx, butterDir, NK_TEXT_LEFT);
            char x_id[32], y_id[32];
            sprintf(x_id, "X%d", i);
            sprintf(y_id, "Y%d", i);
            ants[i].antX = nk_propertyi(ctx,x_id,0,ants[i].antX,199,1,1); //IMPORTANT TO SET FOR WEBSITE: ADD ID'S AND CHANGE MAX TO 199
            ants[i].antY = nk_propertyi(ctx,y_id,0,ants[i].antY,199,1,1);




        }


        float widths[] = {130, 130, 130, 130, 80, 130}; 
        for (int i = 0; i < colourCount; i++) {
            nk_layout_row(ctx, NK_STATIC, 30, 6, widths);

            Colours[i].r = nk_propertyi(ctx, "#R", 0, Colours[i].r, 255, 1, 1);
            Colours[i].g = nk_propertyi(ctx, "#G", 0, Colours[i].g, 255, 1, 1);
            Colours[i].b = nk_propertyi(ctx, "#B", 0, Colours[i].b, 255, 1, 1);
    
            nk_label(ctx, "RULE", NK_TEXT_CENTERED);
            struct nk_rect bounds = nk_widget_bounds(ctx);
            nk_combobox(ctx, rule_pointers, 4, &selected_rules[i], 25, nk_vec2(nk_widget_width(ctx), 200));
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)){ 
                if(selected_rules[i] == 0) nk_tooltip(ctx, "  Turns ant 90 degrees Clockwise");
                if(selected_rules[i] == 1) nk_tooltip(ctx, "  Turns ant 90 degrees anticlockwise");
                if(selected_rules[i] == 3) nk_tooltip(ctx, "  Makes the ant go forwards");
                if(selected_rules[i] == 2) nk_tooltip(ctx, "  Turns the ant 180 degrees");
            }
            nk_button_color(ctx, nk_rgb(Colours[i].r, Colours[i].g, Colours[i].b)); 

        }
    }
    nk_end(ctx);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawNuklear(ctx);
    EndDrawing();

    return startGame;
}
void runSimFrame(RenderTexture2D canvas) {
    BeginTextureMode(canvas);
    for (int i = 0; i < antcount; i++) {
        if (ants[i].antDead) continue;
        int x = ants[i].antX;
        int y = ants[i].antY;
        
        if (x < 0 || x >= 200 || y < 0 || y >= 200) {
            ants[i].antDead = true;
            continue;
        }

        int currentIndex = grid[y][x];
        Rule currentRule = Colours[currentIndex].colourRule;

        switch (currentRule) {
            case R: ants[i].antDir = (ants[i].antDir + 1) % 4; break; 
            case L: ants[i].antDir = (ants[i].antDir + 3) % 4; break;
            case U: ants[i].antDir = (ants[i].antDir + 2) % 4; break;
            case C: break;
        }
        tileColour tile = Colours[grid[y][x]];
        DrawRectangle(x * 5, y * 5, 5, 5, (Color){tile.r, tile.g, tile.b, 255});
        grid[y][x] = (currentIndex + 1) % colourCount; 
        


        switch (ants[i].antDir) {
            case UP:    ants[i].antY--; break;
            case RIGHT: ants[i].antX++; break; 
            case DOWN:  ants[i].antY++; break;
            case LEFT:  ants[i].antX--; break;
        }
    }
    EndTextureMode();

    BeginDrawing();
        ClearBackground(DARKGRAY); 
        DrawTextureRec(canvas.texture, (Rectangle){ 0, 0, (float)canvas.texture.width, (float)-canvas.texture.height }, (Vector2){ 0, 0 }, WHITE);
    EndDrawing();
}

int main(void) {
    srand(time(NULL));
    InitColourStruct();
    InitAntStruct();
    InitWindow(1000, 1000, "Langton's Ant");
    SetTargetFPS(100000);
    const int fontSize = 20;
    Font font = LoadFontFromNuklear(fontSize);
    struct nk_context *ctx = InitNuklearEx(font, fontSize);

    while (!WindowShouldClose()) {
        if (RunLauncherFrame(ctx)) break;
    }

    for (int i = 0; i < antcount; i++) {
        ants[i].antDir = (Direction)selected_dirs[i];
        ants[i].antDead = false;
    }
    for (int i = 0; i < colourCount; i++) {
        Colours[i].colourRule = (Rule)selected_rules[i];
    }

    UnloadNuklear(ctx);

    BeginDrawing();
    EndDrawing();
    RenderTexture2D canvas = LoadRenderTexture(1000, 1000);
    BeginTextureMode(canvas);
    ClearBackground(RAYWHITE); 
    EndTextureMode();
    while (!WindowShouldClose()) {
        runSimFrame(canvas);
    }
    CloseWindow();
    return 0;
}  
