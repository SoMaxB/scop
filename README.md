*This project has been created as part of the 42 curriculum by SoMaxB.*

# Scop

**English | [Espa&ntilde;ol](README_ES.md)**

## Description

Scop is a compact C/OpenGL renderer that parses Wavefront `.obj` files,
triangulates polygon faces, and displays the resulting meshes in perspective.
It is a focused study of the path from raw geometry to pixels: parsing, custom
matrix math, shader setup, and procedural texturing.

The renderer currently provides:

- Manual OBJ parsing and ear-clipping triangulation for projected polygon faces.
- Custom 4x4 matrices for model, view and perspective projection.
- Manual OpenGL function loading through GLFW.
- Per-triangle colors using `gl_PrimitiveID`.
- A project-owned PPM texture loader.
- Triplanar texture mapping weighted by face normals to reduce stretching.
- Smooth color-to-texture blending with the `T` key.
- Automatic model centering using the bounding box.
- Rotation and translation on all three axes.
- A gray, automatically rotating 42 logo demonstration.
- Wireframe debug mode with `G` and normal debug mode with `N`/`B`.

The 42 model is included only for the pedagogical purpose required by the
subject.

## Requirements

The project needs a C compiler, Make, `pkg-config`, and GLFW 3. On Debian or
Ubuntu, dependencies can be checked and installed with:

```sh
pkg-config --exists glfw3 || sudo apt-get install build-essential pkg-config libglfw3-dev
```

External libraries are used only for window, context and event management.
OBJ parsing, matrix operations, shader setup and PPM loading are implemented
inside the project.

## Instructions

Build the renderer and run the automated tests:

```sh
make
make test
```

Run the default cube:

```sh
./scop
```

Run another OBJ without recompiling. The second argument is optional; when it
is omitted, `assets/texture.ppm` is loaded:

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

The OBJ parser currently uses vertex positions and face indices. Optional OBJ
metadata such as `mtllib`, `usemtl`, `vt` and `vn` is accepted and ignored.
This is intentional: the renderer uses its own PPM loader, generated
per-triangle colors and triplanar coordinates. The `.mtl` files shipped with
some models reference JPG or PNG assets that are not part of this project.

Controls:

| Key | Action |
|---|---|
| Up / Down | Rotate around X |
| Left / Right | Rotate around Y |
| Q / E | Rotate around Z |
| A / D | Translate on X |
| S / W | Translate on Y |
| F / R | Translate on Z |
| T | Toggle the texture with a smooth transition |
| G | Toggle wireframe debug mode |
| N / B | Enable / disable normal visualization |
| H | Show / hide the controls HUD |
| ESC | Exit |

Clean generated files with:

```sh
make fclean
```

## Project Structure

```text
assets/cube.obj       default test model
assets/texture.ppm    texture loaded without an external image library
resources/42logo/42.obj
                      42 logo for the defense demonstration
resources/concave.obj
                      concave-face triangulation fixture
resources/*           additional OBJ test models and source materials
src/gl_loader.*       manual OpenGL function declarations and loading
src/obj_parser.*      OBJ parsing, triangulation and model centering
src/mat4.*            custom 4x4 matrix operations
src/texture.*         PPM loading and image ownership
src/shader.*          GLSL sources, compilation and program linking
src/hud.*             bitmap font and HUD geometry
src/main.c            window, buffers, controls and render loop
tests/mat4_test.c     executable matrix checks
tests/obj_parser_test.c
                      parser and concave triangulation checks
```

## Resources

- OpenGL 3.3 Core Profile specification: <https://registry.khronos.org/OpenGL/specs/gl/glspec33.core.pdf>
- GLFW documentation: <https://www.glfw.org/documentation.html>
- GLSL 3.30 specification: <https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.3.30.pdf>
- Wavefront OBJ format references for `v`, `f`, indices and triangulation.
- Netpbm PPM P3/P6 format reference.

### AI usage

AI was used as a teaching and implementation assistant, not as a replacement
for the project's technical decisions. It helped to:

- Review the subject requirements and turn them into a staged implementation.
- Explain OpenGL context creation, manual function loading, buffers, shaders,
  matrices, textures and input handling.
- Suggest focused tests and edge cases for the OBJ parser and matrix code.
- Draft educational notes and bilingual course pages after the implementation
  was compiled and checked.

The final choices remain explicit and reviewable in the source: GLFW is the
only external runtime library, matrices and OBJ parsing are local code, and
the texture loader reads PPM directly. Polygon faces are triangulated with an
ear-clipping pass on the dominant projection plane; this is tested with
`resources/concave.obj` and the supplied teapot files. Highly degenerate
polygons can still require a dedicated modelling pipeline.

## Validation Status

The following checks have been completed manually with the supplied models:

- Every OBJ under `assets/` and `resources/` loads in the renderer.
- `resources/concave.obj` renders correctly after ear-clipping triangulation.
- The shared PPM texture loads for the tested models.
- The 42 logo renders centered, gray and rotating around its central axis.
- Rotation and translation controls work on all three axes.
- Texture transition, wireframe mode, normal visualization, HUD toggle and
  exit controls work interactively.

The automated checks are:

```sh
make test
```

This runs `tests/mat4_test.c` for matrix operations and
`tests/obj_parser_test.c` for the concave OBJ parser fixture. They are
unit-level checks; they do not replace the interactive renderer demonstration.

## Mandatory Audit

| Subject requirement | Evidence |
|---|---|
| Display an OBJ file | `src/obj_parser.c`, `./scop resources/42logo/42.obj` |
| Perspective | `src/mat4.c`, `mat4_perspective`, `P * V * M` |
| Rotate and translate on three axes | Keyboard controls in `src/main.c` |
| Distinguishable faces | `gl_PrimitiveID` in the fragment shader |
| Toggle texture with smooth transition | `u_texture_factor`, `mix`, `delta_time`, `T` |
| 42 logo centered and gray | `center_mesh`, `u_gray`, `resources/42logo/42.obj` |
| Dependencies | `Makefile`, GLFW only for window/context/events |
| README | This file, including AI usage and resources |

### Bonus Status

| Bonus | Status | Evidence |
|---|---|---|
| Complex OBJ files | Implemented and tested | Ear clipping, dominant projection and Newell normal in `src/obj_parser.c`; `resources/concave.obj` and teapots |
| Reduce texture stretching | Implemented and tested | Triplanar mapping weighted by face normals in the fragment shader |
| Additional feature | Implemented and tested | Wireframe mode with `G` and normal visualization with `N`/`B` |

The first bonus is implemented in `src/obj_parser.c`: faces are triangulated
with ear clipping after choosing the dominant projection axis from a Newell
normal. The original and normalized teapots, plus the concave fixture, are
used as smoke tests.

Triplanar mapping uses three texture samples per fragment and is deliberately
documented as a practical anti-stretching technique, not as a replacement for
artist-authored UV unwrapping.

The mandatory implementation remains the priority. The bonus implementation
must still be demonstrated during the defense and must not be used to hide a
failure in any mandatory row.
