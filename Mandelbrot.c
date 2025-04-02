#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "Mandelbrot.h"

void WindowParamsInit (SDL_Window* window, window_params_t* window_params)
{
    assert (window != NULL);
    assert (window_params != NULL);

    window_params->window  = window;
    window_params->surface = SDL_GetWindowSurface (window);
    window_params->width   = START_WINDOW_WIDTH;
    window_params->height  = START_WINDOW_HEIGHT;
    window_params->x_shift = -0.5;
    window_params->y_shift = 0.0;
    window_params->zoom    = 1.0;
}

void FPSParamsInit (FPS_params_t* FPS_params)
{
    assert (FPS_params != NULL);

    FPS_params->current_time = 0;
    FPS_params->last_time    = SDL_GetTicks ();
    FPS_params->frame_count  = 0.0;
    snprintf (FPS_params->title, TITLE_SIZE, "FPS: %.1f", FPS_params->frame_count);
}

void HandleKeyboardEvent (window_params_t* window_params)
{
    assert (window_params != NULL);

    switch (window_params->event.key.scancode)
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

        default:
        {
            break;
        }
    }
}

int HandleEvents (window_params_t* window_params)
{
    assert (window_params != NULL);

    while (SDL_PollEvent (&window_params->event) != 0) 
    {
        switch (window_params->event.type)
        {
            case SDL_EVENT_QUIT:
            {
                return 1;
            }

            case SDL_EVENT_WINDOW_RESIZED:
            {
                SDL_GetWindowSize (window_params->window, &window_params->width, &window_params->height);
                SDL_SetWindowSize (window_params->window, window_params->width, window_params->height);
                window_params->surface = SDL_GetWindowSurface (window_params->window);
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                HandleKeyboardEvent (window_params);
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

void DrawMandelbrot (window_params_t* window_params)
{
    assert (window_params != NULL);

    uint32_t* pixels = (uint32_t*) calloc ((size_t) (window_params->width * window_params->height), sizeof (uint32_t));

    // Nested *pixel* loops
    for (int y_pixel = 0; y_pixel < window_params->height; ++y_pixel)
    {
        for (int x_pixel = 0; x_pixel < window_params->width; ++x_pixel)
        {
            double aspect_ratio = (double) window_params->width / window_params->height;

            // Transform screen coords to Mandelbrot set coords
            double y_0 = (y_pixel - window_params->height / 2.0) * (4.0 / window_params->height)               // y \in (-2, 2)
                        / window_params->zoom + window_params->y_shift;
                
            double x_0 = (x_pixel - window_params->width  / 2.0) * (4.0 * aspect_ratio / window_params->width) // x \in (-4, 3)
                        / window_params->zoom + window_params->x_shift;

            double x_Re = 0;   // current Re(z)
            double y_Im = 0;   // current Im(z)

            double x2_Re = 0;  // Re(z)^2
            double y2_Im = 0;  // Im(z)^2

            int iterations = 0;

            // (x2_Re + y2_Im <= 4) equal (|z|^2 <= 4) equal (|z| <= 2)
            while ((x2_Re + y2_Im <= 4) && (iterations < 256))
            {
                // z_n = z_{n-1}^2 + z_0
                y_Im = 2 * x_Re * y_Im + y_0;
                x_Re = x2_Re - y2_Im + x_0;

                x2_Re = x_Re * x_Re;
                y2_Im = y_Im * y_Im;

                iterations++;
            }

            pixels[y_pixel * window_params->width + x_pixel] = GetColor ((uint32_t) iterations);
        }
    }

    SDL_LockSurface(window_params->surface);
    memcpy(window_params->surface->pixels, pixels, (size_t) (window_params->width * window_params->height) * sizeof (uint32_t));
    SDL_UnlockSurface(window_params->surface);

    SDL_UpdateWindowSurface(window_params->window);

    free (pixels); pixels = NULL;
}

uint32_t GetColor (uint32_t iterations)
{
    uint32_t red   = ((iterations * iterations - 1) % 256);
    uint32_t green = (red + iterations) % 256;
    uint32_t blue  = (red + green + iterations) % 256;

    return (ALPHA << 24) | (red << 16) | (green << 8) | (blue);
}

void UpdateFPS (SDL_Window* window, FPS_params_t* FPS_params)
{
    assert (window != NULL);
    assert (FPS_params != NULL);

    FPS_params->frame_count += 1;
    FPS_params->current_time = SDL_GetTicks ();
    
    if (FPS_params->current_time - FPS_params->last_time >= MS_IN_SECOND)
    {
        float FPS = (FPS_params->frame_count * MS_IN_SECOND) / (float) (FPS_params->current_time - FPS_params->last_time);
        snprintf (FPS_params->title, TITLE_SIZE, "FPS: %.1f", FPS);

        FPS_params->last_time = FPS_params->current_time;
        FPS_params->frame_count = 0;

        SDL_SetWindowTitle (window, FPS_params->title);
    }
}
