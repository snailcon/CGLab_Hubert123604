#version 150

in vec2 pass_UV;
out vec4 out_Color;

uniform sampler2D screen_tex;
uniform vec2 screen_size;

void main() {
    vec2 offsets = 1.0f / screen_size;
    
    vec3 samples[9];
    samples[0] = texture(screen_tex, pass_UV + vec2(-offsets.x, -offsets.y)).xyz;
    samples[1] = texture(screen_tex, pass_UV + vec2(0.0f      , -offsets.y)).xyz;
    samples[2] = texture(screen_tex, pass_UV + vec2(offsets.x , -offsets.y)).xyz;
    samples[3] = texture(screen_tex, pass_UV + vec2(-offsets.x,       0.0f)).xyz;
    samples[4] = texture(screen_tex, pass_UV).xyz;
    samples[5] = texture(screen_tex, pass_UV + vec2(offsets.x ,       0.0f)).xyz;
    samples[6] = texture(screen_tex, pass_UV + vec2(-offsets.x,  offsets.y)).xyz;
    samples[7] = texture(screen_tex, pass_UV + vec2(0.0f      ,  offsets.y)).xyz;
    samples[8] = texture(screen_tex, pass_UV + vec2(offsets.x ,  offsets.y)).xyz;

    vec3 col = 0.0625f * (samples[0] + samples[2] + samples[6] + samples[8]) + 
               0.125f * (samples[1] + samples[3] + samples[5] + samples[7]) + 
               0.25f * samples[4];

    out_Color = vec4(col, 1.0f);
}