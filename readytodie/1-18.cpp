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
	vector<glm::vec3>colors;
	int shape_num;
	vector<float> angle[3];
	Shape(Model model,int i){
		vertexData.clear();
		InitBuffer(model);
		ColorRandom(model);
		shape_num = i;
	}

	void InitBuffer(Model model){
		vertexData.clear();
		angle[0].clear();
		angle[1].clear();
		angle[2].clear();
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
			// 각 꼭짓점에 대한 회전 각도 초기화 (0으로 설정)
			angle[0].push_back(0.0f);
			angle[1].push_back(0.0f);
			angle[2].push_back(0.0f);
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
		colors.clear();
		for(size_t i = 0; i < model.face_count; ++i) {
			for(int j=0;j<3;++j){
				float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				colors.push_back(glm::vec3(r,g,b));
			}
		}
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
	Model model{}; // 강제 초기화: 모든 멤버 0 또는 nullptr로 초기화

	fopen_s(&file,filename,"r");
	if(!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[365];
	model.vertex_count = 0;
	model.face_count = 0;

	while(fgets(line,sizeof(line),file)) {
		read_newline(line);
		if(line[0] == 'v' && line[1] == ' ')
			model.vertex_count++;
		else if(line[0] == 'f' && line[1] == ' ')
			model.face_count++;
	}

	fseek(file,0,SEEK_SET);

	model.vertices = (Vertex*)malloc(model.vertex_count * sizeof(Vertex));
	model.faces = (Face*)malloc(model.face_count * sizeof(Face));

	size_t vertex_index = 0;
	size_t face_index = 0;
	while(fgets(line,sizeof(line),file)) {
		read_newline(line);
		if(line[0] == 'v' && line[1] == ' ') {
			sscanf_s(line + 2,"%f %f %f",
					 &model.vertices[vertex_index].x,
					 &model.vertices[vertex_index].y,
					 &model.vertices[vertex_index].z);
			vertex_index++;
		} else if(line[0] == 'f' && line[1] == ' ') {
			unsigned int v1,v2,v3;
			sscanf_s(line + 2,"%u %u %u",&v1,&v2,&v3);
			model.faces[face_index].v1 = v1 - 1;
			model.faces[face_index].v2 = v2 - 1;
			model.faces[face_index].v3 = v3 - 1;
			face_index++;
		}
	}

	fclose(file);
	return model;
}


vector<Model> model;
int facenum = -1;
int modelType = 0; // 0: Cube, 1: Cone
bool allFaceDraw = true; // true: 전체 면 그리기, false: 한 면씩 그리기

glm::vec3 cameraPos = glm::vec3(5.0f,5.0f,5.0f);
float cameraAngle = glm::atan(1.0f); // 초기 각도 (45도 라디안)

// 도형(모델) 이동 및 회전 관련 추가 전역 변수
glm::vec3 modelPos = glm::vec3(0.0f,0.0f,0.0f);  // 도형 위치 (X, Y, Z 이동량)
float modelRotY = 0.0f;  // Y축 회전 각도 (라디안)
float modelRotX = 0.0f;  // X축 회전 각도 (라디안)

int shape_check = 0;
int x_=0;
int y_=0;
int r=0;
int a=0;
int b=0;
int d=0;
int e=0;
bool t=false;
bool u=false;
bool v=false;
bool c=false;



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

void InitData(){
	shape.clear();
	for(size_t i=0;i<model.size();++i){
		shape.push_back(Shape(model[i],i)); // Shape 생성 시 model 정보 전달
	}
	for(size_t i=0;i<2;++i){
		shape[i].move(-2.0f,0.0f,0.0f);
	}
	for(size_t i=2;i<4;++i){
		shape[i].move(1.0f,0.0f,0.0f);
	}

}
void Update(){
	vector<GLfloat> vertexData;
	vector<GLuint> indices;

	size_t n = min(shape.size(),model.size()); // 안전하게 범위 제한
	for(size_t i=0;i<n;++i){
		shape[i].Update(model[i]);
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
	glutCreateWindow("Prac");

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

	model.push_back(read_obj_file("pyramid.obj"));
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

	glGenBuffers(1,&EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,0,nullptr,GL_DYNAMIC_DRAW);


	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),(void*)0);
	glEnableVertexAttribArray(0);



	glBindVertexArray(0);
}

