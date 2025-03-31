#include <SDL3/SDL.h>

#include <stdio.h>
#include <stdlib.h>

void DrawMandelbrot (SDL_Window* window, SDL_Surface* window_surface, int window_width, int window_height);

const int START_WINDOW_WIDTH  = 2560;
const int START_WINDOW_HEIGHT = 1440;

const double SHIFT = 0.1f;
const double ZOOM  = 1.1f;

int main () 
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow ("Mandelbrot", START_WINDOW_WIDTH, START_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowPosition (window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_Surface* window_surface = SDL_GetWindowSurface (window);
    SDL_Event event;

    int window_width  = START_WINDOW_WIDTH;
    int window_height = START_WINDOW_HEIGHT;

    int quit = 0;

    while (!quit) 
    {
        while (SDL_PollEvent (&event) != 0) 
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                {
                    quit = 1;
                    break;
                }

                case SDL_EVENT_WINDOW_RESIZED:
                {
                    SDL_GetWindowSize (window, &window_width, &window_height);
                    SDL_SetWindowSize (window, window_width, window_height);
                    window_surface = SDL_GetWindowSurface (window);
                    break;
                }

                case SDL_EVENT_KEY_DOWN:
                {
                    switch (event.key.scancode)
                    {
                        case SDL_SCANCODE_H:
                        case SDL_SCANCODE_J:
                        case SDL_SCANCODE_K:
                        case SDL_SCANCODE_L:
                        case SDL_SCANCODE_Z:
                    }
                    break;
                }

                default:
                    break;
            }
        }

        DrawMandelbrot (window, window_surface, window_width, window_height);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void DrawMandelbrot (SDL_Window* window, SDL_Surface* window_surface, int window_width, int window_height)
{
    uint32_t* pixels = (uint32_t*) calloc (window_width * window_height, sizeof (uint32_t));

    // Nested *pixel* loops
    for (int y_pixel = 0; y_pixel < window_height; ++y_pixel)
    {
        for (int x_pixel = 0; x_pixel < window_width; ++x_pixel)
        {
            double aspect_ratio = (double) window_width / window_height;

            // Transform screen coords to Mandelbrot set coords
            double y_0 = (y_pixel - window_height / 2.0) * (4.0 / window_height);                     // y \in (-2, 2)
            double x_0 = (x_pixel - window_width  / 2.0) * (4.0 * aspect_ratio / window_width) - 0.5; // x \in (-3, 2.555)

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

            pixels[y_pixel * window_width + x_pixel] = color;
        }
    }

    SDL_LockSurface(window_surface);
    memcpy(window_surface->pixels, pixels, window_width * window_height * sizeof (uint32_t));
    SDL_UnlockSurface(window_surface);

    SDL_UpdateWindowSurface(window);

    free (pixels); pixels = NULL;
}
