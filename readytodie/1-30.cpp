#define _CRT_SECURE_NO_WARNINGS 

#include "static_model.h" // OBJ 로더 헤더

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <string>

using namespace std;

// --- 함수 선언 ---
void MakeVertexShaders();
void MakeFragmentShaders();
GLuint MakeShaderProgram();
void InitData();
GLvoid DrawScene();
GLvoid Reshape(int w,int h);
void Mouse(int button,int state,int x,int y);
void Keyboard(unsigned char key,int x,int y);
void SpecialKeyboard(int key,int x,int y);
void TimerFunction(int value);
GLchar* filetobuf(const char* file);

// --- 전역 변수 ---
GLint width = 800,height = 800;
GLuint shaderProgramID,vertexShader,fragmentShader;
// VAO, VBO 등은 StaticMesh 내부에서 관리하므로 전역 변수 불필요

// 랜덤 관련
random_device rd;
mt19937 gen(rd());

// 님 코드 변수 그대로 유지
bool change = false;
bool silver = false;
bool solid = false;
int x_ = 0;
bool y_ = false;
bool t = false; // Top
bool f = false; // Front
bool s = false; // Side
bool b = false; // Back
bool o = false; // Open (Pyramid)
bool r = false; // Rotate
bool r_start[4] = {false,};

glm::vec3 cameraPos = glm::vec3(5.0f,5.0f,5.0f);
glm::vec3 cameramove = glm::vec3(0.0f,0.0f,0.0f);
float cameraAngle = glm::atan(1.0f);
float modelRotX = 0.0f;
float modelRotY = 0.0f;

// --- Shape 클래스 (StaticMesh 연결용으로 수정) ---
class Shape {
public:
	StaticMesh* meshPtr; // OBJ 메쉬 포인터
	glm::vec3 t = {0.0f,0.0f,0.0f};
	glm::vec3 s = {1.0f,1.0f,1.0f};
	glm::vec3 r = {0.0f,0.0f,0.0f};
	glm::vec3 colors;
	glm::mat4 modelMat = glm::mat4(1.0f);
	int shape_num;
	float angle = 0.0f;

	Shape(StaticMesh* mesh,int i) {
		meshPtr = mesh;
		shape_num = i;
		ColorRandom();
	}

	void Draw(GLuint shaderID) {
		if(meshPtr) {
			meshPtr->Draw(shaderID);
		}
	}

	void ColorRandom() {
		colors = {0.0f,0.0f,0.0f};
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		colors = glm::vec3(r,g,b);
	}
};

vector<Shape> shape;
StaticModel* cubeModel = nullptr;
StaticModel* pyramidModel = nullptr;
StaticModel* planeModel = nullptr;

// --- 초기화 함수 (OBJ 로드 및 인덱스 매핑) ---
void InitData() {
	shape.clear();

	// 1. 모델 파일 로드
	if(cubeModel == nullptr) cubeModel = new StaticModel("nanacube.obj");
	if(pyramidModel == nullptr) pyramidModel = new StaticModel("nanapyramid.obj");
	if(planeModel == nullptr) planeModel = new StaticModel("plane1.obj");

	// shape 벡터를 11개(0~10)로 넉넉하게 확보 (님 코드 루프가 10까지 돌기 때문)
	shape.reserve(11);
	for(int i = 0; i < 11; i++) {
		shape.push_back(Shape(nullptr,i));
	}

	// 2. 큐브 매핑 (인덱스 0 ~ 5)
	// nanacube.obj의 6개 면을 shape[0]~shape[5]에 넣습니다.
	for(auto& mesh : cubeModel->meshes) {
		if(mesh.vertices.empty()) continue;
		glm::vec3 n = mesh.vertices[0].Normal;

		// 법선 벡터로 방향 판단하여 님 코드 순서에 맞춤
		if(n.z > 0.5f)       shape[0] = Shape(&mesh,0); 
		else if(n.x < -0.5f) shape[1] = Shape(&mesh,1); 
		else if(n.x > 0.5f)  shape[2] = Shape(&mesh,2); 
		else if(n.z < -0.5f) shape[3] = Shape(&mesh,3); 
		else if(n.y < -0.5f) shape[4] = Shape(&mesh,4); 
		else if(n.y > 0.5f)  shape[5] = Shape(&mesh,5); 
	}

	// 3. 피라미드 매핑 (인덱스 6 ~ 9)
	// nanapyramid.obj의 메쉬들을 순서대로 shape[6]부터 넣습니다.
	// (OBJ 파일이 쪼개져 있지 않다면 shape[6] 하나에만 들어갑니다)
	int pyIndex = 6;
	for(auto& mesh : pyramidModel->meshes) {
		if(pyIndex > 10) break;
		shape[pyIndex] = Shape(&mesh,pyIndex);
		pyIndex++;
	}
	// 4. 바닥면 매핑 (인덱스 10)
	shape[10] = Shape(&planeModel->meshes[0],10);
	shape[10].t.z = -5.0f;
	shape[10].angle = 45.0f;

}

