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
  float seed = sin(dot(co, vec2(12.9898, 78.233)));
  float value = fract(seed * 43758.5453);
  value += fract(cos(seed) * 52758.5453);
  value += fract(tan(seed) * 31728.5453);
  return fract(value);
}

void main() {
  // Your awesome shader here!
  // out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  // out_color.a = 1;

  /*
  // float random_value = rand(v_uv);

  // // Set the coefficients based on the random value
  // vec3 k_coefficients;
  // if (random_value < 0.5) {
  //   k_coefficients = vec3(0.1);
  // } else {
  //   k_coefficients = vec3(0.4);
  // }

  // // Adjust dot size based on random value
  // float dot_size = random_value * 5.55;

  // // Calculate the final dots with varying size and opacity
  // vec3 dots = (v_normal.xyz) * k_coefficients * dot_size;
  
  // // Blue color
  // vec3 water_color = vec3(0.5, 0.7, 1.0) + dots;
  
  // // Add transparency to the water
  // vec4 transparent_water = vec4(water_color, 0.5);
  
  // // Output the final color with transparency
  // out_color = transparent_water;*/

  float random_value = rand(v_uv);

  vec3 k_coefficients;
  if (random_value < 0.5) {
      k_coefficients = vec3(0.3);
  } else {
      k_coefficients = vec3(0.9);
  }

  vec3 blue = vec3(0.2, 0.4, 0.7) * k_coefficients;

  vec4 transparent_water = vec4(blue, 1.7);

  out_color = vec4(blue, 1.0) + transparent_water;
}
