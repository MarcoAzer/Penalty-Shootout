#version 330

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
} fs_in;


out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

in vec2 UV;
uniform int hasTexture;
uniform sampler2D myTexture;
in vec3 skyboxUV;
uniform samplerCube skybox;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

in mat3 TBN;
uniform sampler2D fieldNormalMap;
uniform sampler2D wallNormalMap;
vec3 normal;

vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
	diffuse = material.kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow(n_dot_h, material.shininess);
    }

    return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

void main() {
    if( hasTexture == 4 ){
        normal = texture(wallNormalMap, UV).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
	    fragColour = vec4(phongModel(fs_in.position_ES, normal), 1.0);
    }
    else if(hasTexture == 3){
        fragColour = texture(skybox, skyboxUV);
    }
    else if(hasTexture == 2){
        normal = texture(fieldNormalMap, UV).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
	    fragColour = vec4(phongModel(fs_in.position_ES, normal), 1.0);
    }
    else if(hasTexture == 1){
        fragColour = texture( myTexture, UV );
    }
    else if(hasTexture == 0){ 
	    fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    }
}
