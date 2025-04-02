// WARNING! File name has a left cyclic shift!

#ifndef UI_H
#define UI_H

//--------------------------------------------------------------------

#include <SDL3/SDL.h>

//--------------------------------------------------------------------

static const int START_WINDOW_WIDTH  = 2560;
static const int START_WINDOW_HEIGHT = 1440;

#define TITLE_SIZE 10

static const int MS_IN_SECOND = 1000;

//--------------------------------------------------------------------

typedef struct
{
    SDL_Window* window;
    SDL_Surface* surface;
    int width;
    int height;
    double x_shift;
    double y_shift;
    double zoom;
} window_params_t;

typedef struct
{
    uint32_t* pixels;
    uint32_t* colors;
} image_data_t;

typedef struct
{
    uint64_t current_time;
    uint64_t last_time;
    float frame_count;
    char title[TITLE_SIZE];
} fps_params_t;

//--------------------------------------------------------------------

void WindowParamsInit (SDL_Window* window, window_params_t* window_params);
void FpsParamsInit (fps_params_t* fps_params);

int EventsHandle (window_params_t* window_params, image_data_t* image_data);
void KeyboardEventHandle (window_params_t* window_params, SDL_Event event);
void WindowResizeEventHandle (window_params_t* window_params, image_data_t* image_data);

void FpsUpdate (SDL_Window* window, fps_params_t* fps_params);

void* Recalloc (void* memory, size_t n_elements, size_t size_elements);

//--------------------------------------------------------------------

#endif // UI_H
