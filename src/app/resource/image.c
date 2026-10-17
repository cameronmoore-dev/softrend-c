#include "image.h"

#include <stdlib.h>

void image_free(image_ *image)
{
    free(image->pixels);
}