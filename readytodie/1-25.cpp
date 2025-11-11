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
	vector<glm::vec3> normals;
} Model;
class Shape{
public:
	vector<GLfloat> vertexData;
	vector<GLfloat> normalData;
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
	std::vector<glm::vec3> temp_normals;
	std::vector<Face> temp_faces;

	char line[365];
	while(fgets(line,sizeof(line),file)) {
		read_newline(line);

		// v 라인 처리 (정점)
		if(line[0] == 'v' && line[1] == ' ') {
			Vertex v;
			if(sscanf_s(line + 2,"%f %f %f",&v.x,&v.y,&v.z) == 3) {
				temp_vertices.push_back(v);
			}
		}
		// vn 라인 처리 (노멀 벡터)
		else if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
			glm::vec3 normal;
			if(sscanf_s(line + 3,"%f %f %f",&normal.x,&normal.y,&normal.z) == 3) {
				temp_normals.push_back(normal);
			}
		}
		// f 라인 처리 - v//vn 형식 (f 1//2 7//2 5//2)
		else if(line[0] == 'f' && line[1] == ' ') {
			char face_str[365];
			if(strlen(line + 2) >= sizeof(face_str)) {
				continue;
			}
			strcpy_s(face_str,sizeof(face_str),line + 2);

			// strtok를 사용하여 공백으로 분리된 각 정점 데이터를 가져옵니다.
			char* token = strtok(face_str," ");
			std::vector<unsigned int> face_indices; // 정점 인덱스
			std::vector<unsigned int> normal_indices; // 노멀 인덱스

			// 토큰(정점/노멀 인덱스)을 읽습니다.
			while(token != NULL) {
				unsigned int v_idx = 0,vn_idx = 0;

				// v//vn 형태 파싱
				if(strchr(token,'/') != NULL) {
					// v//vn 형식
					if(sscanf_s(token,"%u//%u",&v_idx,&vn_idx) == 2) {
						face_indices.push_back(v_idx - 1); // 1-based to 0-based
						normal_indices.push_back(vn_idx - 1);
					}
					// v/vt/vn 형식도 처리 (만약을 위해)
					else if(sscanf_s(token,"%u/%*u/%u",&v_idx,&vn_idx) == 2) {
						face_indices.push_back(v_idx - 1);
						normal_indices.push_back(vn_idx - 1);
					}
					// v만 있는 경우
					else if(sscanf_s(token,"%u",&v_idx) == 1) {
						face_indices.push_back(v_idx - 1);
						normal_indices.push_back(0); // 기본 노멀
					}
				} else {
					// v만 있는 경우
					if(sscanf_s(token,"%u",&v_idx) == 1) {
						face_indices.push_back(v_idx - 1);
						normal_indices.push_back(0); // 기본 노멀
					}
				}
				token = strtok(NULL," ");
			}

			// 삼각형만 처리 (N각형 삼각분할은 제거 - cube.obj는 모두 삼각형)
			if(face_indices.size() >= 3) {
				// 첫 3개 정점으로 삼각형 생성
				Face f;
				f.v1 = face_indices[0];
				f.v2 = face_indices[1];
				f.v3 = face_indices[2];
				temp_faces.push_back(f);

				// 노멀도 face 순서대로 저장
				if(normal_indices.size() >= 3) {
					model.normals.push_back(temp_normals[normal_indices[0]]);
					model.normals.push_back(temp_normals[normal_indices[1]]);
					model.normals.push_back(temp_normals[normal_indices[2]]);
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
glm::vec3 cameraPos1 = glm::vec3(5.0f,5.0f,5.0f);
glm::vec3 cameraPos2 = glm::vec3(0.0f,10.0f,0.0f);
glm::vec3 cameraPos3 = glm::vec3(0.0f,0.0f,10.0f);
float cameraAngleori=90.0f;
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
bool t=false;
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

glm::vec3 center1;
glm::vec3 center2;
glm::vec3 center3;
glm::vec3 center4;


float rotate_count=0.0f;

int scale_time=0;

int move_num=0;

glm::vec3 center = {0.0f,0.0f,0.0f};
glm::vec3 rotate1 =glm::vec3(0,0.707f,-0.707f);
glm::vec3 rotate2 =glm::vec3(0,1.0f,0.0f);
glm::vec3 rotate3 =glm::vec3(0,0.707f,0.707f);

float _z=0.0f;
int z_mode=0;

//커비
void InitData(){
	shape.clear();
	for(size_t i=0;i<8;++i){
		shape.push_back(Shape(model[0],i));// Shape 생성 시 model 정보 전달
		if(i==0){
			shape[i].s={2.0f,0.5f,1.0f};
		} else if(i==1){
			shape[i].s={1.2f,0.4f,0.6f};
			shape[i].t={0.5f,0.5f,0.25f};
		} else if(i<4){
			shape[i].s={0.8f,0.5f,0.4f};
			shape[i].t={(i-2)*1.5f,0.9f,0.35f};
		} else if(i<6){
			shape[i].s={0.1f,0.1f,0.8f};
			shape[i].t={i==4?0.4f:1.8f,1.1f,0.75f};
		} else{
			shape[i].s={0.15f,1.0f,0.15f};
			shape[i].t={i==6?0.4f:1.8f,1.2f,0.5f};
		}

	}




}
void Update(){
	vector<GLfloat> vertexData;
	vector<GLuint> indices;

	for(size_t i=0;i<shape.size();++i){
		shape[i].Update(model[0]);
		vertexData.insert(vertexData.end(),shape[i].vertexData.begin(),shape[i].vertexData.end());

	}

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,vertexData.size()*sizeof(GLfloat),vertexData.data(),GL_DYNAMIC_DRAW);
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
	model.push_back(read_obj_file("cube.obj"));

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

	


	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);
}

void DrawScene() {

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
	glClearColor(0.5f,0.5f,0.5f,1.0f);


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

	glm::mat4 view = glm::lookAt(cameraPos1,camera_move,glm::vec3(0,1,0));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)(width/2)/height,0.1f,100.0f);
	glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
	glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(proj));
	// 도형 그리기
	GLuint offset = 0;
	for(int i=0;i<shape.size();++i){
		for(int j = 0; j < shape[i].vertexData.size() / 9; ++j){
			glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[i].modelMat));
			glUniform3f(faceColorLoc,shape[i].colors[0],shape[i].colors[1],shape[i].colors[2]);
			glDrawArrays(GL_TRIANGLES,offset,3);
			offset += 3;
		}
	}



	GLint lightPosLocation = glGetUniformLocation(shaderProgramID,"lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	GLint lightColorLocation = glGetUniformLocation(shaderProgramID,"lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	GLint viewPosLocation = glGetUniformLocation(shaderProgramID,"viewPos"); //--- viewPos 값 전달: 카메라 위치
	GLint objColorLocation = glGetUniformLocation(shaderProgramID,"objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색


	glUniform3f (lightPosLocation,0.0,0.0,5.0);
	glUniform3f (lightColorLocation,1.0,1.0,1.0);
	glUniform3f (objColorLocation,1.0,0.5,0.3);
	glUniform3f(viewPosLocation,cameraPos1.x,cameraPos1.y,cameraPos1.z);






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
void Keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case 't':
	t= !t;
	break;
	case 'l':
	if(!l){
		w1=shape[2].t.x;
		w2=shape[3].t.x;
		w=15;
	}
	l=true;
	break;
	case 'g':
	g= !g;
	break;
	case 'p':
	p= !p;
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
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f),glm::radians(1.0f),{0.0f,1.0f,0.0f});
		for(auto& cam :{std::ref(cameraPos1),std::ref(cameraPos2),std::ref(cameraPos3)}) {
			glm::mat4 pan = glm::translate(glm::mat4(1.0f),cam.get()) * rot * glm::translate(glm::mat4(1.0f),-cam.get());
			camera_move = glm::vec3(pan * glm::vec4(camera_move,1.0f));
		}
	}
	break;
	case 'Y':
	{
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f),glm::radians(-1.0f),{0.0f,1.0f,0.0f});
		for(auto& cam :{std::ref(cameraPos1),std::ref(cameraPos2),std::ref(cameraPos3)}) {
			glm::mat4 pan = glm::translate(glm::mat4(1.0f),cam.get()) * rot * glm::translate(glm::mat4(1.0f),-cam.get());
			camera_move = glm::vec3(pan * glm::vec4(camera_move,1.0f));
		}
	}
	break;
	case 'r':
	{
		glm::mat4 cameraori=glm::mat4(1.0f);
		cameraori=glm::rotate(cameraori,glm::radians(1.0f),{0.0f,1.0f,0.0f});
		cameraPos1=glm::vec3(cameraori*glm::vec4(cameraPos1,1.0f));
		cameraPos2=glm::vec3(cameraori*glm::vec4(cameraPos2,1.0f));
		cameraPos3=glm::vec3(cameraori*glm::vec4(cameraPos3,1.0f));
	}
	break;
	case 'R':
	{
		glm::mat4 cameraori=glm::mat4(1.0f);
		cameraori=glm::rotate(cameraori,glm::radians(-1.0f),{0.0f,1.0f,0.0f});
		cameraPos1=glm::vec3(cameraori*glm::vec4(cameraPos1,1.0f));
		cameraPos2=glm::vec3(cameraori*glm::vec4(cameraPos2,1.0f));
		cameraPos3=glm::vec3(cameraori*glm::vec4(cameraPos3,1.0f));
	}
	break;
	case 'a':
	r_=r_!=1?1:0;
	break;
	case 'o':
	ani=!ani;
	if(!ani)
		glutTimerFunc(10,TimerFunction,1);
	break;
	case 'c':
	InitData();
	camera_move =glm::vec3(0.0f,0.0f,0.0f);
	cameraPos1 = glm::vec3(5.0f,5.0f,5.0f);
	cameraPos2 = glm::vec3(0.0f,10.0f,0.0f);
	cameraPos3 = glm::vec3(0.0f,0.0f,10.0f);
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
	switch(key)
	{
	case GLUT_KEY_UP:
	for(int i=0;i<shape.size();++i){
		shape[i].t.z -= 0.1f;
	}
	break;
	case GLUT_KEY_DOWN:
	for(int i=0;i<shape.size();++i){
		shape[i].t.z += 0.1f;
	}
	break;
	case GLUT_KEY_LEFT:
	for(int i=0;i<shape.size();++i){
		shape[i].t.x -= 0.1f;
	}
	break;
	case GLUT_KEY_RIGHT:
	for(int i=0;i<shape.size();++i){
		shape[i].t.x += 0.1f;
	}
	break;
	default:
	break;
	}

	glutPostRedisplay();  // 재렌더링 요청
}

