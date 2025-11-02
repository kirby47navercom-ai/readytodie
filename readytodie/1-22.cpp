#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "dtd.h" 
#define SIZE 600
// 함수 선언
void MakeVertexShaders();
void MakeFragmentShaders();
GLuint MakeShaderProgram();
void InitBuffers();
GLvoid DrawScene();
GLvoid Reshape(int w,int h);
void Mouse(int button,int state,int x,int y);
void Keyboard(unsigned char key,int x,int y);
void SpecialKeyboard(int key,int x,int y); // 특수 키(화살표) 처리 함수 선언
void LoadOBJ(const char* filename);
void InitBuffer ();
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
void TimerFunction (int value);
GLchar* filetobuf(const char *file);
// 전역 변수
GLint width = 600,height = 600;
GLuint shaderProgramID,vertexShader,fragmentShader;
GLuint VAO,VBO,EBO;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> rcolor(0.0f,1.0f);
uniform_int_distribution<int> Rint(1,9);

bool checkbox[10] = {false,};

typedef struct {
	float x,y,z;
} Vertex;
typedef struct {
	unsigned int v1,v2,v3;
} Face;
typedef struct {
	Vertex* vertices;
	size_t vertex_count;
	Face* faces;
	size_t face_count;
} Model;
class Shape{
public:
	vector<GLfloat> vertexData;
	vector<GLuint> indices;
	glm::vec3 t={0.0f,0.0f,0.0f};
	glm::vec3 s={1.0f,1.0f,1.0f};
	glm::vec3 r={0.0f,0.0f,0.0f};
	glm::vec3 colors;
	glm::mat4 modelMat = glm::mat4(1.0f);
	glm::vec3 orimodelMat = glm::vec3(0.0f);
	int shape_num;
	float angle=0.0f;
	Shape(Model model,int i){
		vertexData.clear();
		InitBuffer(model);
		ColorRandom(model);
		shape_num = i;
	}

	void InitBuffer(Model model){
		vertexData.clear();
		// 모든 face의 꼭짓점 좌표를 중복 포함해서 vertexData에 추가
		for(size_t i = 0; i < model.face_count; ++i) {
			Face f = model.faces[i];
			Vertex v1 = model.vertices[f.v1];
			Vertex v2 = model.vertices[f.v2];
			Vertex v3 = model.vertices[f.v3];
			// 첫 번째 꼭짓점
			vertexData.push_back(v1.x);
			vertexData.push_back(v1.y);
			vertexData.push_back(v1.z);
			// 두 번째 꼭짓점
			vertexData.push_back(v2.x);
			vertexData.push_back(v2.y);
			vertexData.push_back(v2.z);
			// 세 번째 꼭짓점
			vertexData.push_back(v3.x);
			vertexData.push_back(v3.y);
			vertexData.push_back(v3.z);
		}
		// 인덱스는 필요 없으므로 비워둡니다
		indices.clear();
	}
	void Update(Model model){
		//vertexData.clear();
		//for(size_t i = 0; i < model.vertex_count; ++i) {
		//	Vertex v = model.vertices[i];
		//	// OBJ 좌표를 OpenGL에 맞게 사용 (스케일링 제거, 0~1 범위 그대로)
		//	float x = v.x;
		//	float y = v.y;
		//	float z = v.z;
		//	vertexData.push_back(x);
		//	vertexData.push_back(y);
		//	vertexData.push_back(z);
		//}
		//indices.clear();
		//for(size_t i = 0; i < model.face_count; ++i) {
		//	indices.push_back(model.faces[i].v1);
		//	indices.push_back(model.faces[i].v2);
		//	indices.push_back(model.faces[i].v3);
		//}
	}
	void ColorRandom(Model model){
		colors={0.0f,0.0f,0.0f};

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colors=glm::vec3(r,g,b);

	}
	void move(float x,float y,float z){
		for(size_t i=0;i<vertexData.size()/3;++i){
			vertexData[3*i] += x;
			vertexData[3*i+1] += y;
			vertexData[3*i+2] += z;
		}
	}
	void Rotate(float x,float y,float z,
				   float x_rotate,float y_rotate,float z_rotate,
				   int index)
	{
		// vertexData: [x0, y0, z0, x1, y1, z1, ...]
		int start = index * 3;
		if(start + 2 >= vertexData.size()) return; // 범위 확인

		// 회전할 정점 하나
		glm::vec3 v(vertexData[start],vertexData[start + 1],vertexData[start + 2]);

		// 회전 행렬 준비
		glm::mat4 model(1.0f);
		model = glm::translate(model,glm::vec3(x,y,z)); // 중심 이동

		if(x_rotate != 0.0f)
			model = glm::rotate(model,glm::radians(x_rotate),glm::vec3(1,0,0));
		if(y_rotate != 0.0f)
			model = glm::rotate(model,glm::radians(y_rotate),glm::vec3(0,1,0));
		if(z_rotate != 0.0f)
			model = glm::rotate(model,glm::radians(z_rotate),glm::vec3(0,0,1));

		model = glm::translate(model,glm::vec3(-x,-y,-z)); // 원위치

		// 회전 적용
		glm::vec4 v4 = model * glm::vec4(v,1.0f);

		vertexData[start + 0] = v4.x;
		vertexData[start + 1] = v4.y;
		vertexData[start + 2] = v4.z;
	}
	void RotateWorld(float cx,float cy,float cz,float angle,int index,int num)
	{
		// 회전할 점의 위치 읽기
		float& vx = vertexData[index * 3 + 0];
		float& vy = vertexData[index * 3 + 1];
		float& vz = vertexData[index * 3 + 2];

		// 기준점 중심으로 이동
		float tx = vx - cx;
		float ty = vy - cy;
		float tz = vz - cz;

		// 월드 z축 기준 회전 (x, y만 변함)
		float cosA = cos(angle);
		float sinA = sin(angle);

		float rx=0.0f;
		float ry=0.0f;
		float rz=0.0f;

		if(num==0){
			rx = tx * cosA - ty * sinA;
			ry = tx * sinA + ty * cosA;
			rz = tz; // z축 회전이므로 z는 그대로 유지
		} else if(num==1){
			//x축회전
			rx = tx;
			ry = ty * cosA - tz * sinA;
			rz = ty * sinA + tz * cosA;
		} else{
			//y축회전
			rx = tx * cosA + tz * sinA;
			ry = ty;
			rz = -tx * sinA + tz * cosA;
		}


		// 다시 원래 위치로 복귀
		vx = rx + cx;
		vy = ry + cy;
		vz = rz + cz;
	}
	void Scale(float cx,float cy,float cz,float scale,int index) {
		int start = index * 3;
		if(start + 2 >= vertexData.size()) return;
		vertexData[start + 0] = (vertexData[start + 0] - cx) * scale + cx;
		vertexData[start + 1] = (vertexData[start + 1] - cy) * scale + cy;
		vertexData[start + 2] = (vertexData[start + 2] - cz) * scale + cz;
	}
	glm::vec3 Center(){
		glm::vec3 center(0.0f);
		for(int i = 0; i < vertexData.size()/3; ++i) {
			center.x += vertexData[i * 3 + 0];
			center.y += vertexData[i * 3 + 1];
			center.z += vertexData[i * 3 + 2];
		}
		center /= static_cast<float>(vertexData.size()/3);
		return center;
	}
	glm::vec3 ToWorldPos(const glm::vec3& localPos)
	{
		glm::vec4 world = modelMat * glm::vec4(localPos,1.0f);
		return glm::vec3(world);
	}


