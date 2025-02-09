#ifndef INNSMOUTH_LIGHT_FRAG
#define INNSMOUTH_LIGHT_FRAG

vec3 phong(vec3 color, vec3 light_color, vec3 N, vec3 L, vec3 view_position, vec3 fragment_position) {
  vec3 V = normalize(view_position - fragment_position);
  vec3 H = normalize(L + V);

  float NdotH = max(dot(N, H), 0.0);
  float NdotL = max(dot(L, N), 0.0);

  vec3 ambient  = 0.1 * color;
  vec3 diffuse  = NdotL * color;
  float specular = pow(NdotH, 32.0);

  vec3 result = ambient + diffuse + specular * light_color;

  return result;
}


#endif // INNSMOUTH_LIGHT_FRAG