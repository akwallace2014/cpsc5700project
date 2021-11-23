#version 130
in vec3 vPoint, vNormal;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

uniform vec3 color = vec3(1.000, 0.714, 0.757);
vec3 ambientLight = vec3(0.5f, 0.5f, 0.5f);
vec3 diffuseLight = vec3(1.0f, 1.0f, 1.0f);
vec3 specularLight = vec3(0.5f, 0.5f, 0.5f);
float lightConstant = 1.0f;
float lightLinear = 0.7f;
float lightQuadratic = 1.8f;

out vec4 pColor;
	
void main() {
        
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

    vec3 result = (ambient + diffuse + specular) * 0.5;

    pColor = vec4((result), 1.0);
}