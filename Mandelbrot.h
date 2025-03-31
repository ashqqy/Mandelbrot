#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <SDL3/SDL.h>

static const int START_WINDOW_WIDTH  = 2560;
static const int START_WINDOW_HEIGHT = 1440;

typedef struct
{
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Event event;
    int width;
    int height;
    double x_shift;
    double y_shift;
    double zoom;
} window_params_t;

void WindowParamsInit (SDL_Window* window, window_params_t* window_params);

void HandleKeyboardEvent (window_params_t* window_params);
int HandleEvents (window_params_t* window_params);

void DrawMandelbrot (window_params_t* window_params);

#endif // MANDELBROT_H
