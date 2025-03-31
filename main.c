#include <Mandelbrot.h>

int main () 
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow ("Mandelbrot", START_WINDOW_WIDTH, START_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition (window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    window_params_t window_params = {};
    WindowParamsInit (window, &window_params);

    int quit = 0;

    while (!quit) 
    {
        quit = HandleEvents (&window_params);
        DrawMandelbrot (&window_params);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}
