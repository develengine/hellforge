#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>

class Network
{

};

struct GeneticSequence
{
    std::vector<uint32_t> ids;
    std::vector<uint32_t> inputs;
    std::vector<uint32_t> outputs;
    std::vector<float> weights;
};

const int WINDOW_WIDTH = 1080;
const int WINDOW_HEIGHT = 720;
const char *FONT_PATH = "res/orbitron/Orbitron-Regular.ttf";
const char *WINDOW_TITLE = "Unsupervised program synthesis";
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *font = nullptr;

void init();
void cleanUp();

int main(int argc, char *argv[])
{
    init();

    bool running = true;
    while (running) {
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
    }

    cleanUp();
    return 0;
}

void init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Error: " << SDL_GetError() << '\n';
        exit(-1);
    }
 
    if (TTF_Init() == -1) {
        std::cerr << "Error: " << SDL_GetError() << '\n';
        exit(-1);
    }

    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if (window == nullptr) {
        std::cerr << "Error: " << SDL_GetError() << '\n';
        exit(-1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::cerr << "Error: " << SDL_GetError() << '\n';
        exit(-1);
    }

    font = TTF_OpenFont(FONT_PATH, 128);

    if (font == nullptr) {
        std::cerr << "Error: " << SDL_GetError() << '\n';
        exit(-1);
    }
}

void cleanUp()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
