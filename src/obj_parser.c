#include "obj_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* Salta espacios/tabs al inicio */
static const char *skip_ws(const char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\r')
        s++;
    return s;
}

/* Cuenta cuántos índices hay en una línea 'f ...' y valida que sean >=3.
   Devuelve n (nº de vértices de la cara) o 0 si error. */
static int count_face_tokens(const char *p, int *out_n)
{
    int n = 0;
    p = skip_ws(p);
    while (*p && *p != '\n' && *p != '#')
    {
        char *end;
        long v = strtol(p, &end, 10);
        if (end == p)
            return 0; /* token no es un entero */
        (void)v;
        n++;
        /* saltar resto del token hasta espacio (para "1/2/3" o "1//3") */
        p = end;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n')
            p++;
        p = skip_ws(p);
    }
    if (n < 3)
        return 0;
    *out_n = n;
    return 1;
}

void free_mesh(t_mesh *mesh)
{
    if (!mesh)
        return;
    free(mesh->vertices);
    free(mesh->indices);
    mesh->vertices = NULL;
    mesh->indices = NULL;
    mesh->v_count = 0;
    mesh->i_count = 0;
}

static void project_vertex(const float *vertices, int index, int axis,
                           float *x, float *y)
{
    const float *v = &vertices[(size_t)index * 3];

    if (axis == 0)
    {
        *x = v[1];
        *y = v[2];
    }
    else if (axis == 1)
    {
        *x = v[0];
        *y = v[2];
    }
    else
    {
        *x = v[0];
        *y = v[1];
    }
}

static int dominant_projection(const float *vertices, const int *face, int n,
                               float *area)
{
    float normal[3] = {0.0f, 0.0f, 0.0f};
    int i;
    int axis;
    float ax;
    float ay;
    float bx;
    float by;
    float signed_area = 0.0f;

    i = 0;
    while (i < n)
    {
        const float *a = &vertices[(size_t)(face[i] - 1) * 3];
        const float *b = &vertices[(size_t)(face[(i + 1) % n] - 1) * 3];
        normal[0] += (a[1] - b[1]) * (a[2] + b[2]);
        normal[1] += (a[2] - b[2]) * (a[0] + b[0]);
        normal[2] += (a[0] - b[0]) * (a[1] + b[1]);
        i++;
    }
    axis = 0;
    if (fabsf(normal[1]) > fabsf(normal[axis]))
        axis = 1;
    if (fabsf(normal[2]) > fabsf(normal[axis]))
        axis = 2;
    i = 0;
    while (i < n)
    {
        project_vertex(vertices, face[i] - 1, axis, &ax, &ay);
        project_vertex(vertices, face[(i + 1) % n] - 1, axis, &bx, &by);
        signed_area += ax * by - bx * ay;
        i++;
    }
    *area = signed_area * 0.5f;
    return axis;
}

static float cross_2d(float ax, float ay, float bx, float by,
                      float cx, float cy)
{
    return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax);
}

static int point_in_triangle(const float *vertices, const int *face, int axis,
                             int a, int b, int c, int point, float winding)
{
    float ax;
    float ay;
    float bx;
    float by;
    float cx;
    float cy;
    float px;
    float py;
    const float epsilon = 0.000001f;

    project_vertex(vertices, face[a] - 1, axis, &ax, &ay);
    project_vertex(vertices, face[b] - 1, axis, &bx, &by);
    project_vertex(vertices, face[c] - 1, axis, &cx, &cy);
    project_vertex(vertices, face[point] - 1, axis, &px, &py);
    return cross_2d(ax, ay, bx, by, px, py) * winding >= -epsilon
        && cross_2d(bx, by, cx, cy, px, py) * winding >= -epsilon
        && cross_2d(cx, cy, ax, ay, px, py) * winding >= -epsilon;
}

