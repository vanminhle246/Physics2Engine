/////////////////////////////////////////////////////////////////

#ifndef _WINDOWS
#define OUTPUT_POS(_x) gl_Position = vec4(_x.y, -_x.x, _x.z, _x.w);
#else
#define OUTPUT_POS(_x) gl_Position = _x;
#endif

#define CONTRAST_ADJUST(_x)
//#define CONTRAST_ADJUST(_x) _x = _x*0.9+0.1

uniform mat4 g_mTransform;
uniform vec4 g_modcol;

attribute vec3 pos_attr;
attribute float psize_attr;
attribute float uv_attr;
attribute vec4 col_attr;
varying vec4 col_var;
//attribute vec2 uv_attr;
//varying vec2 uv_var;
varying vec4 rot_var;

void main()
{
	vec4 pos = g_mTransform * vec4(pos_attr.x, pos_attr.y, pos_attr.z, 1.0);

    col_var = col_attr;
	//uv_var = uv_attr;
	gl_PointSize = psize_attr;
	rot_var.x = cos(uv_attr);
	rot_var.y = sin(uv_attr);
	rot_var.z = rot_var.y;
	rot_var.w = rot_var.x;
	OUTPUT_POS(pos);
}

/////////////////////////////////////////////////////////////////
