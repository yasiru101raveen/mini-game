#include <iostream>
#include <cstring>
#include <sstream>
#include "raylib.h"

using namespace std;

struct AnimData{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
    bool passed;
};

//all used function prototypes
bool isOnGround(AnimData data, int windowheight);
AnimData updateAnimData(AnimData data, float deltaTime,int maxFrame);
void ShowInstructionsFromFile(int windowWidth, int windowHeight);
void DrawGameBackground(float bgX, float mgX, float fgX,Texture2D background,Texture2D midground,Texture2D foreground);
void SaveHighScores(int newScore);
void LoadHighScores(int highScores[]);
void ShowGameOverScreen(int windowWidth,int windowHeight,float bgX,float mgX,float fgX,Texture2D background,Texture2D midground,Texture2D foreground,int score);
void ShowWinScreen(int windowWidth,int windowHeight,float bgX,float mgX,float fgX,Texture2D background,Texture2D midground,Texture2D foreground,int score);
void ResetGame(AnimData& scarfyData,AnimData nebulae[],int sizeOfNebulae,int windowWidth,int windowHeight,Texture2D nebula,int& score, int& difficultyLevel, float& currentNebVel, float& scarfySpeedMultiplier, float baseNebVel,float& bgX, float& mgX, float& fgX);


//function for ground check
bool isOnGround(AnimData data, int windowheight)
{
    return  data.pos.y + data.rec.height>= windowheight ;
}

AnimData updateAnimData(AnimData data, float deltaTime,int maxFrame)
{
    //update running time
    data.runningTime += deltaTime;
   if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    } 
    return data;
}

void ShowInstructionsFromFile(int windowWidth, int windowHeight) {
    FILE *file = fopen("instructions.txt", "r");
    if (!file) {
        BeginDrawing();
        DrawText("INSTRUCTIONS NOT FOUND", 20, 20, 20, RED);
        DrawText("Create instructions.txt file", 20, 50, 20, GRAY);
        DrawText("Press SPACE to return", windowWidth/2 - 100, windowHeight - 50, 20, LIGHTGRAY);
        EndDrawing();
        return;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    
    // Title
    DrawText("INSTRUCTIONS", windowWidth/2 - MeasureText("INSTRUCTIONS", 40)/2, 40, 40, WHITE);
    
    // Read file line by line
    char line[256];
    int yPos = 100;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;
        DrawText(line, 50, yPos, 20, WHITE);
        yPos += 30;
    }
    fclose(file);
    
    // Back prompt
    DrawText("Press SPACE to return", windowWidth/2 - MeasureText("Press SPACE to return", 20)/2, 
             windowHeight - 50, 20, LIGHTGRAY);
    
    EndDrawing();
}

enum GameScreen { MENU, 
                  INSTRUCTIONS, 
                  GAMEPLAY, 
                  GAMEOVER, 
                  WIN, 
                  HIGHSCORES
                };

void DrawGameBackground(float bgX, 
                        float mgX, 
                        float fgX, 
                        Texture2D background, 
                        Texture2D midground, 
                        Texture2D foreground
                    );

const int MAX_HIGH_SCORES = 5;

void ShowGameOverScreen(int windowWidth, 
                        int windowHeight, 
                        float bgX, 
                        float mgX, 
                        float fgX, 
                        Texture2D background, 
                        Texture2D midground, 
                        Texture2D foreground,
                        int score) 
{
    // Draw the game background
    DrawGameBackground(bgX, mgX, fgX, background, midground, foreground);
    
    // Semi-transparent overlay
    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.5f));
    
    // Game over text
    DrawText("GAME OVER", windowWidth/2 - MeasureText("GAME OVER", 40)/2, 50, 40, RED);
    
    // Display current score
    stringstream ss;
    ss << "Your Score: " << score;
    string scoreStr = ss.str();

    DrawText(
        scoreStr.c_str(),
        windowWidth/2 - MeasureText(scoreStr.c_str(), 20)/2,
        windowHeight/2,
        20,
        WHITE
    );   
    
    DrawText("Press R to Play Again", windowWidth/2 - MeasureText("Press R to Play Again", 20)/2, 320, 20, WHITE);
    DrawText("Press SPACE for Main Menu", windowWidth/2 - MeasureText("Press SPACE for Main Menu", 20)/2, 350, 20, WHITE);
}

