#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // YOUR CODE HERE

  float ka = 0.01;
  float kd = 1.0;
  float ks = 0.6;
  vec3 ia = u_color.xyz;
  float p = 32.0;

  vec3 n = normalize(v_normal.xyz);
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

  out_color = vec4(ambient + diffuse + specular, 1.0);

  // (Placeholder code. You will want to replace it.)
  // out_color = (vec4(1, 1, 1, 0) + v_normal) / 2;
  // out_color.a = 1;
}

