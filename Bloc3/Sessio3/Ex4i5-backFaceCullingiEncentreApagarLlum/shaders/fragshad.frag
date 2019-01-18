#version 330 core

// in vec3 fcolor;
in vec4 vertexFS;
in vec3 normalFS;

in vec3 matambFS;
in vec3 matdiffFS;
in vec3 matspecFS;
in float matshinFS;

out vec4 FragColor;

uniform mat4 view;
uniform vec3 colFocus;
uniform vec3 posFocus;
uniform vec3 llumAmbient;
uniform bool focusEscena;


vec3 Lambert (vec3 NormSCO, vec3 L)
{ // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

  // Inicialitzem color a component ambient
  vec3 colRes = llumAmbient * matambFS;

  // Afegim component difusa, si n'hi ha
  if (dot (L, NormSCO) > 0) colRes = colRes + colFocus * matdiffFS * dot (L, NormSCO);
  return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO)
{
  // Els vectors estan normalitzats

  // Inicialitzem color a Lambert
  vec3 colRes = Lambert (NormSCO, L);

  // Calculem R i V
  if (dot(NormSCO,L) < 0) return colRes;  // no hi ha component especular

  vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
  vec3 V = normalize(-vertSCO.xyz);

  if ((dot(R, V) < 0) || (matshinFS == 0)) return colRes;  // no hi ha component especular

  // Afegim la component especular
  float shine = pow(max(0.0, dot(R, V)), matshinFS);
  return (colRes + matspecFS * colFocus * shine);
}

void main ()
{
  // L + normalitzarla
  vec3 Lfragment;
  if (focusEscena)    //si és focusEscena, multiplico per view
  {
    vec4 posFocusNew = view * vec4(posFocus, 1.0);
    Lfragment = normalize((posFocusNew-vertexFS).xyz);
  }
  else Lfragment = normalize(posFocus - vertexFS.xyz);

	// Normal
	vec3 normalNew = normalize(normalFS);

	// Phong
	FragColor = vec4 (Phong(normalNew, Lfragment, vertexFS), 1);
}
