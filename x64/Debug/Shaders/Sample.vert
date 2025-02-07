#version 330 core

layout(location = 0) in vec3 aPos;

//uniform float x;

uniform mat4 transform;

void main()
{
	//vec3 newPos = vec3(aPos.x +x, aPos.y, aPos.z);
	
	//gl_Position = vec4(newPos, 1.0);

	gl_Position = transform * vec4(aPos, 1.0);
}