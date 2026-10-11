#include "hud.h"

#include <stdio.h>
#include <string.h>

static const unsigned char g_font[128][7] = {
    ['0'] = {14, 17, 19, 21, 25, 17, 14},
    ['1'] = {4, 12, 4, 4, 4, 4, 14},
    ['2'] = {14, 17, 1, 2, 4, 8, 31},
    ['3'] = {30, 1, 1, 14, 1, 1, 30},
    ['4'] = {2, 6, 10, 18, 31, 2, 2},
    ['5'] = {31, 16, 16, 30, 1, 1, 30},
    ['6'] = {14, 16, 16, 30, 17, 17, 14},
    ['7'] = {31, 1, 2, 4, 8, 8, 8},
    ['8'] = {14, 17, 17, 14, 17, 17, 14},
    ['9'] = {14, 17, 17, 15, 1, 1, 14},
    ['A'] = {14, 17, 17, 31, 17, 17, 17},
    ['B'] = {30, 17, 17, 30, 17, 17, 30},
    ['C'] = {14, 17, 16, 16, 16, 17, 14},
    ['D'] = {30, 17, 17, 17, 17, 17, 30},
    ['E'] = {31, 16, 16, 30, 16, 16, 31},
    ['F'] = {31, 16, 16, 30, 16, 16, 16},
    ['G'] = {14, 17, 16, 23, 17, 17, 14},
    ['H'] = {17, 17, 17, 31, 17, 17, 17},
    ['I'] = {31, 4, 4, 4, 4, 4, 31},
    ['L'] = {16, 16, 16, 16, 16, 16, 31},
    ['M'] = {17, 27, 21, 21, 17, 17, 17},
    ['N'] = {17, 25, 21, 19, 17, 17, 17},
    ['O'] = {14, 17, 17, 17, 17, 17, 14},
    ['P'] = {30, 17, 17, 30, 16, 16, 16},
    ['Q'] = {14, 17, 17, 17, 21, 18, 13},
    ['R'] = {30, 17, 17, 30, 20, 18, 17},
    ['S'] = {15, 16, 16, 14, 1, 1, 30},
    ['T'] = {31, 4, 4, 4, 4, 4, 4},
    ['U'] = {17, 17, 17, 17, 17, 17, 14},
    ['V'] = {17, 17, 17, 17, 17, 10, 4},
    ['W'] = {17, 17, 17, 21, 21, 27, 17},
    ['X'] = {17, 17, 10, 4, 10, 17, 17},
    ['Y'] = {17, 17, 10, 4, 4, 4, 4},
    ['Z'] = {31, 1, 2, 4, 8, 16, 31},
    ['/'] = {1, 2, 2, 4, 8, 8, 16},
};

static void hud_quad(t_hud_vertex *vertices, size_t *count, size_t capacity,
                     float x, float y, float width, float height,
                     float r, float g, float b)
{
    if (*count + 6 > capacity)
        return;
    vertices[(*count)++] = (t_hud_vertex){x, y, r, g, b};
    vertices[(*count)++] = (t_hud_vertex){x + width, y, r, g, b};
    vertices[(*count)++] = (t_hud_vertex){x + width, y + height, r, g, b};
    vertices[(*count)++] = (t_hud_vertex){x, y, r, g, b};
    vertices[(*count)++] = (t_hud_vertex){x + width, y + height, r, g, b};
    vertices[(*count)++] = (t_hud_vertex){x, y + height, r, g, b};
}

static void hud_text(t_hud_vertex *vertices, size_t *count, size_t capacity,
                     const char *text, float x, float y, float scale,
                     float r, float g, float b)
{
    size_t i = 0;
    int row;
    int column;

    while (text[i])
    {
        const unsigned char *glyph = g_font[(unsigned char)text[i]];

        row = 0;
        while (row < 7)
        {
            column = 0;
            while (column < 5)
            {
                if (glyph[row] & (1 << (4 - column)))
                    hud_quad(vertices, count, capacity,
                             x + (float)column * scale,
                             y + (float)row * scale,
                             scale, scale, r, g, b);
                column++;
            }
            row++;
        }
        x += scale * 6.0f;
        i++;
    }
}

