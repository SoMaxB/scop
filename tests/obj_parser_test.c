#include "obj_parser.h"

#include <assert.h>

static void assert_valid_indices(const t_mesh *mesh)
{
    size_t i;

    assert(mesh->i_count % 3 == 0);
    i = 0;
    while (i < mesh->i_count)
    {
        assert(mesh->indices[i] < mesh->v_count);
        i++;
    }
}

int main(void)
{
    t_mesh mesh = {0};

    assert(parse_obj("resources/concave.obj", &mesh) == 0);
    assert(mesh.v_count == 5);
    assert(mesh.i_count == 9);
    assert_valid_indices(&mesh);

    /* Ear clipping must avoid the invalid diagonal 1-3 of the concave face. */
    assert(mesh.indices[0] == 1);
    assert(mesh.indices[1] == 2);
    assert(mesh.indices[2] == 3);
    assert(mesh.indices[3] == 0);
    assert(mesh.indices[4] == 1);
    assert(mesh.indices[5] == 3);
    assert(mesh.indices[6] == 0);
    assert(mesh.indices[7] == 3);
    assert(mesh.indices[8] == 4);

    assert(mesh.vertices[0] == 0.0f);
    assert(mesh.vertices[1] == 0.0f);
    assert(mesh.vertices[2] == 0.0f);
    free_mesh(&mesh);
    assert(mesh.vertices == NULL);
    assert(mesh.indices == NULL);
    assert(mesh.v_count == 0);
    assert(mesh.i_count == 0);
    return 0;
}
