/* PASS */
#version 110
#extension GL_ARB_draw_buffers: require

uniform vec4 a;

void main()
{
  gl_FragData[0] = a;
}