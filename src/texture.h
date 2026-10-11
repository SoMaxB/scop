#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct s_image
{
    unsigned char *pixels;
    int            width;
    int            height;
} t_image;

int  load_ppm(const char *path, t_image *image);
void free_image(t_image *image);

#endif
