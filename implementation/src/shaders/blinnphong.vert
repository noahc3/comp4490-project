#version 150

in vec4 vPosition;
in vec4 vNormal;

smooth out vec3 Normal;
out vec3 LightDir, Eye, Halfway;

uniform mat4 ModelView, ModelViewInverseTranspose, View, Projection;
uniform vec4 LightDirection;
uniform vec3 WindDirection;

uniform int DrawFur;
uniform float ShellHeight;
uniform float ShellMaxHeight;

out vec3 bary;

void main()
{
    vec4 Position;

    if (DrawFur == 1) {
        bary = vec3(vPosition);
        Position = vPosition;
        Position.xyz += vNormal.xyz * ShellHeight * ShellMaxHeight;
        Position.xyz += WindDirection * ShellHeight * ShellMaxHeight;
        Position = ModelView * Position;
    } else {
		Position = ModelView * vPosition;
	}

    Normal = mat3(ModelViewInverseTranspose) * vNormal.xyz;
    LightDir = normalize((View * (LightDirection * 1000.0)).xyz);
    Eye = -Position.xyz;
    Halfway = normalize(LightDir + Eye);

    gl_Position = Projection * Position;
}
