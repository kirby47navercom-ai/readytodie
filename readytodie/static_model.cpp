#include "static_model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ===================================================================
// StaticMesh
// ===================================================================

unsigned int loadTextureFromFile(const char* path,const std::string& directory)
{
	std::string filename = std::string(path);

	// 1. 역슬래시(\)를 슬래시(/)로 통일 (Windows 경로 호환성 해결)
	std::replace(filename.begin(),filename.end(),'\\','/');

	// 2. 경로 결합 logic 수정
	if(!directory.empty() && directory != ".") {
		filename = directory + '/' + filename;
	}
	// (선택) 만약 directory가 "."이고 filename이 "maps/..."로 시작하면 그대로 둠

	unsigned int textureID;
	glGenTextures(1,&textureID);

	int width,height,nrComponents;

	// 3. [중요] PNG/JPG 로딩 시 메모리 정렬 문제 방지 (이거 없으면 텍스처가 기울거나 검게 나옴)
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	// 텍스처 로드 시도
	unsigned char* data = stbi_load(filename.c_str(),&width,&height,&nrComponents,0);

	if(data)
	{
		GLenum format = GL_RGB; // 기본값
		if(nrComponents == 1)
			format = GL_RED;
		else if(nrComponents == 3)
			format = GL_RGB;
		else if(nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D,textureID);
		glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// 텍스처 파라미터 설정
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		// 로드 성공 메시지 (디버깅용)
		//std::cout << "Texture Loaded: " << filename << " (" << width << "x" << height << ")" << std::endl;
	} else
	{
		// 4. 로드 실패 시 정확한 경로 출력
		std::cerr << "ERROR::TEXTURE::Failed to load at path: [" << filename << "]" << std::endl;
		std::cerr << "Reason: " << stbi_failure_reason() << std::endl; // 실패 이유 출력
		stbi_image_free(data);
		glDeleteTextures(1,&textureID); // 실패한 텍스처 ID 삭제
		return 0;
	}

	stbi_image_free(data);
	return textureID;
}

void StaticMesh::setupMesh() {
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(StaticVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 위치 속성 (layout=0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)0);
    // 법선 속성 (layout=1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, Normal));
    // 텍스처 좌표 속성 (layout=2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticVertex), (void*)offsetof(StaticVertex, TexCoords));

    glBindVertexArray(0);
}

void StaticMesh::Draw(GLuint shaderID) const {
    const Texture& meshTexture = textures[0];

    // 1. 텍스처/Diffuse 색상 설정
    if (!textures.empty() && meshTexture.id != 0) {
        // ... (기존 텍스처 로직 유지)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, meshTexture.id);
        glUniform1i(glGetUniformLocation(shaderID, "texture_diffuse1"), 0);
        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), true);
    }
    else {
        // 텍스처 로드 실패 또는 텍스처가 없는 경우:
        glUniform1i(glGetUniformLocation(shaderID, "bUseTexture"), false);

        // MTL의 Kd를 사용하되, 너무 어두우면 중간 회색을 기본값으로 사용 (선택적)
        glm::vec3 diffuseColorToUse = meshTexture.diffuseColor;

        // Kd가 너무 어두워서 흑백으로 보인다면, 기본 회색으로 대체 (디버그용 안전 장치)
        if (glm::length(diffuseColorToUse) < 0.1f) { // 흑색 Kd (0,0,0)에 가까운 경우
            diffuseColorToUse = glm::vec3(0.5f, 0.5f, 0.5f); // 중간 회색으로 강제 변경
        }

        glUniform3fv(glGetUniformLocation(shaderID, "materialColorDefault"), 1, glm::value_ptr(diffuseColorToUse));
    }

    if (!textures.empty()) {
        glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(meshTexture.specularColor));
        glUniform1i(glGetUniformLocation(shaderID, "shininess"), meshTexture.shininess);
    }

    //glm::vec3 exampleSpecular = glm::vec3(0.35f, 0.35f, 0.35f); // Ks 
    //int exampleShininess = 32;                                   // Ns 
    //glUniform3fv(glGetUniformLocation(shaderID, "materialSpecular"), 1, glm::value_ptr(exampleSpecular));
    //glUniform1i(glGetUniformLocation(shaderID, "shininess"), exampleShininess);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ===================================================================
// StaticModel
// ===================================================================
StaticModel::StaticModel(const std::string& objPath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // 텍스처/재질 로딩은 OBJ 파일의 경로에 따라 다름 (구현 필요)

    processNode(scene->mRootNode, scene);
}

void StaticModel::Draw(GLuint shaderID) {
    for (auto& mesh : meshes) mesh.Draw(shaderID);
}

void StaticModel::processNode(aiNode* node, const aiScene* scene)
{
    // 현재 노드가 가진 모든 메시를 처리합니다.
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // 자식 노드를 재귀적으로 순회하며 처리합니다.
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

StaticMesh StaticModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // 텍스처 로딩 구현 필요

    // 1. 정점 데이터 추출
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        StaticVertex vertex;
        // 위치 (Position)
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        // 법선 (Normal)
        if (mesh->mNormals)
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        // 텍스처 좌표 (TexCoords)
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // 2. 인덱스 데이터 (Faces) 추출
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 3. 재질/텍스처 처리 (구현)
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        Texture matInfo;

        // 👇 Diffuse Color (Kd) 추출 코드 추가/복구!
        aiColor4D color_d(1.f, 1.f, 1.f, 1.f); // 기본값 설정 (흰색)
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color_d)) {
            matInfo.diffuseColor = glm::vec3(color_d.r, color_d.g, color_d.b);
        }

        // 3-1. Specular Color (Ks) 추출 및 저장
        aiColor4D color_s(0.f, 0.f, 0.f, 1.f);
        if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color_s)) {
            matInfo.specularColor = glm::vec3(color_s.r, color_s.g, color_s.b);
        }
        float shininess_val = 1.0f;
        if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess_val)) {
            matInfo.shininess = (int)shininess_val;
        }
        // 👇 Ks 값 강제 조정 (하이라이트 강도 줄이기)
        if (glm::length(matInfo.specularColor) > 1.0f) { // Ks가 너무 강하면 (예: 1.0, 1.0, 1.0)
            matInfo.specularColor *= 0.05f; // Specular 강도를 35%로 조정 (이전 사용자 설정 기반)
        }

        // 3-2. 텍스처 파일 경로 추출 및 로딩
        aiString str;
        // AI_MATKEY_TEXTURE_DIFFUSE: MTL 파일의 map_Kd (확산 텍스처) 경로를 찾습니다.
        if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &str))
        {
            // 경로가 있다면 텍스처 로딩 시도
            matInfo.path = str.C_Str();
            // 텍스처 로드 함수 호출 (현재 파일 경로를 기준으로 로드)
            // OBJ 파일이 있는 "현재 디렉토리"를 가정하고 경로 전달
            matInfo.id = loadTextureFromFile(matInfo.path.c_str(), ".");
            matInfo.type = "texture_diffuse";
        }
        else {
            // 텍스처 경로가 없는 경우, ID를 0으로 설정하여 텍스처를 사용하지 않도록 표시
            matInfo.id = 0;
            matInfo.type = "color_diffuse";
        }

        // 재질 정보를 메시의 첫 번째 텍스처 항목에 저장
        textures.push_back(matInfo);
    }

    // 메시 객체 생성 및 설정
    StaticMesh staticMesh;
    staticMesh.vertices = vertices;
    staticMesh.indices = indices;
    staticMesh.textures = textures;
    staticMesh.setupMesh(); // VAO/VBO/EBO 설정 호출

    return staticMesh;
}