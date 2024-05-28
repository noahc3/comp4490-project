#version 150

uniform vec3 BaseColour;
uniform vec3 TopColour;
uniform float Height;
uniform float Thickness;
uniform int PlaneResolution;
in vec2 uv;
out vec4 out_colour;

const uint k = 1103515245U;

float hash( uvec2 x ) {
    x = ((x>>8U)^x.yx)*k;
    x = ((x>>8U)^x.yx)*k;
    x = ((x>>8U)^x.yx)*k;
    
    return x.x*(1.0/float(0xffffffffU));
}

void main() 
{
    vec2 uvs = uv * float(PlaneResolution);

    float rand = hash(uvec2(uvs));
    vec2 local = fract(uvs) * 2 - 1;
    float dist = length(local);


    if (Height > 0 && dist > Thickness * (rand - Height)) {
        discard;
    }

    out_colour = vec4(mix(BaseColour, TopColour, Height), 1.0);
}
