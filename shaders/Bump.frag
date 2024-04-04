#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  return texture(u_texture_2, uv).x;
}

void main() {
  // YOUR CODE HERE

  //bump
  mat3 tbn;
  tbn[0] = normalize(v_tangent.xyz);
  tbn[1] = normalize(cross(v_normal.xyz, v_tangent.xyz));
  tbn[2] = normalize(v_normal.xyz);

  float width = u_texture_2_size.x;
  float height = u_texture_2_size.y;

  vec2 v_uv1 = vec2((v_uv.x + 1.0)/width, v_uv.y);
  vec2 v_uv2 = vec2(v_uv.x, (v_uv.y + 1.0)/height);

  float du = (h(v_uv1) - h(v_uv)) * u_normal_scaling * u_height_scaling;
  float dv = (h(v_uv2) - h(v_uv)) * u_normal_scaling * u_height_scaling;

  vec3 no = normalize(vec3(-du, -dv, 1.0));
  vec3 nd = normalize(tbn * no);


  // START OF PHONG SHADER

  float ka = 0.01;
  float kd = 1.0;
  float ks = 0.6;
  vec3 ia = u_color.xyz;
  float p = 32.0;

  vec3 n = nd;
  vec3 u_light_dir = normalize(u_light_pos - v_position.xyz);
  vec3 u_out_dir = normalize(u_cam_pos - v_position.xyz);

  // ambient
  vec3 ambient = ka * ia;

  // diffuse
  float dotval = dot(n, u_light_dir);
  float maxval = max(0.0, dotval);

  vec3 diffuse = kd * maxval * normalize(u_light_intensity);

  // specular
  vec3 angle = normalize(u_out_dir + u_light_dir);
  float spec_dotval = dot(n, angle);
  float spec_maxval = pow(max(0.0, spec_dotval), p);
  vec3 specular = ks * spec_maxval * normalize(u_light_intensity);

  // END OF PHONG SHADER

  
  out_color = vec4(ambient + diffuse + specular, 1.0);


  
  // (Placeholder code. You will want to replace it.)
  // out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  // out_color.a = 1;
}

