#include <raylib.h>
#include <iostream>

using namespace std;

// Enum to manage different pages or game states
typedef enum {
    LOGO,
    WELCOME,
    GAME,
    TIME_OVER,  // Updated game state for time over
    LOSS,
    WIN
} Page;

// Define colors used in the game
Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

// Timer variables
float startTime = 0.0f;
float elapsedTime = 0.0f;
float gameDuration = 15.0f;  // Game duration set to 60 seconds
bool isRunning = true;

// Score variables
int player_score = 0;
int cpu_score = 0;

// Button class to handle button properties and actions
class Button {
public:
    Texture2D texture;
    Vector2 position;
    float scale;

    Button(const char *imagePath, Vector2 pos, float s) {
        texture = LoadTexture(imagePath);
        position = pos;
        scale = s;
    }

    ~Button() {
        UnloadTexture(texture);
    }

    // Draw the button
    void Draw() {
        DrawTextureEx(texture, position, 0.0f, scale, MAROON);
    }

    void draw() {
        DrawTextureEx(texture, position, 0.0f, scale, WHITE);
    }

    // Check if the button is pressed
    bool isPressed(Vector2 mousePosition, bool mousePressed) {
        Rectangle buttonRect = { position.x, position.y, texture.width * scale, texture.height * scale };
        return CheckCollisionPointRec(mousePosition, buttonRect) && mousePressed;
    }
};

// Ball class to handle the ball's properties and movement
class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    // Draw the ball
    void Draw() {
        DrawCircle(x, y, radius, WHITE);
    }

    // Update the ball's position and check for screen edge collisions
    void Update() {
        x += speed_x;
        y += speed_y;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) {
            speed_y *= -1;
        }
        if (x + radius >= GetScreenWidth()) { // CPU wins
            cpu_score++;
            ResetBall();
        }
        if (x - radius <= 0) { // Player wins
            player_score++;
            ResetBall();
        }
    }

    // Reset the ball to the center with a random direction
    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x = 7 * speed_choices[GetRandomValue(0, 1)];
        speed_y = 7 * speed_choices[GetRandomValue(0, 1)];
    }
};

// Paddle class to handle player movement
class Paddle {
protected:
    // Limit the paddle's movement to within the screen
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    // Draw the paddle
    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, BLACK);
    }

    // Update paddle movement based on user input
    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y -= speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y += speed;
        }
        LimitMovement();
    }
};

// CpuPaddle class to handle CPU movement
class CpuPaddle : public Paddle {
public:
    // Update CPU movement based on ball position
    void Update(int ball_y) {
        if (y + height / 2 > ball_y) {
            y -= speed;
        }
        if (y + height / 2 <= ball_y) {
            y += speed;
        }
        LimitMovement();
    }
};

// Initialize game objects
Ball ball;
Paddle player;
CpuPaddle cpu;

