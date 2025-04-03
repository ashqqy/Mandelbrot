#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "render.h"

//--------------------------------------------------------------------

void ImageDataInit (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    image_data->height_half = window_params->height / 2.0;
    image_data->width_half  = window_params->width  / 2.0;

    image_data->x_shift = -0.5;
    image_data->y_shift = 0.0;
    image_data->zoom    = 1.0;

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

    double x0_coef = (4.0 * window_params->aspect_ratio / window_params->width) / image_data->zoom;
    double y0_coef = (4.0 / window_params->height) / image_data->zoom;

    double x_Re = 0;   // current Re(z)
    double y_Im = 0;   // current Im(z)
    double x2_Re = 0;  // Re(z)^2
    double y2_Im = 0;  // Im(z)^2

    // Nested *pixel* loops
    for (int y_pixel = 0; y_pixel < window_params->height; ++y_pixel)
    {
        for (int x_pixel = 0; x_pixel < window_params->width; ++x_pixel)
        {
            // Transform screen coords to Mandelbrot set coords (x \in (-4, 3); y \in (-2, 2))
            double y_0 = (y_pixel - image_data->height_half) * y0_coef + image_data->y_shift; 
            double x_0 = (x_pixel - image_data->width_half)  * x0_coef + image_data->x_shift;

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

            x_Re = 0;
            y_Im = 0;
            x2_Re = 0;
            y2_Im = 0;

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
