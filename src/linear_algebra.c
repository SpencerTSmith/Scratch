#include "linear_algebra.h"


static
f32 vec2_len(vec2 v)
{
  return sqrtf(v.x * v.x + v.y * v.y);
}

static
vec2 vec2_add(vec2 v1, vec2 v2)
{
  vec2 result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;

  return result;
}

static
vec2 vec2_sub(vec2 v1, vec2 v2)
{
  vec2 result;
  result.x = v1.x - v2.x;
  result.y = v1.y - v2.y;

  return result;
}

static
vec2 vec2_mul(vec2 v, f32 s)
{
  vec2 result;
  result.x = v.x * s;
  result.y = v.y * s;

  return result;
}

static
vec2 vec2_div(vec2 v, f32 s)
{
  vec2 result;
  result.x = v.x / s;
  result.y = v.y / s;

  return result;
}

static
f32 vec2_dot(vec2 a, vec2 b)
{
  return a.x * b.x + a.y * b.y;
}

static
vec2 vec2_norm(vec2 v)
{
  return vec2_mul(v, 1 / vec2_len(v));
}

static
vec2 vec2_norm0(vec2 v)
{
  f32 len = vec2_len(v);
  return len > 0 ? vec2_mul(v, 1 / len) : (vec2){0};
}

static
f32 vec2_cross(vec2 a, vec2 b)
{
  return a.x * b.y - a.y * b.x;
}