	void draw(){
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
		glBindVertexArray(0);
	}


};
vector<Shape>shape;
void read_newline(char* str) {
	char* pos;
	if((pos = strchr(str,'\n')) != NULL)
		*pos = '\0';
}

Model read_obj_file(const char* filename) {
	FILE* file;
	Model model{};

	// 파일 열기 (fopen_s를 사용하므로 Visual Studio 환경에 적합)
	fopen_s(&file,filename,"r");
	if(!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	// 임시 벡터를 사용하여 동적으로 정점 및 면 데이터를 저장합니다.
	std::vector<Vertex> temp_vertices;
	std::vector<Face> temp_faces;

	char line[365];
	while(fgets(line,sizeof(line),file)) {
		read_newline(line);

		// v 라인 처리
		if(line[0] == 'v' && line[1] == ' ') {
			Vertex v;
			if(sscanf_s(line + 2,"%f %f %f",&v.x,&v.y,&v.z) == 3) {
				temp_vertices.push_back(v);
			}
		}
		// f 라인 처리 - N각형을 삼각 분할(Triangulation)합니다.
		else if(line[0] == 'f' && line[1] == ' ') {
			char face_str[365];
			// 원본 라인을 훼손하지 않기 위해 복사 (strtok 사용을 위함)
			// '제목 없음.obj' 파일이 'f v1 v2 v3 v4...' 형식이라고 가정합니다.
			if(strlen(line + 2) >= sizeof(face_str)) {
				// 버퍼 오버플로우 방지 (필요 시 더 큰 버퍼 사용)
				continue;
			}
			strcpy_s(face_str,sizeof(face_str),line + 2);

			// strtok를 사용하여 공백으로 분리된 각 정점 데이터를 가져옵니다.
			char* token = strtok(face_str," ");
			std::vector<unsigned int> face_indices; // 면을 이루는 정점 인덱스 저장

			// 토큰(정점 인덱스)을 읽습니다.
			while(token != NULL) {
				unsigned int v_idx;

				// v/vt/vn 형태일 경우, 첫 '/' 이전의 v만 파싱
				if(strchr(token,'/') != NULL) {
					// v/vt/vn 형태일 경우, 첫 '/' 이전의 v만 파싱
					if(sscanf_s(token,"%u",&v_idx) == 1) {
						face_indices.push_back(v_idx - 1); // 1-based index를 0-based index로 변환
					}
				} else {
					// v 형태일 경우, v만 파싱
					if(sscanf_s(token,"%u",&v_idx) == 1) {
						face_indices.push_back(v_idx - 1); // 1-based index를 0-based index로 변환
					}
				}
				token = strtok(NULL," ");
			}

			// N각형(N >= 3) 면을 삼각 분할(Fan Triangulation)하여 저장합니다.
			if(face_indices.size() >= 3) {
				// 0번 인덱스를 중심으로 삼각형을 만듭니다. (0, 1, 2), (0, 2, 3), (0, 3, 4)...
				for(size_t i = 1; i < face_indices.size() - 1; ++i) {
					Face f;
					f.v1 = face_indices[0]; // 중심점
					f.v2 = face_indices[i];
					f.v3 = face_indices[i + 1];
					temp_faces.push_back(f);
				}
			}
		}
	}
	fclose(file);

	// 최종적으로 모델 구조체에 동적 할당 및 복사합니다.
	model.vertex_count = temp_vertices.size();
	model.face_count = temp_faces.size();

	// 기존 코드가 malloc/free를 사용하므로 이 방식을 유지합니다.
	model.vertices = (Vertex*)malloc(model.vertex_count * sizeof(Vertex));
	model.faces = (Face*)malloc(model.face_count * sizeof(Face));

	if(model.vertex_count > 0) {
		std::copy(temp_vertices.begin(),temp_vertices.end(),model.vertices);
	}
	if(model.face_count > 0) {
		std::copy(temp_faces.begin(),temp_faces.end(),model.faces);
	}

	return model;
}
glm::vec3 Rotate(glm::vec3 x,glm::vec3 center,float angle,glm::vec3 axis)
{
	glm::mat4 model(1.0f);
	model = glm::translate(model,center);
	model = glm::rotate(model,angle,axis); // angle은 라디안, axis는 단위벡터
	model = glm::translate(model,-center);

	glm::vec4 v4 = model * glm::vec4(x,1.0f);
	return glm::vec3(v4);
}

vector<Model> model;
int facenum = -1;
int modelType = 0; // 0: Cube, 1: Cone
bool allFaceDraw = true; // true: 전체 면 그리기, false: 한 면씩 그리기

//커비
glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,10.0f);
float cameraAngleori=45.0f;
float cameraAngle = glm::atan(glm::radians(cameraAngleori)); // 초기 각도 (45도 라디안)

