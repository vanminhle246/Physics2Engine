/////////////////////////////////////////////////////////////////
// OpenGL ES require that precision is defined for a fragment shader
#ifndef USE_NORMAL_GLSL
#ifdef GL_FRAGMENT_PRECISION_HIGH
   precision highp float;
#else
   precision mediump float;
#endif
#else
#endif

#define CONTRAST_ADJUST(_x)
//#define CONTRAST_ADJUST(_x) _x = _x*0.9+0.1



uniform sampler2D g_texture;

varying vec4 col_var;
//varying vec2 uv_var;
varying vec4 rot_var;

void main()
{
	vec2 uv_var = gl_PointCoord.xy;
	uv_var -= vec2(0.5,0.5);
	vec2 uv;
	uv.x = uv_var.x * rot_var.x - uv_var.y * rot_var.y + 0.5;
	uv.y = uv_var.x * rot_var.z + uv_var.y * rot_var.w + 0.5;
	vec4 col = col_var * texture2D(g_texture, uv);
    gl_FragColor = col;
}