void DrawScene() {
	// 카메라 위치 업데이트
	float r = glm::sqrt(cameraPos.x * cameraPos.x + cameraPos.z * cameraPos.z); // XZ 평면 거리
	cameraPos.x = r * glm::cos(cameraAngle);
	cameraPos.z = r * glm::sin(cameraAngle);

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화
	glClearColor(0.5f,0.5f,0.5f,1.0f);
	//if(silver)
	//	glEnable(GL_CULL_FACE);// 은면 제거 활성화
	//else
	//	glDisable(GL_CULL_FACE);


	//if(u){
	//	glEnable(GL_CULL_FACE);
	//	glCullFace(GL_BACK); // 뒷면 제거
	//} else{
	//	glDisable(GL_CULL_FACE);
	//}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 버퍼 클리어 추가

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	Update();

	/*if(!solid)
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);*/


	// Uniform 매트릭스 매핑
	GLint modelLoc = glGetUniformLocation(shaderProgramID,"model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID,"view");
	GLint projLoc = glGetUniformLocation(shaderProgramID,"proj");
	GLint faceColorLoc = glGetUniformLocation(shaderProgramID,"faceColor");


	//--------------------------------------------------------------------------
	// 각 면(6 indices)마다 색상 uniform 설정 후 그리기
	GLuint offset = 0;
	for(int i=0;i<shape.size();++i){
		for(int j = 0; j < shape[i].vertexData.size() / 9; ++j){ // colors 개수 == face 개수
			if((c&&i%2!=0)||(!c&&i%2==0)){
				// 모델 매트릭스: 이동 + 회전 적용 (회전은 이동 후 적용되도록 순서 주의)
				glm::mat4 modelMat = glm::mat4(1.0f);
				modelMat = glm::translate(modelMat,modelPos);  // 먼저 이동 적용
				modelMat = glm::rotate(modelMat,glm::radians(shape[i].angle[0][j]),glm::vec3(1.0f,0.0f,0.0f));  // X축 회전
				modelMat = glm::rotate(modelMat,glm::radians(shape[i].angle[1][j]),glm::vec3(0.0f,1.0f,0.0f));  // Y축 회전
				modelMat = glm::rotate(modelMat,glm::radians(shape[i].angle[2][j]),glm::vec3(0.0f,0.0f,1.0f));//Z축 회전




				glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(modelMat));
				glm::vec3 col = shape[i].colors[j];

				glUniform3f(faceColorLoc,col.r,col.g,col.b);
				glDrawArrays(GL_TRIANGLES,offset,3);
			}


			offset += 3;




		}
	}

	
	//--------------------------------------------------------------------------
	// Camera (View) 및 Projection 매트릭스 설정
	glm::mat4 view = glm::lookAt(cameraPos,glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f)); // 뷰 매트릭스
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)width / (float)height,0.1f,100.0f); // 프로젝션 매트릭스

	glUniformMatrix4fv(viewLoc,1,GL_FALSE,glm::value_ptr(view));
	glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(proj));


	//---------------------------------------------------------------------------
	// XYZ 축 
	glm::mat4 axisModelMat = glm::mat4(1.0f);
	glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(axisModelMat));

	std::vector<GLfloat> axesData = {
		// X축
		-5.0f,0.0f,0.0f,  // 시작
		5.0f,0.0f,0.0f,  // 끝

		// Y축
		0.0f,-5.0f,0.0f,
		0.0f,5.0f,0.0f,

		// Z축
		0.0f,0.0f,-5.0f,
		0.0f,0.0f,5.0f
	};

	// 축용 VBO 업로드 (위치만, 색상 uniform 사용)
	glBufferData(GL_ARRAY_BUFFER,axesData.size() * sizeof(GLfloat),axesData.data(),GL_DYNAMIC_DRAW);

	// X축
	glUniform3f(faceColorLoc,1.0f,0.0f,0.0f);
	glDrawArrays(GL_LINES,0,2);

	// Y축 
	glUniform3f(faceColorLoc,0.0f,0.0f,1.0f);
	glDrawArrays(GL_LINES,2,2);

	// Z축 
	glUniform3f(faceColorLoc,0.0f,1.0f,0.0f);
	glDrawArrays(GL_LINES,4,2);

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

