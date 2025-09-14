#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include "dtd.h" 

int x, y;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void idleScene();
void Keyboard(unsigned char key,int x,int y);
void SpecialKeyboard (int key,int x,int y);
void Mouse (int button,int state,int x,int y);
void Motion (int x,int y);
int glutGetModifiers (); 
void TimerFunction (int value);

void main(int argc, char** argv)
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
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
	glutTimerFunc (100,TimerFunction,1); // 타이머 콜백함수
	glutMainLoop();
}
GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
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
	
	glutPostRedisplay ();
	glutTimerFunc (100,TimerFunction,1);
}
