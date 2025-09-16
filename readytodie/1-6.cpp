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
int option=0;
//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//glPolygonMode(GL_FRONT_AND_BACK,GL_Line);
//glRectf(-0.5f,-0.5f,0.5f,0.5f);
bool left_mouse=false;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> dis(0.0f,1.0f);
uniform_real_distribution<float> did(-1.0f,1.0f);
uniform_real_distribution<float> dii(0.1f,0.5f);

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
struct dtd{
	GLfloat x1;
	GLfloat y1;
	GLfloat x2;
	GLfloat y2;
	float r,g,b;
	int num=0;
};
vector<dtd> arr;
vector<dtd> carr;
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
void OffsetFRect(FRECT& rect,GLfloat x,GLfloat y){
	rect.left+=x;
	rect.right+=x;
	rect.top+=y;
	rect.bottom+=y;
}
void InflateFRect(FRECT& rect,GLfloat x,GLfloat y){
	rect.left-=x;
	rect.right+=x;
	rect.top+=y;
	rect.bottom-=y;
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
		std::cerr << "GLEW Initialized\n" << std::endl;
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
	case'1':
	option=1;
	break;
	case'2':
	option=2;
	break;
	case'3':
	option=3;
	break;
	case'4':
	option=4;
	break;
	case 'r':
	arr.clear();
	for(int i=0;i<10;++i){
		GLfloat x=did(gen);
		GLfloat y=did(gen);
		GLfloat sixex=dii(gen);
		arr.push_back({x,y,x+sixex,y+sixex,dis(gen),dis(gen),dis(gen)});
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
		for(int i=0;i<arr.size();++i){
			if(tranformx(x)>=arr[i].x1 && tranformx(x)<=arr[i].x2 && tranformy(y)>=arr[i].y1 && tranformy(y)<=arr[i].y2&&arr[i].num==0&&option!=0){
				GLfloat x_=(arr[i].x2-arr[i].x1)/4.0f;
				GLfloat y_=(arr[i].y2-arr[i].y1)/4.0f;
				GLfloat cx=(arr[i].x2+arr[i].x1)/4.0f;
				GLfloat cy=(arr[i].y2+arr[i].y1)/4.0f;
				FPOINT lx = {arr[i].x1,(arr[i].y1+arr[i].y2)/2.0f};
				FPOINT rx = {arr[i].x2,(arr[i].y1+arr[i].y2)/2.0f};
				FPOINT ty = {(arr[i].x1+arr[i].x2)/2.0f,arr[i].y2};
				FPOINT by = {(arr[i].x1+arr[i].x2)/2.0f,arr[i].y1};
				switch(option)
				{
				case 1:
				arr.push_back({lx.x-x_,lx.y-y_,lx.x+x_,lx.y+y_,arr[i].r,arr[i].g,arr[i].b,1});
				arr.push_back({ty.x-x_,ty.y-y_,ty.x+x_,ty.y+y_,arr[i].r,arr[i].g,arr[i].b,2});
				arr.push_back({rx.x-x_,rx.y-y_,rx.x+x_,rx.y+y_,arr[i].r,arr[i].g,arr[i].b,3});
				arr.push_back({by.x-x_,by.y-y_,by.x+x_,by.y+y_,arr[i].r,arr[i].g,arr[i].b,4});
				break;

				case 2:
				arr.push_back({arr[i].x1-x_,arr[i].y2-y_,arr[i].x1+x_,arr[i].y2+y_,arr[i].r,arr[i].g,arr[i].b,5});
				arr.push_back({arr[i].x2-x_,arr[i].y2-y_,arr[i].x2+x_,arr[i].y2+y_,arr[i].r,arr[i].g,arr[i].b,6});
				arr.push_back({arr[i].x1-x_,arr[i].y1-y_,arr[i].x1+x_,arr[i].y1+y_,arr[i].r,arr[i].g,arr[i].b,7});
				arr.push_back({arr[i].x2-x_,arr[i].y1-y_,arr[i].x2+x_,arr[i].y1+y_,arr[i].r,arr[i].g,arr[i].b,8});
				break;

				case 3:
				{
					uniform_int_distribution<int> didi(1,8);
					int a=didi(gen);
					arr.push_back({arr[i].x1-x_,arr[i].y2-y_,arr[i].x1+x_,arr[i].y2+y_,arr[i].r,arr[i].g,arr[i].b,a});
					arr.push_back({arr[i].x2-x_,arr[i].y2-y_,arr[i].x2+x_,arr[i].y2+y_,arr[i].r,arr[i].g,arr[i].b,a});
					arr.push_back({arr[i].x1-x_,arr[i].y1-y_,arr[i].x1+x_,arr[i].y1+y_,arr[i].r,arr[i].g,arr[i].b,a});
					arr.push_back({arr[i].x2-x_,arr[i].y1-y_,arr[i].x2+x_,arr[i].y1+y_,arr[i].r,arr[i].g,arr[i].b,a});
				}
				break;

				case 4:
				arr.push_back({lx.x-x_,lx.y-y_,lx.x+x_,lx.y+y_,arr[i].r,arr[i].g,arr[i].b,1});
				arr.push_back({ty.x-x_,ty.y-y_,ty.x+x_,ty.y+y_,arr[i].r,arr[i].g,arr[i].b,2});
				arr.push_back({rx.x-x_,rx.y-y_,rx.x+x_,rx.y+y_,arr[i].r,arr[i].g,arr[i].b,3});
				arr.push_back({by.x-x_,by.y-y_,by.x+x_,by.y+y_,arr[i].r,arr[i].g,arr[i].b,4});
				arr.push_back({arr[i].x1-x_,arr[i].y2-y_,arr[i].x1+x_,arr[i].y2+y_,arr[i].r,arr[i].g,arr[i].b,5});
				arr.push_back({arr[i].x2-x_,arr[i].y2-y_,arr[i].x2+x_,arr[i].y2+y_,arr[i].r,arr[i].g,arr[i].b,6});
				arr.push_back({arr[i].x1-x_,arr[i].y1-y_,arr[i].x1+x_,arr[i].y1+y_,arr[i].r,arr[i].g,arr[i].b,7});
				arr.push_back({arr[i].x2-x_,arr[i].y1-y_,arr[i].x2+x_,arr[i].y1+y_,arr[i].r,arr[i].g,arr[i].b,8});
				break;
				}
				arr.erase(arr.begin()+i);
				break;
			}
		}

	}
	glutPostRedisplay ();

}
void Motion (int x,int y)
{
	if(left_mouse){

		
	}
	cout<<"x:"<<tranformx(x)<<" y:"<<tranformy(y)<<endl;
}
int glutGetModifiers (){ //컨트롤 알트 시프트 확인
	return 0;

}
void TimerFunction (int value)
{
	for(int i=0;i<arr.size();++i){
		switch(arr[i].num)
		{
		case 1:
		{	
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,-0.01f,0.0f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].x2-arr[i].x1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 2:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,0.0f,0.01f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].y2-arr[i].y1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 3:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,0.01f,0.0f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].x2-arr[i].x1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 4:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,0.0f,-0.01f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].y2-arr[i].y1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 5:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,-0.01f,0.01f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].x2-arr[i].x1<=0.f||arr[i].y2-arr[i].y1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 6:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,0.01f,0.01f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].x2-arr[i].x1<=0.f||arr[i].y2-arr[i].y1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 7:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,-0.01f,-0.01f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].x2-arr[i].x1<=0.f||arr[i].y2-arr[i].y1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		case 8:
		{
			FRECT rect = {arr[i].x1,arr[i].y2,arr[i].x2,arr[i].y1};
			OffsetFRect(rect,0.01f,-0.01f);
			InflateFRect(rect,-0.001f,-0.001f);
			arr[i].x1=rect.left;
			arr[i].x2=rect.right;
			arr[i].y1=rect.bottom;
			arr[i].y2=rect.top;
			if(arr[i].x2-arr[i].x1<=0.f||arr[i].y2-arr[i].y1<=0.f)
				arr.erase(arr.begin()+i);
		}
		break;
		default:
		break;
		}
	}


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