void Keyboard(unsigned char key,int x,int y)
{
	switch(key)
	{
	case '1':
	case '2':
	case '3':
	if(!t&&!u&&!v)
	shape_check=key-'0';
	break;
	case 'x':
	if(!t&&!u&&!v)
	x_ = (x_ != 1) ? 1 : 0;
	break;
	case 'X':
	if(!t&&!u&&!v)
	x_ = (x_ != 2) ? 2 : 0;
	break;
	case 'y':
	if(!t&&!u&&!v)
	y_=(y_ != 1) ?1 : 0;
	break;
	case 'Y':
	if(!t&&!u&&!v)
	y_=(y_ != 2) ? 2 : 0;
	break;
	case 'r':
	if(!t&&!u&&!v)
	r=(r != 1) ? 1 : 0;
	break;
	case 'R':
	if(!t&&!u&&!v)
	r=(r != 2) ? 2 : 0;
	break;
	case 'a':
	if(!t&&!u&&!v)
	a=(a != 1) ? 1 : 0;
	break;
	case 'A':
	if(!t&&!u&&!v)
	a=(a != 2) ? 2 : 0;
	break;
	case 'b':
	if(!t&&!u&&!v)
	b=(b != 1) ? 1 : 0;
	break;
	case 'B':
	if(!t&&!u&&!v)
	b=(b != 2) ? 2 : 0;
	break;
	case 'd':
	if(!t&&!u&&!v)
	d=(d != 1) ? 1 : 0;
	break;
	case 'D':
	if(!t&&!u&&!v)
	d=(d != 2) ? 2 : 0;
	break;
	case 'e':
	if(!t&&!u&&!v)
	e=(e != 1) ? 1 : 0;
	break;
	case 'E':
	if(!t&&!u&&!v)
	e=(e != 2) ? 2 : 0;
	break;
	case 't':
	if(!u&&!v){
		t=true;

	}
	break;
	case 'u':
	if(!t&&!v){
		u=true;
	}
	break;
	case 'v':
	if(!t&&!u){
		v=!v;
	}
	break;

	case 'c':
	c=!c;
	break;

	case 's':
	InitData();
	shape_check = 0;
	x_=0;
	y_=0;
	r=0;
	a=0;
	b=0;
	d=0;
	e=0;
	t=false;
	u=false;
	v=false;
	c=false;
	scale_time=0;
	break;


	case 'q':
	exit(0);
	break;
	default:
	break;
	}

	glutPostRedisplay();
}

