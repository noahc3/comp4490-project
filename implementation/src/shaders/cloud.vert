#version 150

uniform vec3 WindDirection;

in vec4 vPosition;
uniform mat4 ModelView, Projection;
uniform float Height;
uniform float Thickness;
out vec2 uv;

void main()
{
  gl_Position = Projection * ModelView * vPosition + vec4(0, Height * Thickness, 0, 0);
  uv = vPosition.xz + 0.5f;
}

