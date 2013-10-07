#version 400
//uniform sampler2D tex;

/// @brief MVP passed from app
uniform mat4 MVP;

uniform bool textureFlag;

layout(location=0) in vec3 inVert;
layout(location=1) in vec2 inUV;
out vec2 vertUV;

void main()
{
// pre-calculate for speed we will use this a lot

// calculate the vertex position
    if(textureFlag == true)
    {
        gl_Position = MVP*vec4(inVert, 1.0);
    }
    else
    {
        gl_Position = vec4(inVert, 1.0);
    }
// pass the UV values to the frag shader
vertUV=inUV.st;
}
