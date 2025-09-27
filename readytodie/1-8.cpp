#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "dtd.h" 
#define SIZE 600
int x,y;
float f=0.1f,l=0.1f,o=0.1f;
int star=0;
vector<GLfloat> a,c;
vector<int>_i;
int time_count=0;
int stack_count=0;
int check=0;
int mode=0;
GLuint VAO,VBO;
GLchar *vertexSource,*fragmentSource; //--- 소스코드 저장 변수 //--- 세이더 객체
const float vertexData[] =
{
	0.5,1.0,0.0,
	1.0,0.0,0.0,
	0.0,0.0,0.0,
	
	0.0,1.0,0.0,
	1.0,0.0,0.0,
	0.0,0.0,1.0
};


bool left_mouse=false;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dis(0,SIZE/2);
uniform_int_distribution<int> did(-SIZE/2,SIZE/2);
uniform_real_distribution<float> rcolor(0.0f,1.0f);
struct FPOINT{
	GLfloat x;
	GLfloat y;
};
struct FRECT{
	GLfloat left;
	GLfloat top;
	GLfloat right;
	GLfloat bottom;
};
FRECT test;
struct dtd{
	RECT rect;
	float r=0,g=0,b=0;
	int num=0;
};
vector<dtd> arr;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w,int h);
void idleScene();
void Keyboard(unsigned char key,int x,int y);
void Keyupboard(unsigned char key,int x,int y);
void SpecialKeyboard (int key,int x,int y);
void Mouse (int button,int state,int x,int y);
void Motion (int x,int y);
int glutGetModifiers ();
void TimerFunction (int value);
void InitBuffer ();
//필요한 헤더파일 선언
//아래 5 개 함수는 사용자 정의 함수 임
void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape (int w,int h);
//void MakeMenu ();
//void MenuFunction (int button);


//필요한 변수 선언
GLint width,height;
GLuint shaderProgramID; // 세이더 프로그램 이름
GLuint vertexShader; // 버텍스 세이더 객체
GLuint fragmentShader; // 프래그먼트 세이더 객체


GLfloat tranformx(int x){
	return ((float)x/(SIZE/2))-1.0f;
}
GLfloat tranformy(int y){
	return ((SIZE - (float)y)/(SIZE/2)) -1.0f;
}
void GLRectf(FRECT rect){
	glRectf(rect.left,rect.top,rect.right,rect.bottom);
}
void OffsetFRect(FRECT& rect,GLfloat x,GLfloat y){
	rect.left+=x;
	rect.top+=y;
	rect.right+=x;
	rect.bottom+=y;
}
void InflateFRect(FRECT& rect,GLfloat x,GLfloat y){
	rect.left-=x;
	rect.top-=y;
	rect.right+=x;
	rect.bottom+=y;
}
bool FPtInFRect(const FRECT& rect,const FPOINT& point){
	return (std::min(rect.left,rect.right) <= point.x && point.x <= std::max(rect.left,rect.right) &&
			std::min(rect.top,rect.bottom) <= point.y && point.y <= std::max(rect.top,rect.bottom));
}
bool IntersectFRect(FRECT &check,const FRECT& rect1,const FRECT& rect2){

	check.left = std::max(rect1.left,rect2.left);
	check.top = std::max(rect1.top,rect2.top);


	check.right = std::min(rect1.right,rect2.right);
	check.bottom = std::min(rect1.bottom,rect2.bottom);

	if(check.left < check.right && check.top < check.bottom) {
		return true;
	} else {
		check.left = check.top = check.right = check.bottom = 0;
		return false;
	}
}

bool PointToLine(float mx,float my,float p1x,float p1y,float p2x,float p2y,float tolerance)
{
	// 선분의 길이가 0이면 (점이면) 그냥 false
	if(p1x == p2x && p1y == p2y) return false;

	float lineVecX = p2x - p1x;
	float lineVecY = p2y - p1y;
	float mouseVecX = mx - p1x;
	float mouseVecY = my - p1y;

	float lenSq = lineVecX * lineVecX + lineVecY * lineVecY;
	float dot = mouseVecX * lineVecX + mouseVecY * lineVecY;
	float t = dot / lenSq;

	
	if(t < 0 || t > 1) {
		return false;
	}

	
	float numerator = abs(lineVecX * mouseVecY - lineVecY * mouseVecX);
	float distance = numerator / sqrt(lenSq);

	return distance < tolerance;
}