// --- Main ---
int main(int argc,char** argv) {
	srand(static_cast<unsigned>(time(0)));
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(width,height);
	glutCreateWindow("Cube Pyramid Switch");

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) return -1;

	MakeVertexShaders();
	MakeFragmentShaders();
	shaderProgramID = MakeShaderProgram();
	if(shaderProgramID == 0) return -1;

	InitData(); // 데이터 로드

	glutTimerFunc(10,TimerFunction,1);
	glutDisplayFunc(DrawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);

	glutMainLoop();
	return 0;
}

// --- DrawScene (님의 구조 + Change 로직 복구) ---
void DrawScene() {
	// 카메라 설정
	float r = glm::sqrt(cameraPos.x * cameraPos.x + cameraPos.z * cameraPos.z);
	cameraPos.x = r * glm::cos(cameraAngle);
	cameraPos.z = r * glm::sin(cameraAngle);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f,0.2f,0.2f,1.0f); // 배경색 약간 어둡게

	if(silver) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	// 뷰, 프로젝션 행렬 설정 (쉐이더 변수명 uView, uProj 확인 필요)
	glm::mat4 view = glm::lookAt(cameraPos,cameramove,glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),(float)width / (float)height,0.1f,100.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID,"uView"),1,GL_FALSE,glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID,"uProj"),1,GL_FALSE,glm::value_ptr(proj));

	// 조명 변수 (fragment2.glsl 사용 시 필요)
	// 쉐이더에서 빛 계산 뺐으면 무시됨
	// glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 1.0f, 1.0f, 1.0f);

	if(solid) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	GLint modelLoc = glGetUniformLocation(shaderProgramID,"uModel");

	// --------------------------------------------------------------------------
	// [님 코드 그대로] change가 false일 때 -> 큐브 (0~5) 그리기
	// --------------------------------------------------------------------------
	for(int i = 0; i < 6; ++i) {
		if(!change) { // p키 안 눌렀을 때
			if(shape[i].meshPtr != nullptr) {
				glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[i].modelMat));
				shape[i].Draw(shaderProgramID);
			}
		}
	}

	// --------------------------------------------------------------------------
	// [님 코드 그대로] change가 true일 때 -> 피라미드 (6~10) 그리기
	// --------------------------------------------------------------------------
	for(int i = 6; i <= 9; ++i) {
		if(change) { // p키 눌렀을 때
			if(shape[i].meshPtr != nullptr) {
				glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[i].modelMat));
				shape[i].Draw(shaderProgramID);
			}
		}
	}


	glUniformMatrix4fv(modelLoc,1,GL_FALSE,glm::value_ptr(shape[10].modelMat));
	shape[10].Draw(shaderProgramID);


	glutSwapBuffers();
}

