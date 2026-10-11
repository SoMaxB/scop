#ifndef GL_LOADER_H
#define GL_LOADER_H

#include <stddef.h>
#include <stdint.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

typedef uint32_t   GLenum;
typedef uint32_t   GLbitfield;
typedef uint32_t   GLuint;
typedef int32_t    GLint;
typedef int32_t    GLsizei;
typedef uint8_t    GLboolean;
typedef uint8_t    GLubyte;
typedef float      GLfloat;
typedef char       GLchar;
typedef ptrdiff_t  GLsizeiptr;

enum {
    GL_COLOR_BUFFER_BIT      = 0x00004000,
    GL_DEPTH_BUFFER_BIT      = 0x00000100,
    GL_VERSION               = 0x1F02,
    GL_FLOAT                 = 0x1406,
    GL_FALSE                 = 0,
    GL_TRUE                  = 1,
    GL_VERTEX_SHADER         = 0x8B31,
    GL_FRAGMENT_SHADER       = 0x8B30,
    GL_COMPILE_STATUS        = 0x8B81,
    GL_LINK_STATUS           = 0x8B82,
    GL_ARRAY_BUFFER          = 0x8892,
    GL_ELEMENT_ARRAY_BUFFER  = 0x8893,
    GL_STATIC_DRAW           = 0x88E4,
    GL_DYNAMIC_DRAW          = 0x88E8,
    GL_TRIANGLES             = 0x0004,
    GL_UNSIGNED_INT          = 0x1405,
    GL_DEPTH_TEST            = 0x0B71,
    GL_FLOAT_MAT4             = 0x8B5C,
    GL_TEXTURE_2D             = 0x0DE1,
    GL_TEXTURE0                = 0x84C0,
    GL_RGB                    = 0x1907,
    GL_UNSIGNED_BYTE          = 0x1401,
    GL_TEXTURE_MIN_FILTER     = 0x2801,
    GL_TEXTURE_MAG_FILTER     = 0x2800,
    GL_LINEAR                 = 0x2601,
    GL_FRONT_AND_BACK         = 0x0408,
    GL_LINE                   = 0x1B01,
    GL_FILL                   = 0x1B02,
};

void init_gl_functions(void);

extern void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
extern void (*glClear)(GLbitfield);
extern const GLubyte *(*glGetString)(GLenum);


extern GLuint (*glCreateShader)(GLenum);
extern void (*glShaderSource)(GLuint, GLsizei, const GLchar **, const GLint *);
extern void (*glCompileShader)(GLuint);
extern void (*glGetShaderiv)(GLuint, GLenum, GLint *);
extern void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
extern GLuint (*glCreateProgram)(void);
extern void (*glAttachShader)(GLuint, GLuint);
extern void (*glLinkProgram)(GLuint);
extern void (*glGetProgramiv)(GLuint, GLenum, GLint *);
extern void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
extern void (*glDeleteShader)(GLuint);
extern void (*glDeleteProgram)(GLuint);
extern void (*glUseProgram)(GLuint);
extern GLint (*glGetUniformLocation)(GLuint, const GLchar *);
extern void (*glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *);
extern void (*glUniform1i)(GLint, GLint);
extern void (*glUniform1f)(GLint, GLfloat);
extern void (*glUniform2f)(GLint, GLfloat, GLfloat);
extern void (*glGenVertexArrays)(GLsizei, GLuint *);
extern void (*glBindVertexArray)(GLuint);
extern void (*glGenBuffers)(GLsizei, GLuint *);
extern void (*glBindBuffer)(GLenum, GLuint);
extern void (*glBufferData)(GLenum, GLsizeiptr, const void *, GLenum);
extern void (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
extern void (*glEnableVertexAttribArray)(GLuint);
extern void (*glDrawArrays)(GLenum, GLint, GLsizei);
extern void (*glDrawElements)(GLenum, GLsizei, GLenum, const void *);
extern void (*glEnable)(GLenum);
extern void (*glDisable)(GLenum);
extern void (*glViewport)(GLint, GLint, GLsizei, GLsizei);
extern void (*glPolygonMode)(GLenum, GLenum);
extern void (*glDeleteVertexArrays)(GLsizei, const GLuint *);
extern void (*glDeleteBuffers)(GLsizei, const GLuint *);
extern void (*glGenTextures)(GLsizei, GLuint *);
extern void (*glBindTexture)(GLenum, GLuint);
extern void (*glTexParameteri)(GLenum, GLenum, GLint);
extern void (*glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint,
                            GLenum, GLenum, const void *);
extern void (*glActiveTexture)(GLenum);
extern void (*glDeleteTextures)(GLsizei, const GLuint *);

#endif