static void hud_key(t_hud_vertex *vertices, size_t *count, size_t capacity,
                    const char *key, const char *label, float y)
{
    const float key_width = (float)strlen(key) * 12.0f + 10.0f;

    hud_quad(vertices, count, capacity, 26.0f, y, key_width, 18.0f,
             0.16f, 0.23f, 0.30f);
    hud_text(vertices, count, capacity, key, 32.0f, y + 4.0f, 2.0f,
             0.85f, 0.95f, 1.0f);
    hud_text(vertices, count, capacity, label, 26.0f + key_width + 12.0f,
             y + 4.0f, 2.0f, 0.72f, 0.78f, 0.84f);
}

static void hud_right_text(t_hud_vertex *vertices, size_t *count,
                           size_t capacity, const char *text, float y,
                           int framebuffer_width, float r, float g, float b)
{
    const float text_width = (float)strlen(text) * 12.0f;
    const float x = (float)framebuffer_width - text_width - 24.0f;

    hud_text(vertices, count, capacity, text, x, y, 2.0f, r, g, b);
}

size_t build_hud(t_hud_vertex *vertices, size_t capacity,
                 int framebuffer_width, int framebuffer_height,
                 int texture_on, int wireframe, int normals,
                 size_t vertex_count, size_t index_count)
{
    char statistics[64];
    size_t count = 0;

    snprintf(statistics, sizeof(statistics), "V %zu  I %zu  F %zu",
             vertex_count, index_count, index_count / 3);
    hud_quad(vertices, &count, capacity, 12.0f, 12.0f, 260.0f, 272.0f,
             0.035f, 0.055f, 0.075f);
    hud_text(vertices, &count, capacity, "SCOP CONTROLS",
             26.0f, 25.0f, 2.0f, 0.35f, 0.85f, 0.95f);
    hud_key(vertices, &count, capacity, "UP/DN", "ROTATE X", 52.0f);
    hud_key(vertices, &count, capacity, "LT/RT", "ROTATE Y", 76.0f);
    hud_key(vertices, &count, capacity, "Q/E", "ROTATE Z", 100.0f);
    hud_key(vertices, &count, capacity, "A/D", "MOVE X", 124.0f);
    hud_key(vertices, &count, capacity, "W/S", "MOVE Y", 148.0f);
    hud_key(vertices, &count, capacity, "R/F", "MOVE Z", 172.0f);
    hud_key(vertices, &count, capacity, "T", "TEXTURE", 196.0f);
    hud_key(vertices, &count, capacity, "G", "WIREFRAME", 220.0f);
    hud_key(vertices, &count, capacity, "N/B", "NORMALS", 244.0f);
    hud_right_text(vertices, &count, capacity,
                   texture_on ? "TEXTURE ON" : "TEXTURE OFF",
                   25.0f, framebuffer_width, 0.95f, 0.75f, 0.30f);
    hud_right_text(vertices, &count, capacity,
                   wireframe ? "WIRE ON" : "WIRE OFF",
                   43.0f, framebuffer_width, 0.95f, 0.75f, 0.30f);
    hud_right_text(vertices, &count, capacity,
                   normals ? "NORMALS ON" : "NORMALS OFF",
                   61.0f, framebuffer_width, 0.95f, 0.75f, 0.30f);
    hud_right_text(vertices, &count, capacity, statistics,
                   (float)framebuffer_height - 32.0f, framebuffer_width,
                   0.55f, 0.65f, 0.72f);
    hud_quad(vertices, &count, capacity, 26.0f,
             (float)framebuffer_height - 42.0f, 220.0f, 1.0f,
             0.16f, 0.23f, 0.30f);
    hud_text(vertices, &count, capacity, "ESC EXIT / H HELP",
             26.0f, (float)framebuffer_height - 32.0f, 2.0f,
             0.55f, 0.65f, 0.72f);
    return count;
}
