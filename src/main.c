#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "gl_loader.h"
#include "hud.h"
#include "mat4.h"
#include "obj_parser.h"
#include "shader.h"
#include "texture.h"

static int g_fb_width = 800;
static int g_fb_height = 600;

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    g_fb_width = width;
    g_fb_height = height;
    glViewport(0, 0, width, height);
}

static float key_axis(GLFWwindow *window, int positive_key, int negative_key)
{
    float axis = 0.0f;

    if (glfwGetKey(window, positive_key) == GLFW_PRESS)
        axis += 1.0f;
    if (glfwGetKey(window, negative_key) == GLFW_PRESS)
        axis -= 1.0f;
    return axis;
}

int main(int argc, char **argv)
{
    const char *obj_path = argc > 1 ? argv[1] : "assets/cube.obj";
    const char *texture_path = argc > 2 ? argv[2] : "assets/texture.ppm";
    const int is_42_logo = strstr(obj_path, "42.obj") != NULL;

    /* --- parseo del .obj (no necesita ventana/GL) --- */
    t_mesh mesh = {0};
    if (parse_obj(obj_path, &mesh) != 0)
    {
        fprintf(stderr, "OBJ parse fallo (ver mensaje arriba)\n");
        return 1;
    }
    printf("OBJ OK: %zu vertices (%zu floats), %zu indices (%zu triangulos)\n",
           mesh.v_count, mesh.v_count * 3, mesh.i_count, mesh.i_count / 3);
    for (size_t i = 0; i < mesh.v_count && i < 8; i++)
        printf("  v%zu: %.2f %.2f %.2f\n", i + 1,
               mesh.vertices[i * 3 + 0], mesh.vertices[i * 3 + 1], mesh.vertices[i * 3 + 2]);
    if (mesh.v_count > 8)
        printf("  ... (%zu vertices mas)\n", mesh.v_count - 8);
    printf("  indices (base 0, fan):");
    for (size_t i = 0; i < mesh.i_count && i < 18; i++)
    {
        if (i % 3 == 0) printf("\n   ");
        printf("%u ", mesh.indices[i]);
    }
    if (mesh.i_count > 18)
        printf("... (%zu indices mas)", mesh.i_count - 18);
    printf("\n");
    fflush(stdout);
    center_mesh(&mesh);

    t_image image;
    if (load_ppm(texture_path, &image) != 0)
    {
        free_mesh(&mesh);
        return 1;
    }

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        free_image(&image);
        free_mesh(&mesh);
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "scop", NULL, NULL);
    if (!window)
    {
        free_image(&image);
        free_mesh(&mesh);
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    init_gl_functions();
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, 800, 600);

    fprintf(stderr, "OpenGL %s (cargador manual OK)\n",
            (const char *)glGetString(GL_VERSION));

    GLuint program = 0;
    GLint mvp_location;
    GLint texture_location;
    GLint factor_location;
    GLint gray_location;
    GLint normals_location;
    if (create_model_program(&program, &mvp_location, &texture_location,
                             &factor_location, &gray_location,
                             &normals_location) != 0)
    {
        free_image(&image);
        free_mesh(&mesh);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    /* --- buffers desde el mesh parseado (reemplaza el triangulo hardcodeado) --- */
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(mesh.v_count * 3 * sizeof(float)),
                 mesh.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(mesh.i_count * sizeof(unsigned)),
                 mesh.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    /* el VAO ya recuerda el EBO; desbindear VAO para no pisarlo accidentalmente */
    /* Nota: no hacer glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) mientras el VAO esté bindeado */

    glEnable(GL_DEPTH_TEST);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image.pixels);
    free_image(&image);

    GLuint hud_program = 0;
    GLuint hud_vao;
    GLuint hud_vbo;
    GLint hud_screen_location;
    t_hud_vertex *hud_vertices;

    if (create_hud_program(&hud_program, &hud_screen_location) != 0)
    {
        glDeleteProgram(program);
        glDeleteTextures(1, &texture);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        free_mesh(&mesh);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    hud_vertices = malloc(65536 * sizeof(*hud_vertices));
    if (hud_screen_location < 0 || !hud_vertices)
    {
        free(hud_vertices);
        glDeleteProgram(hud_program);
        glDeleteProgram(program);
        glDeleteTextures(1, &texture);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        free_mesh(&mesh);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glGenVertexArrays(1, &hud_vao);
    glBindVertexArray(hud_vao);
    glGenBuffers(1, &hud_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, hud_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float texture_factor = 0.0f;
    int texture_target = 0;
    int texture_key_was_down = 0;
    int wireframe = 0;
    int normals = 0;
    int debug_key_was_down = 0;
    int hud_visible = 1;
    int hud_key_was_down = 0;
    double previous_time = glfwGetTime();
    float rotation_x = 0.0f;
    float rotation_y = 0.0f;
    float rotation_z = 0.0f;
    float translation_x = 0.0f;
    float translation_y = 0.0f;
    float translation_z = 0.0f;

    /* --- render loop --- */
    while (!glfwWindowShouldClose(window))
    {
        /*
         * Model = T * R: la rotacion actua primero y la traslacion despues.
         * El cubo gira sobre su origen y no orbita alrededor del origen mundial.
         */
        const double current_time = glfwGetTime();
        const float delta_time = (float)(current_time - previous_time);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
        rotation_x += key_axis(window, GLFW_KEY_UP, GLFW_KEY_DOWN)
            * delta_time * 1.8f;
        rotation_y += key_axis(window, GLFW_KEY_RIGHT, GLFW_KEY_LEFT)
            * delta_time * 1.8f;
        rotation_z += key_axis(window, GLFW_KEY_E, GLFW_KEY_Q)
            * delta_time * 1.8f;
        translation_x += key_axis(window, GLFW_KEY_D, GLFW_KEY_A)
            * delta_time * 1.5f;
        translation_y += key_axis(window, GLFW_KEY_W, GLFW_KEY_S)
            * delta_time * 1.5f;
        translation_z += key_axis(window, GLFW_KEY_R, GLFW_KEY_F)
            * delta_time * 1.5f;
        if (is_42_logo)
            rotation_y += delta_time * 0.8f;
        const int texture_key_down = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
        if (texture_key_down && !texture_key_was_down)
            texture_target = !texture_target;
        texture_key_was_down = texture_key_down;
        const int debug_key_down = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
        if (debug_key_down && !debug_key_was_down)
            wireframe = !wireframe;
        debug_key_was_down = debug_key_down;
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
            normals = 1;
        else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
            normals = 0;
        {
            const int hud_key_down = glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS;
            if (hud_key_down && !hud_key_was_down)
                hud_visible = !hud_visible;
            hud_key_was_down = hud_key_down;
        }
        previous_time = current_time;
        if (texture_target && texture_factor < 1.0f)
            texture_factor += delta_time * 2.0f;
        if (!texture_target && texture_factor > 0.0f)
            texture_factor -= delta_time * 2.0f;
        if (texture_factor < 0.0f)
            texture_factor = 0.0f;
        if (texture_factor > 1.0f)
            texture_factor = 1.0f;
        const t_mat4 rotate_x = mat4_rotate_x(rotation_x);
        const t_mat4 rotate_y = mat4_rotate_y(rotation_y);
        const t_mat4 rotate_z = mat4_rotate_z(rotation_z);
        const t_mat4 rotation = mat4_mul(rotate_z,
            mat4_mul(rotate_y, rotate_x));
        const t_mat4 model = mat4_mul(
            mat4_translate(translation_x, translation_y, translation_z),
            rotation);
        /* La camara esta en z=5, por eso el mundo se mueve a z=-5. */
        const t_mat4 view = mat4_translate(0.0f, 0.0f, -5.0f);
        const int framebuffer_height = g_fb_height > 0 ? g_fb_height : 1;
        const float aspect = (float)g_fb_width / (float)framebuffer_height;
        const t_mat4 projection = mat4_perspective(
            60.0f * 3.1415926535f / 180.0f, aspect, 0.1f, 100.0f);
        const t_mat4 mvp = mat4_mul(projection, mat4_mul(view, model));

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, mvp.m);
        glUniform1f(factor_location, texture_factor);
        glUniform1i(gray_location, is_42_logo);
        glUniform1i(normals_location, normals);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(texture_location, 0);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.i_count, GL_UNSIGNED_INT, (void *)0);

        if (hud_visible)
        {
             const size_t hud_count = build_hud(hud_vertices, 65536,
                                                g_fb_width, g_fb_height,
                                                texture_target, wireframe, normals,
                                                mesh.v_count, mesh.i_count);
            glDisable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glUseProgram(hud_program);
            glUniform2f(hud_screen_location, (float)g_fb_width,
                        (float)g_fb_height);
            glBindVertexArray(hud_vao);
            glBindBuffer(GL_ARRAY_BUFFER, hud_vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         (GLsizeiptr)(hud_count * sizeof(*hud_vertices)),
                         hud_vertices, GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)hud_count);
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture);
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &hud_vao);
    glDeleteBuffers(1, &hud_vbo);
    glDeleteProgram(hud_program);
    free(hud_vertices);
    free_mesh(&mesh);
    glfwTerminate();
    return 0;
}
