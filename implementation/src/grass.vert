#version 150

uniform vec3 WindDirection;

in vec4 vPosition;
uniform mat4 ModelView, Projection;
uniform float Height;
uniform float MaxHeight;
out vec2 uv;

void main()
{
  gl_Position = Projection * ModelView * (vPosition + (vec4(WindDirection.x, 0, WindDirection.z, 0.0) * MaxHeight * Height)) + vec4(0, MaxHeight * Height, 0, 0);
  uv = vPosition.xz + 0.5f;
}

