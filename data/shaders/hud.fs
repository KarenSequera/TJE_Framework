varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_ratio;
uniform int u_selected;

void main()
{
	vec2 uv = v_uv;
	if (uv.x < u_ratio * float(u_selected) || uv.x > u_ratio * float(u_selected + 1))
	{
		discard;
	}
	
	gl_FragColor = u_color * texture2D(u_texture, uv);
}