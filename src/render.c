#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "render.h"
#include "ui.h"

//--------------------------------------------------------------------

void SetCalculateSlow (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    double x0_coef = (4.0 * window_params->aspect_ratio / window_params->width) / image_data->zoom;
    double y0_coef = (4.0 / window_params->height) / image_data->zoom;

    // Nested *pixel* loops
    for (int y_pixel = 0; y_pixel < window_params->height; ++y_pixel)
    {
        for (int x_pixel = 0; x_pixel < window_params->width; ++x_pixel)
        {
            double z_x = 0;   // current Re(z)
            double x_y = 0;   // current Im(z)
            double z_x2 = 0;  // Re(z)^2
            double x_y2 = 0;  // Im(z)^2

            // Transform screen coords to Mandelbrot set coords (x \in (-4, 3); y \in (-2, 2))
            double y_0 = (y_pixel - image_data->height_half) * y0_coef + image_data->y_shift; 
            double x_0 = (x_pixel - image_data->width_half)  * x0_coef + image_data->x_shift;

            uint32_t iteration = 0;

            // (z_x2 + x_y2 <= 4) equal (|z|^2 <= 4) equal (|z| <= 2)
            while ((z_x2 + x_y2 <= MAX_RADIUS) && (iteration < MAX_ITER))
            {
                // z_n = z_{n-1}^2 + z_0
                x_y = 2 * z_x * x_y + y_0;
                z_x = z_x2 - x_y2 + x_0;

                z_x2 = z_x * z_x;
                x_y2 = x_y * x_y;

                iteration++;
            }

            // Copy iteration number in the array of pixels, then at once recount iterations in colors
            image_data->pixels[y_pixel * window_params->width + x_pixel] = image_data->colors[iteration];
        }
    }
}

int isSet = 0;

void SetCalculateParallel (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    double x0_coef = (4.0 * window_params->aspect_ratio / window_params->width) / image_data->zoom;
    double y0_coef = (4.0 / window_params->height) / image_data->zoom;

    double x_0[N_PARALLEL_CALCULATED_PIXELS] = {};
    double y_0[N_PARALLEL_CALCULATED_PIXELS] = {};

    // *Pixel* loop
    for (int pixel = 0; pixel < window_params->height * window_params->width; pixel += N_PARALLEL_CALCULATED_PIXELS)
    {
        double z_x[N_PARALLEL_CALCULATED_PIXELS]  = {};   // current Re(z)
        double x_y[N_PARALLEL_CALCULATED_PIXELS]  = {};   // current Im(z)
        double z_x2[N_PARALLEL_CALCULATED_PIXELS] = {};   // Re(z)^2
        double x_y2[N_PARALLEL_CALCULATED_PIXELS] = {};   // Im(z)^2

        // Transform screen coords to Mandelbrot set coords (x \in (-4, 3); y \in (-2, 2))
        for (int i = 0; i < N_PARALLEL_CALCULATED_PIXELS; ++i)
        {
            double y_pixel = (pixel + i) / window_params->width;
            double x_pixel = (pixel + i) % window_params->width;

            y_0[i] = (y_pixel - image_data->height_half) * y0_coef + image_data->y_shift; 
            x_0[i] = (x_pixel - image_data->width_half)  * x0_coef + image_data->x_shift;
        }

        uint32_t iterations[N_PARALLEL_CALCULATED_PIXELS] = {};

        for (uint32_t i = 0; i < MAX_ITER; ++i)
        {
            double r2[N_PARALLEL_CALCULATED_PIXELS] = {};
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j)  // radius^2 = Re(z)^2 + Im(z)^2
                r2[j] = z_x2[j] + x_y2[j];

            int is_set[N_PARALLEL_CALCULATED_PIXELS] = {};
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j)
                is_set[j] = (r2[j] < MAX_RADIUS) ? 1 : 0;           // (radius^2 <= 4) ?
            
            int mask = 0;
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j)
                mask += is_set[j];

            if (mask == 0)                                          // if (all pixels "fly away")
                break;                                              //     stop calculating  
                
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j) x_y[j] = 2 * z_x[j] * x_y[j] + y_0[j];
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j) z_x[j] = z_x2[j] - x_y2[j] + x_0[j];
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j) z_x2[j] = z_x[j] * z_x[j];
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j) x_y2[j] = x_y[j] * x_y[j];
            for (int j = 0; j < N_PARALLEL_CALCULATED_PIXELS; ++j) iterations[j] += (uint32_t) is_set[j];
        }

        for (int i = 0; i < N_PARALLEL_CALCULATED_PIXELS; ++i)
            image_data->pixels[pixel + i] = image_data->colors[iterations[i]];
    }
}

void SetCalculateIntrinsics (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);
}

//--------------------------------------------------------------------

SetCalculateFunction SetCalculateFunctionChoose (const int argc, const char* argv[])
{
    assert (argv != NULL);

    if (argc != 2)
    {
        // if the mode is not specified, then use intrinsics by default
        return SetCalculateIntrinsics;
    }
    else if (strcmp (argv[1], "slow") == 0)
    {
        return SetCalculateSlow;
    }
    else if (strcmp (argv[1], "parallel") == 0)
    {
        return SetCalculateParallel;
    }
    else if (strcmp (argv[1], "intrinsics") == 0)
    {
        return SetCalculateIntrinsics;
    }
    else
    {
        printf ("You chose the wrong mode.\nPlease specify the mode of calculating the set:\n 1) slow\n 2) parallel\n 3) intrinsics\n");
        return NULL;
    }
}

//--------------------------------------------------------------------

void ImageDataInit (window_params_t* window_params, image_data_t* image_data)
{
    assert (window_params != NULL);
    assert (image_data    != NULL);

    image_data->pixels_array_size = window_params->width * window_params->height + 3;

    image_data->height_half = window_params->height / 2.0;
    image_data->width_half  = window_params->width  / 2.0;

    image_data->x_shift = -0.5;
    image_data->y_shift = 0.0;
    image_data->zoom    = 1.0;

    image_data->pixels = (uint32_t*) calloc ((size_t) image_data->pixels_array_size, sizeof (uint32_t));
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
