#version 150

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 light_col;
uniform float light_intensity;

uniform bool is_toon = false;
uniform float toon_steps = 5.0f;

uniform float ambient_strength = 0.2f;
uniform int specular_const = 24;

uniform float screenGamma = 2.2f;

in vec3 pass_Normal;
in vec3 world_pos;
in vec3 camera_pos;
out vec4 out_Color;

void main() {
  vec3 normal_dir = normalize(pass_Normal);
  vec3 view_dir = normalize(world_pos - camera_pos);
  vec3 light_dir = normalize(light_pos - world_pos);
  float diffuse = clamp((dot(normal_dir, light_dir)), 0.0f, 1.0f);
  
  // phong
  vec3 reflected = normalize(reflect(-light_dir, normal_dir));
  float specular_val = pow(clamp(dot(reflected, -view_dir), 0.0f, 1.0f), specular_const);

  // blinn-phong
  //vec3 half_way = normalize(light_dir - view_dir);
  //float specular_val = pow(clamp(dot(half_way, normal_dir), 0.0f, 1.0f), specular_const * 4);


  if (is_toon) {
    specular_val = step(0.5f, specular_val);
    diffuse = floor(diffuse * toon_steps)/toon_steps;
  }

  vec3 ambient_color = color * ambient_strength;
  vec3 diffuse_color = color * diffuse;
  vec3 specular = vec3(1.0f) * specular_val;

  vec3 final_color = ambient_color + (diffuse_color + specular) * light_intensity;

  float val = 1.0f;
  if (is_toon) {
    val = round(dot(normal_dir, -view_dir) * 0.95f);
  }
  vec3 colorGammaCorrected = pow(final_color, vec3(1.0 / screenGamma));
  out_Color = vec4(colorGammaCorrected * val, 1.0);
}
