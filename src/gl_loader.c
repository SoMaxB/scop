#include <stdio.h>
#include <stdlib.h>
#include "gl_loader.h"

#define GL_LOAD(name)                                                       \
    do {                                                                    \
        name = (void *)glfwGetProcAddress(#name);                           \
        if (name == NULL) {                                                 \
            fprintf(stderr, "GL: %s no disponible (contexto core?)\n", #name); \
            exit(1);                                                        \
        }                                                                   \
    } while (0)

/* Definiciones (almacenamiento real) de los punteros a funcion */
void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
void (*glClear)(GLbitfield);
const GLubyte *(*glGetString)(GLenum);
GLuint (*glCreateShader)(GLenum);
void (*glShaderSource)(GLuint, GLsizei, const GLchar **, const GLint *);
void (*glCompileShader)(GLuint);
void (*glGetShaderiv)(GLuint, GLenum, GLint *);
void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
GLuint (*glCreateProgram)(void);
void (*glAttachShader)(GLuint, GLuint);
void (*glLinkProgram)(GLuint);
void (*glGetProgramiv)(GLuint, GLenum, GLint *);
void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
void (*glDeleteShader)(GLuint);
void (*glDeleteProgram)(GLuint);
void (*glUseProgram)(GLuint);
GLint (*glGetUniformLocation)(GLuint, const GLchar *);
void (*glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *);
void (*glUniform1i)(GLint, GLint);
void (*glUniform1f)(GLint, GLfloat);
void (*glUniform2f)(GLint, GLfloat, GLfloat);
void (*glGenVertexArrays)(GLsizei, GLuint *);
void (*glBindVertexArray)(GLuint);
void (*glGenBuffers)(GLsizei, GLuint *);
void (*glBindBuffer)(GLenum, GLuint);
void (*glBufferData)(GLenum, GLsizeiptr, const void *, GLenum);
void (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
void (*glEnableVertexAttribArray)(GLuint);
void (*glDrawArrays)(GLenum, GLint, GLsizei);
void (*glDrawElements)(GLenum, GLsizei, GLenum, const void *);
void (*glEnable)(GLenum);
void (*glDisable)(GLenum);
void (*glViewport)(GLint, GLint, GLsizei, GLsizei);
void (*glPolygonMode)(GLenum, GLenum);
void (*glDeleteVertexArrays)(GLsizei, const GLuint *);
void (*glDeleteBuffers)(GLsizei, const GLuint *);
void (*glGenTextures)(GLsizei, GLuint *);
void (*glBindTexture)(GLenum, GLuint);
void (*glTexParameteri)(GLenum, GLenum, GLint);
void (*glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint,
                     GLenum, GLenum, const void *);
void (*glActiveTexture)(GLenum);
void (*glDeleteTextures)(GLsizei, const GLuint *);
void init_gl_functions(void)

{
    GL_LOAD(glClearColor);
    GL_LOAD(glClear);
	GL_LOAD(glGetString);
    GL_LOAD(glCreateShader);
    GL_LOAD(glShaderSource);
    GL_LOAD(glCompileShader);
    GL_LOAD(glGetShaderiv);
    GL_LOAD(glGetShaderInfoLog);
    GL_LOAD(glCreateProgram);
    GL_LOAD(glAttachShader);
    GL_LOAD(glLinkProgram);
    GL_LOAD(glGetProgramiv);
    GL_LOAD(glGetProgramInfoLog);
    GL_LOAD(glDeleteShader);
    GL_LOAD(glDeleteProgram);
    GL_LOAD(glUseProgram);
    GL_LOAD(glGetUniformLocation);
    GL_LOAD(glUniformMatrix4fv);
    GL_LOAD(glUniform1i);
    GL_LOAD(glUniform1f);
    GL_LOAD(glUniform2f);
    GL_LOAD(glGenVertexArrays);
    GL_LOAD(glBindVertexArray);
    GL_LOAD(glGenBuffers);
    GL_LOAD(glBindBuffer);
    GL_LOAD(glBufferData);
    GL_LOAD(glVertexAttribPointer);
    GL_LOAD(glEnableVertexAttribArray);
    GL_LOAD(glDrawArrays);
    GL_LOAD(glDrawElements);
    GL_LOAD(glEnable);
    GL_LOAD(glDisable);
    GL_LOAD(glViewport);
    GL_LOAD(glPolygonMode);
    GL_LOAD(glDeleteVertexArrays);
    GL_LOAD(glDeleteBuffers);
    GL_LOAD(glGenTextures);
    GL_LOAD(glBindTexture);
    GL_LOAD(glTexParameteri);
    GL_LOAD(glTexImage2D);
    GL_LOAD(glActiveTexture);
    GL_LOAD(glDeleteTextures);
}