void ShowWinScreen(int windowWidth, 
                   int windowHeight, 
                   float bgX, 
                   float mgX, 
                   float fgX, 
                   Texture2D background, 
                   Texture2D midground, 
                   Texture2D foreground,
                   int score) 
{
    // Draw the game background
    DrawGameBackground(bgX, mgX, fgX, background, midground, foreground);
    
    // Semi-transparent overlay
    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.5f));
    
    // Win text
    DrawText("YOU WIN!", windowWidth/2 - MeasureText("YOU WIN!", 40)/2, 50, 40, GREEN);
    
    // Display current score
    stringstream ss;
    ss << "Your Score: " << score;
    string scoreStr = ss.str();

    DrawText(
        scoreStr.c_str(),
        windowWidth/2 - MeasureText(scoreStr.c_str(), 20)/2,
        windowHeight/2,
        20,
        WHITE
    );

    DrawText("Press R to Play Again", windowWidth/2 - MeasureText("Press R to Play Again", 20)/2, 320, 20, WHITE);
    DrawText("Press SPACE for Main Menu", windowWidth/2 - MeasureText("Press SPACE for Main Menu", 20)/2, 350, 20, WHITE);
}
void ResetGame(AnimData& scarfyData, 
               AnimData nebulae[], 
               int sizeOfNebulae, 
               int windowWidth, 
               int windowHeight, 
               Texture2D nebula,
               int& score,
               int& difficultyLevel,
               float& currentNebVel,
               float& scarfySpeedMultiplier,
               float baseNebVel,
               float& bgX,       
               float& mgX,
               float& fgX
               ) 
            {
    // Reset scarfy
    scarfyData.pos.x = windowWidth/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowHeight - scarfyData.rec.height;
    scarfyData.frame = 0;
    
    // Reset nebulae
    for (int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i].pos.x = windowWidth + (i *450);
        nebulae[i].pos.y = windowHeight - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].passed = false;
    }

    score = 0;
    difficultyLevel = 0;
    currentNebVel = baseNebVel;
    scarfySpeedMultiplier = 1.0f;
    bgX = 0.0f;
    mgX = 0.0f;
    fgX = 0.0f;
}

void DrawGameBackground(float bgX, float mgX, float fgX, Texture2D background, Texture2D midground, Texture2D foreground) {
    // Draw the background
    Vector2 bg1Pos{bgX, 0.0};
    DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
    Vector2 bg2Pos{bgX + background.width * 2, 0.0};
    DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

    // Draw the midground
    Vector2 mg1Pos{mgX, 0.0};
    DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
    Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
    DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

    // Draw the foreground
    Vector2 fg1Pos{fgX, 0.0};
    DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
    Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
    DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

}

void ShowWinScreen(int windowWidth, 
                   int windowHeight, 
                   float bgX, 
                   float mgX, 
                   float fgX, 
                   Texture2D background, 
                   Texture2D midground, 
                   Texture2D foreground)
                { 
    // Draw the game background
    DrawGameBackground(bgX, mgX, fgX, background, midground, foreground);
    
    // Semi-transparent overlay
    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.5f));
    
    // Win text
    DrawText("YOU WIN!", windowWidth/2 - MeasureText("YOU WIN!", 40)/2, 100, 40, GREEN);
    DrawText("Press R to Play Again", windowWidth/2 - MeasureText("Press R to Play Again", 20)/2, 200, 20, WHITE);
    DrawText("Press SPACE for Main Menu", windowWidth/2 - MeasureText("Press SPACE for Main Menu", 20)/2, 250, 20, WHITE);
}


