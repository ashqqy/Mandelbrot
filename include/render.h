#ifndef RENDER_H
#define RENDER_H

//--------------------------------------------------------------------

#include "ui.h"

//--------------------------------------------------------------------

typedef void (*SetCalculateFunction)(window_params_t*, image_data_t*);

static const uint32_t MAX_ITER = 256;
static const double MAX_RADIUS = 4.0;

// for ARGB format
static const uint32_t ALPHA = 255;

static const int N_PARALLEL_CALCULATED_PIXELS = 4;

//--------------------------------------------------------------------

SetCalculateFunction SetCalculateFunctionChoose (const int argc, const char* argv[]);

void ImageDataInit (window_params_t* window_params, image_data_t* image_data);

void SetCalculateSlow       (window_params_t* window_params, image_data_t* image_data);
void SetCalculateUnrolling  (window_params_t* window_params, image_data_t* image_data);
void SetCalculateIntrinsics (window_params_t* window_params, image_data_t* image_data);

void ImageUpdate (window_params_t* window_params, image_data_t* image_data);

void ImageDataDestroy (image_data_t* image_data);

//--------------------------------------------------------------------

#endif // RENDER_H
