NAME    = scop
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
GLFW    = $(shell pkg-config --cflags --libs glfw3)
SRCS    = src/main.c src/gl_loader.c src/obj_parser.c src/mat4.c \
		  src/texture.c src/hud.c src/shader.c
OBJ     = $(SRCS:.c=.o)
TESTS   = tests/mat4_test tests/obj_parser_test

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(GLFW) -lm
	@echo "✅ Compilación completada: $@"

%.o: %.c src/gl_loader.h src/obj_parser.h src/mat4.h \
		src/texture.h src/hud.h src/shader.h
	$(CC) $(CFLAGS) $(shell pkg-config --cflags glfw3) -c $< -o $@

clean:
	rm -f $(OBJ)
	@echo "🧹 Archivos objeto eliminados"

fclean: clean
	rm -f $(NAME)
	@echo "🗑️ Ejecutable eliminado"

re: fclean all

tests/mat4_test: tests/mat4_test.c src/mat4.c src/mat4.h
	$(CC) $(CFLAGS) -Isrc -o $@ tests/mat4_test.c src/mat4.c -lm

tests/obj_parser_test: tests/obj_parser_test.c src/obj_parser.c \
		src/obj_parser.h
	$(CC) $(CFLAGS) -Isrc -o $@ tests/obj_parser_test.c \
		src/obj_parser.c -lm

test: $(TESTS)
	./tests/mat4_test
	./tests/obj_parser_test
	@echo "🔎 Test compilados"

.PHONY: all clean fclean re test
