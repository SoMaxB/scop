#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <stddef.h>

typedef struct s_mesh {
    float    *vertices;   /* v_count * 3 floats: x,y,z,x,y,z,... */
    size_t    v_count;    /* nº de vértices (no de floats) */
    unsigned *indices;    /* triángulos ya triangulados, base 0 */
    size_t    i_count;    /* nº de índices (múltiplo de 3) */
} t_mesh;

int  parse_obj(const char *path, t_mesh *out);
void center_mesh(t_mesh *mesh);
void free_mesh(t_mesh *mesh);

#endif
