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

uniform sampler2D diffuse_texture;

in vec3 pass_Normal;
in vec2 pass_UV;
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

  vec3 flat_diffuse = texture(diffuse_texture, pass_UV).xyz;

  vec3 ambient_color = flat_diffuse * ambient_strength;
  vec3 diffuse_color = flat_diffuse * diffuse;
  vec3 specular = vec3(1.0f) * specular_val;

  vec3 final_color = ambient_color + (diffuse_color + specular) * light_intensity * light_col;

  float outline_val = 1.0f;
  if (is_toon) {
    outline_val = round(dot(normal_dir, -view_dir) * 1.5f);
  }
  vec3 colorGammaCorrected = pow(final_color, vec3(1.0 / screenGamma));
  if (outline_val > 0.5f) {
    out_Color = vec4(colorGammaCorrected, 1.0);
  } else {
    out_Color = vec4(1.0f);
  }
}
