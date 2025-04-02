#include <render.h>
#include <ui.h>

#define NO_DEBUG

//--------------------------------------------------------------------

int main () 
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow ("Mandelbrot", START_WINDOW_WIDTH, START_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition (window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    window_params_t window_params = {};
    WindowParamsInit (window, &window_params);

    fps_params_t fps_params = {};
    FpsParamsInit (&fps_params);

    image_data_t image_data = {};
    ImageDataInit (&window_params, &image_data);

    int quit = 0;

    while (!quit) 
    {
        quit = EventsHandle (&window_params, &image_data);
        SetCalculate (&window_params, &image_data);
        ImageUpdate (&window_params, &image_data);
        FpsUpdate (window_params.window, &fps_params);
    }

    ImageDataDestroy (&image_data);
    SDL_DestroyWindow (window);
    SDL_Quit ();
}

//--------------------------------------------------------------------
