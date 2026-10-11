#include "mat4.h"

#include <assert.h>
#include <math.h>

static int almost_equal(float a, float b)
{
    return fabsf(a - b) < 0.0001f;
}

static void assert_point(t_mat4 matrix, float x, float y, float z)
{
    /* Multiplicamos manualmente M * (x,y,z,1) para inspeccionar el resultado. */
    const float out_x = matrix.m[0] * x + matrix.m[4] * y
        + matrix.m[8] * z + matrix.m[12];
    const float out_y = matrix.m[1] * x + matrix.m[5] * y
        + matrix.m[9] * z + matrix.m[13];
    const float out_z = matrix.m[2] * x + matrix.m[6] * y
        + matrix.m[10] * z + matrix.m[14];

    assert(almost_equal(out_x, x));
    assert(almost_equal(out_y, y));
    assert(almost_equal(out_z, z));
}

int main(void)
{
    const t_mat4 identity = mat4_identity();
    const t_mat4 translation = mat4_translate(3.0f, -1.0f, 2.0f);
    const t_mat4 rotation = mat4_rotate_z(3.1415926535f * 0.5f);
    const t_mat4 model = mat4_mul(translation, rotation);
    const t_mat4 projection = mat4_perspective(
        3.1415926535f / 3.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    assert(identity.m[0] == 1.0f);
    assert(identity.m[5] == 1.0f);
    assert(identity.m[10] == 1.0f);
    assert(identity.m[15] == 1.0f);

    /* La identidad deja el punto igual; la prueba detecta indices mezclados. */
    assert_point(identity, 1.0f, 2.0f, 3.0f);

    /* La traslacion se almacena en la ultima columna. */
    assert(almost_equal(translation.m[12], 3.0f));
    assert(almost_equal(translation.m[13], -1.0f));
    assert(almost_equal(translation.m[14], 2.0f));

    /* Rz(90) transforma (1,0,0) en (0,1,0); T lo deja en (3,0,2). */
    {
        const float x = model.m[0] * 1.0f + model.m[4] * 0.0f
            + model.m[12];
        const float y = model.m[1] * 1.0f + model.m[5] * 0.0f
            + model.m[13];
        const float z = model.m[2] * 1.0f + model.m[6] * 0.0f
            + model.m[10] * 0.0f + model.m[14];

        assert(almost_equal(x, 3.0f));
        assert(almost_equal(y, 0.0f));
        assert(almost_equal(z, 2.0f));
    }

    /* En perspectiva, la matriz necesita producir w dependiente de z. */
    assert(almost_equal(projection.m[11], -1.0f));
    assert(projection.m[14] < 0.0f);
    return 0;
}