// 도형(모델) 이동 및 회전 관련 추가 전역 변수
glm::vec3 modelPos = glm::vec3(0.0f,0.0f,0.0f);  // 도형 위치 (X, Y, Z 이동량)
float modelRotY = 0.0f;  // Y축 회전 각도 (라디안)
float modelRotX = 0.0f;  // X축 회전 각도 (라디안)

int shape_check = 0;
int ws_=0;
int ad_=0;
int pn_=0;
int y_=0;
int r_=0;
int o=1;
bool l=false;
bool g=false;
bool p=false;
bool silver=false;
bool solid=false;

glm::vec3 camera_move=glm::vec3(0.0f,0.0f,0.0f);

float w1=0.0f;
float w2=0.0f;
int w=15;

float x_rotate=1.0f;
float y_rotate=1.0f;
float z_rotate=1.0f;



float rotate_count=0.0f;

int scale_time=0;

int move_num=0;


float _z=0.0f;
int z_mode=0;

// 점프 관련 변수 수정 및 추가 (라인 290-296 부근) - 플랫폼 관련 변수 추가
bool isJumping = false;
float jumpVelocity = 0.0f;
float jumpInitialVelocity = 0.05f;
float gravity = -0.002f;
float groundLevel = -1.7f;
glm::vec3 jumpDirection = glm::vec3(0.0f);
float speedMultiplier = 2.0f;

// 플랫폼(밟을 수 있는 정육면체) 관련 변수 추가
struct Platform {
	glm::vec3 position;
	glm::vec3 size;
	float height;
};
vector<Platform> platforms;

// 플랫폼 생성 함수 선언 추가 (함수 선언 부분에)
void CreatePlatforms();
bool CheckPlatformCollision(glm::vec3 characterPos,float characterY);