bool PointToTriangle(float px,float py,float ax,float ay,float bx,float by,float cx,float cy)
{
	float cross1 = (bx - ax) * (py - ay) - (by - ay) * (px - ax);
	float cross2 = (cx - bx) * (py - by) - (cy - by) * (px - bx);
	float cross3 = (ax - cx) * (py - cy) - (ay - cy) * (px - cx);

	bool has_neg = (cross1 < 0) || (cross2 < 0) || (cross3 < 0);
	bool has_pos = (cross1 > 0) || (cross2 > 0) || (cross3 > 0);

	return !(has_neg && has_pos);
}

vector<float> pointver;
vector<float> linever;
vector<float> triver;
vector<float> twotriver;


void updateVBO()
{
	// 1. 두 벡터를 합칠 임시 벡터 생성
	vector<float> comver;
	comver.insert(comver.end(),pointver.begin(),pointver.end());
	comver.insert(comver.end(),linever.begin(),linever.end());
	comver.insert(comver.end(),triver.begin(),triver.end());
	comver.insert(comver.end(),twotriver.begin(),twotriver.end());

	// 2. 합쳐진 데이터로 VBO를 새로 업데이트
	if(!comver.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * comver.size(),comver.data(),GL_DYNAMIC_DRAW);
	}
}
vector<dtd> art;



