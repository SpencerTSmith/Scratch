#define COMMON_IMPLEMENTATION
#include "../common.h"

#include "testing.h"
#include "testing.c"

#include "../linear_algebra.h"
#include "../linear_algebra.c"

int main(int argc, char **argv)
{
  Arena arena = arena_make();

  TEST_BLOCK(STR("vec2 construction"))
  {
    vec2 v = vec2(3.0f, 4.0f);
    TEST_EVAL(v.x == 3.0f);
    TEST_EVAL(v.y == 4.0f);
    TEST_EVAL(v.u == 3.0f);
    TEST_EVAL(v.v == 4.0f);
    TEST_EVAL(v.w == 3.0f);
    TEST_EVAL(v.h == 4.0f);
    TEST_EVAL(v.d[0] == 3.0f);
    TEST_EVAL(v.d[1] == 4.0f);
  }

  TEST_BLOCK(STR("vec2_add"))
  {
    vec2 v1 = vec2(1.0f, 2.0f);
    vec2 v2 = vec2(3.0f, 4.0f);
    vec2 result = vec2_add(v1, v2);
    TEST_EVAL(result.x == 4.0f);
    TEST_EVAL(result.y == 6.0f);
  }

  TEST_BLOCK(STR("vec2_sub"))
  {
    vec2 v1 = vec2(5.0f, 7.0f);
    vec2 v2 = vec2(2.0f, 3.0f);
    vec2 result = vec2_sub(v1, v2);
    TEST_EVAL(result.x == 3.0f);
    TEST_EVAL(result.y == 4.0f);
  }

  TEST_BLOCK(STR("vec2_mul"))
  {
    vec2 v = vec2(2.0f, 3.0f);
    vec2 result = vec2_mul(v, 2.0f);
    TEST_EVAL(result.x == 4.0f);
    TEST_EVAL(result.y == 6.0f);
  }

  TEST_BLOCK(STR("vec2_div"))
  {
    vec2 v = vec2(4.0f, 6.0f);
    vec2 result = vec2_div(v, 2.0f);
    TEST_EVAL(result.x == 2.0f);
    TEST_EVAL(result.y == 3.0f);
  }

  TEST_BLOCK(STR("vec2_len"))
  {
    vec2 v = vec2(3.0f, 4.0f);
    f32 len = vec2_len(v);
    TEST_EVAL(EPSILON_EQUAL(len, 5.0f));
  }

  TEST_BLOCK(STR("vec2_dot"))
  {
    vec2 v1 = vec2(1.0f, 2.0f);
    vec2 v2 = vec2(3.0f, 4.0f);
    f32 dot = vec2_dot(v1, v2);
    TEST_EVAL(EPSILON_EQUAL(dot, 11.0f));
  }

  TEST_BLOCK(STR("vec2_norm"))
  {
    vec2 v = vec2(3.0f, 4.0f);
    vec2 normalized = vec2_norm(v);
    TEST_EVAL(EPSILON_EQUAL(normalized.x, 0.6f));
    TEST_EVAL(EPSILON_EQUAL(normalized.y, 0.8f));
    TEST_EVAL(EPSILON_EQUAL(vec2_len(normalized), 1.0f));
  }

  TEST_BLOCK(STR("vec2_norm0"))
  {
    vec2 zero = vec2(0.0f, 0.0f);
    vec2 normalized = vec2_norm0(zero);
    TEST_EVAL(normalized.x == 0.0f);
    TEST_EVAL(normalized.y == 0.0f);
  }

  TEST_BLOCK(STR("vec2_cross"))
  {
    vec2 v1 = vec2(1.0f, 0.0f);
    vec2 v2 = vec2(0.0f, 1.0f);
    f32 cross = vec2_cross(v1, v2);
    TEST_EVAL(EPSILON_EQUAL(cross, 1.0f));
  }

  TEST_BLOCK(STR("vec3 construction"))
  {
    vec3 v = vec3(1.0f, 2.0f, 3.0f);
    TEST_EVAL(v.x == 1.0f);
    TEST_EVAL(v.y == 2.0f);
    TEST_EVAL(v.z == 3.0f);
    TEST_EVAL(v.r == 1.0f);
    TEST_EVAL(v.g == 2.0f);
    TEST_EVAL(v.b == 3.0f);
    TEST_EVAL(v.xy.x == 1.0f);
    TEST_EVAL(v.xy.y == 2.0f);
    TEST_EVAL(v.yz.x == 2.0f);
    TEST_EVAL(v.yz.y == 3.0f);
    TEST_EVAL(v.d[0] == 1.0f);
    TEST_EVAL(v.d[1] == 2.0f);
    TEST_EVAL(v.d[2] == 3.0f);
  }

  TEST_BLOCK(STR("vec3_add"))
  {
    vec3 v1 = vec3(1.0f, 2.0f, 3.0f);
    vec3 v2 = vec3(4.0f, 5.0f, 6.0f);
    vec3 result = vec3_add(v1, v2);
    TEST_EVAL(result.x == 5.0f);
    TEST_EVAL(result.y == 7.0f);
    TEST_EVAL(result.z == 9.0f);
  }

  TEST_BLOCK(STR("vec3_sub"))
  {
    vec3 v1 = vec3(5.0f, 7.0f, 9.0f);
    vec3 v2 = vec3(2.0f, 3.0f, 4.0f);
    vec3 result = vec3_sub(v1, v2);
    TEST_EVAL(result.x == 3.0f);
    TEST_EVAL(result.y == 4.0f);
    TEST_EVAL(result.z == 5.0f);
  }

  TEST_BLOCK(STR("vec3_mul"))
  {
    vec3 v = vec3(2.0f, 3.0f, 4.0f);
    vec3 result = vec3_mul(v, 2.0f);
    TEST_EVAL(result.x == 4.0f);
    TEST_EVAL(result.y == 6.0f);
    TEST_EVAL(result.z == 8.0f);
  }

  TEST_BLOCK(STR("vec3_div"))
  {
    vec3 v = vec3(4.0f, 6.0f, 8.0f);
    vec3 result = vec3_div(v, 2.0f);
    TEST_EVAL(result.x == 2.0f);
    TEST_EVAL(result.y == 3.0f);
    TEST_EVAL(result.z == 4.0f);
  }

  TEST_BLOCK(STR("vec3_inv"))
  {
    vec3 v = vec3(1.0f, 2.0f, 4.0f);
    vec3 result = vec3_inv(v);
    TEST_EVAL(EPSILON_EQUAL(result.x, 1.0f));
    TEST_EVAL(EPSILON_EQUAL(result.y, 0.5f));
    TEST_EVAL(EPSILON_EQUAL(result.z, 0.25f));
  }

  TEST_BLOCK(STR("vec3_len"))
  {
    vec3 v = vec3(2.0f, 3.0f, 6.0f);
    f32 len = vec3_len(v);
    TEST_EVAL(EPSILON_EQUAL(len, 7.0f));
  }

  TEST_BLOCK(STR("vec3_dot"))
  {
    vec3 v1 = vec3(1.0f, 2.0f, 3.0f);
    vec3 v2 = vec3(4.0f, 5.0f, 6.0f);
    f32 dot = vec3_dot(v1, v2);
    TEST_EVAL(EPSILON_EQUAL(dot, 32.0f));
  }

  TEST_BLOCK(STR("vec3_cross"))
  {
    vec3 x_axis = vec3(1.0f, 0.0f, 0.0f);
    vec3 y_axis = vec3(0.0f, 1.0f, 0.0f);
    vec3 result = vec3_cross(x_axis, y_axis);
    TEST_EVAL(VEC3_EPSILON_EQUAL(result, vec3(0.0f, 0.0f, 1.0f)));
  }

  TEST_BLOCK(STR("vec3_norm"))
  {
    vec3 v = vec3(3.0f, 0.0f, 4.0f);
    vec3 normalized = vec3_norm(v);
    TEST_EVAL(EPSILON_EQUAL(normalized.x, 0.6f));
    TEST_EVAL(EPSILON_EQUAL(normalized.y, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(normalized.z, 0.8f));
    TEST_EVAL(EPSILON_EQUAL(vec3_len(normalized), 1.0f));
  }

  TEST_BLOCK(STR("vec3_norm0"))
  {
    vec3 zero = vec3(0.0f, 0.0f, 0.0f);
    vec3 normalized = vec3_norm0(zero);
    TEST_EVAL(VEC3_EPSILON_EQUAL(normalized, zero));
  }

  TEST_BLOCK(STR("vec3_to_vec4"))
  {
    vec3 v3 = vec3(1.0f, 2.0f, 3.0f);
    vec4 v4 = vec3_to_vec4(v3);
    TEST_EVAL(v4.x == 1.0f);
    TEST_EVAL(v4.y == 2.0f);
    TEST_EVAL(v4.z == 3.0f);
    TEST_EVAL(v4.w == 1.0f);
  }

  TEST_BLOCK(STR("vec4 construction"))
  {
    vec4 v = vec4(1.0f, 2.0f, 3.0f, 4.0f);
    TEST_EVAL(v.x == 1.0f);
    TEST_EVAL(v.y == 2.0f);
    TEST_EVAL(v.z == 3.0f);
    TEST_EVAL(v.w == 4.0f);
    TEST_EVAL(v.r == 1.0f);
    TEST_EVAL(v.g == 2.0f);
    TEST_EVAL(v.b == 3.0f);
    TEST_EVAL(v.a == 4.0f);
    TEST_EVAL(v.xyz.x == 1.0f);
    TEST_EVAL(v.xyz.y == 2.0f);
    TEST_EVAL(v.xyz.z == 3.0f);
    TEST_EVAL(v.xy.x == 1.0f);
    TEST_EVAL(v.xy.y == 2.0f);
    TEST_EVAL(v.d[0] == 1.0f);
    TEST_EVAL(v.d[3] == 4.0f);
  }

  TEST_BLOCK(STR("vec4_add"))
  {
    vec4 v1 = vec4(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2 = vec4(5.0f, 6.0f, 7.0f, 8.0f);
    vec4 result = vec4_add(v1, v2);
    TEST_EVAL(result.x == 6.0f);
    TEST_EVAL(result.y == 8.0f);
    TEST_EVAL(result.z == 10.0f);
    TEST_EVAL(result.w == 12.0f);
  }

  TEST_BLOCK(STR("vec4_sub"))
  {
    vec4 v1 = vec4(5.0f, 7.0f, 9.0f, 11.0f);
    vec4 v2 = vec4(2.0f, 3.0f, 4.0f, 5.0f);
    vec4 result = vec4_sub(v1, v2);
    TEST_EVAL(result.x == 3.0f);
    TEST_EVAL(result.y == 4.0f);
    TEST_EVAL(result.z == 5.0f);
    TEST_EVAL(result.w == 6.0f);
  }

  TEST_BLOCK(STR("vec4_mul"))
  {
    vec4 v = vec4(2.0f, 3.0f, 4.0f, 5.0f);
    vec4 result = vec4_mul(v, 2.0f);
    TEST_EVAL(result.x == 4.0f);
    TEST_EVAL(result.y == 6.0f);
    TEST_EVAL(result.z == 8.0f);
    TEST_EVAL(result.w == 10.0f);
  }

  TEST_BLOCK(STR("vec4_div"))
  {
    vec4 v = vec4(4.0f, 6.0f, 8.0f, 10.0f);
    vec4 result = vec4_div(v, 2.0f);
    TEST_EVAL(result.x == 2.0f);
    TEST_EVAL(result.y == 3.0f);
    TEST_EVAL(result.z == 4.0f);
    TEST_EVAL(result.w == 5.0f);
  }

  TEST_BLOCK(STR("vec4_len"))
  {
    vec4 v = vec4(1.0f, 2.0f, 2.0f, 4.0f);
    f32 len = vec4_len(v);
    TEST_EVAL(EPSILON_EQUAL(len, 5.0f));
  }

  TEST_BLOCK(STR("vec4_dot"))
  {
    vec4 v1 = vec4(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 v2 = vec4(5.0f, 6.0f, 7.0f, 8.0f);
    f32 dot = vec4_dot(v1, v2);
    TEST_EVAL(EPSILON_EQUAL(dot, 70.0f));
  }

  TEST_BLOCK(STR("vec4_norm"))
  {
    vec4 v = vec4(1.0f, 2.0f, 2.0f, 4.0f);
    vec4 normalized = vec4_norm(v);
    TEST_EVAL(EPSILON_EQUAL(vec4_len(normalized), 1.0f));
    TEST_EVAL(EPSILON_EQUAL(normalized.x, 0.2f));
    TEST_EVAL(EPSILON_EQUAL(normalized.w, 0.8f));
  }

  TEST_BLOCK(STR("mat4_identity"))
  {
    mat4 m = mat4_identity();
    TEST_EVAL(m.m[0][0] == 1.0f);
    TEST_EVAL(m.m[1][1] == 1.0f);
    TEST_EVAL(m.m[2][2] == 1.0f);
    TEST_EVAL(m.m[3][3] == 1.0f);
    TEST_EVAL(m.m[0][1] == 0.0f);
    TEST_EVAL(m.m[1][0] == 0.0f);
  }

  TEST_BLOCK(STR("mat4_diagonal"))
  {
    mat4 m = mat4_diagonal(2.0f);
    TEST_EVAL(m.m[0][0] == 2.0f);
    TEST_EVAL(m.m[1][1] == 2.0f);
    TEST_EVAL(m.m[2][2] == 2.0f);
    TEST_EVAL(m.m[3][3] == 2.0f);
    TEST_EVAL(m.m[0][1] == 0.0f);
  }

  TEST_BLOCK(STR("mat4_scale"))
  {
    vec3 scale = vec3(2.0f, 3.0f, 4.0f);
    mat4 m = mat4_scale(scale);
    TEST_EVAL(m.m[0][0] == 2.0f);
    TEST_EVAL(m.m[1][1] == 3.0f);
    TEST_EVAL(m.m[2][2] == 4.0f);
    TEST_EVAL(m.m[3][3] == 1.0f);
  }

  TEST_BLOCK(STR("mat4_translation"))
  {
    vec3 translation = vec3(5.0f, 10.0f, 15.0f);
    mat4 m = mat4_translation(translation);
    TEST_EVAL(m.m[3][0] == 5.0f);
    TEST_EVAL(m.m[3][1] == 10.0f);
    TEST_EVAL(m.m[3][2] == 15.0f);
    TEST_EVAL(m.m[0][0] == 1.0f);
    TEST_EVAL(m.m[1][1] == 1.0f);
    TEST_EVAL(m.m[2][2] == 1.0f);
  }

  TEST_BLOCK(STR("mat4_mul_vec4 identity"))
  {
    mat4 identity = mat4_identity();
    vec4 v = vec4(1.0f, 2.0f, 3.0f, 4.0f);
    vec4 result = mat4_mul_vec4(identity, v);
    TEST_EVAL(VEC4_EPSILON_EQUAL(result, v));
  }

  TEST_BLOCK(STR("mat4_mul_vec4 scale"))
  {
    mat4 scale = mat4_scale(vec3(2.0f, 2.0f, 2.0f));
    vec4 v = vec4(1.0f, 2.0f, 3.0f, 1.0f);
    vec4 result = mat4_mul_vec4(scale, v);
    TEST_EVAL(EPSILON_EQUAL(result.x, 2.0f));
    TEST_EVAL(EPSILON_EQUAL(result.y, 4.0f));
    TEST_EVAL(EPSILON_EQUAL(result.z, 6.0f));
  }

  TEST_BLOCK(STR("mat4_mul_vec4 translation"))
  {
    mat4 trans = mat4_translation(vec3(5.0f, 10.0f, 15.0f));
    vec4 v = vec4(1.0f, 2.0f, 3.0f, 1.0f);
    vec4 result = mat4_mul_vec4(trans, v);
    TEST_EVAL(EPSILON_EQUAL(result.x, 6.0f));
    TEST_EVAL(EPSILON_EQUAL(result.y, 12.0f));
    TEST_EVAL(EPSILON_EQUAL(result.z, 18.0f));
  }

  TEST_BLOCK(STR("mat4_mul identity"))
  {
    mat4 identity = mat4_identity();
    mat4 m = mat4_scale(vec3(2.0f, 3.0f, 4.0f));
    mat4 result = mat4_mul(identity, m);
    TEST_EVAL(result.m[0][0] == 2.0f);
    TEST_EVAL(result.m[1][1] == 3.0f);
    TEST_EVAL(result.m[2][2] == 4.0f);
  }

  TEST_BLOCK(STR("mat4_mul associativity"))
  {
    mat4 scale = mat4_scale(vec3(2.0f, 2.0f, 2.0f));
    mat4 trans = mat4_translation(vec3(1.0f, 1.0f, 1.0f));
    vec4 v = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    mat4 combined = mat4_mul(scale, trans);
    vec4 result1 = mat4_mul_vec4(combined, v);
    vec4 result2 = mat4_mul_vec4(scale, mat4_mul_vec4(trans, v));

    TEST_EVAL(VEC4_EPSILON_EQUAL(result1, result2));
  }

  TEST_BLOCK(STR("vec3_rotate_x"))
  {
    vec3 v = vec3(0.0f, 1.0f, 0.0f);
    vec3 rotated = vec3_rotate_x(v, PI / 2.0f);
    TEST_EVAL(EPSILON_EQUAL(rotated.x, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(rotated.y, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(rotated.z, 1.0f));
  }

  TEST_BLOCK(STR("vec3_rotate_y"))
  {
    vec3 v = vec3(1.0f, 0.0f, 0.0f);
    vec3 rotated = vec3_rotate_y(v, PI / 2.0f);
    TEST_EVAL(EPSILON_EQUAL(rotated.x, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(rotated.y, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(rotated.z, 1.0f));
  }

  TEST_BLOCK(STR("vec3_rotate_z"))
  {
    vec3 v = vec3(1.0f, 0.0f, 0.0f);
    vec3 rotated = vec3_rotate_z(v, PI / 2.0f);
    TEST_EVAL(EPSILON_EQUAL(rotated.x, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(rotated.y, 1.0f));
    TEST_EVAL(EPSILON_EQUAL(rotated.z, 0.0f));
  }

  TEST_BLOCK(STR("mat4_rotation_z"))
  {
    mat4 rot = mat4_rotation_z(PI / 2.0f);
    vec4 v = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    vec4 result = mat4_mul_vec4(rot, v);
    TEST_EVAL(EPSILON_EQUAL(result.x, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(result.y, 1.0f));
    TEST_EVAL(EPSILON_EQUAL(result.z, 0.0f));
  }

  TEST_BLOCK(STR("mat4_look_at basic"))
  {
    vec3 eye = vec3(0.0f, 0.0f, 5.0f);
    vec3 target = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    mat4 view = mat4_look_at(eye, target, up);

    // Looking down -Z axis, so camera should be at origin after transform
    vec4 camera_pos = mat4_mul_vec4(view, vec4(0.0f, 0.0f, 5.0f, 1.0f));
    TEST_EVAL(EPSILON_EQUAL(camera_pos.x, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(camera_pos.y, 0.0f));
  }

  TEST_BLOCK(STR("mat4_orthographic basic"))
  {
    mat4 ortho = mat4_orthographic(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    // Center point should map to origin in NDC
    vec4 center = vec4(0.0f, 0.0f, -50.0f, 1.0f);
    vec4 result = mat4_mul_vec4(ortho, center);
    result.x /= result.w;
    result.y /= result.w;
    result.z /= result.w;

    TEST_EVAL(EPSILON_EQUAL(result.x, 0.0f));
    TEST_EVAL(EPSILON_EQUAL(result.y, 0.0f));
  }

  TEST_BLOCK(STR("mat4_perspective basic"))
  {
    mat4 persp = mat4_perspective(PI / 2.0f, 1.0f, 0.1f, 100.0f);

    // Point on near plane should map close to z=0 in NDC
    vec4 near_point = vec4(0.0f, 0.0f, -0.1f, 1.0f);
    vec4 result = mat4_mul_vec4(persp, near_point);
    result.z /= result.w;

    TEST_EVAL(EPSILON_EQUAL(result.z, 0.0f));
  }

  tester_summarize();

  arena_free(&arena);
}
