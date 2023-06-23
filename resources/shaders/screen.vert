#version 150
#extension GL_ARB_explicit_attrib_location : require
// glVertexAttribPointer mapped positions to first
layout(location = 0) in vec3 in_Position;
// glVertexAttribPointer mapped color  to second attribute 
layout(location = 1) in vec2 in_UV;

out vec2 pass_UV;

void main() {
	gl_Position = vec4(in_Position, 1.0);
	pass_UV = in_UV;
}