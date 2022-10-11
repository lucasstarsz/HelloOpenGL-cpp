#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};

struct DirectionalLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;
};

struct SpotLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 position;
    vec3 direction;
    float cutoff;
    float outerCutoff;

    float constant;
    float linear;
    float quadratic;
};

struct PointLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;

    float constant;
    float linear;
    float quadratic;
};

#define NUMBER_POINT_LIGHTS 4

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinates;

out vec4 fragmentColor;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];

uniform vec3 viewPosition;
uniform float time;

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normalizedNormal, vec3 viewDirection);
vec3 calculateSpotLight(SpotLight spotLight, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 calculatePointLight(PointLight pointLight, vec3 normalizedNormal, vec3 fragmentPosition, vec3 viewDirection);
vec3 calculateEmission();

void main()
{
    vec3 colorOutput = vec3(0.0f);

    vec3 normalizedNormal = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);

    colorOutput += calculateDirectionalLight(directionalLight, normalizedNormal, viewDirection);
    colorOutput += calculateSpotLight(spotLight, normalizedNormal, fragmentPosition, viewDirection);
    colorOutput += calculateEmission();

    for (int i = 0; i < NUMBER_POINT_LIGHTS; i++)
    {
        colorOutput += calculatePointLight(pointLights[i], normalizedNormal, fragmentPosition, viewDirection);
    }

    fragmentColor = vec4(colorOutput, 1.0f);
}


vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normalizedNormal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diffuseImpact = max(dot(normalizedNormal, lightDirection), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess + 1.0f);

    vec3 ambient = directionalLight.ambient * material.ambient;
    vec3 diffuse = diffuseImpact * directionalLight.diffuse * material.diffuse;
    vec3 specular = specularImpact * directionalLight.specular * material.specular;

    return ambient + diffuse + specular;
}

vec3 calculateSpotLight(SpotLight spotLight, vec3 normal, vec3 fragmentPosition, vec3 viewDirection)
{
    vec3 lightDirection = normalize(spotLight.position - fragmentPosition);
    float diffuseImpact = max(dot(normal, lightDirection), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);

    float lightDistance = length(spotLight.position - fragmentPosition);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * lightDistance + spotLight.quadratic * (lightDistance * lightDistance));

    float theta = dot(lightDirection, normalize(-spotLight.direction));
    float epsilon = spotLight.cutoff - spotLight.outerCutoff;
    float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0f, 1.0f);

    vec3 ambient = spotLight.ambient * material.ambient;
    vec3 diffuse = (attenuation * intensity * diffuseImpact) * spotLight.diffuse * material.diffuse;
    vec3 specular = (attenuation * intensity * specularImpact) * spotLight.specular * material.specular;

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight pointLight, vec3 normalizedNormal, vec3 fragmentPosition, vec3 viewDirection)
{
    vec3 lightDirection = normalize(spotLight.position - fragmentPosition);
    float diffuseImpact = max(dot(normal, lightDirection), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);

    float lightDistance = length(pointLight.position - fragmentPosition);
    float attenuation = 1.0f / (pointLight.constant + (pointLight.linear * lightDistance) + pointLight.quadratic * (lightDistance * lightDistance));

    vec3 ambient = pointLight.ambient * material.ambient;
    vec3 diffuse = (attenuation * diffuseImpact) * pointLight.diffuse * material.diffuse;
    vec3 specular = (attenuation * specularImpact) * pointLight.specular * material.specular;

    return ambient + diffuse + specular;
}

vec3 calculateEmission()
{
    // map will only apply emission for completely black parts of specular lighting.
    vec3 invertedBlackSpecularMap = floor(1.0f - material.specular);
    vec3 emission = invertedBlackSpecularMap * material.emission;

    return emission;
}
