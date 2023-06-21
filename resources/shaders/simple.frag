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
uniform sampler2D normal_texture;

uniform float normal_strength;

in vec3 pass_Normal;
in vec2 pass_UV;
in vec3 world_pos;
in vec3 camera_pos;
out vec4 out_Color;

vec3 perturbNormal( vec3 vertex_pos, vec3 surf_norm ) {
  vec3 q0 = dFdx( vertex_pos.xyz );
  vec3 q1 = dFdy( vertex_pos.xyz );
  vec2 st0 = dFdx( pass_UV );
  vec2 st1 = dFdy( pass_UV );
  vec3 S = normalize( q0 * st1.t - q1 * st0.t );
  vec3 T = normalize( -q0 * st1.s + q1 * st0.s );
  vec3 N = normalize( surf_norm );
  vec3 mapN = texture2D( normal_texture, pass_UV ).xyz * 2.0 - 1.0;
  if (length(mapN+1.0) < 0.001f) return surf_norm;
  mapN.xy = mapN.xy * normal_strength;
  mat3 tsn = mat3( S, T, N );
  return normalize( tsn * mapN );
}

void main() {
  vec3 normal_dir = normalize(pass_Normal);
  normal_dir = perturbNormal(world_pos, normal_dir);
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
