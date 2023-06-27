varying vec2 v_uv;

uniform sampler2D u_texture;


vec3 chromaticAberration(vec2 uv)
{
	vec3 color = vec3(0.0);
	color.r = texture2D( u_texture, uv + 0.01 ).r;
	color.g = texture2D( u_texture, uv ).g;
	color.b = texture2D( u_texture, uv - 0.01 ).b;
	return color;
}

void main()
{
	vec2 uv = v_uv;

	vec3 color = vec3(0.0);

	color = chromaticAberration(uv);
	
	gl_FragColor = vec4(color, 1.0);
}	
