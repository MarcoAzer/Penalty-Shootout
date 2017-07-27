#version 330

// Model-Space coordinates
in vec3 position;
in vec3 normal;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};
uniform LightSource light;

uniform mat4 ModelView;
uniform mat4 Perspective;

// Remember, this is transpose(inverse(ModelView)).  Normals should be
// transformed using this matrix instead of the ModelView matrix.
uniform mat3 NormalMatrix;

out VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
} vs_out;

// Texture mapping
out vec2 UV;
in vec2 uv;
out vec3 skyboxUV;

// Normal Mapping
in vec3 tangents;
in vec3 bitangents;

out mat3 TBN;

void main() {
	vec4 pos4 = vec4(position, 1.0);

	//-- Convert position and normal to Eye-Space:
	vs_out.position_ES = (ModelView * pos4).xyz;
	vs_out.normal_ES = normalize(NormalMatrix * normal);

    vec3 vertexTangent_cameraspace = normalize(mat3(ModelView) * tangents);
    vec3 vertexBitangent_cameraspace = normalize(mat3(ModelView) * bitangents);
    vec3 vertexNormal_cameraspace = normalize(mat3(ModelView) * normal);

    //vec3 vertexTangent_cameraspace = mat3(ModelView) * tangents;
    //vec3 vertexBitangent_cameraspace = mat3(ModelView) * bitangents;
    //vec3 vertexNormal_cameraspace = mat3(ModelView) * normal;

    TBN = mat3(vertexTangent_cameraspace, vertexBitangent_cameraspace, vertexNormal_cameraspace);

	vs_out.light = light;

	gl_Position = Perspective * ModelView * vec4(position, 1.0);

    UV = uv;
    skyboxUV = position;
}
