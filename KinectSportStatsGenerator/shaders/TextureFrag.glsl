#version 400

layout (location=0) out vec4 outColour;

// this is a pointer to the current 2D texture object
uniform sampler2D tex;
// the vertex UV
in vec2 vertUV;
void main ()
{
 // set the fragment colour to the current texture
 outColour = texture(tex,vertUV);
}