int main(int argc,char** argv)
{
	//--- 윈도우 생성하기		 //--- 윈도우 출력하고 콜백함수 설정
	glutInit(&argc,argv);		// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(100,100);	// 윈도우의 위치 지정
	glutInitWindowSize(SIZE,SIZE);	// 윈도우의 크기 지정
	glutCreateWindow("Example1");	// 윈도우 생성 (윈도우 이름)
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit ();

	if(glewInit() != GLEW_OK)	// glew 초기화
	{
		std::cerr << "GLEW Initialized\n" << std::endl;
		exit(EXIT_FAILURE);
	} else
		std::cout << "GLEW Initialized\n";

	//세이더 읽어와서 세이더 프로그램 만들기 : 사용자 정의함수 호출
	make_vertexShaders();
	//버텍스 세이더 만들기
	make_fragmentShaders();
	//프래그먼트 세이더 만들기
	shaderProgramID = make_shaderProgram();
	//세이더 프로그램 만들기
	InitBuffer();


	glutTimerFunc (10,TimerFunction,1);
	glutDisplayFunc(drawScene);	// 출력 함수의 지정
	glutReshapeFunc(Reshape);	// 다시 그리기 함수 지정
	glutIdleFunc(idleScene);			// 아이들 타임에 호출하는함수의지정
	glutKeyboardFunc(Keyboard);			// 키보드 입력 콜백함수
	glutGetModifiers();				// 컨트롤 알트 시프트 확인
	glutKeyboardUpFunc(Keyupboard);		// 키보드 뗄때 콜백함수
	glutSpecialFunc (SpecialKeyboard); // 특수키 입력 콜백함수
	glutMouseFunc (Mouse);				// 마우스 입력 콜백함수 마우스를 누르고 움직일때
	glutMotionFunc (Motion);			// 마우스 움직임 콜백함수 마우스를 때고 움직일때
	glutMainLoop();	// 이벤트 처리 시작
}
GLvoid drawScene () //--- 콜백 함수: 그리기 콜백 함수
{
	updateVBO();


	GLfloat rColor,gColor,bColor;
	rColor = bColor = 0.5;
	gColor = 0.5; //--- 배경색을 파랑색으로 설정
	glClearColor(rColor,gColor,bColor,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//--- uniform 변수의 인덱스 값
	//int vColorLocation = glGetUniformLocation (shaderProgramID,"in_Color");
	//--- uniform 변수가 있는 프로그램 활성화:
	glUseProgram (shaderProgramID);
	//--- 사용할 VAO 불러오기
	//glBindVertexArray(vao);
	//--- uniform 변수의 위치에 변수의 값 설정
	//glUniform4f (vColorLocation,1.0f,0.0f,0.0f,1.0f);

	//--- 필요한 드로잉 함수 호출
	glBindVertexArray(VAO);

	int triangleCount = triver.size() / 6;
	int pointCount = pointver.size() / 6;
	int lineCount = linever.size() / 6;
	int rectangleCount = twotriver.size() / 6;
	glPointSize(10.0f);
	int offset = 0;

	// 1. 점 그리기
	if(pointCount > 0)
	{
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS,offset,pointCount);
		offset += pointCount; 
	}
	// 2. 선 그리기
	if(lineCount > 0)
	{
		glLineWidth(10.0f);
		glDrawArrays(GL_LINES,offset,lineCount);
		glLineWidth(1.0f);
		offset += lineCount; 
	}
	// 3. 삼각형 그리기
	if(triangleCount > 0)
	{
		glDrawArrays(GL_TRIANGLES,offset,triangleCount);
		offset += triangleCount;
	}
	// 4. 사각형 그리기
	if(rectangleCount > 0)
	{
		glDrawArrays(GL_TRIANGLES,offset,rectangleCount);
		offset += lineCount;
	}


	//for(int i=0;i<pointver.size()/6;++i){
	//	glPointSize(5.0);
	//	glDrawArrays (GL_POINTS,i,1); //--- 렌더링하기: 0번 인덱스에서 1개의 버텍스를 사용하여 점 그리기
	//}


	//glPointSize(10.0);
	//glDrawArrays (GL_TRIANGLES,0,3); //--- 렌더링하기: 0번 인덱스에서 1개의 버텍스를 사용하여 점 그리기
	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w,int h)
{
	glViewport(0,0,w,h);
}
void idleScene()
{
	x += 0.02;
	y += 0.02;
	glutPostRedisplay();
}
void Keyupboard(unsigned char key,int x,int y){

}
void Keyboard(unsigned char key,int x,int y)
{
	switch(key) {
	case 'p':
	mode=0;
	break;
	case 'e':
	mode=1;
	break;
	case 't':
	mode=2;
	break;
	case 'r':
	mode=3;
	break;
	case 'y':
	mode=-1;
	break;
	if(star!=-1){
	case 'w':
	{
		int num = star;
		
		if(num<pointver.size()/6){
			pointver[num*6+1]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12+1]+=0.02f;
			linever[num*12+7]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18+1]+=0.02f;
			triver[num*18+7]+=0.02f;
			triver[num*18+13]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36+1]+=0.02f;
			twotriver[num*36+7]+=0.02f;
			twotriver[num*36+13]+=0.02f;
			twotriver[num*36+19]+=0.02f;
			twotriver[num*36+25]+=0.02f;
			twotriver[num*36+31]+=0.02f;
		}

	}
	break;
	case 'a':
	{
		int num = star;

		if(num<pointver.size()/6){
			pointver[num*6]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12]-=0.02f;
			linever[num*12+6]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18]-=0.02f;
			triver[num*18+6]-=0.02f;
			triver[num*18+12]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36]-=0.02f;
			twotriver[num*36+6]-=0.02f;
			twotriver[num*36+12]-=0.02f;
			twotriver[num*36+18]-=0.02f;
			twotriver[num*36+24]-=0.02f;
			twotriver[num*36+30]-=0.02f;
		}

	}
	break;
	case 's':
	{
		int num = star;
		if(num<pointver.size()/6){
			pointver[num*6+1]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12+1]-=0.02f;
			linever[num*12+7]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18+1]-=0.02f;
			triver[num*18+7]-=0.02f;
			triver[num*18+13]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36+1]-=0.02f;
			twotriver[num*36+7]-=0.02f;
			twotriver[num*36+13]-=0.02f;
			twotriver[num*36+19]-=0.02f;
			twotriver[num*36+25]-=0.02f;
			twotriver[num*36+31]-=0.02f;
		}
	}
	break;
	case 'd':
	{
		int num = star;

		if(num<pointver.size()/6){
			pointver[num*6]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12]+=0.02f;
			linever[num*12+6]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18]+=0.02f;
			triver[num*18+6]+=0.02f;
			triver[num*18+12]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36]+=0.02f;
			twotriver[num*36+6]+=0.02f;
			twotriver[num*36+12]+=0.02f;
			twotriver[num*36+18]+=0.02f;
			twotriver[num*36+24]+=0.02f;
			twotriver[num*36+30]+=0.02f;
		}
	}
	break;
	case 'i':
	{
		int num = star;

		if(num<pointver.size()/6){
			pointver[num*6+1]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12+1]+=0.02f;
			linever[num*12+7]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18+1]+=0.02f;
			triver[num*18+7]+=0.02f;
			triver[num*18+13]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36+1]+=0.02f;
			twotriver[num*36+7]+=0.02f;
			twotriver[num*36+13]+=0.02f;
			twotriver[num*36+19]+=0.02f;
			twotriver[num*36+25]+=0.02f;
			twotriver[num*36+31]+=0.02f;
		}

		num = star;

		if(num<pointver.size()/6){
			pointver[num*6]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12]-=0.02f;
			linever[num*12+6]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18]-=0.02f;
			triver[num*18+6]-=0.02f;
			triver[num*18+12]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36]-=0.02f;
			twotriver[num*36+6]-=0.02f;
			twotriver[num*36+12]-=0.02f;
			twotriver[num*36+18]-=0.02f;
			twotriver[num*36+24]-=0.02f;
			twotriver[num*36+30]-=0.02f;
		}
	}
	break;
	case 'j':
	{
		int num = star;

		if(num<pointver.size()/6){
			pointver[num*6+1]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12+1]+=0.02f;
			linever[num*12+7]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18+1]+=0.02f;
			triver[num*18+7]+=0.02f;
			triver[num*18+13]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36+1]+=0.02f;
			twotriver[num*36+7]+=0.02f;
			twotriver[num*36+13]+=0.02f;
			twotriver[num*36+19]+=0.02f;
			twotriver[num*36+25]+=0.02f;
			twotriver[num*36+31]+=0.02f;
		}

		num = star;

		if(num<pointver.size()/6){
			pointver[num*6]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12]+=0.02f;
			linever[num*12+6]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18]+=0.02f;
			triver[num*18+6]+=0.02f;
			triver[num*18+12]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36]+=0.02f;
			twotriver[num*36+6]+=0.02f;
			twotriver[num*36+12]+=0.02f;
			twotriver[num*36+18]+=0.02f;
			twotriver[num*36+24]+=0.02f;
			twotriver[num*36+30]+=0.02f;
		}
	}
	break;
	case 'k':
	{
		int num = star;

		if(num<pointver.size()/6){
			pointver[num*6]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12]-=0.02f;
			linever[num*12+6]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18]-=0.02f;
			triver[num*18+6]-=0.02f;
			triver[num*18+12]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36]-=0.02f;
			twotriver[num*36+6]-=0.02f;
			twotriver[num*36+12]-=0.02f;
			twotriver[num*36+18]-=0.02f;
			twotriver[num*36+24]-=0.02f;
			twotriver[num*36+30]-=0.02f;
		}
		num = star;
		if(num<pointver.size()/6){
			pointver[num*6+1]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12+1]-=0.02f;
			linever[num*12+7]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18+1]-=0.02f;
			triver[num*18+7]-=0.02f;
			triver[num*18+13]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36+1]-=0.02f;
			twotriver[num*36+7]-=0.02f;
			twotriver[num*36+13]-=0.02f;
			twotriver[num*36+19]-=0.02f;
			twotriver[num*36+25]-=0.02f;
			twotriver[num*36+31]-=0.02f;
		}

	}
	break;
	case 'l':
	{
		int num = star;
		if(num<pointver.size()/6){
			pointver[num*6+1]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12+1]-=0.02f;
			linever[num*12+7]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18+1]-=0.02f;
			triver[num*18+7]-=0.02f;
			triver[num*18+13]-=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36+1]-=0.02f;
			twotriver[num*36+7]-=0.02f;
			twotriver[num*36+13]-=0.02f;
			twotriver[num*36+19]-=0.02f;
			twotriver[num*36+25]-=0.02f;
			twotriver[num*36+31]-=0.02f;
		}

		num = star;

		if(num<pointver.size()/6){
			pointver[num*6]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12){
			num-=pointver.size()/6;
			linever[num*12]+=0.02f;
			linever[num*12+6]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18){
			num-=pointver.size()/6+linever.size()/12;
			triver[num*18]+=0.02f;
			triver[num*18+6]+=0.02f;
			triver[num*18+12]+=0.02f;
		} else if(num<pointver.size()/6+linever.size()/12+triver.size()/18+twotriver.size()/36){
			num-=pointver.size()/6+linever.size()/12+triver.size()/18;
			twotriver[num*36]+=0.02f;
			twotriver[num*36+6]+=0.02f;
			twotriver[num*36+12]+=0.02f;
			twotriver[num*36+18]+=0.02f;
			twotriver[num*36+24]+=0.02f;
			twotriver[num*36+30]+=0.02f;
		}
	}
	break;
	}
	case 'c':
	check=0;
	star=-1;
	pointver.clear();
	linever.clear();
	triver.clear();
	twotriver.clear();
	break;
	case 'q':
	exit(0);
	break;
	}
}
void SpecialKeyboard (int key,int x,int y)
{

}
void Mouse (int button,int state,int x,int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		bool b=false;
		for(int i=0;i<pointver.size()/6;++i){
			if(tranformx(x)>=pointver[i*6]-0.02&&tranformx(x)<=pointver[i*6]+0.02&&tranformy(y)>=pointver[i*6+1]-0.02&&tranformy(y)<=pointver[i*6+1]+0.02){
				b=true;
				star=i;
				cout<<star<<endl;
				break;
			}
		}

		if(!b){
			for(int i=0;i<linever.size()/12;++i){
				if(PointToLine(tranformx(x),tranformy(y),linever[i*12],linever[i*12+1],linever[i*12+6],linever[i*12+7],0.02f)){
					b=true;
					star=i+pointver.size()/6;
					cout<<star<<endl;
					break;
				}
			}
		}

		if(!b){
			for(int i=0;i<triver.size()/18;++i){
				if(PointToTriangle(tranformx(x),tranformy(y),triver[i*18],triver[i*18+1],triver[i*18+6],triver[i*18+7],triver[i*18+12],triver[i*18+13])){
					b=true;
					star=i+pointver.size()/6+linever.size()/12;
					cout<<star<<endl;
					break;
				}
			}
		}

		if(!b){
			for(int i=0;i<twotriver.size()/36;++i){
				if(PointToTriangle(tranformx(x),tranformy(y),twotriver[i*36],twotriver[i*36+1],twotriver[i*36+6],twotriver[i*36+7],twotriver[i*36+12],twotriver[i*36+13])){
					b=true;
					star=i+pointver.size()/6+linever.size()/12+triver.size()/18;
					cout<<star<<endl;
					break;
				}
				else if(PointToTriangle(tranformx(x),tranformy(y),twotriver[i*36+18],twotriver[i*36+1+18],twotriver[i*36+6+18],twotriver[i*36+7+18],twotriver[i*36+12+18],twotriver[i*36+13+18])){
					b=true;
					star=i+pointver.size()/6+linever.size()/12+triver.size()/18;
					cout<<star<<endl;
					break;
				}
			}

		}



		if(check<10&&!b&&mode>=0){
			float cor=rcolor(gen);
			star = -1;
			switch(mode){

			case 0:
			{
				float point[]={
					tranformx(x),tranformy(y),0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
				};
				pointver.insert(pointver.end(),point,point+6);
			}
				break;
			case 1:
			{
				uniform_int_distribution<int> dqd(0,360);
				float line[]={
					tranformx(x),tranformy(y),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)+0.3f*cos(static_cast<float>(dqd(gen))/180.0f*3.14f),tranformy(y)+0.3f*sin(static_cast<float>(dqd(gen))/180.0f*3.14f),0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
				};
				linever.insert(linever.end(),line,line+12);
			}
				break;
			case 2:
			{

				float triangle[]={
					tranformx(x),tranformy(y)+0.2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)-0.2,tranformy(y)-0.2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)+0.2,tranformy(y)-0.2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
				};
				triver.insert(triver.end(),triangle,triangle+18);


			}
				break;
			case 3:
			{
				float triangle1[]={
					tranformx(x)+0.1f,tranformy(y)+0.1f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)-0.1f,tranformy(y)+0.1f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)-0.1f,tranformy(y)-0.1f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
				};
				twotriver.insert(twotriver.end(),triangle1,triangle1+18);
				float triangle2[]={
					tranformx(x)-0.1f,tranformy(y)-0.1f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)+0.1f,tranformy(y)-0.1f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
					tranformx(x)+0.1f,tranformy(y)+0.1f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
				};
				twotriver.insert(twotriver.end(),triangle2,triangle2+18);
			}
			break;
			}
			++check;
		}
		
	}
	glutPostRedisplay ();

}
void Motion (int x,int y)
{
	if(left_mouse){
		
	}

}
int glutGetModifiers (){ //컨트롤 알트 시프트 확인
	return 0;

}
void TimerFunction (int value)
{
	
	if(!left_mouse){
		
	}
	glutPostRedisplay ();
	glutTimerFunc (10,TimerFunction,1);
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
		if (!result)
	{
			glGetShaderInfoLog (vertexShader,512,NULL,errorLog);
			cerr << "ERROR: vertex shader 컴파일 실패 \n" << errorLog << std :: endl;
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
		std::cerr << "ERROR: frag_shader 컴파일 실패 \n" << errorLog << std :: endl;
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
		std::cerr << "ERROR: shader program 연결 실패 \n" << errorLog << std::endl;
			return false;
	}
	glUseProgram (shaderID); //만들어진 세이더 프로그램 사용하기
	//여러 개의 세이더프로그램 만들 수 있고,그 중 한개의 프로그램을 사용하려면
	//glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다
	//사용하기 직전에 호출할 수 있다
	return shaderID;
}

