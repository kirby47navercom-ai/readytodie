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
		for(size_t i = 0; i < model.normals.size(); ++i) {
			normalData.push_back(model.normals[i].x);
			normalData.push_back(model.normals[i].y);
			normalData.push_back(model.normals[i].z);
		}
		// 인덱스는 필요 없으므로 비워둡니다
		indices.clear();
	}
	void ColorRandom(Model model){
		colors={0.0f,0.0f,0.0f};

		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colors=glm::vec3(r,g,b);

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
bool r_=0;
bool n=false;
bool m=false;
bool y_=false;
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


float _z=0.0f;
int z_mode=0;

//커비
void InitData(){
	shape.clear();
	for(size_t i=0;i<2;++i){
		shape.push_back(Shape(model[i],i));// Shape 생성 시 model 정보 전달
	}
	shape.push_back(Shape(model[0],0));
	shape.back().s={0.5f,0.5f,0.5f};
	shape.back().t={2.0f,0.0f,0.0f};
	for(size_t i=0;i<20;++i){
		shape.push_back(Shape(model[2],i));
		shape.back().t=shape[2].t;
		shape.back().s={0.2f,0.2f,0.2f};
		shape.back().angle=static_cast<float>(i)*18.0f;
	}



}
void Update(){
	vector<GLfloat> combinateData;
	vector<GLuint> indices;
	for(size_t i=0;i<shape.size();++i){
		for(size_t j=0;j<shape[i].vertexData.size()/3;++j){
			combinateData.push_back(shape[i].vertexData[j*3+0]);
			combinateData.push_back(shape[i].vertexData[j*3+1]);
			combinateData.push_back(shape[i].vertexData[j*3+2]);
			combinateData.push_back(shape[i].normalData[j*3+0]);
			combinateData.push_back(shape[i].normalData[j*3+1]);
			combinateData.push_back(shape[i].normalData[j*3+2]);
			
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,combinateData.size()*sizeof(GLfloat),combinateData.data(),GL_DYNAMIC_DRAW);
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
	model.push_back(read_obj_file("pyramid.obj"));
	model.push_back(read_obj_file("sphere.obj"));

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

	


	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)(3 * sizeof(float))); //--- 노말 속성
	glEnableVertexAttribArray(1);


	glBindVertexArray(0);
}

void DrawScene() {

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
	glClearColor(0.5f,0.5f,0.5f,1.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
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
	GLint modelNormalLocation = glGetUniformLocation(shaderProgramID,"modelNormal"); //--- modelNormal 값 전달: 모델 매트릭스의 역전치 행렬


	//--------------------------------------------------------------------------

	glm::mat4 view = glm::lookAt(cameraPos1,camera_move,glm::vec3(0,1,0));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)(width)/height,0.1f,100.0f);
	glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
	glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(proj));
	// 도형 그리기
	GLuint offset = 0;
	
	for(int j = 0; j < shape[0].vertexData.size() / 9; ++j){
		if(!n){
			glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[0].modelMat));
			glUniformMatrix3fv(modelNormalLocation,1,GL_FALSE,glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(shape[0].modelMat)))));
			glUniform3f(faceColorLoc,shape[0].colors[0],shape[0].colors[1],shape[0].colors[2]);
			glDrawArrays(GL_TRIANGLES,offset,3);

		}
		offset += 3;
	}

	for(int j = 0; j < shape[1].vertexData.size() / 9; ++j){
		if(n){
			glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[1].modelMat));
			glUniformMatrix3fv(modelNormalLocation,1,GL_FALSE,glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(shape[1].modelMat)))));
			glUniform3f(faceColorLoc,shape[1].colors[0],shape[1].colors[1],shape[1].colors[2]);
			glDrawArrays(GL_TRIANGLES,offset,3);
		}
		offset += 3;
	}

	for(int j = 0; j < shape[2].vertexData.size() / 9; ++j){
	
		glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[2].modelMat));
		glUniformMatrix3fv(modelNormalLocation,1,GL_FALSE,glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(shape[2].modelMat)))));
		glUniform3f(faceColorLoc,shape[1].colors[0],shape[2].colors[1],shape[2].colors[2]);
		glDrawArrays(GL_TRIANGLES,offset,3);
	
		offset += 3;
	}

	for(int i=3;i<shape.size();++i){
		for(int j = 0; j < shape[i].vertexData.size() / 9; ++j){

			glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[i].modelMat));
			glUniformMatrix3fv(modelNormalLocation,1,GL_FALSE,glm::value_ptr(glm::mat3(glm::transpose(glm::inverse(shape[i].modelMat)))));
			glUniform3f(faceColorLoc,shape[i].colors[0],shape[i].colors[1],shape[i].colors[2]);
			glDrawArrays(GL_TRIANGLES,offset,3);

			offset += 3;
		}
	}

	



	GLint lightPosLocation = glGetUniformLocation(shaderProgramID,"lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	GLint lightColorLocation = glGetUniformLocation(shaderProgramID,"lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	GLint viewPosLocation = glGetUniformLocation(shaderProgramID,"viewPos"); //--- viewPos 값 전달: 카메라 위치
	
	


	glm::vec3 lightPos={0.0f,0.0f,0.0f};
	glm::mat4 lightModelMat = shape[2].modelMat;
	lightPos = glm::vec3(lightModelMat * glm::vec4(lightPos,1.0f));


	glUniform3f (lightPosLocation,lightPos.x,lightPos.y,lightPos.z);

	if(!m)
		glUniform3f (lightColorLocation,1.0,1.0,1.0);
	else
		glUniform3f (lightColorLocation,0.0,0.0,0.0);

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
	case'n':
	n=!n;
	break;
	case 'm':
	m=!m;
	break;
	case 'y':
	y_=!y_;
	break;
	case 'r':
	r_ = !r_;
	break;
	case 'z':
	shape[2].t.x += 0.1f;
	break;
	case 'Z':
	shape[2].t.x -= 0.1f;
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

	for(int i=0;i<shape.size();++i){
		glm::mat4 m(1.0f);
		if(i<2){
			if(y_)
			shape[i].angle += 0.01f;
			m = glm::rotate(m,shape[i].angle,glm::vec3(0.0f,1.0f,0.0f));
		}
		else if(i==2){
			if(r_)shape[i].angle += 0.01f;
			m = glm::rotate(m,shape[i].angle,glm::vec3(0.0f,1.0f,0.0f));
		} 
		else if(i>2){
			m = glm::rotate(m,glm::radians(shape[i].angle),glm::vec3(0.0f,1.0f,0.0f));
		}
		if(i<3)
			m = glm::translate(m,shape[i].t);
		else 
			m = glm::translate(m,shape[2].t);
		m = glm::scale(m,shape[i].s);
		shape[i].modelMat = m;
	}

	glutTimerFunc(10,TimerFunction,1);
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


