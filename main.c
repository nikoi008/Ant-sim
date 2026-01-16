    #include <stdio.h>
    #include "raylib.h"
    #include <stdlib.h>
    #include <time.h>
    #define RAYLIB_NUKLEAR_IMPLEMENTATION
    #define RAYLIB_NUKLEAR_INCLUDE_DEFAULT_FONT
    #include "raylib-nuklear.h"
    #include <string.h>
    #include <stdbool.h>
    #include "rlgl.h"
    #include "raymath.h"
    #if defined(PLATFORM_WEB)
        #include <emscripten/emscripten.h>
    #endif

    typedef enum{
        N,
        NE,
        SE,
        S,
        SW,
        NW

    }HexDirection;
    typedef enum{
        UP,
        RIGHT,
        DOWN,
        LEFT
    }TileDirection;

    typedef enum{
        R,
        L,
        U,
        C
    }TileRules;
    typedef enum{
        R1,
        R2,
        L1,
        L2,
        UR,
        NR
    }HexRules;
    typedef struct {
        int antX;
        int antY;
        TileDirection tAntDir;
        HexDirection hAntDir;
        bool antDead;
    } Ant;

    typedef struct {
        int r;
        int g;
        int b;
        TileRules colourRuleTile;
        HexRules colourRuleHex;
    } tileColour;

    typedef enum{
        MODE_HEX,
        MODE_TILE
    }Modes;
    Modes CurrentMode = MODE_TILE;

    Ant ants[8];
    tileColour Colours[20];
    static int grid[400][400];

    int colourCount = 2;
    int antcount = 1;

    int antlen[8][3] = {0};
    int colourlen[20][3] = {0};


    char antTileDirs[4][7] = {"UP", "RIGHT", "DOWN", "LEFT"};
    const char *tileDirPointers[] = { antTileDirs[0], antTileDirs[1], antTileDirs[2], antTileDirs[3] };
    int selectedTileDirs[8] = {0};

    char antHexDirs[6][4] = {"N","NE","SE","S","SW","NW"};
    const char *hexDirPointers[] = {antHexDirs[0],antHexDirs[1],antHexDirs[2],antHexDirs[3],antHexDirs[4],antHexDirs[5]};
    int selectedHexDirs[8] = {0};

    char tileRules[4][4] = {"R", "L", "U", "C"};
    const char *tileRulePointers[] = { tileRules[0], tileRules[1], tileRules[2], tileRules[3] };
    int selectedTileRules[20] = {0};

    char hexRules[6][4] = {"R1","R2","L1","L2","UR","NR"};
    const char *hexRulePointers[] = {hexRules[0],hexRules[1],hexRules[2],hexRules[3],hexRules[4],hexRules[5]};
    int selectedHexRules[20] = {0};
void InitAntStruct(){
    for(int i = 0; i < 8; i++){
        ants[i].antX = rand() % 200;
        ants[i].antY = rand() % 200;
        selectedTileDirs[i] = rand() % 4;
        selectedHexDirs[i] = rand() % 6;


    }
}