//초기위치로 가는 요정 반디를 입력하세요
void InitData(){
	shape.clear();
	for(int i=0;i<6;++i){
		shape.push_back(Shape(model[i+1],0));
		shape[i].s={4.0f,4.0f,4.0f};
	}
	//머리
	shape.push_back(Shape(model[0],1));
	shape.back().t={0.0f,-0.5f,0.0f};
	shape.back().s={0.3f,0.3f,0.3f};

	//코
	shape.push_back(Shape(model[0],1));
	shape.back().t={0.0f,-0.5f,0.2f};
	shape.back().s={0.1f,0.2f,0.1f};

	//몸통
	shape.push_back(Shape(model[0],1));
	shape.back().t={0.0f,-1.0f,0.0f};
	shape.back().s={0.6f,0.6f,0.6f};

	//왼팔
	shape.push_back(Shape(model[0],1));
	shape.back().t={-0.4f,-1.0f,0.0f};
	shape.back().s={0.15f,0.5f,0.15f};

	//오른팔
	shape.push_back(Shape(model[0],1));
	shape.back().t={0.4f,-1.0f,0.0f};
	shape.back().s={0.15f,0.5f,0.15f};

	//왼다리
	shape.push_back(Shape(model[0],1));
	shape.back().t={-0.15f,-1.7f,0.0f};
	shape.back().s={0.2f,0.7f,0.2f};

	//오른다리
	shape.push_back(Shape(model[0],1));
	shape.back().t={0.15f,-1.7f,0.0f};
	shape.back().s={0.2f,0.7f,0.2f};


	// 플랫폼 생성
	CreatePlatforms();



}
// 새로운 함수들 추가 (InitData 함수 뒤에)
void CreatePlatforms() {
	platforms.clear();

	uniform_real_distribution<float> xDist(-1.5f,1.5f);
	uniform_real_distribution<float> zDist(-1.5f,1.5f);

	// 플랫폼 바닥이 땅 위에 오도록 수정
	float platformY = groundLevel -0.3f; // 플랫폼 높이(0.5f)의 절반만큼 위로

	for(int i = 0; i < 3; ++i) {
		Platform platform;
		platform.position = glm::vec3(xDist(gen),platformY,zDist(gen));
		platform.size = glm::vec3(0.4f,0.5f,0.4f);
		platform.height = 0.5f;
		platforms.push_back(platform);

		// 플랫폼 Shape 생성
		shape.push_back(Shape(model[0],2));
		shape.back().t = platform.position;
		shape.back().s = platform.size;
		shape.back().colors = glm::vec3(0.8f,0.6f,0.4f);
	}
}

bool CheckPlatformCollision(glm::vec3 characterPos,float characterY) {
	for(const auto& platform : platforms) {
		// xz 평면 범위 체크
		if(characterPos.x >= platform.position.x - platform.size.x/2 &&
		   characterPos.x <= platform.position.x + platform.size.x/2 &&
		   characterPos.z >= platform.position.z - platform.size.z/2 &&
		   characterPos.z <= platform.position.z + platform.size.z/2) {

			// 플랫폼 상단 표면 계산
			float platformTop = platform.position.y + platform.size.y/2;

			// 캐릭터 다리가 플랫폼 상단 표면에 닿았을 때만 true
			if(characterY <= platformTop + 0.1f && characterY >= platformTop - 0.1f) {
				return true;
			}
		}
	}
	return false;
}
void Update(){
	vector<GLfloat> vertexData;
	vector<GLuint> indices;

	for(size_t i=0;i<shape.size();++i){
		shape[i].Update(model[0]);
		vertexData.insert(vertexData.end(),shape[i].vertexData.begin(),shape[i].vertexData.end());

		// 인덱스 오프셋 보정 필요 (여러 모델 합칠 경우)
		GLuint offset = i==0 ? 0 : (GLuint)(vertexData.size()/3 - shape[i].vertexData.size()/3);
		for(auto idx: shape[i].indices) indices.push_back(idx + offset);
	}

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,vertexData.size()*sizeof(GLfloat),vertexData.data(),GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLuint),indices.data(),GL_DYNAMIC_DRAW);
}
int main(int argc,char** argv) {
	srand(static_cast<unsigned>(time(0))); // 랜덤 시드 초기화
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); // GLUT_DEPTH 추가
	glutInitWindowPosition(100,100);
	glutInitWindowSize(width,height);
	glutCreateWindow("tung tung tung tung tung tung tung tung tung sours");

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		std::cerr << "GLEW 초기화 실패" << std::endl;
		return -1;
	}

	MakeVertexShaders();
	MakeFragmentShaders();
	shaderProgramID = MakeShaderProgram();
	if(shaderProgramID == 0) {
		std::cerr << "셰이더 프로그램 생성 실패" << std::endl;
		return -1;
	}
	model.push_back(read_obj_file("cube2.obj"));
	model.push_back(read_obj_file("cube - front.obj"));
	model.push_back(read_obj_file("cube - left.obj"));
	model.push_back(read_obj_file("cube - right.obj"));
	model.push_back(read_obj_file("cube - back.obj"));
	model.push_back(read_obj_file("cube - top.obj"));
	model.push_back(read_obj_file("cube - bottom.obj"));

	InitBuffers();
	InitData();
	glutTimerFunc (10,TimerFunction,1);
	glutDisplayFunc(DrawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard); // 특수 키(화살표) 함수 등록

	glutMainLoop();
	return 0;
}



