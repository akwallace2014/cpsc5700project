#version 130
in vec3 vPoint, vNormal;
in vec2 vUv;
uniform sampler2D textureImage;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

vec3 ambientLight = vec3(0.1f, 0.1f, 0.1f);
vec3 diffuseLight = vec3(1.0f, 1.0f, 1.0f);
vec3 specularLight = vec3(0.5f, 0.5f, 0.5f);
float lightConstant = 1.0f;
float lightLinear = 0.7f;
float lightQuadratic = 1.8f;

out vec4 pColor;
	
void main() {
    
    vec3 color = texture(textureImage, vUv).rgb;
    vec3 ambient = ambientLight * color;

    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPosition - vPoint);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseLight * diff * color;

    vec3 viewDir = normalize(viewPosition - vPoint);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32.0f); 
    vec3 specular = specularLight * spec * color;

    float distance = length(lightPosition - vPoint);
    float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular);

    pColor = vec4((result), 1.0);
}