#ifndef UI_H
#define UI_H

//--------------------------------------------------------------------

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

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
    double aspect_ratio;
} window_params_t;

typedef struct
{
    uint32_t* pixels;
    uint32_t* colors;
    int pixels_array_size;
    double width_half;
    double height_half;
    double x_shift;
    double y_shift;
    double zoom;
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

bool EventsHandle (window_params_t* window_params, image_data_t* image_data);
void KeyboardEventHandle (image_data_t* image_data, SDL_Event event);
void WindowResizeEventHandle (window_params_t* window_params, image_data_t* image_data);

void FpsUpdate (SDL_Window* window, fps_params_t* fps_params);

void* Recalloc (void* memory, size_t n_elements, size_t size_elements);

//--------------------------------------------------------------------

#endif // UI_H
