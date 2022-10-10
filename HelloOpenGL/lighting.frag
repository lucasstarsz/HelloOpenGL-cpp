#version 330 core

in vec3 normal;
in vec3 fragmentPosition;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

out vec4 fragmentColor;

void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 normalizedNormal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float diffuseImpact = max(dot(normalizedNormal, lightDirection), 0.0f);
    vec3 diffuse = diffuseImpact * lightColor;

    float specularStrength = 0.5f;
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);
    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), 32);
    vec3 specular = specularStrength * specularImpact * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragmentColor = vec4(result, 1.0);
}