void MakeVertexShaders() {
	GLchar* vertexSource = filetobuf("vertex.glsl");
	if(!vertexSource) {
		std::cerr << "ERROR: vertex.glsl." << std::endl;
		return;
	}

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1,&vertexSource,NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&result);
	if(!result) {
		glGetShaderInfoLog(vertexShader,512,NULL,errorLog);
		std::cerr << "ERROR: vertex shader\n" << errorLog << std::endl;
	}
	free(vertexSource);
}

void MakeFragmentShaders() {
	GLchar* fragmentSource = filetobuf("fragment.glsl");
	if(!fragmentSource) {
		std::cerr << "ERROR: fragment.glsl." << std::endl;
		return;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,&fragmentSource,NULL);
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&result);
	if(!result) {
		glGetShaderInfoLog(fragmentShader,512,NULL,errorLog);
		std::cerr << "ERROR: fragment shader\n" << errorLog << std::endl;
	}
	free(fragmentSource);
}

GLuint MakeShaderProgram() {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID,vertexShader);
	glAttachShader(shaderID,fragmentShader);
	glLinkProgram(shaderID);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(shaderID,GL_LINK_STATUS,&result);
	if(!result) {
		glGetProgramInfoLog(shaderID,512,NULL,errorLog);
		std::cerr << "ERROR: shader program\n" << errorLog << std::endl;
		return 0;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(shaderID);
	return shaderID;
}

void InitBuffers() {
	glGenVertexArrays(1,&VAO);

	glBindVertexArray(VAO);

	glGenBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,0,nullptr,GL_DYNAMIC_DRAW);

	glGenBuffers(1,&EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,0,nullptr,GL_DYNAMIC_DRAW);


	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(0);



	glBindVertexArray(0);
}

void DrawScene() {
	// 카메라 위치 업데이트
	//float r = glm::sqrt(cameraPos.x * cameraPos.x + cameraPos.z * cameraPos.z); // XZ 평면 거리
	//cameraPos.x = r * glm::cos(cameraAngle);
	//cameraPos.z = r * glm::sin(cameraAngle);

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
	glClearColor(0.5f,0.5f,0.5f,1.0f);

	//if(silver)
	//	glEnable(GL_CULL_FACE);// 은면 제거 활성화
	//else
	//	glDisable(GL_CULL_FACE);


	if(silver){
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // 뒷면 제거
	} else{
		glDisable(GL_CULL_FACE);
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 버퍼 클리어 추가

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	Update();

	if(!solid)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);


	// Uniform 매트릭스 매핑
	GLint modelLoc = glGetUniformLocation(shaderProgramID,"model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID,"view");
	GLint projLoc = glGetUniformLocation(shaderProgramID,"proj");
	GLint faceColorLoc = glGetUniformLocation(shaderProgramID,"faceColor");


	//--------------------------------------------------------------------------


	GLuint offset = 0;
	for(int i=0;i<shape.size();++i){
		for(int j = 0; j < shape[i].vertexData.size() / 9; ++j){ // colors 개수 == face 개수
			glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[i].modelMat));
			glUniform3f(faceColorLoc,shape[i].colors[0],shape[i].colors[1],shape[i].colors[2]);
			glDrawArrays(GL_TRIANGLES,offset,3);
			offset += 3;
		}
	}

	//--------------------------------------------------------------------------
	// Camera (View) 및 Projection 매트릭스 설정
	glm::vec3 camera_=cameraPos+camera_move;
	glm::mat4 view = glm::lookAt(camera_,camera_move,glm::vec3(0.0f,1.0f,0.0f)); // 뷰 매트릭스
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)width / (float)height,0.1f,100.0f); // 프로젝션 매트릭스

	glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
	glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(proj));


	glm::mat4 axisModelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(axisModelMat));


	glBindVertexArray(0);
	glutSwapBuffers();
}


void Reshape(int w,int h) {
	glViewport(0,0,w,h);
	width = w;
	height = h;
}