static int triangulate_face(const float *vertices, const int *face, int n,
                            unsigned *indices, size_t *index_cursor)
{
    int polygon[64];
    int axis;
    int polygon_size;
    int guard;
    int i;
    float area;
    float winding;

    if (n < 3 || n > 64)
        return 1;
    if (n == 3)
    {
        indices[(*index_cursor)++] = (unsigned)(face[0] - 1);
        indices[(*index_cursor)++] = (unsigned)(face[1] - 1);
        indices[(*index_cursor)++] = (unsigned)(face[2] - 1);
        return 0;
    }
    axis = dominant_projection(vertices, face, n, &area);
    if (fabsf(area) < 0.000001f)
        return 1;
    winding = area > 0.0f ? 1.0f : -1.0f;
    i = 0;
    while (i < n)
    {
        polygon[i] = i;
        i++;
    }
    polygon_size = n;
    guard = 0;
    while (polygon_size > 3 && guard++ < n * n)
    {
        int ear_found = 0;
        i = 0;
        while (i < polygon_size)
        {
            const int previous = polygon[(i + polygon_size - 1) % polygon_size];
            const int current = polygon[i];
            const int next = polygon[(i + 1) % polygon_size];
            float ax;
            float ay;
            float bx;
            float by;
            float cx;
            float cy;
            int point;
            int contains_point = 0;

            project_vertex(vertices, face[previous] - 1, axis, &ax, &ay);
            project_vertex(vertices, face[current] - 1, axis, &bx, &by);
            project_vertex(vertices, face[next] - 1, axis, &cx, &cy);
            if (cross_2d(ax, ay, bx, by, cx, cy) * winding > 0.000001f)
            {
                point = 0;
                while (point < polygon_size)
                {
                    const int candidate = polygon[point];
                    if (candidate != previous && candidate != current
                        && candidate != next
                        && point_in_triangle(vertices, face, axis, previous,
                                             current, next, candidate, winding))
                    {
                        contains_point = 1;
                        break;
                    }
                    point++;
                }
                if (!contains_point)
                {
                    indices[(*index_cursor)++] = (unsigned)(face[previous] - 1);
                    indices[(*index_cursor)++] = (unsigned)(face[current] - 1);
                    indices[(*index_cursor)++] = (unsigned)(face[next] - 1);
                    while (i + 1 < polygon_size)
                    {
                        polygon[i] = polygon[i + 1];
                        i++;
                    }
                    polygon_size--;
                    ear_found = 1;
                    break;
                }
            }
            i++;
        }
        if (!ear_found)
            return 1;
    }
    if (polygon_size != 3)
        return 1;
    indices[(*index_cursor)++] = (unsigned)(face[polygon[0]] - 1);
    indices[(*index_cursor)++] = (unsigned)(face[polygon[1]] - 1);
    indices[(*index_cursor)++] = (unsigned)(face[polygon[2]] - 1);
    return 0;
}

