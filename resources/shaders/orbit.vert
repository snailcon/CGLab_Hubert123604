#version 150
#extension GL_ARB_explicit_attrib_location : require
// glVertexAttribPointer mapped positions to first
layout(location = 0) in vec3 in_Position;

//Matrix Uniforms uploaded with glUniform*
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_Color;

void main() {
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Color = vec3(1.0f, 1.0f, 1.0f); // to reuse veo.frag
}