// OpenGL ES require that precision is defined for a fragment shader
#ifndef USE_NORMAL_GLSL
#ifdef GL_FRAGMENT_PRECISION_HIGH
   precision highp float;
#else
   precision mediump float;
#endif
#endif

varying vec4 col_var;

void main()
{
	gl_FragColor = col_var;
}