// 특수 키(화살표) 처리 함수: 도형 회전
void SpecialKeyboard(int key,int x,int y)
{


	glutPostRedisplay();  // 재렌더링 요청
}
void X_rotate(){
	if(x_==1){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(x_rotate),i,1);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(x_rotate),i,1);
				}
			}
		}
	}
	else if(x_==2){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(-x_rotate),i,1);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(-x_rotate),i,1);
				}
			}
		}
	}
}
void Y_rotate(){
	if(y_==1){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(y_rotate),i,2);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(y_rotate),i,2);
				}
			}
		}
	} else if(y_==2){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(-y_rotate),i,2);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].RotateWorld(cx,cy,cz,glm::radians(-y_rotate),i,2);
				}
			}
		}
	}
}
void Y_gong(){
	if(r==1){
		if(shape_check==1||shape_check==3){
			for(int i=0;i<shape[0].angle[1].size();++i)shape[0].angle[1][i] += y_rotate;
			for(int i=0;i<shape[1].angle[1].size();++i)shape[1].angle[1][i] += y_rotate;

		}
		if(shape_check==2||shape_check==3){
			for(int i=0;i<shape[2].angle[1].size();++i)shape[2].angle[1][i] += y_rotate;
			for(int i=0;i<shape[3].angle[1].size();++i)shape[3].angle[1][i] += y_rotate;
		}
	} else if(r==2){
		if(shape_check==1||shape_check==3){
			for(int i=0;i<shape[0].angle[1].size();++i)shape[0].angle[1][i] -= y_rotate;
			for(int i=0;i<shape[1].angle[1].size();++i)shape[1].angle[1][i] -= y_rotate;

		}
		if(shape_check==2||shape_check==3){
			for(int i=0;i<shape[2].angle[1].size();++i)shape[2].angle[1][i] -= y_rotate;
			for(int i=0;i<shape[3].angle[1].size();++i)shape[3].angle[1][i] -= y_rotate;
		}
	}
}
void SizeUPDown(){
	if(a==1){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(cx,cy,cz,1.01001f,i);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(cx,cy,cz,1.01001f,i);
				}
			}
		}
	} else if(a==2){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(cx,cy,cz,0.99f,i);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				float cx = 0,cy = 0,cz = 0;
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					cx += shape[j].vertexData[i * 3 + 0];
					cy += shape[j].vertexData[i * 3 + 1];
					cz += shape[j].vertexData[i * 3 + 2];
				}
				cx /=shape[j].vertexData.size()/3;
				cy /=shape[j].vertexData.size()/3;
				cz /=shape[j].vertexData.size()/3;

				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(cx,cy,cz,0.99f,i);
				}
			}
		}
	}
}
void SizeOne(){
	if(b==1){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(0,0,0,1.01001f,i);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(0,0,0,1.01001f,i);
				}
			}
		}
	} else if(b==2){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(0,0,0,0.99f,i);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].Scale(0,0,0,0.99f,i);
				}
			}
		}
	}
}
void MoveX(){
	if(d==1){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(0.0005f,0,0);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(0.0005f,0,0);
				}
			}
		}
	} else if(d==2){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(-0.0005f,0,0);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(-0.0005f,0,0);
				}
			}
		}
	}
}
void MoveY(){
	if(e==1){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(0,0.0005f,0);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(0,0.0005f,0);
				}
			}
		}
	} else if(e==2){
		if(shape_check==1||shape_check==3){
			for(int j = 0; j < 2; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(0,-0.0005f,0);
				}
			}

		}
		if(shape_check==2||shape_check==3){
			for(int j = 2; j < 4; ++j){
				for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
					shape[j].move(0,-0.0005f,0);
				}
			}
		}
	}
}
void ChangeOne(){
	if(t)
	{
		static bool to_origin = true;
		static bool initialized = false;
		static glm::vec3 target_pos[4];
		float speed = 0.05f; // 이동 속도

		if(!initialized)
		{
			glm::vec3 c0 = shape[0].Center();
			glm::vec3 c1 = shape[1].Center();
			glm::vec3 c2 = shape[2].Center();
			glm::vec3 c3 = shape[3].Center();

			target_pos[0] = c2;
			target_pos[2] = c0;
			target_pos[1] = c3;
			target_pos[3] = c1;

			initialized = true;
		}

		if(to_origin)
		{
			bool all_origin = true;

			for(int i = 0; i < 4; ++i)
			{
				glm::vec3 c = shape[i].Center();
				glm::vec3 dir = glm::vec3(0,0,0) - c;

				if(glm::length(dir) > 0.001f)
				{
					for(int j = 0; j < shape[i].vertexData.size() / 3; ++j)
					{
						shape[i].vertexData[j * 3 + 0] += dir.x * speed;
						shape[i].vertexData[j * 3 + 1] += dir.y * speed;
						shape[i].vertexData[j * 3 + 2] += dir.z * speed;
					}
					all_origin = false;
				}
			}

			if(all_origin)
				to_origin = false;
		} else
		{
			bool all_done = true;

			for(int i = 0; i < 4; ++i)
			{
				glm::vec3 c = shape[i].Center();
				glm::vec3 dir = target_pos[i] - c;

				if(glm::length(dir) > 0.001f)
				{
					for(int j = 0; j < shape[i].vertexData.size() / 3; ++j)
					{
						shape[i].vertexData[j * 3 + 0] += dir.x * speed;
						shape[i].vertexData[j * 3 + 1] += dir.y * speed;
						shape[i].vertexData[j * 3 + 2] += dir.z * speed;
					}
					all_done = false;
				}
			}

			if(all_done)
			{
				t = false;
				to_origin = true;
				initialized = false;
			}
		}
	}

}
void ChangeUpDown(){
	if (u)
{
    static bool move_updown = true;
    static bool initialized = false;
    static glm::vec3 target_pos[4];
	static glm::vec3 original_pos[4];
    float speed = 0.05f; // 이동 속도 (점진적)

    if (!initialized)
    {
        glm::vec3 c0 = shape[0].Center();
        glm::vec3 c1 = shape[1].Center();
        glm::vec3 c2 = shape[2].Center();
        glm::vec3 c3 = shape[3].Center();

        // 1단계 목표 (위/아래로 이동)
        target_pos[0] = glm::vec3(c0.x, c0.y + 3.0f, c0.z);
        target_pos[1] = glm::vec3(c1.x, c1.y + 3.0f, c1.z);
        target_pos[2] = glm::vec3(c2.x, c2.y - 3.0f, c2.z);
        target_pos[3] = glm::vec3(c3.x, c3.y - 3.0f, c3.z);

		original_pos[0] = c2;
		original_pos[2] = c0;
		original_pos[1] = c3;
		original_pos[3] = c1;

        initialized = true;
    }

    if (move_updown)
    {
        bool all_done = true;

        for (int i = 0; i < 4; ++i)
        {
            glm::vec3 c = shape[i].Center();
            glm::vec3 dir = target_pos[i] - c;

            if (glm::length(dir) > 0.001f)
            {
                for (int j = 0; j < shape[i].vertexData.size() / 3; ++j)
                {
                    shape[i].vertexData[j * 3 + 0] += dir.x * speed;
                    shape[i].vertexData[j * 3 + 1] += dir.y * speed;
                    shape[i].vertexData[j * 3 + 2] += dir.z * speed;
                }
                all_done = false;
            }
        }

        if (all_done)
        {
            // 위아래 이동 완료 후 교환 준비
            move_updown = false;

            // 교체 목표 위치 설정
            glm::vec3 c0 = shape[0].Center();
            glm::vec3 c1 = shape[1].Center();
            glm::vec3 c2 = shape[2].Center();
            glm::vec3 c3 = shape[3].Center();

            target_pos[0] = c2;
            target_pos[2] = c0;
            target_pos[1] = c3;
            target_pos[3] = c1;
        }
    }
    else
    {
        bool all_swapped = true;

        for (int i = 0; i < 4; ++i)
        {
            glm::vec3 c = shape[i].Center();
            glm::vec3 dir = original_pos[i] - c;

            if (glm::length(dir) > 0.001f)
            {
                for (int j = 0; j < shape[i].vertexData.size() / 3; ++j)
                {
                    shape[i].vertexData[j * 3 + 0] += dir.x * speed;
                    shape[i].vertexData[j * 3 + 1] += dir.y * speed;
                    shape[i].vertexData[j * 3 + 2] += dir.z * speed;
                }
                all_swapped = false;
            }
        }

        if (all_swapped)
        {
            u = false;
            move_updown = true;
            initialized = false;
        }
    }
}
}
void Allmove(){
	if(v){
		
			
		for(int j = 0; j < 2; ++j){
			float cx = 0,cy = 0,cz = 0;
			for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
				cx += shape[j].vertexData[i * 3 + 0];
				cy += shape[j].vertexData[i * 3 + 1];
				cz += shape[j].vertexData[i * 3 + 2];
			}
			cx /=shape[j].vertexData.size()/3;
			cy /=shape[j].vertexData.size()/3;
			cz /=shape[j].vertexData.size()/3;

			for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
				shape[j].RotateWorld(cx,cy,cz,glm::radians(x_rotate),i,1);
				shape[j].Scale(cx,cy,cz,1.001f,i);
			}
		}
		for(int i=0;i<shape[0].angle[1].size();++i)shape[0].angle[1][i] += y_rotate;
		for(int i=0;i<shape[1].angle[1].size();++i)shape[1].angle[1][i] += y_rotate;
	
	
		for(int j = 2; j < 4; ++j){
			float cx = 0,cy = 0,cz = 0;
			for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
				cx += shape[j].vertexData[i * 3 + 0];
				cy += shape[j].vertexData[i * 3 + 1];
				cz += shape[j].vertexData[i * 3 + 2];
			}
			cx /=shape[j].vertexData.size()/3;
			cy /=shape[j].vertexData.size()/3;
			cz /=shape[j].vertexData.size()/3;

			for(int i = 0; i < shape[j].vertexData.size()/3; ++i) {
				shape[j].RotateWorld(cx,cy,cz,glm::radians(x_rotate),i,1);
				shape[j].Scale(cx,cy,cz,0.999f,i);
			}
		}
		for(int i=0;i<shape[2].angle[1].size();++i)shape[2].angle[1][i] += y_rotate;
		for(int i=0;i<shape[3].angle[1].size();++i)shape[3].angle[1][i] += y_rotate;
		
	}
}
void TimerFunction(int value)
{
	X_rotate();
	Y_rotate();
	Y_gong();
	SizeUPDown();
	SizeOne();
	MoveX();
	MoveY();
	ChangeOne();
	ChangeUpDown();
	Allmove();

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


