#ifndef MAT4_H
#define MAT4_H

/*
 * Matrices 4x4 propias de Scop.
 *
 * Convencion elegida:
 * - vectores columna: M * v
 * - almacenamiento column-major: m[col * 4 + row]
 *
 * Esta es la convencion que espera GLSL cuando enviamos la matriz con
 * glUniformMatrix4fv(..., GL_FALSE, ...).
 */
typedef struct s_mat4
{
    float m[16];
} t_mat4;

t_mat4 mat4_identity(void);
t_mat4 mat4_translate(float x, float y, float z);
t_mat4 mat4_scale(float x, float y, float z);
t_mat4 mat4_rotate_x(float angle);
t_mat4 mat4_rotate_y(float angle);
t_mat4 mat4_rotate_z(float angle);
t_mat4 mat4_perspective(float fov, float aspect, float near_plane,
                        float far_plane);
t_mat4 mat4_mul(t_mat4 a, t_mat4 b);

#endif
