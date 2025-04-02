#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <SDL3/SDL.h>

static const int START_WINDOW_WIDTH  = 2560;
static const int START_WINDOW_HEIGHT = 1440;

const int TITLE_SIZE = 10;
const int MS_IN_SECOND = 1000;

const uint32_t ALPHA = 255;

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

typedef struct
{
    uint64_t current_time;
    uint64_t last_time;
    float frame_count;
    char title[TITLE_SIZE];
} FPS_params_t;


void WindowParamsInit (SDL_Window* window, window_params_t* window_params);
void FPSParamsInit (FPS_params_t* FPS_params);

void HandleKeyboardEvent (window_params_t* window_params);
int HandleEvents (window_params_t* window_params);

void DrawMandelbrot (window_params_t* window_params);
uint32_t GetColor (uint32_t iterations);

void UpdateFPS (SDL_Window* window, FPS_params_t* FPS_params);

#endif // MANDELBROT_H
