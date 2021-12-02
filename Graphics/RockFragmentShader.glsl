#version 130

// inputs
in vec3 vPoint, vNormal;
in vec2 vUv;

// uniforms
uniform sampler2D textureImage;
uniform int numLights;
uniform vec3 lights[5];
uniform vec3 viewPosition;

// constants
vec3 ambientLight = vec3(0.0f, 0.0f, 0.0f);
vec3 diffuseLight = vec3(1.0f, 1.0f, 1.0f);
vec3 specularLight = vec3(0.5f, 0.5f, 0.5f);
float lightConstant = 1.0f;
float lightLinear = 1.0f;
float lightQuadratic = 5.0f;

// outputs
out vec4 pColor;

vec3 calcPointLight(vec3 lightPosition) {
    vec3 color = texture(textureImage, vUv).rgb;
    vec3 ambient = ambientLight * color;

    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPosition - vPoint);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseLight * diff * color;

    vec3 viewDir = normalize(viewPosition - vPoint);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 128.0f); 
    vec3 specular = specularLight * spec * color;

    float distance = length(lightPosition - vPoint);
    float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}
	
void main() {
    vec3 result;
    for (int i = 0; i < numLights; i++) {
        result += calcPointLight(lights[i]);
    }
   
    pColor = vec4((result), 1.0);
}