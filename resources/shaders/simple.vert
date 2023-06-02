#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

out vec3 pass_Normal;
// pass the world pos
out vec3 world_pos;
out vec3 camera_pos;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	world_pos = (ModelMatrix * vec4(in_Position, 1.0)).xyz;
	camera_pos = (inverse(ViewMatrix) * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
}
