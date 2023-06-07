varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform vec3 u_camera_position;
uniform samplerCube u_texture;

void main()
{
	vec3 N = normalize(v_normal);
	vec3 V = normlaize(u_camera_position - v_world_position);
	vec3 R = reflect(V, N);
	vec2 uv = v_uv;
	gl_FragColor = u_color * texture2D( u_texture, uv );
}
