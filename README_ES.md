*Este proyecto ha sido creado como parte del plan de estudios de 42 por SoMaxB.*

# Scop

**[English](README.md) | Espa&ntilde;ol**

## Descripcion

Scop es un pequeno renderer OpenGL escrito en C. Interpreta archivos Wavefront
`.obj` sin usar una libreria de carga de objetos, sube la malla a OpenGL y la
renderiza en perspectiva.

El renderer ofrece actualmente:

- Parseo manual de OBJ y triangulacion ear clipping para caras poligonales.
- Matrices 4x4 propias para modelo, vista y proyeccion en perspectiva.
- Carga manual de funciones OpenGL mediante GLFW.
- Colores por triangulo usando `gl_PrimitiveID`.
- Cargador PPM implementado dentro del proyecto.
- Triplanar mapping ponderado por normales para reducir el stretching.
- Transicion suave entre color y textura con la tecla `T`.
- Centrado automatico de modelos mediante su bounding box.
- Rotacion y traslacion en los tres ejes.
- Demostracion del logo 42 en gris y con rotacion automatica.
- Modo wireframe con `G` y visualizacion de normales con `N`/`B`.

El modelo 42 se incluye unicamente con el fin pedagogico exigido por el
subject.

## Requisitos

El proyecto necesita un compilador de C, Make, `pkg-config` y GLFW 3. En
Debian o Ubuntu, las dependencias se pueden comprobar e instalar con:

```sh
pkg-config --exists glfw3 || sudo apt-get install build-essential pkg-config libglfw3-dev
```

Las librerias externas se usan unicamente para la ventana, el contexto y los
eventos. El parseo OBJ, las matrices, la configuracion de shaders y la carga
PPM estan implementados dentro del proyecto.

## Instrucciones

Compilar el renderer y ejecutar los tests automaticos:

```sh
make
make test
```

Ejecutar el cubo por defecto:

```sh
./scop
```

Ejecutar otros OBJ sin recompilar. El segundo argumento es opcional; si se
omite, se carga `assets/texture.ppm`:

```sh
./scop assets/cube.obj
./scop resources/42logo/42.obj
./scop resources/dragon/dragon.obj
./scop resources/pyramid.obj
./scop resources/skull/skull.obj
./scop resources/teapot/teapot.obj
./scop resources/teapot/teapot2.obj
./scop resources/ninjaTurtle/ninjaTurtle.obj
./scop resources/miniTurtle/miniTurtle.obj
./scop resources/xenomorprh.obj
./scop resources/concave.obj
./scop resources/42logo/42.obj assets/texture.ppm
```

El parser OBJ usa actualmente posiciones de vertices e indices de caras. Los
metadatos opcionales del OBJ, como `mtllib`, `usemtl`, `vt` y `vn`, se aceptan
y se ignoran de forma intencionada. El renderer usa su propio cargador PPM,
colores generados por triangulo y coordenadas triplanares. Los archivos `.mtl`
incluidos con algunos modelos referencian imagenes JPG o PNG que no forman
parte de este proyecto.

### Controles

| Tecla | Accion |
|---|---|
| Arriba / Abajo | Rotar alrededor de X |
| Izquierda / Derecha | Rotar alrededor de Y |
| Q / E | Rotar alrededor de Z |
| A / D | Trasladar en X |
| S / W | Trasladar en Y |
| F / R | Trasladar en Z |
| T | Alternar textura con transicion suave |
| G | Alternar modo wireframe |
| N / B | Activar / desactivar visualizacion de normales |
| H | Mostrar / ocultar el HUD de controles |
| ESC | Salir |

Limpiar los archivos generados:

```sh
make fclean
```

## Estructura del proyecto

```text
assets/cube.obj       modelo de prueba por defecto
assets/texture.ppm    textura cargada sin libreria externa de imagenes
resources/42logo/42.obj
                      logo 42 para la demostracion de defensa
resources/concave.obj
                      fixture de triangulacion de una cara concava
resources/*           modelos OBJ adicionales y materiales de origen
src/gl_loader.*       declaraciones y carga manual de funciones OpenGL
src/obj_parser.*      parseo OBJ, triangulacion y centrado de modelos
src/mat4.*            operaciones propias de matrices 4x4
src/texture.*         carga PPM y propiedad de imagenes
src/shader.*          fuentes GLSL, compilacion y enlace de programas
src/hud.*             fuente bitmap y geometria del HUD
src/main.c            ventana, buffers, controles y render loop
tests/mat4_test.c     comprobaciones de matrices
tests/obj_parser_test.c
                      comprobaciones del parser y de la triangulacion concava
```

