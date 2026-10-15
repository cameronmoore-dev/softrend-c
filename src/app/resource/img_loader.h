#pragma once

#include "app/resource/image.h"

image_ image_load(const char *path);
void image_free(image_ *image);