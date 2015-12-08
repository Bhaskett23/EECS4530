#version 330 core

uniform mat4 modelingTransform;
uniform mat4 viewingTransform;
uniform mat4 projectionTransform;

in  vec4 vPosition;
in  vec4 vColor;
in  vec4 vNormal;
out vec4 color;
out vec3 fNormal;

void main() 
{
  gl_Position = viewingTransform * modelingTransform * projectionTransform * vPosition;
  //gl_Position = projectionTransform * modelingTransform *  vPosition;
  color = vColor;
  fNormal = vec3(0.0, 1.0, 0.0);
} 