void Mouse(int button,int state,int x,int y) {

}
bool ani=false;
int movemode=0;
int oldmovemode=0;
float speed=0.01f;
float y_pivot=-1.7;
float y_delta=0.0f;
void Keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 'w':
	if(movemode!=1)movemode=1;
	break;
	case 'a':
	if(movemode!=2)movemode=2;
	break;
	case 's':
	if(movemode!=3)movemode=3;
	break;
	case 'd':
	if(movemode!=4)movemode=4;
	break;
	case 'j':
	// 점프 기능: 땅에 있을 때만 점프 가능, 스피드에 따른 대각선 점프
	if(!isJumping) {
		isJumping = true;
		jumpVelocity = jumpInitialVelocity;

		// 현재 이동 방향과 속도에 따라 점프 방향 계산
		jumpDirection = glm::vec3(0.0f);

		if(movemode == 1) {  // 앞으로 이동 중 (z-축 음의 방향)
			jumpDirection.z = -speed * speedMultiplier;
		} else if(movemode == 2) {  // 왼쪽으로 이동 중 (x-축 음의 방향)
			jumpDirection.x = -speed * speedMultiplier;
		} else if(movemode == 3) {  // 뒤로 이동 중 (z-축 양의 방향)
			jumpDirection.z = speed * speedMultiplier;
		} else if(movemode == 4) {  // 오른쪽으로 이동 중 (x-축 양의 방향)
			jumpDirection.x = speed * speedMultiplier;
		}
		// movemode가 0이면 제자리 점프 (jumpDirection은 0,0,0)
	}
	break;
	case '+':
	speed+=0.01f;
	break;
	case '-':
	speed-=0.01f;
	if(speed < 0.0f) speed = 0.0f; // 음수 방지
	break;
	case 'o':
	if(o==0)o=1;
	break;
	case 'O':
	if(o==1)o=0;
	break;
	case 'z':
	++pn_;
	break;
	case 'Z':
	--pn_;
	break;
	case 'x':
	++ad_;
	break;
	case 'X':
	--ad_;
	break;
	case 'y':
	{
		glm::mat4 cameraori=glm::mat4(1.0f);
		cameraori=glm::rotate(cameraori,glm::radians(1.0f),{0.0f,1.0f,0.0f});
		cameraPos=glm::vec3(cameraori*glm::vec4(cameraPos,1.0f));
	}
	break;
	case 'Y':
	{
		glm::mat4 cameraori=glm::mat4(1.0f);
		cameraori=glm::rotate(cameraori,glm::radians(-1.0f),{0.0f,1.0f,0.0f});
		cameraPos=glm::vec3(cameraori*glm::vec4(cameraPos,1.0f));
	}
	break;
	case 'c':
	InitData();
	movemode=0;
	oldmovemode=0;
	speed=0.01f;
	y_pivot=-1.7;
	y_delta=0.0f;
	camera_move =glm::vec3(0.0f,0.0f,0.0f);
	cameraPos = glm::vec3(0.0f,0.0f,10.0f);
	// 점프 상태 초기화
	isJumping = false;
	jumpVelocity = 0.0f;
	jumpDirection = glm::vec3(0.0f);
	break;
	case 'q':
	exit(0);
	break;
	}
	glutPostRedisplay();
}

// 특수 키(화살표) 처리 함수: 도형 회전
void SpecialKeyboard(int key,int x,int y)
{


	glutPostRedisplay();  // 재렌더링 요청
}

