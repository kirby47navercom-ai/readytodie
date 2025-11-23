#pragma once
#include "dtd.h"

unsigned int loadTextureFromFile(const char* path,const std::string& directory);

// --- 뼈대 정보 구조체 (FBX 애니메이션용) ---
//struct BoneInfo {
//    int id;
//    glm::mat4 offset; // Assimp의 오프셋 행렬을 GLM 행렬로 변환해야 함
//};
void aiMatrix4x4ToGlm(const aiMatrix4x4* from,glm::mat4& to); // Assimp 행렬을 GLM으로 변환하는 함수 (구현 필요)

// --- 텍스처 정보 구조체 (MTL/FBX 재질용) ---
struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
	glm::vec3 diffuseColor = glm::vec3(1.0f);   // 재질 색상 추가: 텍스처가 없을 경우 사용할 확산색

	glm::vec3 specularColor = glm::vec3(0.0f); // Ks (거울 반사 색상)
	int shininess = 1;                         // Ns (반사 강도)
};

struct StaticVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct StaticMesh {
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO, VBO, EBO;

    void setupMesh();

    void Draw(GLuint shaderID) const;
};

class StaticModel {
private:
    void processNode(aiNode* node, const aiScene* scene);
    StaticMesh processMesh(aiMesh* mesh, const aiScene* scene);

public:
    std::vector<StaticMesh> meshes;
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    StaticModel(const std::string& objPath);

    void Draw(GLuint shaderID);
};
