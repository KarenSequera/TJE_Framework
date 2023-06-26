uniform vec3 u_Kd;
uniform vec3 u_Ka;
uniform vec3 u_Ks;
uniform vec3 u_light_color;
uniform vec3 u_light_dir;
uniform vec3 u_Ia;
uniform vec3 u_camera_pos;

varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_light_dir);
    vec3 V = normalize(u_camera_pos - v_world_position);
    vec3 R = reflect(-L, N);

    // Ambient
    vec3 ambient = u_Ka * u_Ia * u_Kd;

    // Diffuse
    float NdotL = max(0.0, dot(N, L));
    vec3 diffuse = u_Kd * u_light_color * NdotL;

    // Specular
    float s = 5;
    float RdotV = pow(max(0.0, dot(R, V)), s);
    vec3 specular = u_Ks * u_light_color * RdotV;

    // Combine all components to get the final color
    vec3 final_color = ambient + diffuse + specular;

    float dist = length(u_camera_pos - v_world_position);
    float fogMinDist = 300.0;   
    float fogMaxDist = 3500.0;   

    float fog_factor = clamp(1.0 - (fogMaxDist - dist) / (fogMaxDist - fogMinDist), 0.0, 1.0);

    vec3 fog_color = vec3(0.7, 0.7, 0.7); 

    final_color = mix( final_color, fog_color, fog_factor );

    gl_FragColor = vec4(final_color, 1.0)* 1.0;
}

