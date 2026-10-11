#include "texture.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_ppm_token(FILE *fp, char *buffer, size_t capacity)
{
    int c;
    size_t length = 0;

    while (1)
    {
        c = fgetc(fp);
        if (c == EOF)
            return 0;
        if (isspace((unsigned char)c))
            continue;
        if (c == '#')
        {
            while (c != EOF && c != '\n')
                c = fgetc(fp);
            continue;
        }
        break;
    }
    while (c != EOF && !isspace((unsigned char)c))
    {
        if (length + 1 < capacity)
            buffer[length++] = (char)c;
        c = fgetc(fp);
    }
    if (c != EOF)
        ungetc(c, fp);
    buffer[length] = '\0';
    return length > 0;
}

int load_ppm(const char *path, t_image *image)
{
    FILE *fp;
    char magic[3] = {0};
    char token[64];
    int max_value;
    size_t count;
    size_t i;
    long value;

    if (!path || !image)
        return 1;
    *image = (t_image){0};
    fp = fopen(path, "rb");
    if (!fp || !read_ppm_token(fp, magic, sizeof(magic))
        || (strcmp(magic, "P3") != 0 && strcmp(magic, "P6") != 0)
        || !read_ppm_token(fp, token, sizeof(token)))
    {
        fprintf(stderr, "PPM invalido: %s\n", path);
        if (fp)
            fclose(fp);
        return 1;
    }
    image->width = (int)strtol(token, NULL, 10);
    if (!read_ppm_token(fp, token, sizeof(token)))
    {
        fclose(fp);
        return 1;
    }
    image->height = (int)strtol(token, NULL, 10);
    if (!read_ppm_token(fp, token, sizeof(token)))
    {
        fclose(fp);
        return 1;
    }
    max_value = (int)strtol(token, NULL, 10);
    if (image->width <= 0 || image->height <= 0 || max_value != 255)
    {
        fprintf(stderr, "PPM invalido (se requiere maximo 255): %s\n", path);
        fclose(fp);
        return 1;
    }
    count = (size_t)image->width * (size_t)image->height * 3;
    image->pixels = malloc(count);
    if (!image->pixels)
    {
        fclose(fp);
        return 1;
    }
    if (strcmp(magic, "P6") == 0)
    {
        int separator = fgetc(fp);

        if (separator == '\r')
        {
            int next = fgetc(fp);
            if (next != '\n' && next != EOF)
                ungetc(next, fp);
        }
        else if (!isspace((unsigned char)separator))
        {
            free(image->pixels);
            image->pixels = NULL;
            fclose(fp);
            return 1;
        }
        if (fread(image->pixels, 1, count, fp) != count)
        {
            free(image->pixels);
            image->pixels = NULL;
            fclose(fp);
            fprintf(stderr, "PPM incompleto: %s\n", path);
            return 1;
        }
    }
    else
    {
        i = 0;
        while (i < count && read_ppm_token(fp, token, sizeof(token)))
        {
            value = strtol(token, NULL, 10);
            if (value < 0 || value > 255)
            {
                free(image->pixels);
                image->pixels = NULL;
                fclose(fp);
                return 1;
            }
            image->pixels[i++] = (unsigned char)value;
        }
        if (i != count)
        {
            free(image->pixels);
            image->pixels = NULL;
            fclose(fp);
            fprintf(stderr, "PPM incompleto: %s\n", path);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void free_image(t_image *image)
{
    if (image)
    {
        free(image->pixels);
        image->pixels = NULL;
    }
}