## Recursos

- Especificacion del perfil OpenGL 3.3 Core:
  <https://registry.khronos.org/OpenGL/specs/gl/glspec33.core.pdf>
- Documentacion de GLFW: <https://www.glfw.org/documentation.html>
- Especificacion GLSL 3.30:
  <https://registry.khronos.org/OpenGL/specs/GLSLangSpec.3.30.pdf>
- Referencias del formato Wavefront OBJ para `v`, `f`, indices y triangulacion.
- Referencia del formato Netpbm PPM P3/P6.

### Uso de IA

La IA se uso como asistente de ensenanza e implementacion, no como sustituto
de las decisiones tecnicas del proyecto. Ayudo a:

- Revisar los requisitos del subject y convertirlos en una implementacion por
  etapas.
- Explicar la creacion del contexto OpenGL, la carga manual de funciones,
  buffers, shaders, matrices, texturas y entrada de teclado.
- Sugerir tests concretos y casos limite para el parser OBJ y las matrices.
- Redactar notas educativas y paginas bilingues despues de compilar y revisar
  la implementacion.

Las decisiones finales siguen siendo explicitas y revisables en el codigo:
GLFW es la unica libreria externa de ejecucion, las matrices y el parser OBJ
son locales y el cargador de texturas lee PPM directamente. Las caras
poligonales se triangulan mediante ear clipping sobre el plano de proyeccion
dominante; esto se prueba con `resources/concave.obj` y las teteras incluidas.
Los poligonos muy degenerados pueden requerir una herramienta especifica de
modelado.

## Estado de validacion

Se han completado manualmente las siguientes comprobaciones con los modelos
incluidos:

- Todos los OBJ de `assets/` y `resources/` cargan en el renderer.
- `resources/concave.obj` se renderiza correctamente tras la triangulacion
  ear clipping.
- La textura PPM comun carga en los modelos probados.
- El logo 42 aparece centrado, gris y girando sobre su eje central.
- Los controles de rotacion y traslacion funcionan en los tres ejes.
- La transicion de textura, wireframe, normales, HUD y salida funcionan de
  forma interactiva.

Los tests automaticos se ejecutan con:

```sh
make test
```

Esto ejecuta `tests/mat4_test.c` para las operaciones de matrices y
`tests/obj_parser_test.c` para el fixture OBJ concavo. Son comprobaciones de
unidad y no sustituyen la demostracion interactiva del renderer.

## Auditoria de la parte obligatoria

| Requisito del subject | Evidencia |
|---|---|
| Mostrar un archivo OBJ | `src/obj_parser.c`, `./scop resources/42logo/42.obj` |
| Perspectiva | `src/mat4.c`, `mat4_perspective`, `P * V * M` |
| Rotar y trasladar en tres ejes | Controles de teclado en `src/main.c` |
| Caras distinguibles | `gl_PrimitiveID` en el fragment shader |
| Alternar textura con transicion suave | `u_texture_factor`, `mix`, `delta_time`, `T` |
| Logo 42 centrado y gris | `center_mesh`, `u_gray`, `resources/42logo/42.obj` |
| Dependencias | `Makefile`, GLFW solo para ventana/contexto/eventos |
| README | Este archivo y `README.md`, incluyendo recursos y uso de IA |

## Estado de los bonus

| Bonus | Estado | Evidencia |
|---|---|---|
| OBJ complejos | Implementado y probado | Ear clipping, proyeccion dominante y normal de Newell en `src/obj_parser.c`; `resources/concave.obj` y teteras |
| Reducir el stretching de textura | Implementado y probado | Triplanar mapping ponderado por normales en el fragment shader |
| Funcionalidad adicional | Implementado y probado | Wireframe con `G` y visualizacion de normales con `N`/`B` |

El primer bonus esta implementado en `src/obj_parser.c`: las caras se
triangulan con ear clipping despues de elegir el eje de proyeccion dominante
a partir de una normal de Newell. Las teteras original y normalizada, junto al
fixture concavo, se usan como smoke tests.

El triplanar mapping usa tres muestras de textura por fragmento y se documenta
como una tecnica practica para reducir el stretching, no como sustituto de un
desplegado UV creado por un artista.

La implementacion obligatoria sigue siendo la prioridad. Los bonus deben
demostrarse durante la defensa y no deben ocultar ningun fallo de la parte
obligatoria.
