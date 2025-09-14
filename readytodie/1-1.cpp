#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include "dtd.h" 

int x, y;
float f,l,o;
int _time=0;
int _stack=0;
bool b=false;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> dis(0.0f,1.0f);
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void idleScene();
void Keyboard(unsigned char key,int x,int y);
void SpecialKeyboard (int key,int x,int y);
void Mouse (int button,int state,int x,int y);
void Motion (int x,int y);
int glutGetModifiers (); 
void TimerFunction (int value);
//void MakeMenu ();
//void MenuFunction (int button);

void main(int argc, char** argv)
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutIdleFunc(idleScene);			// 아이들 타임에 호출하는함수의지정
	glutKeyboardFunc(Keyboard);			// 키보드 입력 콜백함수
	//glutGetModifiers();				// 컨트롤 알트 시프트 확인
	//glutKeyboardUpFunc(Keyboard);		// 키보드 뗄때 콜백함수
	glutSpecialFunc (SpecialKeyboard); // 특수키 입력 콜백함수
	glutMouseFunc (Mouse);				// 마우스 입력 콜백함수 마우스를 누르고 움직일때
	glutMotionFunc (Motion);			// 마우스 움직임 콜백함수 마우스를 때고 움직일때
	glutMainLoop();
}
GLvoid drawScene()
{
	glClearColor(f, l, o, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// 그리기 부분 구현: 그리기 관련 부분이 여기에포함된다.


	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	//--- 필요한 헤더파일 include
	//--- 윈도우 출력하고 콜백함수 설정
   // glut 초기화
   // 디스플레이 모드 설정
   // 윈도우의 위치 지정
   // 윈도우의 크기 지정
   // 윈도우 생성 (윈도우 이름)
	// glew 초기화
   // 출력 함수의 지정
   // 다시 그리기 함수 지정
   // 이벤트 처리 시작
   //--- 콜백 함수: 출력 콜백 함수
   // 바탕색을 ‘blue’ 로 지정
   // 설정된 색으로 전체를칠하기
   // 화면에 출력하기
   //--- 콜백 함수: 다시 그리기 콜백 함수
}
void idleScene()
{
	x += 0.02;
	y += 0.02;
	glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c':
	f=0;
	l=1.0f;
	o=1.0f;
	break;
	case 'm':
	f=1.0f;
	l=0;
	o=1.0f;
	break;
	case 'y':
	f=1.0f;
	l=1.0f;
	o=0;
	break;
	case 'a':
	f=dis(gen);
	l=dis(gen);
	o=dis(gen);
	break;
	case 'w':
	f=1.0f;
	l=1.0f;
	o=1.0f;
	break;
	case 'k':
	f=0;
	l=0;
	o=0;
	break;
	case 't':
	glutTimerFunc (100,TimerFunction,1);
	b=true;
	break;
	case 's':
	b=false;
	_stack=0;
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

	/*if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		std::cout << “x = “ << x << “ y = “ << y << std::endl;*/
}
void Motion (int x,int y)
{
	/*if(left_button == true)
	{
	}*/
}
int glutGetModifiers (){ //컨트롤 알트 시프트 확인
	return 0;	

}
void TimerFunction (int value)
{
	_stack++;
	if(_stack>50){
		_stack=0;
		f=dis(gen);
		l=dis(gen);
		o=dis(gen);
	}
	glutPostRedisplay ();
	if(b)
	glutTimerFunc (100,TimerFunction,1);
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