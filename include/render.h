#ifndef RENDER_H
#define RENDER_H

//--------------------------------------------------------------------

#include "ui.h"

//--------------------------------------------------------------------

static const uint32_t MAX_ITER = 256;

static const uint32_t ALPHA = 255;

//--------------------------------------------------------------------

void ImageDataInit (window_params_t* window_params, image_data_t* image_data);

void SetCalculate (window_params_t* window_params, image_data_t* image_data);
void ImageUpdate (window_params_t* window_params, image_data_t* image_data);

void ImageDataDestroy (image_data_t* image_data);

//--------------------------------------------------------------------

#endif // RENDER_H
