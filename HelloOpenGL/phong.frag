#version 330 core

struct Material
{
    sampler2D diffuse1;
    sampler2D specular1;
    vec3 diffuseColor;
    vec3 specularColor;
    vec3 emissionColor;
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
};

struct PointLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

#define NUMBER_POINT_LIGHTS 16

in vec3 fragmentPosition;
in vec3 normal;
in vec2 textureCoordinates;

out vec4 fragmentColor;

uniform Material material;
uniform DirectionalLight directionalLight;
uniform SpotLight spotLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];

uniform vec3 viewPosition;

const float gamma = 2.2;

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normalizedNormal, vec3 viewDirection);
vec3 calculateSpotLight(SpotLight spotLight, vec3 normal, vec3 fragmentPosition, vec3 viewDirection);
vec3 calculatePointLight(PointLight pointLight, vec3 normalizedNormal, vec3 fragmentPosition, vec3 viewDirection);
vec3 calculateEmission();

void main()
{
    vec3 colorOutput = vec3(0.0f);

    vec3 normalizedNormal = normalize(normal);
    vec3 viewDirection = normalize(viewPosition - fragmentPosition);

    colorOutput += clamp(calculateDirectionalLight(directionalLight, normalizedNormal, viewDirection), 0.0f, 1.0f);
    colorOutput += clamp(calculateSpotLight(spotLight, normalizedNormal, fragmentPosition, viewDirection), 0.0f, 1.0f);
    for (int i = 0; i < NUMBER_POINT_LIGHTS; i++)
    {
        colorOutput += clamp(calculatePointLight(pointLights[i], normalizedNormal, fragmentPosition, viewDirection), 0.0f, 1.0f);
    }

    colorOutput += calculateEmission();

    fragmentColor = vec4(colorOutput, 1.0f);
}

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normalizedNormal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diffuseImpact = max(dot(normalizedNormal, lightDirection), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);

    vec3 ambient = directionalLight.ambient * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 diffuse = diffuseImpact * directionalLight.diffuse * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 specular = specularImpact * directionalLight.specular * clamp(texture(material.specular1, textureCoordinates).rgb + material.specularColor, 0.0f, 1.0f);

    return ambient + diffuse + specular;
}

vec3 calculateSpotLight(SpotLight spotLight, vec3 normalizedNormal, vec3 fragmentPosition, vec3 viewDirection)
{
    vec3 lightDirection = normalize(spotLight.position - fragmentPosition);
    float diffuseImpact = max(dot(lightDirection, normalizedNormal), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float specularImpact = pow(max(dot(normalizedNormal, halfwayDirection), 0.0f), material.shininess);

    float theta = dot(lightDirection, normalize(-spotLight.direction));
    float epsilon = spotLight.cutoff - spotLight.outerCutoff;
    float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0f, 1.0f);

    vec3 ambient = spotLight.ambient * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 diffuse = (intensity * diffuseImpact) * spotLight.diffuse * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 specular = (intensity * specularImpact) * spotLight.specular * clamp(texture(material.specular1, textureCoordinates).rgb + material.specularColor, 0.0f, 1.0f);

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight pointLight, vec3 normalizedNormal, vec3 fragmentPosition, vec3 viewDirection)
{
    vec3 lightDirection = normalize(spotLight.position - fragmentPosition);
    float diffuseImpact = max(dot(lightDirection, normalizedNormal), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    float specularImpact = pow(max(dot(normalizedNormal, reflectDirection), 0.0f), material.shininess);

    float lightDistance = length(pointLight.position - fragmentPosition);
    float attenuation = 1.0f / (lightDistance);

    vec3 ambient = pointLight.ambient * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 diffuse = (attenuation * diffuseImpact) * pointLight.diffuse * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 specular = (attenuation * specularImpact) * pointLight.specular * clamp(texture(material.specular1, textureCoordinates).rgb + material.specularColor, 0.0f, 1.0f);

    return ambient + diffuse + specular;
}

vec3 calculateEmission()
{
    vec3 emission = material.emissionColor;
    return emission;
}