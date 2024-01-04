#include <iostream>
#include <SDL2/SDL.h>

#define SCREENWIDTH 600
#define SCREENHEIGHT 600
#define BLOCKWIDTH 10
#define BLOCKHEIGHT 10
#define BLOCKSONSCREENWIDTH SCREENWIDTH/BLOCKWIDTH 
#define BLOCKSONSCREENHEIGHT SCREENHEIGHT/BLOCKHEIGHT

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

    bool running = true, lClick = false, rClick = false, pause = false;
    int mouseX(0), mouseY(0);
    SDL_Event windowEvent;
    SDL_KeyboardEvent keyEvent;

    // a ptr to a const Uint8, has an array of all the states that now hold
    const Uint8 *state;
    int clickState;
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
        pause = state[SDL_SCANCODE_SPACE] || state[SDL_SCANCODE_P];
        
        

        // draw grid on screen
        SDL_Color gridColor{70,70,40,255};
        drawGrid(renderer, gridColor);

        // draw big squares

        if (lClick || rClick) {

            int blockXIndex = ceil((abs(mouseX)+1)/BLOCKWIDTH);
            int blockYIndex = ceil((abs(mouseY)+1)/BLOCKHEIGHT);
            SDL_Rect rectToFill; rectToFill.h = BLOCKHEIGHT; rectToFill.w = BLOCKWIDTH;
            rectToFill.x = blockXIndex*BLOCKWIDTH; rectToFill.y = blockYIndex*BLOCKHEIGHT;  

            SDL_Color drawColor;
            if (lClick) {drawColor = {255,255,255,255};}
            if (rClick) {drawColor = {0,0,0,255};}
            std::cout << drawColor.r <<  drawColor.g << drawColor.b << drawColor.a << std::endl;
            SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

            SDL_RenderFillRect(renderer, &rectToFill);
        }

        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}