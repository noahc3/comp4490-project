#version 150

out vec4 fColor;

smooth in vec3 Normal;
in vec3 LightDir, Eye, Halfway;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

uniform int DrawFur;
uniform float ShellHeight;
uniform float Thickness;
uniform int Resolution;
in vec3 bary;

const uint k = 1103515245U;

/* --- Random Noise --- */
// Source: https://www.shadertoy.com/view/XlXcW4 by Inigo Quilez
float hash( uvec2 x ) {
    x = ((x>>8U)^x.yx)*k;
    x = ((x>>8U)^x.yx)*k;
    x = ((x>>8U)^x.yx)*k;
    
    return x.x*(1.0/float(0xffffffffU));
}
/* -------------------- */

void main() 
{ 
        vec3 N = normalize(Normal);
        float diffuseIntensity = max(dot(LightDir, N), 0.0);
        vec4 diffuse = DiffuseProduct * diffuseIntensity;
        
        float specularIntensity = max(dot(N, Halfway), 0);
        vec4 specular = SpecularProduct * pow(specularIntensity, Shininess);

        if (dot(LightDir, N) < 0.0) {
            specular = vec4(0,0,0,1);
        }

        if (DrawFur == 1) {
            vec2 uvs = bary.xy * float(Resolution);

            float rand = hash(uvec2(uvs));
            vec2 local = fract(uvs) * 2 - 1;
            float dist = abs(length(local));


            if (dist > Thickness * (rand - ShellHeight)) {
                discard;
            }

            //fColor = vec4(vec3(dist), 1);
            //return;
        }


        fColor = AmbientProduct + diffuse + specular;
        fColor.a = 1;
}
