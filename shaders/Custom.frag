#version 330

// (Every uniform is available here.)

uniform mat4 u_view_projection;
uniform mat4 u_model;

uniform float u_normal_scaling;
uniform float u_height_scaling;

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

// Feel free to add your own textures. If you need more than 4,
// you will need to modify the skeleton.
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform sampler2D u_texture_4;

// Environment map! Take a look at GLSL documentation to see how to
// sample from this.
uniform samplerCube u_texture_cubemap;


in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float rand(vec2 co) {
  float seed = sin(dot(co, vec2(130, 700)));
  float value = fract(seed * 4000);
  value += fract(cos(seed) * 5000);
  value += fract(tan(seed) * 6000);
  return fract(value);
}

void main() {
  // Your awesome shader here!
  // out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  // out_color.a = 1;

  float random_value = rand(v_uv);

  // Changes color (higher is more darker)
  vec3 k_coefficients;
  if (random_value < 0.5) {
    k_coefficients = vec3(0.4);
  } else {
    k_coefficients = vec3(0.9);
  }

  // Calculate the final dots with varying size and opacity
  vec3 dots = v_normal.xyz * k_coefficients;
  
  // Blue color
  vec3 water_color = vec3(0.6, 0.6, 1.0) + dots;
  
  // Add transparency 
  vec4 transparent_water = vec4(water_color, 0.5);
  
  out_color = transparent_water;
}