static
f32 vec3_len(vec3 v)
{
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static
vec3 vec3_add(vec3 v1, vec3 v2)
{
  vec3 result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  result.z = v1.z + v2.z;

  return result;
}

static
vec3 vec3_sub(vec3 v1, vec3 v2)
{
  vec3 result;
  result.x = v1.x - v2.x;
  result.y = v1.y - v2.y;
  result.z = v1.z - v2.z;

  return result;
}

static
vec3 vec3_mul(vec3 v, f32 s)
{
  vec3 result;
  result.x = v.x * s;
  result.y = v.y * s;
  result.z = v.z * s;

  return result;
}

static
vec3 vec3_div(vec3 v, f32 s)
{
  vec3 result;
  result.x = v.x / s;
  result.y = v.y / s;
  result.z = v.z / s;

  return result;
}

static
vec3 vec3_inv(vec3 v)
{
  vec3 result;
  result.x = 1.f / v.x;
  result.y = 1.f / v.y;
  result.z = 1.f / v.z;

  return result;
}

static
vec3 vec3_cross(vec3 left, vec3 right)
{
  vec3 result;
  result.x = (left.y * right.z) - (left.z * right.y);
  result.y = (left.z * right.x) - (left.x * right.z);
  result.z = (left.x * right.y) - (left.y * right.x);

  return result;
}

static
f32 vec3_dot(vec3 a, vec3 b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

static
vec3 vec3_norm(vec3 v)
{
  return vec3_mul(v, 1.0f / vec3_len(v));
}

static
vec3 vec3_norm0(vec3 v)
{
  f32 len = vec3_len(v);
  return len > 0 ? vec3_mul(v, 1 / len) : (vec3){0};
}

static
vec3 vec3_rotate_x(vec3 v, f32 angle)
{
  vec3 result;
  result.x = v.x;
  result.y = v.y * cosf(angle) - v.z * sinf(angle);
  result.z = v.y * sinf(angle) + v.z * cosf(angle);

  return result;
}

static
vec3 vec3_rotate_y(vec3 v, f32 angle)
{
  vec3 result;
  result.x = v.x * cosf(angle) - v.z * sinf(angle);
  result.y = v.y;
  result.z = v.x * sinf(angle) + v.z * cosf(angle);

  return result;
}

static
vec3 vec3_rotate_z(vec3 v, f32 angle)
{
  vec3 result;
  result.x = v.x * cosf(angle) - v.y * sinf(angle);
  result.y = v.x * sinf(angle) + v.y * cosf(angle);
  result.z = v.z;

  return result;
}

static
vec4 vec3_to_vec4(vec3 v)
{
  vec4 result;
  result.xyz = v;
  result.w = 1.0f;

  return result;
}

static
f32 vec4_len(vec4 v)
{
  return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

static
vec4 vec4_add(vec4 v1, vec4 v2)
{
  vec4 result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  result.z = v1.z + v2.z;
  result.w = v1.w + v2.w;

  return result;
}

static
vec4 vec4_sub(vec4 v1, vec4 v2)
{
  vec4 result;
  result.x = v1.x - v2.x;
  result.y = v1.y - v2.y;
  result.z = v1.z - v2.z;
  result.w = v1.w - v2.w;

  return result;
}

static
vec4 vec4_mul(vec4 v, f32 s)
{
  vec4 result;
  result.x = v.x * s;
  result.y = v.y * s;
  result.z = v.z * s;
  result.w = v.w * s;

  return result;
}

static
vec4 vec4_div(vec4 v, f32 s)
{
  vec4 result;
  result.x = v.x / s;
  result.y = v.y / s;
  result.z = v.z / s;
  result.w = v.w / s;

  return result;
}

static
f32 vec4_dot(vec4 a, vec4 b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static
vec4 vec4_norm(vec4 v)
{
  return vec4_mul(v, 1.0f / vec4_len(v));
}

static
vec4 vecf_norm0(vec4 v)
{
  f32 len = vec4_len(v);
  return len > 0 ? vec4_mul(v, 1 / len) : (vec4){0};
}

static
mat4 mat4_diagonal(f32 d)
{
  mat4 m = {0};
  m.cols[0].x = d;
  m.cols[1].y = d;
  m.cols[2].z = d;
  m.cols[3].w = d;

  return m;
}

static
mat4 mat4_scale(vec3 v)
{
  mat4 s = mat4_identity();
  s.cols[0].x = v.x;
  s.cols[1].y = v.y;
  s.cols[2].z = v.z;

  return s;
}

static
mat4 mat4_rotation(f32 radians, vec3 axis)
{
  axis = vec3_norm(axis);
  f32 sin = sinf(radians);
  f32 cos = cosf(radians);
  f32 one_minus_cos = 1.0f - cos;

  mat4 r = mat4_identity();
  r.cols[0].x = (axis.x * axis.x * one_minus_cos) + cos;
  r.cols[0].y = (axis.x * axis.y * one_minus_cos) + (axis.z * sin);
  r.cols[0].z = (axis.x * axis.z * one_minus_cos) - (axis.y * sin);

  r.cols[1].x = (axis.y * axis.x * one_minus_cos) - (axis.z * sin);
  r.cols[1].y = (axis.y * axis.y * one_minus_cos) + cos;
  r.cols[1].z = (axis.y * axis.z * one_minus_cos) + (axis.x * sin);

  r.cols[2].x = (axis.z * axis.x * one_minus_cos) + (axis.y * sin);
  r.cols[2].y = (axis.z * axis.y * one_minus_cos) - (axis.x * sin);
  r.cols[2].z = (axis.z * axis.z * one_minus_cos) + cos;

  return r;
}

static
mat4 mat4_translation(vec3 v)
{
  mat4 t = mat4_identity();
  t.cols[3].x = v.x;
  t.cols[3].y = v.y;
  t.cols[3].z = v.z;

  return t;
}

static
mat4 mat4_look_direction(vec3 position, vec3 direction, vec3 up)
{
  vec3 z = vec3_norm(direction);
  vec3 x = vec3_norm(vec3_cross(z, up));
  vec3 y = vec3_cross(z, x); // already normal

  f32 x_dot = vec3_dot(x, position);
  f32 y_dot = vec3_dot(y, position);
  f32 z_dot = vec3_dot(z, position);

  mat4 m = {0};
  m.cols[0] = vec4(x.x, -y.x, -z.x, 0.0f);
  m.cols[1] = vec4(x.y, -y.y, -z.y, 0.0f);
  m.cols[2] = vec4(x.z, -y.z, -z.z, 0.0f);
  m.cols[3] = vec4(-x_dot, y_dot, z_dot, 1.0f);

  return m;
}

static
mat4 mat4_look_at(vec3 position, vec3 target, vec3 up)
{
  return mat4_look_direction(position, vec3_sub(target, position), up);
}

static
mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
  mat4 o = mat4_identity();
  o.m[0][0] = 2.0f / (right - left);  // Scale to NDC
  o.m[1][1] = -2.0f / (top - bottom); // Scale to NDC, vulkan goes oppsoite, y grows down
  o.m[2][2] = -1.0f / (near - far);   // Scale to NDC, vulkan goes opposite, z grows to far

  o.m[3][0] = -(right + left) / (right - left); // translate x to canoncial origin
  o.m[3][1] = -(bottom + top) / (top - bottom); // translate y to canoncial origin
  o.m[3][2] = -(near) / (far - near);           // translate z to canoncial origin

  return o;
}

static
mat4 mat4_perspective(f32 fov, f32 aspect_ratio, f32 z_near, f32 z_far)
{
  mat4 p = {0};
  f32 cotan = 1.0f / tanf(fov / 2.0f);
  p.m[0][0] = cotan / aspect_ratio;                // x normalization
  p.m[1][1] = -cotan;                              // y normalization, and flip for Vulkan
  p.m[2][2] = z_far / (z_near - z_far);            // z normalization
  p.m[3][2] = (z_far * z_near) / (z_near - z_far); // z offset
  p.m[2][3] = -1.0f;                               // z  in w for persp div, negative for vulkan

  return p;
}

static
vec4 mat4_mul_vec4(mat4 m, vec4 v)
{
  vec4 result;
  result.x = m.cols[0].x * v.x;
  result.y = m.cols[0].y * v.x;
  result.z = m.cols[0].z * v.x;
  result.w = m.cols[0].w * v.x;

  result.x += m.cols[1].x * v.y;
  result.y += m.cols[1].y * v.y;
  result.z += m.cols[1].z * v.y;
  result.w += m.cols[1].w * v.y;

  result.x += m.cols[2].x * v.z;
  result.y += m.cols[2].y * v.z;
  result.z += m.cols[2].z * v.z;
  result.w += m.cols[2].w * v.z;

  result.x += m.cols[3].x * v.w;
  result.y += m.cols[3].y * v.w;
  result.z += m.cols[3].z * v.w;
  result.w += m.cols[3].w * v.w;

  return result;
}

static
mat4 mat4_mul(mat4 left, mat4 right)
{
  mat4 result;
  result.cols[0] = mat4_mul_vec4(left, right.cols[0]);
  result.cols[1] = mat4_mul_vec4(left, right.cols[1]);
  result.cols[2] = mat4_mul_vec4(left, right.cols[2]);
  result.cols[3] = mat4_mul_vec4(left, right.cols[3]);

  return result;
}