// --- Keyboard (님 코드 그대로) ---
void Keyboard(unsigned char key,int x,int y) {
	switch(key) {
	case 'p':
	change = !change; // [핵심] 여기서 토글됨
	break;
	case 'h':
	silver = !silver;
	break;
	case 'y':
	y_ = !y_;
	break;
	case 't':
	if(!change) t = !t;
	break;
	case 'f':
	if(!change) f = !f;
	break;
	case 's':
	if(!change) s = !s;
	break;
	case 'b':
	if(!change) b = !b;
	break;
	case 'o':
	// 피라미드일 때만 작동
	if(change && (shape[6].angle <= 0.0f || shape[6].angle >= 120.0f)) {
		o = !o;
	}
	break;
	case 'r':
	if(change && (r_start[3] == r_start[0])) {
		r = !r;
	}
	break;
	case 'c':
	x_ = 0; y_ = false;
	change = false;
	InitData();
	break;
	case 'q':
	exit(0);
	break;
	}
	glutPostRedisplay();
}

// --- TimerFunction (님 코드 100% 그대로 복붙) ---
void TimerFunction(int value) {
	static float x = 0.0f,y = 0.0f;
	for(int i = 0; i < shape.size(); ++i) {
		glm::mat4 m(1.0f);
		if(y_) y += 0.1f; // 회전 속도 조절

		if(i!=10)
		m = glm::rotate(m,glm::radians(y),{0.0f,1.0f,0.0f});

		// 큐브 애니메이션 (0~5)
		if(i == 0) {
			if(f && shape[i].angle > -90.0f) {
				shape[i].angle -= 1.0f;
			} else if(shape[i].angle < 0.0f) {
				shape[i].angle += 1.0f;
			}
			m = glm::translate(m,{0.0f,0.5,0.5f});
			m = glm::rotate(m,glm::radians(shape[i].angle),{1.0f,0.0f,0.0f});
			m = glm::translate(m,{0.0f,-0.5f,-0.5f});
		}
		if(i == 1 || i == 2) {
			if(s) shape[i].angle += 1.0f;
			if(i == 1) {
				m = glm::translate(m,{-0.5f,0.0f,0.0f});
				m = glm::rotate(m,glm::radians(shape[i].angle),{1.0f,0.0f,0.0f});
				m = glm::translate(m,{0.5f,0.0f,-0.0f});
			}
			if(i == 2) {
				m = glm::translate(m,{0.5f,0.0f,0.0f});
				m = glm::rotate(m,glm::radians(shape[i].angle),{1.0f,0.0f,0.0f});
				m = glm::translate(m,{-0.5f,0.0f,-0.0f});
			}
		}
		if(i == 5) {
			if(t) shape[i].angle += 1.0f;
			m = glm::translate(m,{0.0f,0.5f,0.0f});
			m = glm::rotate(m,glm::radians(shape[i].angle),{0.0f,0.0f,1.0f});
			m = glm::translate(m,{0.0f,-0.5f,0.0f});
		}
		m = glm::translate(m,shape[i].t);
		if(i == 3) {
			if(b && shape[i].s.x >= 0.0) {
				shape[i].s = {shape[i].s.x - 0.1f,shape[i].s.y - 0.1f,shape[i].s.z};
			} else if(shape[i].s.x < 1.0f) {
				shape[i].s = {shape[i].s.x + 0.1f,shape[i].s.y + 0.1f,shape[i].s.z};
			}
			m = glm::translate(m,{0.0f,0.0f,-0.5f});
			m = glm::scale(m,shape[i].s);
			m = glm::translate(m,{0.0f,0.0f,0.5f});
		}

		// 피라미드 애니메이션 (6~9)
		if(i == 6) {
			if(o && shape[i].angle < 120.0f) {
				shape[i].angle += 1.0f;
			} else if(!r && shape[i].angle > 0.0f) {
				shape[i].angle -= 1.0f;
				if(shape[i].angle <= 0.0f)r_start[0] = false;
			}

			if(r && shape[i].angle < 90.0f) {
				shape[i].angle += 1.0f;
				if(shape[i].angle >= 90.0f)r_start[0] = true;
			}

			m = glm::translate(m,{0.0f,-0.2f,0.5f});
			m = glm::rotate(m,glm::radians(shape[i].angle),{1.0f,0.0f,0.0f});
			m = glm::translate(m,{0.0f,0.2f,-0.5f});
		}
		if(i == 7) {
			if(o && shape[i].angle < 120.0f) {
				shape[i].angle += 1.0f;
			} else if(!r && shape[i].angle > 0.0f) {
				shape[i].angle -= 1.0f;
				if(shape[i].angle <= 0.0f)r_start[1] = false;
			}
			if(r && shape[i].angle < 90.0f && r_start[0]) {
				shape[i].angle += 1.0f;
				if(shape[i].angle >= 90.0f)r_start[1] = true;
			}
			m = glm::translate(m,{-0.5f,-0.2f,0.0f});
			m = glm::rotate(m,glm::radians(shape[i].angle),{0.0f,0.0f,1.0f});
			m = glm::translate(m,{0.5f,0.2f,-0.0f});
		}
		if(i == 8) {
			if(o && shape[i].angle > -120.0f) {
				shape[i].angle -= 1.0f;
			} else if(!r && shape[i].angle < 0.0f) {
				shape[i].angle += 1.0f;
				if(shape[i].angle >= 0.0f)r_start[2] = false;
			}
			if(r && shape[i].angle > -90.0f && r_start[1]) {
				shape[i].angle -= 1.0f;
				if(shape[i].angle <= -90.0f)r_start[2] = true;
			}
			m = glm::translate(m,{0.5f,-0.2f,0.0f});
			m = glm::rotate(m,glm::radians(shape[i].angle),{0.0f,0.0f,1.0f});
			m = glm::translate(m,{-0.5f,0.2f,-0.0f});
		}
		if(i == 9) {
			if(o && shape[i].angle > -120.0f) {
				shape[i].angle -= 1.0f;
			} else if(!r && shape[i].angle < 0.0f) {
				shape[i].angle += 1.0f;
				if(shape[i].angle >= 0.0f)r_start[3] = false;
			}
			if(r && shape[i].angle > -90.0f && r_start[2]) {
				shape[i].angle -= 1.0f;
				if(shape[i].angle <= -90.0f)r_start[3] = true;
			}
			m = glm::translate(m,{0.0f,-0.2f,-0.5f});
			m = glm::rotate(m,glm::radians(shape[i].angle),{1.0f,0.0f,0.0f});
			m = glm::translate(m,{0.0f,0.2f,0.5f});
		}
		if(i==10){
			m = glm::translate(m,shape[i].t);
			m = glm::rotate(m,glm::radians(45.0f),{0.0f,1.0f,0.0f});
		}
		m = glm::scale(m,shape[i].s);
		shape[i].modelMat = m;
	}

	glutTimerFunc(10,TimerFunction,1);
	glutPostRedisplay();
}

