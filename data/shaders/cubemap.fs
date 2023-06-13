varying vec3 v_world_position;

uniform vec3 u_camera_position;
uniform samplerCube u_sky_texture;

void main()
{
	vec3 V = normalize(u_camera_position - v_world_position);

	// Read the sky texture (ignoring mipmaps to avoid problems)
	vec3 sky_color = textureCube( u_sky_texture, V, 0.0).xyz;
	gl_FragColor = vec4(sky_color, 1.0);
}
