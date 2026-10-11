#include "mat4.h"

#include <math.h>

/*
 * Como guardamos m[col * 4 + row], estos son los indices de la diagonal:
 * m[0] = fila 0, columna 0; m[5] = 1,1; m[10] = 2,2; m[15] = 3,3.
 */
t_mat4 mat4_identity(void)
{
    t_mat4 out = {0};

    out.m[0] = 1.0f;
    out.m[5] = 1.0f;
    out.m[10] = 1.0f;
    out.m[15] = 1.0f;
    return out;
}

/*
 * Para un punto homogeneo (x,y,z,1), esta matriz produce:
 * (x + tx, y + ty, z + tz, 1).
 *
 * La traslacion esta en la ultima columna: indices 12, 13 y 14.
 */
t_mat4 mat4_translate(float x, float y, float z)
{
    t_mat4 out = mat4_identity();

    out.m[12] = x;
    out.m[13] = y;
    out.m[14] = z;
    return out;
}

/* La escala multiplica cada coordenada por su factor y conserva w. */
t_mat4 mat4_scale(float x, float y, float z)
{
    t_mat4 out = {0};

    out.m[0] = x;
    out.m[5] = y;
    out.m[10] = z;
    out.m[15] = 1.0f;
    return out;
}

/* Rotacion alrededor de X: el plano que gira es YZ. */
t_mat4 mat4_rotate_x(float angle)
{
    const float c = cosf(angle);
    const float s = sinf(angle);
    t_mat4 out = mat4_identity();

    out.m[5] = c;
    out.m[9] = -s;
    out.m[6] = s;
    out.m[10] = c;
    return out;
}

/* Rotacion alrededor de Y: el plano que gira es XZ. */
t_mat4 mat4_rotate_y(float angle)
{
    const float c = cosf(angle);
    const float s = sinf(angle);
    t_mat4 out = mat4_identity();

    out.m[0] = c;
    out.m[8] = s;
    out.m[2] = -s;
    out.m[10] = c;
    return out;
}

/* Rotacion alrededor de Z: el plano que gira es XY. */
t_mat4 mat4_rotate_z(float angle)
{
    const float c = cosf(angle);
    const float s = sinf(angle);
    t_mat4 out = mat4_identity();

    out.m[0] = c;
    out.m[4] = -s;
    out.m[1] = s;
    out.m[5] = c;
    return out;
}

/*
 * Multiplicacion de matrices: C = A * B.
 *
 * La matriz de la derecha actua primero sobre un vector columna:
 * (A * B) * v == A * (B * v).
 */
t_mat4 mat4_mul(t_mat4 a, t_mat4 b)
{
    t_mat4 out = {0};
    int row;
    int col;
    int k;

    row = 0;
    while (row < 4)
    {
        col = 0;
        while (col < 4)
        {
            k = 0;
            while (k < 4)
            {
                out.m[col * 4 + row] +=
                    a.m[k * 4 + row] * b.m[col * 4 + k];
                k++;
            }
            col++;
        }
        row++;
    }
    return out;
}

/*
 * Proyeccion en perspectiva.
 *
 * fov esta en radianes. Despues del vertex shader, OpenGL divide x, y y z
 * por w. La ultima fila (-1 en la posicion z) hace que w dependa de la
 * profundidad y produce el efecto "lo lejano se ve mas pequeno".
 */
t_mat4 mat4_perspective(float fov, float aspect, float near_plane,
                        float far_plane)
{
    const float f = 1.0f / tanf(fov * 0.5f);
    t_mat4 out = {0};

    out.m[0] = f / aspect;
    out.m[5] = f;
    out.m[10] = (far_plane + near_plane) / (near_plane - far_plane);
    out.m[14] = (2.0f * far_plane * near_plane)
        / (near_plane - far_plane);
    out.m[11] = -1.0f;
    return out;
}
