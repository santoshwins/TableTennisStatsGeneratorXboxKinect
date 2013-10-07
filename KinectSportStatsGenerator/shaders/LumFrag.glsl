#version 400

layout (location=0) out vec4 outColour;

// this is a pointer to the current 2D texture object
uniform sampler2D tex;
// the vertex UV
in vec2 vertUV;
void main ()
{
 // set the fragment colour to the current texture
	// as we are using luminance only the red channel is active
	// so we set this accordingly
 outColour = vec4(vec3(texture(tex,vertUV).r),1.0);
}
