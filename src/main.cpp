#include <iostream>
#include <SDL2/SDL.h>
#include <windows.h>

#define SCREENWIDTH 600
#define SCREENHEIGHT 600
#define BLOCKWIDTH 10
#define BLOCKHEIGHT 10
#define BLOCKSONSCREENWIDTH SCREENWIDTH/BLOCKWIDTH 
#define BLOCKSONSCREENHEIGHT SCREENHEIGHT/BLOCKHEIGHT

bool board [BLOCKSONSCREENWIDTH][BLOCKSONSCREENHEIGHT] = {0};
bool boardcopy [BLOCKSONSCREENWIDTH][BLOCKSONSCREENHEIGHT] = {0};

void moveCopyToBoard () {
    for (int i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        for (int j = 0; j < BLOCKSONSCREENHEIGHT; j++)
        {
            board[i][j] = boardcopy[i][j];
        }
    }
}

void moveBoardToCopy () {
    for (int i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        for (int j = 0; j < BLOCKSONSCREENHEIGHT; j++)
        {
            boardcopy[i][j] = board[i][j];
        }
    }
}

/*
    1.    <2 neighbors = die
    2     2 or 3 neighbors = stay alive
    3.    >3 neighbors = die
    4.    3 neighbors = awaken forth! To life!
*/
void enactGameRules () {
    for (int i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        for (int j = 0; j < BLOCKSONSCREENHEIGHT; j++)
        {
            // I really hope bools count as 1's or 0's when ints.
            int sum = 0;
            if (i > 0) {
                if (j < BLOCKSONSCREENHEIGHT) {sum += board[i-1][j+1];}
                if (j > 0) {sum += board[i-1][j-1];}
                sum += board[i-1][j];
            }
            
            if (j < BLOCKSONSCREENHEIGHT) {sum += board[i][j+1];}
            if (j > 0) {sum += board[i][j-1];}

            if (i < BLOCKSONSCREENWIDTH) {
                if (j < BLOCKSONSCREENHEIGHT) {sum += board[i+1][j+1];}
                if (j > 0) {sum += board[i+1][j-1];}
                sum += board[i+1][j];
            }


            if (sum < 2 || sum > 3) {
                boardcopy[i][j] = 0;
            }

            if (sum == 3) {
                boardcopy[i][j] = 1;
            }
        }
    }
}

void drawBoard (SDL_Renderer *&renderer) {
    for (int i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        for (int j = 0; j < BLOCKSONSCREENHEIGHT; j++)
        {
            SDL_Rect rectToFill; rectToFill.h = BLOCKHEIGHT; rectToFill.w = BLOCKWIDTH;
            rectToFill.x = i*BLOCKWIDTH; rectToFill.y = j*BLOCKHEIGHT;  

            SDL_Color drawColor;
            if (board[i][j]) {drawColor = {255,255,255,255};}
            else {drawColor = {0,0,0,255};}

            SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
            SDL_RenderFillRect(renderer, &rectToFill);
        }
    }
    
}

void drawGrid (SDL_Renderer *renderer, SDL_Color color) {

    enum Coords {
        X, Y
    };

    int p1 [2] = {0,0};
    int p2 [2] = {0,0};

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // ------ draw rows -----------
    p1[X] = 0;
    p2[X] = SCREENWIDTH;

    for (size_t i = 0; i < BLOCKSONSCREENHEIGHT; i++)
    {
        p1[Y] = p2[Y] = i*BLOCKHEIGHT;

        SDL_RenderDrawLine(renderer, p1[X], p1[Y], p2[X], p2[Y]);
    }
    

    // ------ draw cols -----------
    p1[Y] = 0;
    p2[Y] = SCREENHEIGHT;
    
    for (size_t i = 0; i < BLOCKSONSCREENWIDTH; i++)
    {
        p1[X] = p2[X] = i*BLOCKWIDTH;

        SDL_RenderDrawLine(renderer, p1[X], p1[Y], p2[X], p2[Y]);
    }
}

int main (int argc, char* argv[]) {
    
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(SCREENWIDTH, SCREENHEIGHT, 0, &window, &renderer);   

    if (window == NULL) {
        std::cout << "Could not create window: " << SDL_GetError() << std::endl;
    }

    bool running = true, lClick = false, rClick = false, pause = true;
    int mouseX(0), mouseY(0);
    SDL_Event windowEvent;
    SDL_KeyboardEvent keyEvent;

    // a ptr to a const Uint8, has an array of all the states that now hold
    const Uint8 *state;
    int clickState;
    float tickTime = 0.2;

    while (running) {

        if (SDL_PollEvent(&windowEvent)) {
            
            switch (windowEvent.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEMOTION:
                    SDL_GetMouseState(&mouseX, &mouseY);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    clickState = SDL_GetMouseState(&mouseX, &mouseY);
                    lClick = (clickState == 1);
                    rClick = (clickState == 4);
                    break;
                case SDL_MOUSEBUTTONUP:
                    lClick = rClick = false;
                    break;
            }
        }

        state = SDL_GetKeyboardState(nullptr);

        // pause is true if either space or p are pressed.
        // if pause was already pressed then stop
        // if it wasn't then start. This is represented by XOR
        pause ^= state[SDL_SCANCODE_SPACE] || state[SDL_SCANCODE_P];
        
        
        bool flag = false;
        // draw big squares

        if (pause && (lClick || rClick)) {

            int blockXIndex = ceil((abs(mouseX)+1)/BLOCKWIDTH);
            int blockYIndex = ceil((abs(mouseY)+1)/BLOCKHEIGHT);
            
            if (lClick) {flag = true;}
            if (rClick) {flag = false;}

            board[blockXIndex][blockYIndex] = flag;
        }

        if (!pause) {
            // we only need the copy if the game is running
            moveBoardToCopy();
            // tickTime is in seconds, and Sleep gets MS
            Sleep(tickTime*1000);
            // std::cout << "rules are rules" << std::endl;
            enactGameRules();
            moveCopyToBoard();
        }
        
        drawBoard(renderer);
        // draw grid on screen
        SDL_Color gridColor{70,70,40,255};
        drawGrid(renderer, gridColor);
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}