int parse_obj(const char *path, t_mesh *out)
{
    FILE *fp;
    char line[1024];
    size_t v_count = 0;
    size_t total_indices = 0;

    if (!path || !out)
        return 1;
    memset(out, 0, sizeof(*out));

    fp = fopen(path, "r");
    if (!fp)
    {
        fprintf(stderr, "parse_obj: no se pudo abrir '%s'\n", path);
        return 1;
    }

    /* ---------- pasada 1: contar ---------- */
    while (fgets(line, sizeof(line), fp))
    {
        const char *s = skip_ws(line);
        if (*s == '\0' || *s == '\n' || *s == '#')
            continue;
        if (*s == 'o' && (s[1] == ' ' || s[1] == '\t'))
            continue; /* nombre de objeto: ignorar */
        if (*s == 's' && (s[1] == ' ' || s[1] == '\t' || s[1] == '\n'))
            continue; /* smoothing group */
        if (s[0] == 'v' && s[1] == ' ')
        {
            float x, y, z;
            if (sscanf(s, "v %f %f %f", &x, &y, &z) != 3)
            {
                fprintf(stderr, "parse_obj: linea 'v' malformada: %s", line);
                fclose(fp);
                return 1;
            }
            v_count++;
        }
        else if (s[0] == 'v' && s[1] == 't')
            continue; /* vt: ignoramos en clase 3 */
        else if (s[0] == 'v' && s[1] == 'n')
            continue; /* vn: ignoramos en clase 3 */
        else if (s[0] == 'f' && s[1] == ' ')
        {
            int n = 0;
            if (!count_face_tokens(s + 2, &n))
            {
                fprintf(stderr, "parse_obj: linea 'f' malformada (<3 vertices o token invalido): %s", line);
                fclose(fp);
                return 1;
            }
            total_indices += (size_t)(n - 2) * 3;
        }
        else
        {
            /* Otras lineas (mtllib, usemtl, etc.) se ignoran */
            continue;
        }
    }

    if (v_count == 0)
    {
        fprintf(stderr, "parse_obj: no se encontraron vertices 'v'\n");
        fclose(fp);
        return 1;
    }
    if (total_indices == 0)
    {
        fprintf(stderr, "parse_obj: no se encontraron caras 'f'\n");
        fclose(fp);
        return 1;
    }

    /* ---------- reservar ---------- */
    out->vertices = (float *)malloc(v_count * 3 * sizeof(float));
    out->indices = (unsigned *)malloc(total_indices * sizeof(unsigned));
    if (!out->vertices || !out->indices)
    {
        fprintf(stderr, "parse_obj: malloc fallo\n");
        free_mesh(out);
        fclose(fp);
        return 1;
    }
    out->v_count = v_count;
    out->i_count = total_indices;

    /* ---------- pasada 2: llenar ---------- */
    rewind(fp);
    {
        size_t v_idx = 0;
        size_t i_idx = 0;
        while (fgets(line, sizeof(line), fp))
        {
            const char *s = skip_ws(line);
            if (*s == '\0' || *s == '\n' || *s == '#')
                continue;
            if (*s == 'o' && (s[1] == ' ' || s[1] == '\t'))
                continue;
            if (*s == 's' && (s[1] == ' ' || s[1] == '\t' || s[1] == '\n'))
                continue;
            if (s[0] == 'v' && s[1] == ' ')
            {
                float x, y, z;
                sscanf(s, "v %f %f %f", &x, &y, &z);
                out->vertices[v_idx * 3 + 0] = x;
                out->vertices[v_idx * 3 + 1] = y;
                out->vertices[v_idx * 3 + 2] = z;
                v_idx++;
            }
            else if (s[0] == 'v' && (s[1] == 't' || s[1] == 'n'))
                continue;
            else if (s[0] == 'f' && s[1] == ' ')
            {
                int tmp[64];
                int n = 0;
                const char *p = s + 2;
                p = skip_ws(p);
                while (*p && *p != '\n' && *p != '#' && n < 64)
                {
                    char *end;
                    long v = strtol(p, &end, 10);
                    if (end == p)
                        break;
                    tmp[n++] = (int)v;
                    p = end;
                    while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n')
                        p++;
                    p = skip_ws(p);
                }
                if (n >= 64)
                {
                    fprintf(stderr, "parse_obj: cara con demasiados vertices (>64): %s", line);
                    free_mesh(out);
                    fclose(fp);
                    return 1;
                }
                {
                    int k = 0;
                    while (k < n)
                    {
                        if (tmp[k] < 1 || (size_t)tmp[k] > v_count)
                        {
                            fprintf(stderr, "parse_obj: indice fuera de rango en: %s", line);
                            free_mesh(out);
                            fclose(fp);
                            return 1;
                        }
                        k++;
                    }
                }
                if (triangulate_face(out->vertices, tmp, n, out->indices,
                                     &i_idx) != 0)
                {
                    fprintf(stderr, "parse_obj: no se pudo triangular la cara: %s", line);
                    free_mesh(out);
                    fclose(fp);
                    return 1;
                }
            }
        }
    }

    fclose(fp);
    return 0;
}

void center_mesh(t_mesh *mesh)
{
    float min_x;
    float min_y;
    float min_z;
    float max_x;
    float max_y;
    float max_z;
    float center_x;
    float center_y;
    float center_z;
    size_t i;

    if (!mesh || !mesh->vertices || mesh->v_count == 0)
        return;
    min_x = mesh->vertices[0];
    max_x = min_x;
    min_y = mesh->vertices[1];
    max_y = min_y;
    min_z = mesh->vertices[2];
    max_z = min_z;
    i = 1;
    while (i < mesh->v_count)
    {
        const float x = mesh->vertices[i * 3 + 0];
        const float y = mesh->vertices[i * 3 + 1];
        const float z = mesh->vertices[i * 3 + 2];
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
        if (z < min_z) min_z = z;
        if (z > max_z) max_z = z;
        i++;
    }
    center_x = (min_x + max_x) * 0.5f;
    center_y = (min_y + max_y) * 0.5f;
    center_z = (min_z + max_z) * 0.5f;
    i = 0;
    while (i < mesh->v_count)
    {
        mesh->vertices[i * 3 + 0] -= center_x;
        mesh->vertices[i * 3 + 1] -= center_y;
        mesh->vertices[i * 3 + 2] -= center_z;
        i++;
    }
}