void TimerFunction(int value)
{
	camera_move -= glm::vec3(ad_ * 0.5f,ws_ * 0.5f,pn_ * 0.5f);


	if(oldmovemode!=movemode){
		glm::vec3 center = shape[8].t;


		float newAngle = 0.0f;
		if(movemode==1) newAngle = 180.0f;
		if(movemode==2) newAngle = -90.0f;
		if(movemode==3) newAngle = 0.0f;
		if(movemode==4) newAngle = 90.0f;

		float angleDiff = newAngle - shape[8].angle;
		shape[8].angle = newAngle;


		for(int i=6; i<=12; ++i){
			if(i != 8) {
				glm::vec3 relativePos = shape[i].t - center;

				glm::mat4 rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix,glm::radians(angleDiff),glm::vec3(0.0f,1.0f,0.0f));

				glm::vec3 rotatedPos = glm::vec3(rotationMatrix * glm::vec4(relativePos,1.0f));
				shape[i].t = center + rotatedPos;
			}
		}

		oldmovemode = movemode;
	}



	// 점프 물리 처리 - 대각선 점프 포함
	if(isJumping) {
		// 모든 캐릭터 부품에 점프 속도 적용 (수직)
		for(int i=6; i<=12; ++i) {
			shape[i].t.y += jumpVelocity;
			// 수평 이동 (대각선 점프)
			shape[i].t.x += jumpDirection.x;
			shape[i].t.z += jumpDirection.z;
		}

		// 중력 적용
		jumpVelocity += gravity;

		// 바닥이나 플랫폼에 닿으면 처리
		glm::vec3 characterPos = shape[8].t; // 몸통 위치
		bool onPlatform = CheckPlatformCollision(characterPos,shape[11].t.y);

		if(onPlatform) {
			// 플랫폼 상단 표면 위에 착지
			for(int i=6; i<=12; ++i) {
				float yOffset = 0.0f;
				float baseLevel = groundLevel;

				// 착지한 플랫폼의 상단 표면 y좌표 계산
				for(const auto& platform : platforms) {
					if(characterPos.x >= platform.position.x - platform.size.x/2 &&
					   characterPos.x <= platform.position.x + platform.size.x/2 &&
					   characterPos.z >= platform.position.z - platform.size.z/2 &&
					   characterPos.z <= platform.position.z + platform.size.z/2) {
						// 플랫폼 상단 표면 + 추가 높이를 baseLevel로 설정
						baseLevel = platform.position.y + platform.size.y/2 + 0.25f;
						break;
					}
				}

				// 각 부품별 높이 오프셋 (플랫폼 상단 표면 기준)
				if(i == 6) yOffset = 1.2f;       // 머리
				else if(i == 7) yOffset = 1.2f;  // 코  
				else if(i == 8) yOffset = 0.7f;  // 몸통
				else if(i == 9 || i == 10) yOffset = 0.7f; // 팔
				else if(i == 11 || i == 12) yOffset = 0.0f; // 다리 (상단 표면에 정확히 위치)

				shape[i].t.y = baseLevel + yOffset;
			}

			jumpVelocity = 0.0f;
		}
		// 실제 바닥(땅)에 닿았을 때만 점프 완전 종료
		else if(shape[11].t.y <= groundLevel) {
			for(int i=6; i<=12; ++i) {
				float yOffset = 0.0f;
				if(i == 6) yOffset = 1.2f;
				else if(i == 7) yOffset = 1.2f;
				else if(i == 8) yOffset = 0.7f;
				else if(i == 9 || i == 10) yOffset = 0.7f;
				else if(i == 11 || i == 12) yOffset = 0.0f;
				shape[i].t.y = groundLevel + yOffset;
			}
			// 땅에 닿았을 때만 점프 완전 종료
			isJumping = false;
			jumpVelocity = 0.0f;
			jumpDirection = glm::vec3(0.0f);
		}
	}


	for(size_t i=0;i<shape.size();++i){
		glm::mat4 ori=glm::mat4(1.0f);

		if(i==3){
			if(o==0&&shape[i].t.y>0.0f)shape[i].t.y-=0.2f;
			if(o==1&&shape[i].t.y<4.0f)shape[i].t.y+=0.2f;
			ori=glm::translate(ori,shape[i].t);
			ori=glm::scale(ori,shape[i].s);
		} else if(i<6){
			ori=glm::translate(ori,shape[i].t);
			ori=glm::scale(ori,shape[i].s);
		} else if(i<=12){
			if(!isJumping) {  // 점프 중이 아닐 때만 이동
				if(movemode==1)shape[i].t.z-=speed;
				if(movemode==2)shape[i].t.x-=speed;
				if(movemode==3)shape[i].t.z+=speed;
				if(movemode==4)shape[i].t.x+=speed;
			}
			ori=glm::translate(ori,shape[i].t);
			ori=glm::scale(ori,shape[i].s);
		} else{
			ori=glm::translate(ori,shape[i].t);
			ori=glm::scale(ori,shape[i].s);
		}
		shape[i].modelMat=ori;
	}


	static float walkPhase = 0.0f; // 걷기 애니메이션 위상
	float maxLegAngle = glm::clamp(600.0f * speed,10.0f,90.0f);
	float maxArmAngle = glm::clamp(400.0f * speed,5.0f,60.0f);

	if(movemode >= 1 && movemode <= 4 && !isJumping) {
		walkPhase += speed * 120.0f;
		if(walkPhase > 360.0f) walkPhase -= 360.0f;

		float legAngle = maxLegAngle * sin(glm::radians(walkPhase));
		float armAngle = maxArmAngle * sin(glm::radians(walkPhase + 180.0f));

		glm::vec3 legAxis,armAxis;
		if(movemode == 1 || movemode == 3) {
			// 앞/뒤 이동: x축 기준으로 흔들림
			legAxis = glm::vec3(1,0,0);
			armAxis = glm::vec3(1,0,0);
		} else {
			// 좌/우 이동: z축 기준으로 흔들림
			legAxis = glm::vec3(0,0,1);
			armAxis = glm::vec3(0,0,1);
		}

		// 다리 회전
		shape[11].modelMat = glm::mat4(1.0f);
		shape[11].modelMat = glm::translate(shape[11].modelMat,shape[11].t);
		shape[11].modelMat = glm::rotate(shape[11].modelMat,glm::radians(legAngle),legAxis);
		shape[11].modelMat = glm::scale(shape[11].modelMat,shape[11].s);

		shape[12].modelMat = glm::mat4(1.0f);
		shape[12].modelMat = glm::translate(shape[12].modelMat,shape[12].t);
		shape[12].modelMat = glm::rotate(shape[12].modelMat,glm::radians(-legAngle),legAxis);
		shape[12].modelMat = glm::scale(shape[12].modelMat,shape[12].s);

		// 팔 회전
		shape[9].modelMat = glm::mat4(1.0f);
		shape[9].modelMat = glm::translate(shape[9].modelMat,shape[9].t);
		shape[9].modelMat = glm::rotate(shape[9].modelMat,glm::radians(armAngle),armAxis);
		shape[9].modelMat = glm::scale(shape[9].modelMat,shape[9].s);

		shape[10].modelMat = glm::mat4(1.0f);
		shape[10].modelMat = glm::translate(shape[10].modelMat,shape[10].t);
		shape[10].modelMat = glm::rotate(shape[10].modelMat,glm::radians(-armAngle),armAxis);
		shape[10].modelMat = glm::scale(shape[10].modelMat,shape[10].s);
	}



	if(!isJumping) {
		for(const auto& platform : platforms) {
			// xz 평면에서 충돌 체크
			if(shape[8].t.x >= platform.position.x - platform.size.x/2 &&
			   shape[8].t.x <= platform.position.x + platform.size.x/2 &&
			   shape[8].t.z >= platform.position.z - platform.size.z/2 &&
			   shape[8].t.z <= platform.position.z + platform.size.z/2) {

				// 방향 반전 (예시: 1<->3, 2<->4)
				if(movemode == 1) movemode = 3;
				else if(movemode == 3) movemode = 1;
				else if(movemode == 2) movemode = 4;
				else if(movemode == 4) movemode = 2;
			}
		}
	}
	if(movemode==1){
		if(shape[8].t.z<-2.0f)movemode=3;
	}
	if(movemode==2){
		if(shape[8].t.x<-2.0f)movemode=4;
	}
	if(movemode==3){
		if(shape[8].t.z>2.0f)movemode=1;
	}
	if(movemode==4){
		if(shape[8].t.x>2.0f)movemode=2;
	}
	ad_=ws_=pn_=0;
	if(!ani)glutTimerFunc(10,TimerFunction,1);
	glutPostRedisplay();
}
GLchar ch[256]{};
GLchar* filetobuf(const char *file)
{
	FILE *fptr;
	long length;
	char *buf;
	fptr = fopen (file,"rb"); // Open file for reading
	if(!fptr) // Return NULL on failure
		return NULL;
	fseek (fptr,0,SEEK_END); // Seek to the end of the file
	length = ftell (fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc (length+1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek (fptr,0,SEEK_SET); // Go back to the beginning of the file
	fread (buf,length,1,fptr); // Read the contents of the file in to the buffer
	fclose (fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}
//버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	GLchar* vertexSource;

	//버텍스 세이더 읽어 저장하고 컴파일 하기
	//filetobuf : 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1,&vertexSource,NULL);
	glCompileShader (vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv (vertexShader,GL_COMPILE_STATUS,&result);
	if(!result)
	{
		glGetShaderInfoLog (vertexShader,512,NULL,errorLog);
		cerr << "ERROR: vertex shader \n" << errorLog << std :: endl;
		return;
	}
}

//프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	GLchar *fragmentSource;
	//프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fragmentShader,1,&fragmentSource,NULL);
	glCompileShader (fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv (fragmentShader,GL_COMPILE_STATUS,&result);
	if(!result)
	{
		glGetShaderInfoLog (fragmentShader,512,NULL,errorLog);
		std::cerr << "ERROR: frag_shader \n" << errorLog << std::endl;
		return;
	}
}

//세이더 프로그램 만들고 세이더 객체 링크하기
GLuint make_shaderProgram()
{
	GLuint shaderID;
	shaderID = glCreateProgram(); //세이더 프로그램 만들기
	glAttachShader (shaderID,vertexShader); //세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader (shaderID,fragmentShader); //세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram (shaderID); //세이더 프로그램 링크하기
	glDeleteShader (vertexShader); //세이더 객체를 세이더 프로그램에 링크했음으로 세이더 객체 자체는 삭제 가능
	glDeleteShader (fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetProgramiv (shaderID,GL_LINK_STATUS,&result);// 세이더가 잘 연결되었는지 체크하기
	if(!result) {
		glGetProgramInfoLog (shaderID,512,NULL,errorLog);
		std::cerr << "ERROR: shader program  \n" << errorLog << std::endl;
		return false;
	}
	glUseProgram (shaderID); //만들어진 세이더 프로그램 사용하기
	//여러 개의 세이더프로그램 만들 수 있고,그 중 한개의 프로그램을 사용하려면
	//glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다
	//사용하기 직전에 호출할 수 있다
	return shaderID;
}

