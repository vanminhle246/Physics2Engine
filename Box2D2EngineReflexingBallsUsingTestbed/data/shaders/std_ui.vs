uniform mat4 g_mTransform;
uniform vec4 g_modcol;
uniform mat3 g_mNormalMatrix;

attribute vec3 pos_attr;
attribute vec4 col_attr;
attribute vec2 uv_attr;

varying vec4 col_var;
varying vec2 uv_var;

void main()
{
	#ifdef _IPHONE
	vec4 pos = g_mTransform * vec4(pos_attr.x, pos_attr.y, pos_attr.z, 1.0);
	gl_Position = vec4(pos.y, -pos.x, pos.z, pos.w);
	#else
	gl_Position = g_mTransform * vec4(pos_attr.x, pos_attr.y, pos_attr.z, 1.0);
	#endif

	col_var = col_attr * g_modcol;
	uv_var = uv_attr;
}