int main() {
    cout << "Starting game!" << endl;

    const int screen_width = 1200;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "My Pong Game!");
    SetTargetFPS(60);

    // Enable window resizing
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    // Load textures
    Texture2D logoTexture = LoadTexture("picture/logo.png"); // Assume you have a logo image
    Texture2D background = LoadTexture("picture/2.png");

    // Initialize buttons with image paths, positions, and scales
    Button startButton("picture/play_game.png", {520, 500}, 0.2f);
    Button exitButton("picture/QUIT.png", {880, 280}, 0.2f);
    Button replayButton("picture/replay.png" , {100 , 300} , 0.2f);
    Button over("picture/over.png" , {380, 180} , 0.2f);
    Button congratulation ("picture/smiley.png" , {400, 250} , 0.5f);

    // Initialize ball properties
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    // Initialize player paddle properties
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 10;

    // Initialize CPU paddle properties
    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    Page Current_Page = LOGO; // Start with the logo page

    // Start timer at the beginning
    startTime = GetTime();

    Music music;
    InitAudioDevice();
    music = LoadMusicStream("music/song1.mp3");
    PlayMusicStream(music);

    // Main game loop
    while (!WindowShouldClose()) {

        UpdateMusicStream(music);

        Vector2 mousePosition = GetMousePosition();
        bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        // Update elapsed time
        if (isRunning) {
            elapsedTime = GetTime() - startTime;
        }

        // Handle different game pages
        switch (Current_Page) {
            case LOGO:
                BeginDrawing();
                ClearBackground(WHITE);
                
                // Draw the logo texture centered
                DrawTexture(logoTexture, screen_width / 2 - logoTexture.width / 2, screen_height / 2 - logoTexture.height / 2, WHITE);
                
                // Switch to welcome page after 3 seconds
                if (elapsedTime >= 3.0f) {
                    Current_Page = WELCOME;
                    startTime = GetTime(); // Reset timer for the next page
                }
                EndDrawing();
                break;

            case WELCOME:
                BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(background, screen_width / 2 - background.width / 2, screen_height / 2 - background.height / 2, RED);

                // Draw buttons
                startButton.Draw();

                // Check for button clicks
                if (startButton.isPressed(mousePosition, mousePressed)) {
                    Current_Page = GAME;
                    startTime = GetTime(); // Start the game timer
                }
                

                EndDrawing();
                break;

            case GAME:
                BeginDrawing();
                ball.Update();
                player.Update();
                cpu.Update(ball.y);

                // Check collisions between the ball and paddles
                if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})) {
                    ball.speed_x *= -1;
                }
                if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height})) {
                    ball.speed_x *= -1;
                }

                // Draw game elements
                ClearBackground(RED);
                DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, MAROON);
                DrawCircle(screen_width / 2, screen_height / 2, 150, { 139, 0, 0, 255 });
                DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, { 139, 0, 0, 255 });
                ball.Draw();
                cpu.Draw();
                player.Draw();
                DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, { 100, 0, 0, 255 });
                DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, { 100, 0, 0, 255 });

                // Draw remaining time
                DrawText(TextFormat("Time: %.0f", gameDuration - elapsedTime), screen_width / 2 - 100, 20, 60, { 100, 0, 0, 255 });

                EndDrawing();

                // Check for game over or win conditions
                if (elapsedTime >= gameDuration) {
                    if (cpu_score < player_score)
                      Current_Page = WIN;
                    if (cpu_score >= player_score)
                      Current_Page = LOSS;
                    if (cpu_score == player_score)
                      Current_Page = TIME_OVER;
                }
                break;

            case TIME_OVER:
                BeginDrawing();
                ClearBackground(BLACK);
                exitButton.Draw();
                replayButton.Draw();
                DrawRectangleRounded(Rectangle{10, 10, 220, 80}, 0.3, 0, { 230, 41, 55, 40 });
                DrawText(TextFormat("SCORE_CPU : %d", cpu_score), 20, 20, 20, MAROON);
                DrawText(TextFormat("SCORE_PLAYER : %d", player_score), 20, 50, 20, MAROON);
                DrawText("COMPUTER_AI", 900, 700, 30, MAROON);
                DrawText("TIME IS FINISH ...!\n\nGAME TIED\n\nDO YOU WANT TO REPLAY?", 460, 50, 30, MAROON);
                if (exitButton.isPressed(mousePosition, mousePressed)) {
                    CloseWindow();
                    return 0;
                }
                EndDrawing();

                // Reset game and return to game page on replay button press
                if (replayButton.isPressed(mousePosition, mousePressed)) {
                    Current_Page = GAME;
                    player_score = 0;
                    cpu_score = 0;
                    ball.ResetBall();
                    startTime = GetTime(); // Reset timer
                }
                break;

            case LOSS:
                BeginDrawing();
                ClearBackground(BLACK);
                exitButton.Draw();
                replayButton.Draw();
                over.draw();
                DrawRectangleRounded(Rectangle{10, 10, 220, 80}, 0.3, 0, { 230, 41, 55, 40 });
                DrawText(TextFormat("SCORE_CPU : %d", cpu_score), 20, 20, 20, MAROON);
                DrawText(TextFormat("SCORE_PLAYER : %d", player_score), 20, 50, 20, MAROON);
                DrawText("COMPUTER_AI", 900, 700, 30, MAROON);
                DrawText("TIME IS FINISH ...!\n\n\nGAME OVER", 460, 30, 50, MAROON);
                if (exitButton.isPressed(mousePosition, mousePressed)) {
                    CloseWindow();
                    return 0;
                }
                EndDrawing();

                // Reset game and return to game page on replay button press
                if (replayButton.isPressed(mousePosition, mousePressed)) {
                    Current_Page = GAME;
                    player_score = 0;
                    cpu_score = 0;
                    ball.ResetBall();
                    startTime = GetTime(); // Reset timer
                }
                break;

            case WIN:
                BeginDrawing();
                ClearBackground(BLACK);
                exitButton.Draw();
                replayButton.Draw();
                congratulation.draw();
                DrawRectangleRounded(Rectangle{10, 10, 220, 80}, 0.3, 0, { 230, 41, 55, 40 });
                DrawText(TextFormat("SCORE_CPU : %d", cpu_score), 20, 20, 20, MAROON);
                DrawText(TextFormat("SCORE_PLAYER : %d", player_score), 20, 50, 20, MAROON);
                DrawText("COMPUTER_AI", 900, 700, 30, MAROON);
                DrawText("TIME IS FINISH ...!\n\n\nCONGRATULATIONS", 400, 50, 50, GREEN);
                if (exitButton.isPressed(mousePosition, mousePressed)) {
                    CloseWindow();
                    return 0;
                }
                EndDrawing();

                // Reset game and return to game page on replay button press
                if (replayButton.isPressed(mousePosition, mousePressed)) {
                    Current_Page = GAME;
                    player_score = 0;
                    cpu_score = 0;
                    ball.ResetBall();
                    startTime = GetTime(); // Reset timer
                }
                break;
        }
    }

    // Unload resources and close the window
    UnloadMusicStream(music);
    CloseAudioDevice();

    UnloadTexture(logoTexture);
    UnloadTexture(background);
    CloseWindow();

    return 0;
}
