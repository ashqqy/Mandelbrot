#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui.h"

//--------------------------------------------------------------------

void WindowParamsInit (SDL_Window* window, window_params_t* window_params)
{
    assert (window        != NULL);
    assert (window_params != NULL);

    window_params->window  = window;
    window_params->surface = SDL_GetWindowSurface (window);
    window_params->width   = START_WINDOW_WIDTH;
    window_params->height  = START_WINDOW_HEIGHT;
    window_params->aspect_ratio = (double) window_params->width / window_params->height;
}

void FpsParamsInit (fps_params_t* fps_params)
{
    assert (fps_params != NULL);

    fps_params->current_time = 0;
    fps_params->last_time    = SDL_GetTicks ();
    fps_params->frame_count  = 0.0;
    snprintf (fps_params->title, TITLE_SIZE, "FPS: %.1f", fps_params->frame_count);
}

//--------------------------------------------------------------------

int EventsHandle (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);

    SDL_Event event;

    while (SDL_PollEvent (&event) != 0) 
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
            {
                return 1;
            }

            case SDL_EVENT_WINDOW_RESIZED:
            {
                WindowResizeEventHandle (window_params, image_data);
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                KeyboardEventHandle (image_data, event);
                break;
            }

            default:
            {
                break;
            }
        }
    }

    return 0;
}

void WindowResizeEventHandle (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (window_params != NULL);

    SDL_GetWindowSize (window_params->window, &window_params->width, &window_params->height);
    SDL_SetWindowSize (window_params->window, window_params->width, window_params->height);

    window_params->surface = SDL_GetWindowSurface (window_params->window);

    image_data->pixels = (uint32_t*) Recalloc (image_data->pixels, 
        (size_t) window_params->width * (size_t) window_params->height, sizeof (uint32_t));
        
    if (image_data->pixels == NULL)
    {
        printf ("Pixels array realloc error\n");
        EXIT_FAILURE;
    }

    image_data->pixels_array_size = window_params->width * window_params->height + 3;
    window_params->aspect_ratio = (double) window_params->width / window_params->height;
    image_data->height_half = window_params->height / 2.0;
    image_data->width_half  = window_params->width  / 2.0;
}

void KeyboardEventHandle (image_data_t* image_data, SDL_Event event)
{
    assert (image_data != NULL);

    switch (event.key.scancode)
    {
        case SDL_SCANCODE_H:
        {
            image_data->x_shift -= 0.2 / image_data->zoom;
            break;
        }

        case SDL_SCANCODE_J:
        {
            image_data->y_shift += 0.2 / image_data->zoom;
            break;
        }

        case SDL_SCANCODE_K:
        {
            image_data->y_shift -= 0.2 / image_data->zoom;
            break;
        }

        case SDL_SCANCODE_L:
        {
            image_data->x_shift += 0.2 / image_data->zoom;
            break;
        }

        case SDL_SCANCODE_Z:
        {
            image_data->zoom *= 1.1;
            break;
        }

        case SDL_SCANCODE_X:
        {
            image_data->zoom /= 1.1;
            break;
        }

        default:
        {
            break;
        }
    }
}

//--------------------------------------------------------------------

void FpsUpdate (SDL_Window* window, fps_params_t* fps_params)
{
    assert (window     != NULL);
    assert (fps_params != NULL);

    fps_params->frame_count += 1;
    fps_params->current_time = SDL_GetTicks ();
    
    if (fps_params->current_time - fps_params->last_time >= MS_IN_SECOND)
    {
        float FPS = (fps_params->frame_count * MS_IN_SECOND) / 
            (float) (fps_params->current_time - fps_params->last_time);

        snprintf (fps_params->title, TITLE_SIZE, "FPS: %.1f", FPS);

        fps_params->last_time = fps_params->current_time;
        fps_params->frame_count = 0;

        SDL_SetWindowTitle (window, fps_params->title);
    }
}

//--------------------------------------------------------------------

void* Recalloc (void* memory, size_t n_elements, size_t size_elements)
{
    void* save_memory = memory;
    memory = realloc (memory, n_elements * size_elements);
    if (memory == NULL)
    {
        free (save_memory); save_memory = NULL;
        return NULL;
    }

    return memory;
}

//--------------------------------------------------------------------