void InitBuffer ()
{
	//--- VAO 객체 생성 및 바인딩
	glGenVertexArrays (1,&VAO);
	glBindVertexArray (VAO);
	//--- vertex data 저장을 위한 VBO 생성 및 바인딩.
	glGenBuffers (1,&VBO);
	glBindBuffer (GL_ARRAY_BUFFER,VBO);
	//--- vertex data 데이터 입력.
	glBufferData (GL_ARRAY_BUFFER,0,NULL,GL_STATIC_DRAW);
	//--- 위치 속성: 속성 위치 0
	glVertexAttribPointer (0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)0);
	glEnableVertexAttribArray (0);
	//--- 색상 속성: 속성 위치 1
	glVertexAttribPointer (1,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)(3* sizeof(float)));
	glEnableVertexAttribArray (1);

}
//--- 프래그먼트 세이더 객체 만들기
//void make_fragmentShaders ()
//{
//	fragmentSource = filetobuf ("fragment.glsl");
//	//--- 프래그먼트 세이더 객체 만들기
//	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	//--- 세이더 코드를 세이더 객체에 넣기
//	glShaderSource(fragmentShader,1,(const GLchar**)&fragmentSource,0);
//	//--- 프래그먼트 세이더 컴파일
//	glCompileShader(fragmentShader);
//	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
//	GLint result;
//	GLchar errorLog[512];
//	glGetShaderiv (fragmentShader,GL_COMPILE_STATUS,&result);
//	if(!result)
//	{
//		glGetShaderInfoLog (fragmentShader,512,NULL,errorLog);
//		cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << endl;
//		return;
//	}
//}

