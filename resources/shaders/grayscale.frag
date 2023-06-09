#version 150

in vec2 pass_UV;
out vec4 out_Color;

uniform sampler2D screen_tex;

void main() {
    vec3 col = texture(screen_tex, pass_UV).xyz;
    col = vec3(dot(col, vec3(0.299f, 0.587f, 0.114f)));
    out_Color = vec4(col, 1.0f);
}