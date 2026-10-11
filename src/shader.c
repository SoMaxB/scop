#include "shader.h"

#include <stdio.h>

static int compile_shader(GLenum type, const char *source, GLuint *shader)
{
    GLint compiled;
    char log[1024];
    GLsizei log_length;

    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);
    if (compiled)
        return 0;
    glGetShaderInfoLog(*shader, sizeof(log), &log_length, log);
    fprintf(stderr, "Shader compilation failed: %.*s\n",
            (int)log_length, log);
    glDeleteShader(*shader);
    *shader = 0;
    return 1;
}

static int link_program(GLuint vertex_shader, GLuint fragment_shader,
                        GLuint *program)
{
    GLint linked;
    char log[1024];
    GLsizei log_length;

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader);
    glAttachShader(*program, fragment_shader);
    glLinkProgram(*program);
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (linked)
        return 0;
    glGetProgramInfoLog(*program, sizeof(log), &log_length, log);
    fprintf(stderr, "Shader link failed: %.*s\n", (int)log_length, log);
    glDeleteProgram(*program);
    *program = 0;
    return 1;
}

static int make_program(const char *vertex_source, const char *fragment_source,
                        GLuint *program)
{
    GLuint vertex_shader;
    GLuint fragment_shader;

    if (compile_shader(GL_VERTEX_SHADER, vertex_source, &vertex_shader) != 0)
        return 1;
    if (compile_shader(GL_FRAGMENT_SHADER, fragment_source, &fragment_shader) != 0)
    {
        glDeleteShader(vertex_shader);
        return 1;
    }
    if (link_program(vertex_shader, fragment_shader, program) != 0)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 1;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return 0;
}

int create_model_program(GLuint *program, GLint *mvp_location,
                         GLint *texture_location, GLint *factor_location,
                         GLint *gray_location, GLint *normals_location)
{
    static const char *vertex_source =
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPos;\n"
        "uniform mat4 u_mvp;\n"
        "out vec3 v_position;\n"
        "void main() {\n"
        "    gl_Position = u_mvp * vec4(aPos, 1.0);\n"
        "    v_position = aPos;\n"
        "}\n";
    static const char *fragment_source =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 v_position;\n"
        "uniform sampler2D u_texture;\n"
        "uniform float u_texture_factor;\n"
        "uniform int u_gray;\n"
        "uniform int u_debug_normals;\n"
        "void main() {\n"
        "    /* gl_PrimitiveID is constant for every fragment of one triangle. */\n"
        "    float id = float(gl_PrimitiveID);\n"
        "    vec3 color = vec3(0.55 + 0.35 * sin(id * 1.7),\n"
        "                     0.55 + 0.35 * sin(id * 1.7 + 2.1),\n"
        "                     0.55 + 0.35 * sin(id * 1.7 + 4.2));\n"
        "    if (u_gray != 0) {\n"
        "        float shade = 0.25 + 0.06 * mod(id, 8.0);\n"
        "        color = vec3(shade);\n"
        "    }\n"
        "    vec3 normal = normalize(cross(dFdx(v_position), dFdy(v_position)));\n"
        "    vec3 weights = pow(abs(normal), vec3(4.0));\n"
        "    weights /= weights.x + weights.y + weights.z;\n"
        "    vec3 xy = texture(u_texture, v_position.xy * 0.45).rgb;\n"
        "    vec3 xz = texture(u_texture, v_position.xz * 0.45).rgb;\n"
        "    vec3 yz = texture(u_texture, v_position.yz * 0.45).rgb;\n"
        "    vec3 texture_color = xy * weights.z + xz * weights.y\n"
        "                       + yz * weights.x;\n"
        "    vec3 result = mix(color, texture_color, u_texture_factor);\n"
        "    if (u_debug_normals != 0)\n"
        "        result = normal * 0.5 + 0.5;\n"
        "    FragColor = vec4(result, 1.0);\n"
        "}\n";

    if (make_program(vertex_source, fragment_source, program) != 0)
        return 1;
    *mvp_location = glGetUniformLocation(*program, "u_mvp");
    *texture_location = glGetUniformLocation(*program, "u_texture");
    *factor_location = glGetUniformLocation(*program, "u_texture_factor");
    *gray_location = glGetUniformLocation(*program, "u_gray");
    *normals_location = glGetUniformLocation(*program, "u_debug_normals");
    if (*mvp_location < 0 || *texture_location < 0 || *factor_location < 0
        || *gray_location < 0 || *normals_location < 0)
    {
        glDeleteProgram(*program);
        *program = 0;
        return 1;
    }
    return 0;
}

int create_hud_program(GLuint *program, GLint *screen_location)
{
    static const char *vertex_source =
        "#version 330 core\n"
        "layout(location = 0) in vec2 aPos;\n"
        "layout(location = 1) in vec3 aColor;\n"
        "uniform vec2 u_screen;\n"
        "out vec3 v_color;\n"
        "void main() {\n"
        "    float x = aPos.x / u_screen.x * 2.0 - 1.0;\n"
        "    float y = 1.0 - aPos.y / u_screen.y * 2.0;\n"
        "    gl_Position = vec4(x, y, 0.0, 1.0);\n"
        "    v_color = aColor;\n"
        "}\n";
    static const char *fragment_source =
        "#version 330 core\n"
        "in vec3 v_color;\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(v_color, 1.0); }\n";

    if (make_program(vertex_source, fragment_source, program) != 0)
        return 1;
    *screen_location = glGetUniformLocation(*program, "u_screen");
    if (*screen_location < 0)
    {
        glDeleteProgram(*program);
        *program = 0;
        return 1;
    }
    return 0;
}
