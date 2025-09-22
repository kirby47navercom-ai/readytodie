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
//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//glPolygonMode(GL_FRONT_AND_BACK,GL_Line);
//glRectf(-0.5f,-0.5f,0.5f,0.5f);
bool left_mouse=false;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int> dis(0,SIZE/2);
uniform_int_distribution<int> did(-SIZE/2,SIZE/2);

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
	return x - (SIZE / 2);
}
GLfloat tranformy(int y){
	return (SIZE / 2) - y;
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
void GLRectf(RECT rect){
	FRECT frect= {static_cast<GLfloat>(rect.left)/(SIZE/2),static_cast<GLfloat>(rect.top)/(SIZE/2),static_cast<GLfloat>(rect.right)/(SIZE/2),static_cast<GLfloat>(rect.bottom)/(SIZE/2)};
	glRectf(frect.left,frect.top,frect.right,frect.bottom);
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
	GLfloat rColor,gColor,bColor;
	rColor = gColor = 0.0;
	bColor = 1.0; //--- 배경색을 파랑색으로 설정
	glClearColor(rColor,gColor,bColor,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram (shaderProgramID);
	glPointSize(5.0);
	glDrawArrays (GL_TRIANGLES,0,3); //--- 렌더링하기: 0번 인덱스에서 1개의 버텍스를 사용하여 점 그리기
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
	case 'r':
	{
		arr.clear();
		for(int i=0;i<10;++i){
			

		}
	}
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
		
	} else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		
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