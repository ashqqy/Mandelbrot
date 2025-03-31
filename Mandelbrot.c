#include <stdlib.h>

#include "Mandelbrot.h"

void WindowParamsInit (SDL_Window* window, window_params_t* window_params)
{
    window_params->window  = window;
    window_params->surface = SDL_GetWindowSurface (window);
    window_params->width   = START_WINDOW_WIDTH;
    window_params->height  = START_WINDOW_HEIGHT;
    window_params->x_shift = -0.5;
    window_params->y_shift = 0.0;
    window_params->zoom    = 1.0;
}

void HandleKeyboardEvent (window_params_t* window_params)
{
    switch (window_params->event.key.scancode)
    {
        case SDL_SCANCODE_H:
        {
            window_params->x_shift -= 0.2;
            break;
        }

        case SDL_SCANCODE_J:
        {
            window_params->y_shift += 0.2;
            break;
        }

        case SDL_SCANCODE_K:
        {
            window_params->y_shift -= 0.2;
            break;
        }

        case SDL_SCANCODE_L:
        {
            window_params->x_shift += 0.2;
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
    uint32_t* pixels = (uint32_t*) calloc ((size_t) (window_params->width * window_params->height), sizeof (uint32_t));

    // Nested *pixel* loops
    for (int y_pixel = 0; y_pixel < window_params->height; ++y_pixel)
    {
        for (int x_pixel = 0; x_pixel < window_params->width; ++x_pixel)
        {
            double aspect_ratio = (double) window_params->width / window_params->height;

            // Transform screen coords to Mandelbrot set coords
            double y_0 = ((y_pixel - window_params->height / 2.0) * 
                (4.0 / window_params->height) + window_params->y_shift) / window_params->zoom;               // y \in (-2, 2)
                
            double x_0 = ((x_pixel - window_params->width  / 2.0) * 
                (4.0 * aspect_ratio / window_params->width) + window_params->x_shift) / window_params->zoom; // x \in (-4, 3)

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

            uint32_t color;
            if (iterations == 256)
                color = 0xFFFFFFFF;
            else 
                color = 0;

            pixels[y_pixel * window_params->width + x_pixel] = color;
        }
    }

    SDL_LockSurface(window_params->surface);
    memcpy(window_params->surface->pixels, pixels, (size_t) (window_params->width * window_params->height) * sizeof (uint32_t));
    SDL_UnlockSurface(window_params->surface);

    SDL_UpdateWindowSurface(window_params->window);

    free (pixels); pixels = NULL;
}
