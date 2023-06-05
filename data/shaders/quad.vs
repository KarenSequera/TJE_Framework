attribute vec3 a_vertex;
attribute vec2 a_uv;
attribute vec4 a_color;

uniform mat4 u_viewprojection;

varying vec3 v_position;
varying vec3 v_world_position;
varying vec2 v_uv;
varying vec4 v_color;


void main()
{	
	v_position = a_vertex;
	v_world_position = v_position;
	
	v_color = a_color;

	//store the texture coordinates
	v_uv = a_uv;

	//calcule the position of the vertex using the matrices
	gl_Position = u_viewprojection * vec4( v_world_position, 1.0 );
}