void SaveHighScores(int newScore) {
    int highScores[MAX_HIGH_SCORES] = {0};
    
    // Load existing scores
    FILE *file = fopen("highscores.dat", "rb");
    if (file) {
        fread(highScores, sizeof(int), MAX_HIGH_SCORES, file);
        fclose(file);
    }
    
    // Only proceed if the new score is greater than 0
    if (newScore <= 0) {
        return;
    }
    
    // Check if the score already exists in the list
    bool scoreExists = false;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (highScores[i] == newScore) {
            scoreExists = true;
            break;
        }
    }
    
    if (scoreExists) {
        return; // Score already in the list, no need to add again
    }
    
    // Find the position to insert the new score
    int insertPos = -1;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (highScores[i] == 0 || newScore > highScores[i]) {
            insertPos = i;
            break;
        }
    }
    
    if (insertPos != -1) {
        // Shift scores down to make room
        for (int j = MAX_HIGH_SCORES - 1; j > insertPos; j--) {
            highScores[j] = highScores[j - 1];
        }
        // Insert the new score
        highScores[insertPos] = newScore;
    }
    
    // Save updated scores
    file = fopen("highscores.dat", "wb");
    if (file) {
        fwrite(highScores, sizeof(int), MAX_HIGH_SCORES, file);
        fclose(file);
    }
}

void LoadHighScores(int highScores[]) {
    // Initialize with zeros
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        highScores[i] = 0;
    }
    
    // Load from file if exists
    FILE *file = fopen("highscores.dat", "rb");
    if (file) {
        fread(highScores, sizeof(int), MAX_HIGH_SCORES, file);
        fclose(file);
    }
}

