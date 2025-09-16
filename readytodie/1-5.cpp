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
int _time=0;
int _stack=0;
bool b[5]={0,};
bool b1[5]={0,};
bool b2[5]={0,};
bool b3[5]={0,};
bool b4[5]={0,};
int dialog[5]={0,};
int snake[5]={0,};
int snake_[5]={0,};
int core_[5]={0,1,0,1,0};
//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//glPolygonMode(GL_FRONT_AND_BACK,GL_Line);
//glRectf(-0.5f,-0.5f,0.5f,0.5f);
bool left_mouse=false;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> dis(0.0f,1.0f);
uniform_real_distribution<float> did(-1.0f,1.0f);

struct dtd{
	GLfloat x1;
	GLfloat y1;
	GLfloat x2;
	GLfloat y2;
	float r,g,b;
};
vector<dtd> arr;
vector<dtd> carr;
dtd erase;
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
//void MakeMenu ();
//void MenuFunction (int button);

GLfloat tranformx(int x){
	return ((float)x/(SIZE/2))-1.0f;
}
GLfloat tranformy(int y){
	return ((SIZE - (float)y)/(SIZE/2)) -1.0f;
}


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
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	} else
		std::cout << "GLEW Initialized\n";


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
GLvoid drawScene()
{
	glClearColor(f,l,o,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 그리기 부분 구현: 그리기 관련 부분이 여기에포함된다.

	for(int i=0;i<arr.size();++i){
		glColor3f (arr[i].r,arr[i].g,arr[i].b);
		glRectf(arr[i].x1,arr[i].y1,arr[i].x2,arr[i].y2);
	}
	if(left_mouse){
		glColor3f (erase.r,erase.g,erase.b);
		glRectf(erase.x1,erase.y1,erase.x2,erase.y2);
	}
	glutSwapBuffers();
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
	arr.clear();
	for(int i=0;i<30;++i){
		GLfloat x=did(gen);
		GLfloat y=did(gen);
		arr.push_back({x,y,x+0.1f,y+0.1f,dis(gen),dis(gen),dis(gen)});
	}
	erase = dtd{0,0,0.2f,0.2f,0,0,0};
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
		left_mouse=true;
		GLfloat a=(erase.x2-erase.x1)/2.0f;
		GLfloat b=(erase.y2-erase.y1)/2.0f;
		erase.x1=tranformx(x)-a;
		erase.y1=tranformy(y)-b;
		erase.x2=tranformx(x)+a;
		erase.y2=tranformy(y)+b;

	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP&&left_mouse){
		left_mouse=false;
		erase.r=0;
		erase.g=0;
		erase.b=0;
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN&&star>0){
		--star;
		arr.push_back({tranformx(x)-0.05f,tranformy(y)-0.05f,tranformx(x)+0.05f,tranformy(y)+0.05f,dis(gen),dis(gen),dis(gen)});
		erase.x1+=0.01f;
		erase.y1+=0.01f;
		erase.x2-=0.01f;
		erase.y2-=0.01f;
	}
	glutPostRedisplay ();

}
void Motion (int x,int y)
{
	if(left_mouse){
		
		GLfloat a=(erase.x2-erase.x1)/2.0f;
		GLfloat b=(erase.y2-erase.y1)/2.0f;
		erase.x1=tranformx(x)-a;
		erase.y1=tranformy(y)-b;
		erase.x2=tranformx(x)+a;
		erase.y2=tranformy(y)+b;

		for(int i=0;i<arr.size();++i){
			if(arr[i].x1<erase.x2&&arr[i].y1<erase.y2&&arr[i].x2>erase.x1&&arr[i].y2>erase.y1){
				erase.r=arr[i].r;
				erase.g=arr[i].g;
				erase.b=arr[i].b;
				erase.x1-=0.01f;
				erase.y1-=0.01f;
				erase.x2+=0.01f;
				erase.y2+=0.01f;
				++star;
				arr.erase(arr.begin()+i);
				--i;
			}
		}
	}

}
int glutGetModifiers (){ //컨트롤 알트 시프트 확인
	return 0;

}
void TimerFunction (int value)
{
	
	glutPostRedisplay ();
	glutTimerFunc (10,TimerFunction,1);
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