// --- 기타 함수들 ---
void Reshape(int w,int h) {
	glViewport(0,0,w,h);
	width = w; height = h;
}
void Mouse(int button,int state,int x,int y) {}
void SpecialKeyboard(int key,int x,int y) {
	switch(key) {
	case GLUT_KEY_UP:    modelRotX += 0.1f; break;
	case GLUT_KEY_DOWN:  modelRotX -= 0.1f; break;
	case GLUT_KEY_LEFT:  modelRotY += 0.1f; break;
	case GLUT_KEY_RIGHT: modelRotY -= 0.1f; break;
	}
	glutPostRedisplay();
}
GLchar* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file,"rb");
	if(!fptr) return NULL;
	fseek(fptr,0,SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr,0,SEEK_SET);
	fread(buf,length,1,fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}
void MakeVertexShaders() {
	GLchar* vertexSource = filetobuf("static_vertex.glsl");
	if(!vertexSource) {
		cerr << "vertex source missing" << endl; return;
	}
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1,&vertexSource,NULL);
	glCompileShader(vertexShader);
	free(vertexSource);
}
void MakeFragmentShaders() {
	GLchar* fragmentSource = filetobuf("fragment2.glsl");
	if(!fragmentSource) {
		cerr << "frag source missing" << endl; return;
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,&fragmentSource,NULL);
	glCompileShader(fragmentShader);
	free(fragmentSource);
}
GLuint MakeShaderProgram() {
	GLuint pID = glCreateProgram();
	glAttachShader(pID,vertexShader);
	glAttachShader(pID,fragmentShader);
	glLinkProgram(pID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(pID);
	return pID;
}