int main(){

    //variable declarations
    int windowDimensions[2]{512, 380};
    int score = 0; // Player's score
    float baseNebVel = -200.0f;  // Base nebula velocity
    float currentNebVel = baseNebVel;
    float scarfySpeedMultiplier = 1.0f;
    int difficultyLevel = 0;

    
    //initialize the wimdow
    InitWindow(windowDimensions[0],windowDimensions[1], "Dapper Dasher");

     // Game state
    GameScreen currentScreen = MENU;

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D scarfy = LoadTexture("textures/scarfy.png"); 

    // acceleration due to gravity (pixels/ s) / s
    const int gravity{1000};

    int sizeOfNebulae{100};
    AnimData *nebulae = new AnimData[sizeOfNebulae]; 

    for (int i = 0; i < sizeOfNebulae; i++){
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] -  nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 1.0 / 16.0;
        nebulae[i].passed = false;

        nebulae[i].pos.x = windowDimensions[0] + (i * 600);
    }
    
    //AnimData for scarfy
    AnimData scarfyData;

    //rectangle for scarfy
    scarfyData.rec.width = scarfy.width/6;  
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    //scarfy's position
    scarfyData.pos.x = windowDimensions[0] /2 - scarfyData.rec.width/2; 
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    //Scarfy's animation variables
    //animation frame
    scarfyData.frame= 0;
    //amount of time before we update the animation frame
    scarfyData.updateTime = 1.0/12.0;
    //running time
    scarfyData.runningTime = 0.0;
    
    //Variable to check where is the rectangle
    bool isInAir;

    //jump velocity(pixels/sec)
    const int jumpvel{-600};

    int velocity {0}; // pixels per frame 

    bool collision{};

    SetTargetFPS(60);

    while(!WindowShouldClose()){

        //delta time (time since last frame)
        const float dT{GetFrameTime()};

        HideCursor();//hides cursor from the gamescreen

        if (currentScreen == GAMEOVER || currentScreen == WIN) {
            if (IsKeyPressed(KEY_R)) {
                ResetGame(scarfyData, 
                          nebulae, 
                          sizeOfNebulae, 
                          windowDimensions[0], 
                          windowDimensions[1], 
                          nebula,
                          score,
                          difficultyLevel, 
                          currentNebVel, 
                          scarfySpeedMultiplier, 
                          baseNebVel,
                          bgX, 
                          mgX, 
                          fgX
                        );
                
                collision = false;
                currentScreen = GAMEPLAY;
            }
        }
         // Handle screen transitions
        switch (currentScreen) {
            case MENU: {
                if (IsKeyPressed(KEY_ONE)) {

                // Reset game state when starting new game
                ResetGame(scarfyData, 
                          nebulae, 
                          sizeOfNebulae, 
                          windowDimensions[0], 
                          windowDimensions[1], 
                          nebula,
                          score,
                          difficultyLevel, 
                          currentNebVel, 
                          scarfySpeedMultiplier, 
                          baseNebVel,
                          bgX, 
                          mgX, 
                          fgX
                        );
                
                collision = false;
                currentScreen = GAMEPLAY;             
            }  
                else if (IsKeyPressed(KEY_TWO)) {
                    currentScreen = INSTRUCTIONS;
                    score = 0;
                    difficultyLevel = 0;
                    currentNebVel = baseNebVel;
                    scarfySpeedMultiplier = 1.0f;
                } 
                else if (IsKeyPressed(KEY_THREE)) {
                currentScreen = HIGHSCORES;
                }
                else if (IsKeyPressed(KEY_FOUR)) {
                    CloseWindow();
                    return 0;
                }
                break;
        }
            case INSTRUCTIONS: {
                ShowInstructionsFromFile(windowDimensions[0], windowDimensions[1]);
                if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = MENU;
                }
                break;
            }

            case GAMEPLAY: {
                cout << "Entering GAMEPLAY state" << endl;
                for (int i = 0; i < sizeOfNebulae; i++) {
                    if (!nebulae[i].passed && scarfyData.pos.x > nebulae[i].pos.x + nebulae[i].rec.width) {
                        score++;
                        nebulae[i].passed = true;
                    
                        // Increase difficulty every 10 points
                        if (score % 10 == 0) {
                            difficultyLevel++;
                            currentNebVel = baseNebVel * (1.0f + difficultyLevel * 0.075f);
                            scarfySpeedMultiplier = 1.0f + difficultyLevel * 0.025f;
                        }
                    }
                break;
            }
        }
                
            case GAMEOVER: {
                if (IsKeyPressed(KEY_R)) {
                    currentScreen = GAMEPLAY;
                }
                break;
            }
            
            case WIN: {
                SaveHighScores(score);
                if (IsKeyPressed(KEY_R)) {
                    score = 0;
                    currentScreen = GAMEPLAY;
                    
                }
                else if (IsKeyPressed(KEY_SPACE)) {
                    score = 0;
                    currentScreen = MENU;
                }
                break;
            }

            case HIGHSCORES: {
                break;
            }
        } 

        //start drawing
        BeginDrawing();

        switch (currentScreen) {
            case MENU: {
                ClearBackground(BLACK);
                DrawTextureEx(background, {0, 0}, 0, 2.0, WHITE);
                DrawTextureEx(midground, {0, 0}, 0, 2.0, WHITE);
                DrawTextureEx(foreground, {0, 0}, 0, 2.0, WHITE);
                
                // Menu UI
                DrawRectangle(0, 0, windowDimensions[0], windowDimensions[1], Fade(BLACK, 0.5f));
                DrawText("DAPPER DASHER", windowDimensions[0]/2 - MeasureText("DAPPER DASHER", 40)/2, 50, 40, WHITE); 
                
                DrawText("1. START GAME", windowDimensions[0]/2 - 100, 150, 30, WHITE); 
                DrawText("2. INSTRUCTIONS", windowDimensions[0]/2 - 100, 190, 30, WHITE); 
                DrawText("3. HIGH SCORES", windowDimensions[0]/2 - 100, 230, 30, WHITE);
                DrawText("4. QUIT", windowDimensions[0]/2 - 100, 270, 30, WHITE);
                DrawText("Press number keys to select", windowDimensions[0]/2 - MeasureText("Press number keys to select", 20)/2, 
                        350, 20, LIGHTGRAY); 
                break;
            }

            case INSTRUCTIONS: {
                ShowInstructionsFromFile(windowDimensions[0], windowDimensions[1]);
                if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = MENU;
                }
                break;
            }
            case GAMEPLAY: {
            
                if (currentScreen == GAMEPLAY) {
                    //scroll background
                    bgX -= 20 * scarfySpeedMultiplier * dT;
                    if (bgX <= -background.width * 2) {
                        bgX = 0.0;
                    }

                    //scroll midground
                    mgX -= 40 * scarfySpeedMultiplier * dT;
                    if (mgX <= -midground.width * 2) {
                        mgX = 0.0;
                    }

                    //scroll foreground
                    fgX -= 80 * scarfySpeedMultiplier * dT;
                    if (fgX <= -foreground.width * 2) {
                        fgX = 0.0;
                    }
                }

                DrawGameBackground(bgX, mgX, fgX, background, midground, foreground);

                Vector2 bg1Pos{bgX, 0.0};
                DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
                Vector2 bg2Pos{bgX + background.width * 2, 0.0};
                DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

                Vector2 mg1Pos{mgX, 0.0};
                DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
                Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
                DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

                Vector2 fg1Pos{fgX, 0.0};
                DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
                Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
                DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

                //ground check
                if (isOnGround(scarfyData, windowDimensions[1])) {
                    velocity = 0;
                    isInAir = false;
                }
                else {
                    velocity += gravity * dT * scarfySpeedMultiplier;
                    isInAir = true;
                }

                //jump check
                if (IsKeyPressed(KEY_SPACE) && !isInAir) {
                    velocity += jumpvel / scarfySpeedMultiplier;
                    
                }

                //update nebulae position
                for (int i = 0; i < sizeOfNebulae; i++) {
                    nebulae[i].pos.x += currentNebVel * dT;
                }

                //update Y position
                scarfyData.pos.y += velocity * dT;

                if (!isInAir) {
                    scarfyData = updateAnimData(scarfyData, dT, 5);
                }

                //update nebulae's animation frames
                for (int i = 0; i < sizeOfNebulae; i++) {
                    nebulae[i] = updateAnimData(nebulae[i], dT, 7);
                }

                // Draw nebulae
                for (int i = 0; i < sizeOfNebulae; i++) {
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                }

                // Draw scarfy
                DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);

                // Check collisions
                collision = false;
                for (int i = 0; i < sizeOfNebulae; i++) {
                    const AnimData& nebula = nebulae[i];
                    float pad{50};
                    Rectangle nebRec{
                        nebula.pos.x + pad,
                        nebula.pos.y + pad,
                        nebula.rec.width - 2 * pad,
                        nebula.rec.height - 2 * pad
                    };
                    Rectangle scarfyRec{
                        scarfyData.pos.x,
                        scarfyData.pos.y,
                        scarfyData.rec.width,
                        scarfyData.rec.height
                    };
                    if (CheckCollisionRecs(nebRec, scarfyRec)) {
                        collision = true;
                        break;
                    }
                }

                // Check win condition
                if (score >= sizeOfNebulae) {  
                    currentScreen = WIN;
                }

                else if (collision) {
                    currentScreen = GAMEOVER;
                }

                // Check for score update
                for (int i = 0; i < sizeOfNebulae; i++) {
                    if (!nebulae[i].passed && scarfyData.pos.x > nebulae[i].pos.x + nebulae[i].rec.width) {
                        score++;
                        nebulae[i].passed = true;

                    // Increase difficulty every 10 points
                    if (score % 10 == 0) {
                        difficultyLevel++;
                        currentNebVel = baseNebVel * (1.0f + difficultyLevel * 0.075f);  
                        scarfySpeedMultiplier = 1.0f + difficultyLevel * 0.025f;  
                    } 
                }
            }

                // Draw score
                stringstream ss;  
                ss << "Score: " << score;  
                string scoreStr = ss.str();  

                DrawText(
                    scoreStr.c_str(),  
                    windowDimensions[0] - 100,
                    10,
                    20,
                    WHITE
                );
                break;
            
        }
            
            case GAMEOVER: {
            SaveHighScores(score);
            ShowGameOverScreen(windowDimensions[0], 
                               windowDimensions[1], 
                               bgX, 
                               mgX, 
                               fgX, 
                               background, 
                               midground, 
                               foreground,
                               score
                            );
            if (IsKeyPressed(KEY_R)) {
                ResetGame(scarfyData, 
                        nebulae, 
                        sizeOfNebulae, 
                        windowDimensions[0], 
                        windowDimensions[1], 
                        nebula,
                        score,
                        difficultyLevel, 
                        currentNebVel, 
                        scarfySpeedMultiplier, 
                        baseNebVel,
                        bgX, 
                        mgX, 
                        fgX
                    );
                collision = false;
                currentScreen = GAMEPLAY;
            }
            else if (IsKeyPressed(KEY_SPACE)) {
                    currentScreen = MENU;
                }

            break;
        }
        
        case WIN: {
            SaveHighScores(score);
            ShowWinScreen(windowDimensions[0], 
                          windowDimensions[1], 
                          bgX, 
                          mgX, 
                          fgX, 
                          background, 
                          midground, 
                          foreground,
                          score
                        );

            if (IsKeyPressed(KEY_R)) {
                ResetGame(scarfyData, 
                        nebulae, 
                        sizeOfNebulae, 
                        windowDimensions[0], 
                        windowDimensions[1], 
                        nebula,
                        score,
                        difficultyLevel, 
                        currentNebVel, 
                        scarfySpeedMultiplier, 
                        baseNebVel,
                        bgX, 
                        mgX, 
                        fgX
                    );
                collision = false;
                currentScreen = GAMEPLAY;
            }
    else if (IsKeyPressed(KEY_SPACE)) {              
        currentScreen = MENU;
    }
            
            break;
        }
        
        case HIGHSCORES: {
            int highScores[MAX_HIGH_SCORES];
            LoadHighScores(highScores);
            
            ClearBackground(BLACK);
            DrawRectangle(0, 0, windowDimensions[0], windowDimensions[1], Fade(BLACK, 0.7f));
            DrawText("HIGH SCORES", windowDimensions[0]/2 - MeasureText("HIGH SCORES", 40)/2, 50, 40, WHITE);
            
            int yPos = 120;
            
            for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                stringstream ss; // Create a string stream
                ss << (i+1) << ". "; // Format position number
                
                if (highScores[i] > 0) {
                    ss << highScores[i]; // Add actual score
                } else {
                    ss << "--"; // Add placeholder
                }
                
                string scoreStr = ss.str(); // Convert to string
                DrawText(
                    scoreStr.c_str(),
                    windowDimensions[0]/2 - MeasureText(scoreStr.c_str(), 30)/2,
                    yPos,
                    30,
                    WHITE
                );
                yPos += 40;
            }
            
            DrawText("Press SPACE to return", 
                     windowDimensions[0]/2 - MeasureText("Press SPACE to return", 20)/2, 
                     windowDimensions[1] - 50, 
                     20,
                     LIGHTGRAY
                    );
            
            if (IsKeyPressed(KEY_SPACE)) {
                currentScreen = MENU;
            }
            break;
        }
    }
        //stop drawing
        EndDrawing();
    }
    delete[] nebulae;

    UnloadTexture(nebula);
    UnloadTexture(scarfy);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow( );
 
    return 0;
}