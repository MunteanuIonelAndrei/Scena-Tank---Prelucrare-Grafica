#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
float constant = 1.0f;

vec3 ambientFelinar;
vec3 specularFelinar;
vec3 difuzaFelinar;


//variabilele pentru lumina
uniform vec3 luminaColora;
uniform vec3 luminaObiect;

//variabila pentru a stabili daca e ceata sau nu
uniform int transmitere;
//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;

// ----constantele pentru lumina

float linear=4.0f;
float cuadratic=3.0f;



vec3 specular;
float specularStrength = 0.5f;
 vec4 fPosEye;

float computeFog()
{
    float fogDensity = 0.05f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 

    return clamp(fogFactor, 0.0f, 1.0f);
}
void computeLightOne()
{
   //eye spae coordinates pt obiecte raman aceleasi
    //compute eye space coordinates
    vec4 fPosEyeF = view * model * vec4(fPosition, 1.0f);
    vec3 normalEyeF = normalize(normalMatrix * fNormal);
    //compute light direction for felinar
     //duc coord felinarului in eye space
      vec4 lightPosEyeFelinar = view * model * vec4(luminaObiect, 1.0f);
      vec3 lightDirNFelinar = vec3(normalize(vec3(lightPosEyeFelinar) - fPosEyeF.xyz));

  //viewDir e aceeasi ca e acelasi privitoru adica camera
  vec3 viewDirF = normalize(- fPosEyeF.xyz);


   //pt atenuare
     //compute distance to light
            float dist = length(lightPosEyeFelinar - fPosEyeF);
     //compute attenuation
           float att = 1.0f / (constant + linear* dist + cuadratic * (dist * dist));

 

    //compute ambient light
    ambientFelinar = att*0.9 * luminaObiect;

 

    //compute diffuse light
    difuzaFelinar = att*max(dot(normalEyeF, lightDirNFelinar), 0.0f) * luminaObiect;

 

    //compute specular light
    vec3 reflectDirFelinar = reflect(-lightDirNFelinar, normalEyeF);
    float specCoeffFelinar = pow(max(dot(viewDirF, reflectDirFelinar), 0.0f), 32);
    specularFelinar = att*3.0 * specCoeffFelinar * luminaObiect;

 



}

void computeDirLight()
{
    //compute eye space coordinates
    fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}

void main() 
{
    if(texture(diffuseTexture, fTexCoords).a<0.1f)
    discard;
    computeDirLight();
    computeLightOne();
      //compute final vertex color
    vec3 color = min((ambient + ambientFelinar+difuzaFelinar+diffuse) * texture(diffuseTexture, fTexCoords).rgb + (specular+specularFelinar) * texture(specularTexture, fTexCoords).rgb, 1.0f);

 

    float fogFactor=computeFog();
    vec4  fogColor=vec4(0.5f, 0.5f, 0.5f, 1.0f);
     if(transmitere==0)
   {
   fColor = vec4(color, 1.0f);
   }
   else
   if(transmitere==1)
   {

    fColor= fogColor*(1-fogFactor)+ vec4(color, 0.0f)*fogFactor;
    }

 

}
