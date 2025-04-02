#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

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
    window_params->x_shift = -0.5;
    window_params->y_shift = 0.0;
    window_params->zoom    = 1.0;
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
                KeyboardEventHandle (window_params, event);
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
}

void KeyboardEventHandle (window_params_t* window_params, SDL_Event event)
{
    assert (window_params != NULL);

    switch (event.key.scancode)
    {
        case SDL_SCANCODE_H:
        {
            window_params->x_shift -= 0.2 / window_params->zoom;
            break;
        }

        case SDL_SCANCODE_J:
        {
            window_params->y_shift += 0.2 / window_params->zoom;
            break;
        }

        case SDL_SCANCODE_K:
        {
            window_params->y_shift -= 0.2 / window_params->zoom;
            break;
        }

        case SDL_SCANCODE_L:
        {
            window_params->x_shift += 0.2 / window_params->zoom;
            break;
        }

        case SDL_SCANCODE_Z:
        {
            window_params->zoom *= 1.1;
            break;
        }

        case SDL_SCANCODE_X:
        {
            window_params->zoom /= 1.1;
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