void TimerFunction(int value)
{
	camera_move -= glm::vec3(ad_ * 0.5f,ws_ * 0.5f,pn_ * 0.5f);
	for(size_t i=0;i<shape.size();++i){
		glm::mat4 ori=glm::mat4(1.0f);
		if(i==1){
			if(t){
				shape[i].angle+=1.0f;
				ori=glm::translate(ori,{shape[i].t.x+shape[i].s.x/2,shape[i].t.y,shape[i].t.z+shape[i].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[i].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[i].t.x+shape[i].s.x/2),-shape[i].t.y,-(shape[i].t.z+shape[i].s.z/2)});

			}
		}
		if(i==2){
			if(t){
				ori=glm::translate(ori,{shape[1].t.x+shape[1].s.x/2,shape[1].t.y,shape[1].t.z+shape[1].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[1].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[1].t.x+shape[1].s.x/2),-shape[1].t.y,-(shape[1].t.z+shape[1].s.z/2)});

			}
			if(l){
				if(w1>w2){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}
				--w;

				if(w==0) l=false;
			}
		}
		if(i==3){
			if(t){
				ori=glm::translate(ori,{shape[1].t.x+shape[1].s.x/2,shape[1].t.y,shape[1].t.z+shape[1].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[1].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[1].t.x+shape[1].s.x/2),-shape[1].t.y,-(shape[1].t.z+shape[1].s.z/2)});

			}
			if(l){

				if(w2>w1){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}

			}
		}
		if(i==4){
			if(t){
				ori=glm::translate(ori,{shape[1].t.x+shape[1].s.x/2,shape[1].t.y,shape[1].t.z+shape[1].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[1].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[1].t.x+shape[1].s.x/2),-shape[1].t.y,-(shape[1].t.z+shape[1].s.z/2)});

			}
			if(g){
				shape[i].angle+=1.0f;
				ori=glm::translate(ori,{shape[2].t.x+shape[2].s.x/2,shape[2].t.y,shape[2].t.z+shape[2].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[4].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[2].t.x+shape[2].s.x/2),-shape[2].t.y,-(shape[2].t.z+shape[2].s.z/2)});

			}
			if(l){
				if(w1>w2){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}
			}

		}
		if(i==5){
			if(t){
				ori=glm::translate(ori,{shape[1].t.x+shape[1].s.x/2,shape[1].t.y,shape[1].t.z+shape[1].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[1].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[1].t.x+shape[1].s.x/2),-shape[1].t.y,-(shape[1].t.z+shape[1].s.z/2)});

			}
			if(g){
				ori=glm::translate(ori,{shape[3].t.x+shape[3].s.x/2,shape[3].t.y,shape[3].t.z+shape[3].s.z/2});
				ori=glm::rotate(ori,glm::radians(-shape[4].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[3].t.x+shape[3].s.x/2),-shape[3].t.y,-(shape[3].t.z+shape[3].s.z/2)});

			}
			if(l){

				if(w2>w1){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}

			}
		}
		if(i==6){
			if(t){
				ori=glm::translate(ori,{shape[1].t.x+shape[1].s.x/2,shape[1].t.y,shape[1].t.z+shape[1].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[1].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[1].t.x+shape[1].s.x/2),-shape[1].t.y,-(shape[1].t.z+shape[1].s.z/2)});

			}
			if(p){
				shape[i].angle+=1.0f;
				ori=glm::translate(ori,{shape[2].t.x+shape[2].s.x/2,shape[2].t.y,shape[2].t.z+shape[2].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[6].angle),{1.0f,0.0f,0.0f});
				ori=glm::translate(ori,{-(shape[2].t.x+shape[2].s.x/2),-shape[2].t.y,-(shape[2].t.z+shape[2].s.z/2)});

			}
			if(l){
				if(w1>w2){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}
			}
		}
		if(i==7){
			if(t){
				ori=glm::translate(ori,{shape[1].t.x+shape[1].s.x/2,shape[1].t.y,shape[1].t.z+shape[1].s.z/2});
				ori=glm::rotate(ori,glm::radians(shape[1].angle),{0.0f,1.0f,0.0f});
				ori=glm::translate(ori,{-(shape[1].t.x+shape[1].s.x/2),-shape[1].t.y,-(shape[1].t.z+shape[1].s.z/2)});

			}
			if(p){
				shape[i].angle+=1.0f;
				ori=glm::translate(ori,{shape[3].t.x+shape[3].s.x/2,(shape[3].t.y+shape[3].s.y/2),shape[3].t.z+shape[3].s.z/2});
				ori=glm::rotate(ori,glm::radians(-shape[6].angle),{1.0f,0.0f,0.0f});
				ori=glm::translate(ori,{-(shape[3].t.x+shape[3].s.x/2),-(shape[3].t.y+shape[3].s.y/2),-(shape[3].t.z+shape[3].s.z/2)});

			}
			if(l){

				if(w2>w1){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}

			}

		}
		/*if(((i==1)&&t)||(g&&(i==4||i==5))){
			shape[i].angle+=90.0f;
			ori=glm::rotate(ori,glm::radians(shape[i].angle),{0.0f,1.0f,0.0f});
		}
		if(p&&(i==6||i==7)){
			shape[i].angle+=1.0f;
			ori=glm::rotate(ori,glm::radians(shape[i].angle),{1.0f,0.0f,0.0f});
		}
		if(l){
			if(i==2){
				if(w1>w2){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}
				--w;
			} else if(i==3){
				if(w2>w1){
					ori=glm::translate(ori,{-0.1f,0.0f,0.0f});
					shape[i].t.x-=0.1f;
				} else{
					ori=glm::translate(ori,{0.1f,0.0f,0.0f});
					shape[i].t.x+=0.1f;
				}
			}

			if(w==0) l=false;
		}*/
		ori=glm::translate(ori,shape[i].t);
		ori=glm::scale(ori,shape[i].s);
		shape[i].modelMat=ori;
	}

	if(r_==1){
		glm::mat4 cameraori=glm::mat4(1.0f);
		cameraori=glm::rotate(cameraori,glm::radians(1.0f),{0.0f,1.0f,0.0f});
		cameraPos1=glm::vec3(cameraori*glm::vec4(cameraPos1,1.0f));
		cameraPos2=glm::vec3(cameraori*glm::vec4(cameraPos2,1.0f));
		cameraPos3=glm::vec3(cameraori*glm::vec4(cameraPos3,1.0f));
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
		std::cerr << "ERROR: frag_shader \n" << errorLog << std :: endl;
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


