/////////////////////////////////////////////////////////////////
// OpenGL ES require that precision is defined for a fragment shader
#ifndef USE_NORMAL_GLSL
#ifdef GL_FRAGMENT_PRECISION_HIGH
   precision highp float;
#else
   precision mediump float;
#endif
#endif

uniform sampler2D g_texture;
uniform vec4 g_modcol;

//varying vec4 col_var;
varying vec2 uv_var;

void main()
{
	vec4 col		= texture2D(g_texture, uv_var);
	col = smoothstep(0.49, 0.54, col.aaaa);
	
	
	//gl_FragColor	= col * col_var;
	gl_FragColor	= vec4(1,1,1,col.r) * g_modcol;
	
}
