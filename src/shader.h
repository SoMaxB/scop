#ifndef SHADER_H
#define SHADER_H

#include "gl_loader.h"

int create_model_program(GLuint *program, GLint *mvp_location,
                         GLint *texture_location, GLint *factor_location,
                         GLint *gray_location, GLint *normals_location);
int create_hud_program(GLuint *program, GLint *screen_location);

#endif
