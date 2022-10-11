#version 330 core

struct Material
{
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D emission1;
    vec3 diffuseColor;
    vec3 specularColor;
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
uniform float time;

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
    colorOutput += clamp(calculateEmission(), 0.0f, 1.0f);

    for (int i = 0; i < NUMBER_POINT_LIGHTS; i++)
    {
        colorOutput += clamp(calculatePointLight(pointLights[i], normalizedNormal, fragmentPosition, viewDirection), 0.0f, 1.0f);
    }

    fragmentColor = vec4(colorOutput, 1.0f);

    fragmentColor.rgb = pow(fragmentColor.rgb, vec3(1.0 / gamma));
}

vec3 calculateDirectionalLight(DirectionalLight directionalLight, vec3 normalizedNormal, vec3 viewDirection)
{
    vec3 lightDirection = normalize(-directionalLight.direction);
    float diffuseImpact = max(dot(normalizedNormal, lightDirection), 0.0f);

    vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.shininess);

    vec3 ambient = directionalLight.ambient * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 diffuse = diffuseImpact * directionalLight.diffuse * pow(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, vec3(gamma));
    vec3 specular = specularImpact * directionalLight.specular * (texture(material.specular1, textureCoordinates).rgb + material.specularColor);

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

    vec3 ambient = spotLight.ambient * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 diffuse = (attenuation * intensity * diffuseImpact) * spotLight.diffuse * pow(clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f), vec3(gamma));
    vec3 specular = (attenuation * intensity * specularImpact) * spotLight.specular * clamp(texture(material.specular1, textureCoordinates).rgb + material.specularColor, 0.0f, 1.0f);

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

    vec3 ambient = pointLight.ambient * clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f);
    vec3 diffuse = (attenuation * diffuseImpact) * pointLight.diffuse * pow(clamp(texture(material.diffuse1, textureCoordinates).rgb + material.diffuseColor, 0.0f, 1.0f), vec3(gamma));
    vec3 specular = (attenuation * specularImpact) * pointLight.specular * clamp(texture(material.specular1, textureCoordinates).rgb + material.specularColor, 0.0f, 1.0f);

    return ambient + diffuse + specular;
}

vec3 calculateEmission()
{
    // map will only apply emission for completely black parts of specular lighting.
    vec3 invertedBlackSpecularMap = floor(1.0f - clamp(texture(material.specular1, textureCoordinates).rgb + material.specularColor, 0.0f, 1.0f));
    vec3 emission = invertedBlackSpecularMap * texture(material.emission1, textureCoordinates + vec2(0.0f, time)).rgb;

    return emission;
}
