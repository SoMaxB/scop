#ifndef HUD_H
#define HUD_H

#include <stddef.h>

typedef struct s_hud_vertex
{
    float x;
    float y;
    float r;
    float g;
    float b;
} t_hud_vertex;

size_t build_hud(t_hud_vertex *vertices, size_t capacity,
                 int framebuffer_width, int framebuffer_height,
                 int texture_on, int wireframe, int normals,
                 size_t vertex_count, size_t index_count);

#endif