//void MakeMenu ()
//{
//	int SubMenu1,SubMenu2,SubMenu3;
//	int MainMenu;
//	SubMenu1 = glutCreateMenu (MenuFunction);
//	glutAddMenuEntry ("Teapot",1);
//	glutAddMenuEntry ("Torus",2);
//	glutAddMenuEntry ("Cone",3);
//	glutAddMenuEntry ("Cube",4);
//	glutAddMenuEntry ("Sphere",5);
//	SubMenu2 = glutCreateMenu (MenuFunction);
//	glutAddMenuEntry ("Red",11);
//	glutAddMenuEntry ("Green",22);
//	SubMenu3= glutCreateMenu (MenuFunction);
//	glutAddMenuEntry ("Wire",111);
//	glutAddMenuEntry ("Solid",222);
//	MainMenu = glutCreateMenu (MenuFunction);
//	glutAddSubMenu ("Shape",SubMenu1);
//	glutAddsubMenu ("Color",SubMenu2);
//	glutAddsubMenu ("State",SubMenu3);
//	glutAttachMenu (GLUT_RIGHT_BUTTON);
//	
//}
//void MenuFunction (int button)
//{
//	//--- button: 메뉴의 구별자값
//	switch(button) {
//	case 1: …;
//		break;
//	case 2: …;
//		break;
//	case 11: …;
//		break;
//	case 111:…;
//	}
//	//--- Teapot 인 경우
//   //--- Torus 인 경우
//   //--- Red 인 경우
//   //--- Wire인 경우
//	glutPostRedisplay ();
//}