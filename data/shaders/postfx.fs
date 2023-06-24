varying vec2 v_uv;

uniform sampler2D u_texture;
uniform vec3 u_options;

#define CONTRAST 1.2
#define ABERRATION_OFFSET 0.005

vec3 colorCorrection(vec3 c_in){
	vec3 color = c_in;
	color.b = (color.b - 0.15) * CONTRAST;

	float avg = (color.r + color.g + color.y) / 3.0;

	color.r = mix(color.r, avg, 0.5);
	color.g = mix(color.g, avg, 0.5);
	color.b = mix(color.b, avg, 0.5);

	return color;
}

vec3 vigneting(vec3 c_in, vec2 uv)
{
	vec3 color = c_in;
	color *= 1.1 - length(uv - vec2(0.5));
	return color;
}

vec3 chromaticAberration(vec2 uv)
{
	vec3 color = vec3(0.0);
	color.r = texture2D( u_texture, uv + ABERRATION_OFFSET ).r;
	color.g = texture2D( u_texture, uv ).g;
	color.b = texture2D( u_texture, uv - ABERRATION_OFFSET ).b;
	return color;
}

void main()
{
	vec2 uv = v_uv;

	vec3 color = vec3(0.0);

	if(u_options.x > 0.0)
		color = chromaticAberration(uv);
	else
		color = texture2D( u_texture, uv ).xyz;

	if(u_options.y > 0.0)
		color = colorCorrection(color);

	if(u_options.z > 0.0)
		color = vigneting(color, uv);

	gl_FragColor = vec4(color, 1.0);
}	
