#version 150

in vec2 pass_UV;
out vec4 out_Color;

uniform sampler2D screen_tex;

void main() {
    vec2 new_UV = vec2(pass_UV.x, 1.0f - pass_UV.y);
    vec3 col = texture(screen_tex, new_UV).xyz;
    out_Color = vec4(col, 1.0f);
}