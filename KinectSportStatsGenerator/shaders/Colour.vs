#version 150


uniform mat4 MVP;

in vec3 inVert;

void main(void)
{
  gl_Position = MVP*vec4(inVert,1.0);
}

