#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "render.h"

//--------------------------------------------------------------------

void ImageDataInit (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    image_data->pixels = (uint32_t*) calloc ((size_t) window_params->width * (size_t) window_params->height, sizeof (uint32_t));
    if (image_data->pixels == NULL)
    {
        printf ("Pixels array init error\n");
        EXIT_FAILURE;
    }

    image_data->colors = (uint32_t*) calloc (MAX_ITER + 1, sizeof (uint32_t));
    if (image_data->colors == NULL)
    {
        printf ("Colors array init error\n");
        EXIT_FAILURE;
    }

    // Generate array of colors
    for (uint32_t iter = 0; iter <= MAX_ITER; ++iter)
    {
        uint32_t red   = (iter * iter - 1) % MAX_ITER;
        uint32_t green = (red + iter) % MAX_ITER;
        uint32_t blue  = (red + green + iter) % MAX_ITER;

        // Pack into ARGB format
        image_data->colors[iter] = (ALPHA << 24) | (red << 16) | (green << 8) | (blue);
    }
}

//--------------------------------------------------------------------

void SetCalculate (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    // Nested *pixel* loops
    for (int y_pixel = 0; y_pixel < window_params->height; ++y_pixel)
    {
        for (int x_pixel = 0; x_pixel < window_params->width; ++x_pixel)
        {
            double aspect_ratio = (double) window_params->width / window_params->height;

            // Transform screen coords to Mandelbrot set coords (x \in (-4, 3); y \in (-2, 2))
            double y_0 = (y_pixel - window_params->height / 2.0) * (4.0 / window_params->height)
                        / window_params->zoom + window_params->y_shift;
                
            double x_0 = (x_pixel - window_params->width  / 2.0) * (4.0 * aspect_ratio / window_params->width)
                        / window_params->zoom + window_params->x_shift;

            double x_Re = 0;   // current Re(z)
            double y_Im = 0;   // current Im(z)

            double x2_Re = 0;  // Re(z)^2
            double y2_Im = 0;  // Im(z)^2

            uint32_t iteration = 0;

            // (x2_Re + y2_Im <= 4) equal (|z|^2 <= 4) equal (|z| <= 2)
            while ((x2_Re + y2_Im <= 4) && (iteration < MAX_ITER))
            {
                // z_n = z_{n-1}^2 + z_0
                y_Im = 2 * x_Re * y_Im + y_0;
                x_Re = x2_Re - y2_Im + x_0;

                x2_Re = x_Re * x_Re;
                y2_Im = y_Im * y_Im;

                iteration++;
            }

            image_data->pixels[y_pixel * window_params->width + x_pixel] = image_data->colors[iteration];
        }
    }
}

void ImageUpdate (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    SDL_LockSurface (window_params->surface);
    
    memcpy (window_params->surface->pixels, image_data->pixels, 
        (size_t) window_params->width * (size_t) window_params->height * sizeof (uint32_t));

    SDL_UnlockSurface (window_params->surface);

    SDL_UpdateWindowSurface (window_params->window);
}

//--------------------------------------------------------------------

void ImageDataDestroy (image_data_t* image_data)
{
    assert (image_data != NULL);

    free (image_data->pixels); image_data->pixels = NULL;
    free (image_data->colors); image_data->colors = NULL;
}

//--------------------------------------------------------------------
