#version 400 core
/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the normal passed in
layout (location = 1) in vec3 inColour;
/// @brief MVP passed from app
uniform mat4 MVP;

out vec3 vertColour;

void main()
{
// pre-calculate for speed we will use this a lot

// calculate the vertex position
gl_Position = MVP*vec4(inVert, 1.0);
// pass the UV values to the frag shader
vertColour=inColour;
}
