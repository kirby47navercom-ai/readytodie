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
	case '1':
	if(!b[4]){
		if(b1[0]==false)memset(b1,1,sizeof(b1));
		else memset(b1,0,sizeof(b1));
		memset(b2,0,sizeof(b2));
	}
	else {
		b1[star]=true;
		b2[star]=false;
		b3[star]=false;
		b4[star]=false;
	}
	
	break;
	case '2':
	if(!b[4]){
		if(b2[0]==false)memset(b2,1,sizeof(b2));
		else memset(b2,0,sizeof(b2));
		memset(b1,0,sizeof(b1));
	}
	else {
		b2[star]=true;
		b1[star]=false;
		b3[star]=false;
		b4[star]=false;
	}
	
	break;
	case '3':
	if(!b[4]){
		if(b3[0]==false)memset(b3,1,sizeof(b3));
		else memset(b3,0,sizeof(b3));
	}
	else {
		b3[star]=true;
		b1[star]=false;
		b2[star]=false;
		b4[star]=false;
	}
	break;
	case '4':
	if(!b[4]){
		if(b4[0]==false)memset(b4,1,sizeof(b4));
		else memset(b4,0,sizeof(b4));
	}
	else {
		b4[star]=true;
		b1[star]=false;
		b2[star]=false;
		b3[star]=false;
	}
	break;
	case '5':
	{
		b[4]=!b[4];
		uniform_int_distribution<int> didi(0,arr.size());
		star=didi(gen);
	}
	break;
	case 's':
	memset(b,0,sizeof(b));
	memset(b1,0,sizeof(b1));
	memset(b2,0,sizeof(b2));
	memset(b3,0,sizeof(b3));
	memset(b4,0,sizeof(b4));
	break;
	case 'm':
	for(int i=0;i<arr.size();++i){
		GLfloat x1 = (arr[i].x1 + arr[i].x2) / 2.0f;
		GLfloat y1 = (arr[i].y1 + arr[i].y2) / 2.0f;
		GLfloat x2 = (carr[i].x1 + carr[i].x2) / 2.0f;
		GLfloat y2 = (carr[i].y1 + carr[i].y2) / 2.0f;

		arr[i].x1+=x2-x1;
		arr[i].y1+=y2-y1;
		arr[i].x2+=x2-x1;
		arr[i].y2+=y2-y1;

	}
	break;
	case 'r':
	arr.clear();
	carr.clear();
	b[4]=false;
	memset(dialog,0,sizeof(dialog));
	memset(snake,0,sizeof(snake));
	memset(snake_,0,sizeof(snake_));
	core_[0]={0};core_[1]={1};core_[2]={0};core_[3]={1};core_[4]={0};core_[5]={1};
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
		
		if(arr.size()<5){
			arr.push_back({tranformx(x)-0.1f,tranformy(y)-0.1f,tranformx(x)+0.1f,tranformy(y)+0.1f,dis(gen),dis(gen),dis(gen)});
			carr.push_back(arr.back());
		}

	}
	
	glutPostRedisplay ();

}
void Motion (int x,int y)
{
	
	
}
int glutGetModifiers (){ //컨트롤 알트 시프트 확인
	return 0;

}
void TimerFunction (int value)
{
	if(b[4]&&(b1[star]||b2[star]||b3[star]||b4[star])){
		for(int i=0;i<arr.size();++i){

			if(_time>100&&i!=star&&b1[star]==true&&b1[i]==false){
				b1[i]=true;
				b2[i]=false;
				b3[i]=false;
				b4[i]=false;
				_time=0;
				break;
			}
			if(_time>100&&i!=star&&b2[star]==true&&b2[i]==false){
				b2[i]=true;
				b1[i]=false;
				b3[i]=false;
				b4[i]=false;
				_time=0;
				break;
			}
			if(_time>100&&i!=star&&b3[star]==true&&b3[i]==false){
				b3[i]=true;
				b1[i]=false;
				b2[i]=false;
				b4[i]=false;
				_time=0;
				break;
			}
			if(_time>100&&i!=star&&b4[star]==true&&b4[i]==false){
				b4[i]=true;
				b1[i]=false;
				b2[i]=false;
				b3[i]=false;
				_time=0;
				break;
			}
	
		}
		if(_time>100)_time=0;
		++_time;
	}

	for(int i=0;i<arr.size();++i){
		if(b1[i]){
				if(dialog[i]==0){
					arr[i].x1-=0.005f;
					arr[i].y1+=0.005f;
					arr[i].x2-=0.005f;
					arr[i].y2+=0.005f;
					if(arr[i].y2>=1.0f||arr[i].x1<=-1.0f)dialog[i]=1;
				}			   
				else if(dialog[i]==1){
					arr[i].x1-=0.005f;
					arr[i].y1-=0.005f;
					arr[i].x2-=0.005f;
					arr[i].y2-=0.005f;
					if(arr[i].y1<=-1.0f||arr[i].x1<=-1.0f)dialog[i]=2;
				}
			
				else if(dialog[i]==2){
					arr[i].x1+=0.005f;
					arr[i].y1-=0.005f;
					arr[i].x2+=0.005f;
					arr[i].y2-=0.005f;
					if(arr[i].y1<=-1.0f||arr[i].x2>=1.0f)dialog[i]=3;
				}
				else if(dialog[i]==3){
					arr[i].x1+=0.005f;
					arr[i].y1+=0.005f;
					arr[i].x2+=0.005f;
					arr[i].y2+=0.005f;
					if(arr[i].y2>=1.0f||arr[i].x2>=1.0f)dialog[i]=0;
				}
			
		}
	}
	for(int i=0;i<arr.size();++i){
		if(b2[i]){
			if(snake[i]){
				if(snake_[i]<200){
					if(core_[i]==0){
						arr[i].y1-=0.005f;
						arr[i].y2-=0.005f;
						if(arr[i].y1<=-1.0f){
							core_[i]=1;
							snake_[i]=395;
							snake[i]=false;
						}
					} 
					else{
						arr[i].y1+=0.005f;
						arr[i].y2+=0.005f;
						if(arr[i].y2>=1.0f){
							core_[i]=0;
							snake_[i]=395;
							snake[i]=false;
						}
					}
					++snake_[i]+=5;
					
				}
				else if(snake_[i]>=200){
					if(core_[i]==0){
						arr[i].x1-=0.005f;
						arr[i].x2-=0.005f;
						if(arr[i].x1<=-1.0f){
							snake_[i]=200;
							snake[i]=false;
						}
					} 
					else{
						arr[i].x1+=0.005f;
						arr[i].x2+=0.005f;
						if(arr[i].x2>=1.0f){
							snake_[i]=200;
							snake[i]=false;
						}
					}
				}
			} 
			else{
				if(snake_[i]<200||snake_[i]>=400){
					if(core_[i]==0){
						arr[i].x1+=0.005f;
						arr[i].x2+=0.005f;
						if(arr[i].x2>=1.0f){
							snake_[i]=0;
							snake[i]=true;
						}
					} 
					else{
						arr[i].x1-=0.005f;
						arr[i].x2-=0.005f;
						if(arr[i].x1<=-1.0f){
							snake_[i]=0;
							snake[i]=true;
						}
					}
				}
				else if(snake_[i]<400){
					if(core_[i]==0){
						arr[i].y1-=0.005f;
						arr[i].y2-=0.005f;
						if(arr[i].y1<=-1.0f){
							core_[i]=1;
							snake[i]=true;
							snake_[i]=195;
						}
					} 
					else{
						arr[i].y1+=0.005f;
						arr[i].y2+=0.005f;
						if(arr[i].y2>=1.0f){
							core_[i]=0;
							snake[i]=true;
							snake_[i]=195;
						}
					}
					++snake_[i]+=5;
				}
			}
		}
		
	}
	for(int i=0;i<arr.size();++i){
		if(b3[i]){
			GLfloat x = did(gen)/4;
			GLfloat y = did(gen)/4;
			arr[i].x1=-x+(arr[i].x2+arr[i].x1)/2;
			arr[i].y1=-y+(arr[i].y2+arr[i].y1)/2;
			arr[i].x2=x+(arr[i].x2+arr[i].x1)/2;
			arr[i].y2=y+(arr[i].y2+arr[i].y1)/2;
		}
	}
	for(int i=0;i<arr.size();++i){
		if(b4[i]){
			arr[i].r=dis(gen);
			arr[i].g=dis(gen);
			arr[i].b=dis(gen);
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