void InitColourStruct() {
    for (int i = 0; i < 20; i++) {
        Colours[i].r = rand() % 255;
        Colours[i].g = rand() % 255;
        Colours[i].b = rand() % 255;
        selectedTileRules[i] = rand() % 4;
        selectedHexRules[i] = rand() % 6;

    }
}

    //TODO READD PRESETS
    bool startGame;

    void drawHexSpecificLauncher(struct nk_context *ctx){
        //todo add hex presets
    }
    void drawTileSpecificLauncher(struct nk_context *ctx){
            float ratio_three[] = {0.33333333333333,0.3333333333,0.333333333}; 
            nk_layout_row(ctx, NK_DYNAMIC, 30, 3, ratio_three);
            struct nk_rect bounds = nk_widget_bounds(ctx);
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)){ 
                nk_tooltip(ctx, "  Classic rules of Langtons ant: 1 ant and 2 colours with rules R L");
                }
            if(nk_button_label(ctx,"DEFAULT RULES")){
            // setDefaultRules();
                startGame = true;
            }
            struct nk_rect bounds2 = nk_widget_bounds(ctx);
            if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds2)){ 
                    nk_tooltip(ctx, "  Infinitely looping symmetrical pattern. 4 ants and 2 colours of rules R L");
                }
            if(nk_button_label(ctx,"SYMMETRY")){
                //setSymmetryRules();
                startGame = true;
            }
            struct nk_rect bounds3 = nk_widget_bounds(ctx);
            if(nk_button_label(ctx,"RANDOM RULES")){
            // setRandomRules();
                startGame = true;
            }
    }
    static int int_slider = 0;
    bool RunLauncherFrame(struct nk_context *ctx) {
        bool startGame = false;
        UpdateNuklear(ctx);
        float ratio_two[] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
        if (nk_begin(ctx, "Settings", nk_rect(50, 50, 800, 800), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
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
            float width[] = {200,300,300};
            nk_layout_row(ctx, NK_STATIC, 30, 2, width);
            nk_label(ctx,"SIMULATION SPEED",NK_TEXT_CENTERED); //common
            nk_slider_int(ctx, 1, &int_slider, 10, 1); 
            if(nk_button_label(ctx,"MODE")){
                CurrentMode ++;
                CurrentMode = CurrentMode % 2;
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
                if(CurrentMode == MODE_TILE) nk_combobox(ctx, tileDirPointers, 4, &selectedTileDirs[i], 25, nk_vec2(nk_widget_width(ctx), 200));
                else nk_combobox(ctx, hexDirPointers, 4, &selectedHexDirs[i], 25, nk_vec2(nk_widget_width(ctx), 200));
                
            }


            float widths[] = {130, 130, 130, 80, 130, 130}; 
            for (int i = 0; i < colourCount; i++) {
                nk_layout_row(ctx, NK_STATIC, 30, 6, widths);

                Colours[i].r = nk_propertyi(ctx, "#R", 0, Colours[i].r, 255, 1, 1);
                Colours[i].g = nk_propertyi(ctx, "#G", 0, Colours[i].g, 255, 1, 1);
                Colours[i].b = nk_propertyi(ctx, "#B", 0, Colours[i].b, 255, 1, 1);
                struct nk_rect bounds = nk_widget_bounds(ctx);
                nk_label(ctx, "RULE", NK_TEXT_CENTERED);
                if(CurrentMode == MODE_TILE){
                nk_combobox(ctx, tileRulePointers, 4, &selectedTileRules[i], 25, nk_vec2(nk_widget_width(ctx), 200));

                if (nk_input_is_mouse_hovering_rect(&ctx->input, bounds)){ 
                    if(selectedTileRules[i] == 0) nk_tooltip(ctx, "  Turns ant 90 degrees Clockwise");
                    if(selectedTileRules[i] == 1) nk_tooltip(ctx, "  Turns ant 90 degrees anticlockwise");
                    if(selectedTileRules[i] == 3) nk_tooltip(ctx, "  Makes the ant go forwards");
                    if(selectedTileRules[i] == 2) nk_tooltip(ctx, "  Turns the ant 180 degrees");
                }
                nk_button_color(ctx, nk_rgb(Colours[i].r, Colours[i].g, Colours[i].b)); 
                }else{
                    nk_combobox(ctx, hexRulePointers, 6, &selectedHexRules[i], 25, nk_vec2(nk_widget_width(ctx), 200));
                    //TODO ADD HOBVERING
                    nk_button_color(ctx, nk_rgb(Colours[i].r, Colours[i].g, Colours[i].b)); 
                }

            }
            if(startGame == true) return true;
        }
        nk_end(ctx);

        ClearBackground(RAYWHITE);
        DrawNuklear(ctx);

        return startGame;
    }
    void runHexSim() {
    float radius = 5.0f;
    float height = sqrtf(3.0f) * radius;

    for (int i = 0; i < antcount; i++) {
        if (ants[i].antDead) continue;

        int x = ants[i].antX;
        int y = ants[i].antY;

        if (x < 0 || x >= 400 || y < 0 || y >= 400) {
            ants[i].antDead = true;
            continue;
        }


        int currentIndex = grid[y][x];
        HexRules currentRule = Colours[currentIndex].colourRuleHex;

        switch (currentRule) {
            case R1: ants[i].hAntDir = (ants[i].hAntDir + 1) % 6; break;
            case R2: ants[i].hAntDir = (ants[i].hAntDir + 2) % 6; break;
            case L1: ants[i].hAntDir = (ants[i].hAntDir + 5) % 6; break;
            case L2: ants[i].hAntDir = (ants[i].hAntDir + 4) % 6; break;
            case UR: ants[i].hAntDir = (ants[i].hAntDir + 3) % 6; break;
            case NR: break;
        }

        grid[y][x] = (currentIndex + 1) % colourCount;
        tileColour tile = Colours[grid[y][x]];

        float posX = x * (radius * 1.5f);
        float posY = y * height;
        if (x % 2 != 0) posY += height / 2.0f;
        
        DrawPoly((Vector2){posX, posY}, 6, radius, 0, (Color){tile.r, tile.g, tile.b, 255});
        DrawPolyLines((Vector2){posX, posY}, 6, radius, 0, BLACK);
        if (x % 2 == 0) {
            switch(ants[i].hAntDir) {
                case N:  y--; break;
                case S:  y++; break;
                case NE: x++; y--; break;
                case SE: x++; break;
                case NW: x--; y--; break;
                case SW: x--; break;
            }
        } else {
            switch(ants[i].hAntDir) {
                case N:  y--; break;
                case S:  y++; break;
                case NE: x++; break;
                case SE: x++; y++; break;
                case NW: x--; break;
                case SW: x--; y++; break;
            }
        }

        ants[i].antX = x;
        ants[i].antY = y;
    }
}
    void runTileSim(RenderTexture2D canvas){    
        for (int i = 0; i < antcount; i++) {
            if (ants[i].antDead) continue;
            
            int x = ants[i].antX;
            int y = ants[i].antY;
            

            if (x < 0 || x >= 200 || y < 0 || y >= 200) {
                ants[i].antDead = true;
                continue;
            }

            int currentIndex = grid[y][x];
            TileRules currentRule = Colours[currentIndex].colourRuleTile;

            switch (currentRule) {
                case R: ants[i].tAntDir = (ants[i].tAntDir + 1) % 4; break; 
                case L: ants[i].tAntDir = (ants[i].tAntDir + 3) % 4; break;
                case U: ants[i].tAntDir = (ants[i].tAntDir + 2) % 4; break;
                case C: break; 
            }

            grid[y][x] = (currentIndex + 1) % colourCount; 
            tileColour tile = Colours[grid[y][x]];
            DrawRectangle(x * 5, y * 5, 5, 5, (Color){tile.r, tile.g, tile.b, 255});
            switch (ants[i].tAntDir) {
                case UP:    ants[i].antY--; break;
                case RIGHT: ants[i].antX++; break; 
                case DOWN:  ants[i].antY++; break;
                case LEFT:  ants[i].antX--; break;
                }
            }
        
            for (int i = 0; i < antcount; i++) {
            if (ants[i].antDead) continue;
            
            int x = ants[i].antX;
            int y = ants[i].antY;
            

            if (x < 0 || x >= 200 || y < 0 || y >= 200) {
                ants[i].antDead = true;
                continue;
            }

            int currentIndex = grid[y][x];
            TileRules currentRule = Colours[currentIndex].colourRuleTile;
            }

        }



    typedef enum{
        INITIALISING,
        LAUNCHER,
        INITSIM,
        SIM,
        PAUSED
    }states;
    states state = INITIALISING;
        
    Camera2D camera = { 0 };
    int zoomMode = 0;

    void zoomyCameraStuff(){
            /*
    
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Vector2 delta = GetMouseDelta();
                delta = Vector2Scale(delta, -1.0f/camera.zoom);
                camera.target = Vector2Add(camera.target, delta);
            } *///curently commented out because its buggy and i dont know how to fix it :)

            if (zoomMode == 0)
            {
    
                float wheel = GetMouseWheelMove();
                if (wheel != 0)
                {
                    Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
                    camera.offset = GetMousePosition();
                    camera.target = mouseWorldPos;

                    float scale = 0.2f*wheel;
                    camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.5f, 64.0f);
                    
                }
            }
    }


    int main(void) {

            srand(time(NULL));
            memset(grid, 0, sizeof(grid));
            InitColourStruct();
            InitAntStruct();
            InitWindow(1000, 1000, "Langton's Ant");
            SetTargetFPS(90);
            const int fontSize = 20;
            Font font = LoadFontFromNuklear(fontSize);
            struct nk_context *ctx = InitNuklearEx(font, fontSize);
            state = LAUNCHER;
            RenderTexture2D canvas = LoadRenderTexture(2000, 2000);
            camera.target = (Vector2){ 500.0f, 500.0f };
            camera.offset = (Vector2){ 500.0f, 500.0f };
            camera.rotation = 0.0f;
            camera.zoom = 1.0f;
            while(!WindowShouldClose()){
        BeginDrawing();
        if (state == SIM) zoomyCameraStuff();
        switch(state){

        case(LAUNCHER):

        if (RunLauncherFrame(ctx)) {
            state = INITSIM;

        }
        break;
        
        case(INITSIM):
            memset(grid, 0, sizeof(grid));
            for(int i = 0; i < antcount; i++) {
                ants[i].tAntDir = (TileDirection)selectedTileDirs[i];
                ants[i].hAntDir = (HexDirection)selectedHexDirs[i];
                ants[i].antDead = false;
            }
            for (int i = 0; i < colourCount; i++) {
                Colours[i].colourRuleHex = (HexRules)selectedHexRules[i];
                Colours[i].colourRuleTile = (TileRules)selectedTileRules[i];
            }

            UnloadNuklear(ctx);
            int fps = int_slider * int_slider * 50;
            if (fps > 3000) fps = 3000;
                SetTargetFPS(fps);
            BeginTextureMode(canvas);
            ClearBackground(RAYWHITE);
            EndTextureMode();
            state = SIM;
            break;
        case(SIM):
        BeginTextureMode(canvas);
        for(int i = 0; i < 10 * int_slider; i++){
            if(CurrentMode == MODE_TILE){
            runTileSim(canvas);
            }else{
                runHexSim();
            }
        }
        EndTextureMode();
            BeginMode2D(camera);
            DrawTextureRec(canvas.texture, (Rectangle){ 0, 0, (float)canvas.texture.width, (float)-canvas.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndMode2D();
            break;
        }
        EndDrawing();
        }
        
    } 


        
