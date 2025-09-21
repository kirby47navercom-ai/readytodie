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
FRECT test;
struct dtd{
	FRECT rect;
	float r=0,g=0,b=0;
	int num=0;
	bool b_=false;
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
	return (std::min(rect.left, rect.right) <= point.x && point.x <= std::max(rect.left, rect.right) &&
            std::min(rect.top, rect.bottom) <= point.y && point.y <= std::max(rect.top, rect.bottom));
}
bool IntersectFRect(FRECT &check, const FRECT& rect1,const FRECT& rect2){
	
	check.left = std::max(rect1.left,rect2.left);
	check.top = std::max(rect1.top,rect2.top);


	check.right = std::min(rect1.right,rect2.right);
	check.bottom = std::min(rect1.bottom,rect2.bottom);
	 
	if(check.left < check.right && check.top < check.bottom) {
		return true; 
	} 
	else {
		check.left = check.top = check.right = check.bottom = 0;
		return false; 
	}
}
void GLRectf(FRECT rect){
	glRectf(rect.left,rect.top,rect.right,rect.bottom);
}

vector<dtd> art={{-0.75f,-0.85f,-0.6f,-0.6f},
				{-0.4f,-0.9f,-0.25f,-0.6f},
				{-0.75f,-0.6f,-0.25f,-0.4f},
				{-0.65f,-0.4f,-0.35f,-0.2f},
				{-0.75f,-0.2f,-0.25f,0.2f},
				{-0.25f,-0.05f,-0.15f,0.2f},
				{-0.85f,-0.15f,-0.75f,0.2f},
				{-0.65f,0.2f,-0.35f,0.5f},
				{-0.65f,0.5f,-0.55f,0.7f},
				{-0.45f,0.5f,-0.35f,0.6f}
};

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
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glColor3f (1,1,1);
	glRectf(0,-1.0f,0,1.0f);

	for(int i=0;i<art.size();++i){
		GLRectf(art[i].rect);
	}

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	for(int i=0;i<arr.size();++i){
		glColor3f (arr[i].r,arr[i].g,arr[i].b);
		GLRectf(arr[i].rect);
	}
	//GLRectf(test);
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
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	if(arr.size()>0)
		arr[key-'0'].b_=true;
	break;
	case 'r':
	{
		arr.clear();
		for(int i=0;i<10;++i){
			uniform_real_distribution<float> ddddx(0.2f,0.8f);
			uniform_real_distribution<float> ddddy(-0.8f,0.8f);
			GLfloat x=ddddx(gen);
			GLfloat y=ddddy(gen);
			GLfloat z1 = (art[i].rect.right - art[i].rect.left)/2;
			GLfloat z2 = (art[i].rect.bottom - art[i].rect.top)/2;
			arr.push_back({{x-z1,y-z2,x+z1,y+z2},dis(gen),dis(gen),dis(gen)});
			
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
		FPOINT point={tranformx(x),tranformy(y)};
		for(int i=0;i<arr.size();++i){
			if(FPtInFRect(arr[i].rect,point)){
				check=i;
				left_mouse=true;
				break;
			}
		}
	} 
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		left_mouse=false;
	}
	glutPostRedisplay ();

}
void Motion (int x,int y)
{
	if(left_mouse){
		GLfloat t1=(arr[check].rect.right-arr[check].rect.left)/2;
		GLfloat t2=(arr[check].rect.bottom-arr[check].rect.top)/2;
		arr[check].rect.left=tranformx(x)-t1;
		arr[check].rect.top=tranformy(y)-t2;
		arr[check].rect.right=tranformx(x)+t1;
		arr[check].rect.bottom=tranformy(y)+t2;
	}
	
}
int glutGetModifiers (){ //컨트롤 알트 시프트 확인
	return 0;

}
void TimerFunction (int value)
{
	int min=0;
	if(!left_mouse){
		for(int i=0;i<arr.size();++i){
			FRECT space = art[i].rect;
			InflateFRect(space,-0.05f,-0.05f);
			if(FPtInFRect(arr[i].rect,FPOINT{space.left,space.top})
				&&FPtInFRect(arr[i].rect,FPOINT{space.right,space.bottom})){
				++min;
			}
			//if(i==4)test=space;
		}
		if(min!=stack_count){
			stack_count=min;
			cout<<stack_count<<"gea"<<endl;
			if(stack_count==10)cout<<"dtd!!!"<<endl;
		}
	}
	for(int i=0;i<arr.size();++i){
		if(arr[i].b_){
			OffsetFRect(arr[i].rect,(art[i].rect.left   - arr[i].rect.left)   * 0.05f,(art[i].rect.top    - arr[i].rect.top)    * 0.05f);
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