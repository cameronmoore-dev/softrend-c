#include "img_loader.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#pragma pack(push, 1)
typedef struct
{
    u8 idLength;
    u8 colourMap;
    u8 dataTypeCode;
    u16 colourMapOrigin;
    u16 colourMapLength;
    u8 colourMapDepth;
    u16 xOrigin;
    u16 yOrigin;
    u16 width;
    u16 height;
    u8 bitsPerPixel;
    u8 imageDescriptor;
} tga_header_;
#pragma pack(pop)

void read_uncompressed(FILE* file, image_ *image, bool has_alpha);
void read_compressed(FILE* file, image_ *image, bool has_alpha);
void read_raw_packet(FILE* file, image_ *image, u8 chunk_header, u32 *current_pixel, bool has_alpha);
void read_rle_packet(FILE* file, image_ *image, u8 chunk_header, u32 *current_pixel, bool has_alpha);

image_ image_load(const char *path)
{
    FILE* fp = fopen(path, "rb");
    assert(fp && "Failed to load TGA image from path!");

    tga_header_ tga;
    fread(&tga, sizeof(tga_header_), 1, fp);
    
    image_ image = 
    {
        .width = tga.width,
        .height = tga.height,
        .bpp = tga.bitsPerPixel / 8
    };

    assert((image.bpp == 3 || image.bpp == 4) && "1 and 2 byte pixel depth images are not supported!");

    image.pixels = (u32 *)malloc(image.width * image.height * sizeof(u32));

    bool is_rle = tga.dataTypeCode == 10 || tga.dataTypeCode == 11;
    bool has_alpha = image.bpp == 4;
    bool topleft_origin = (tga.imageDescriptor & 0x20) != 0;

    (is_rle) 
        ? read_compressed(fp, &image, has_alpha) 
        : read_uncompressed(fp, &image, has_alpha);

    if (topleft_origin)
    {
        // Flip the texture vertically so the origin is on the bottom left,
        // keeping in line with OpenGL and Vulkan standards
        for (u32 y = 0; y < (image.height / 2); y++)
        {
            for (u32 x = 0; x < image.width; x++)
            {
                u32 top_index = x + y * image.width;
                u32 bottom_index = x + (image.height - 1 - y) * image.width;

                u32 tmp = image.pixels[top_index];
                image.pixels[top_index] = image.pixels[bottom_index];
                image.pixels[bottom_index] = tmp;
            }
        }
    }

    fclose(fp);
    return image;
}

void image_free(image_ *image)
{
    free(image->pixels);
}


void read_uncompressed(FILE* file, image_ *image, bool has_alpha)
{
    for (u32 i = 0; i < image->width * image->height; i++)
    {
        u8 image_pixel[4];
        fread(image_pixel, image->bpp, 1, file);

        u8 *buffer_pixel = (u8 *)&image->pixels[i];
        buffer_pixel[0] = image_pixel[0];
        buffer_pixel[1] = image_pixel[1];
        buffer_pixel[2] = image_pixel[2];
        buffer_pixel[3] = (has_alpha) ? image_pixel[3] : 255;
    }
}

void read_compressed(FILE* file, image_ *image, bool has_alpha)
{
    u32 current_pixel = 0;
    u32 num_pixels = image->width * image->height;
    while (current_pixel < num_pixels)
    {
        u8 chunk_header = fgetc(file);
        if (chunk_header < 128)
        {
            chunk_header++;
            read_raw_packet(file, image, chunk_header, &current_pixel, has_alpha);
        }
        else
        {
            chunk_header -= 127;
            read_rle_packet(file, image, chunk_header, &current_pixel, has_alpha);
        }
    }
}

void read_raw_packet(FILE* file, image_ *image, u8 chunk_header, u32 *current_pixel, bool has_alpha)
{
    for (u32 i = 0; i < chunk_header; i++)
    {
        u8 image_pixel[4];
        fread(image_pixel, image->bpp, 1, file);

        u8 *buffer_pixel = (u8 *)&image->pixels[*current_pixel];
        buffer_pixel[0] = image_pixel[0];
        buffer_pixel[1] = image_pixel[1];
        buffer_pixel[2] = image_pixel[2];
        buffer_pixel[3] = (has_alpha) ? image_pixel[3] : 255;

        *current_pixel += 1;
    }
}

void read_rle_packet(FILE* file, image_ *image, u8 chunk_header, u32 *current_pixel, bool has_alpha)
{
    u8 image_pixel[4];
    fread(image_pixel, image->bpp, 1, file);
    
    for (u32 i = 0; i < chunk_header; i++)
    {
        u8 *buffer_pixel = (u8 *)&image->pixels[*current_pixel];
        buffer_pixel[0] = image_pixel[0];
        buffer_pixel[1] = image_pixel[1];
        buffer_pixel[2] = image_pixel[2];
        buffer_pixel[3] = (has_alpha) ? image_pixel[3] : 255;

        *current_pixel += 1;
    }
}