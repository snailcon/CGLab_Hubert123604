#version 150 core

out vec4 out_Color;

in vec3 pass_Texcoords;

uniform samplerCube cubemap;

void main()
{    
    out_Color = texture(cubemap, pass_Texcoords);
}