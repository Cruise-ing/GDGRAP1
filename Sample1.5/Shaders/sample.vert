#version 330 core

// Gets the data at Attrib Index 0
// Converts it and stores it into a Vec3
layout(location = 0) in vec3 aPos;

// declare a variable to hold the data that we're going to pass
uniform mat4 transform;

//Projection martrix
uniform mat4 projection;

void main() {
	gl_Position = projection * transform * vec4(aPos,1.0);
}