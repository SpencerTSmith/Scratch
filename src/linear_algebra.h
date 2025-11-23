#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include "common.h"

#include <math.h>

/*
    Obviously huge inspiration from HandmadeMath
    Vectors are column vectors, Matrices are column major
    We assume a 0 - > 1 NDC and a right handed system, with z pointing toward us
*/

// TODO: I've been looking through godbolt, and mat4_mul_vec4 seems to easily autovectorise in
// case -O3 and -mavx, we may not need to so explicit SIMD intrinsics but something to look into as
// we progress

typedef union vec2 vec2;
union vec2
{
  struct
  {
    f32 x, y;
  };
  struct
  {
    f32 u, v;
  };
  struct
  {
    f32 w, h;
  };
  f32 d[2];
};

typedef union vec3 vec3;
union vec3
{
  struct
  {
    f32 x, y, z;
  };
  struct
  {
    f32 r, g, b;
  };
  struct
  {
    f32 _ignored_x;
    vec2 yz;
  };
  struct
  {
    vec2 xy;
    f32 _ignored_z;
  };
  struct
  {
    f32 _ignored_u;
    vec2 vw;
  };
  struct
  {
    vec2 uv;
    f32 _ignored_w;
  };
  f32 d[3];
};

typedef union vec4 vec4;
union vec4
{
  struct
  {
    union
    {
      struct
      {
        f32 x, y, z;
      };
      vec3 xyz;
    };
    f32 w;
  };
  struct
  {
    union
    {
      struct
      {
        f32 r, g, b;
      };
      vec3 rgb;
    };
    f32 a;
  };
  struct
  {
    vec2 xy;
    vec2 _ignored_zw;
  };
  struct
  {
    f32 _ignored_x;
    vec2 yz;
    f32 _ignored_w;
  };
  struct
  {
    vec2 _ignored_xy;
    vec2 zw;
  };
  f32 d[4];
};

typedef union mat4 mat4;
union mat4
{
  f32 m[4][4];
  vec4 cols[4];
};

typedef union mat3 mat3;
union mat3
{
  f32 m[3][3];
  vec3 cols[3];
};

#define vec2(xx, yy) ((vec2){.x = (xx), .y = (yy)})

static
f32 vec2_len(vec2 v);

static
vec2 vec2_add(vec2 v1, vec2 v2);

static
vec2 vec2_sub(vec2 v1, vec2 v2);

static
vec2 vec2_mul(vec2 v, f32 s);

static
vec2 vec2_div(vec2 v, f32 s);

static
f32 vec2_dot(vec2 a, vec2 b);

// Only 1 division like this
static
vec2 vec2_norm(vec2 v);
// Nice little 0 checker variant
static
vec2 vec2_norm0(vec2 v);

static
f32 vec2_cross(vec2 a, vec2 b);

#define vec3(xx, yy, zz) ((vec3){.x = (xx), .y = (yy), .z = (zz)})

static
f32 vec3_len(vec3 v);

static
vec3 vec3_add(vec3 v1, vec3 v2);

static
vec3 vec3_sub(vec3 v1, vec3 v2);

static
vec3 vec3_mul(vec3 v, f32 s);

static
vec3 vec3_div(vec3 v, f32 s);

static
vec3 vec3_inv(vec3 v);

static
vec3 vec3_cross(vec3 left, vec3 right);

static
f32 vec3_dot(vec3 a, vec3 b);

static
vec3 vec3_norm(vec3 v);
static
vec3 vec3_norm0(vec3 v);

static
vec3 vec3_rotate_x(vec3 v, f32 angle);

static
vec3 vec3_rotate_y(vec3 v, f32 angle);

static
vec3 vec3_rotate_z(vec3 v, f32 angle);

static
vec4 vec3_to_vec4(vec3 v);

#define vec4(xx, yy, zz, ww) ((vec4){.x = (xx), .y = (yy), .z = (zz), .w = (ww)})

static
f32 vec4_len(vec4 v);

static
vec4 vec4_add(vec4 v1, vec4 v2);

static
vec4 vec4_sub(vec4 v1, vec4 v2);

static
vec4 vec4_mul(vec4 v, f32 s);

static
vec4 vec4_div(vec4 v, f32 s);

static
f32 vec4_dot(vec4 a, vec4 b);

static
vec4 vec4_norm(vec4 v);

static
vec4 vecf_norm0(vec4 v);

#define mat4_identity() mat4_diagonal(1.0f)
static
mat4 mat4_diagonal(f32 d);

static
mat4 mat4_scale(vec3 v);

#define mat4_rotation_x(radians) mat4_rotation(radians, vec3(1.0f, 0.0f, 0.0f))
#define mat4_rotation_y(radians) mat4_rotation(radians, vec3(0.0f, 1.0f, 0.0f))
#define mat4_rotation_z(radians) mat4_rotation(radians, vec3(0.0f, 0.0f, 1.0f))
// General form taken from wikipedia, but makes sense,
static
mat4 mat4_rotation(f32 radians, vec3 axis);

static
mat4 mat4_translation(vec3 v);

// Basically, with the new x, y, z that gotten by crossing, we use them as the new basis vectors in
// camera space Then, we also offset everything so that the camera is the origin, why we have the
// negative dot products, as always right hand rule, also again since vulkan is weird and NDC says y
// is down we do the inverse and such
static
mat4 mat4_look_direction(vec3 position, vec3 direction, vec3 up);

// Always have the camera looking from the position to the target, as opposed to the above where it
// is the camera and a direction relative to it... Also right hand rule
static
mat4 mat4_look_at(vec3 position, vec3 target, vec3 up);

// For Vulkan's canonical with x left to right -1 -> 1, y top to bottom -1 to 1, and z near to far
// from 0 -> 1. Assumes orthographic view volume is using a right handed coordinate system, ie
// x grows left to right, y grows top to bottom, and z grows far to near
static
mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

// Right handed coordinate system, DO NOT MAKE THE SAME MISTAKE... z_near, and z_far are ABSOLUTE
// distances from the view position!!!!!! Even in right handed system... they are still positive!!!
static
mat4 mat4_perspective(f32 fov, f32 aspect_ratio, f32 z_near, f32 z_far);

static
vec4 mat4_mul_vec4(mat4 m, vec4 v);

static
mat4 mat4_mul(mat4 left, mat4 right);

#endif // LINEAR_ALGEBRA_H
