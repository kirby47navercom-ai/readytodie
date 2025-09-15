#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "dtd.h" 
#define SIZE 600
int x,y;
float f,l,o;
int star=0;
vector<GLfloat> a,c;
vector<int>_i;
int _time=0;
int _stack=0;
bool b=false;
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

int main(int argc,char** argv)
{

	//--- ������ �����ϱ�		 //--- ������ ����ϰ� �ݹ��Լ� ����
	glutInit(&argc,argv);		// glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// ���÷��� ��� ����
	glutInitWindowPosition(100,100);	// �������� ��ġ ����
	glutInitWindowSize(SIZE,SIZE);	// �������� ũ�� ����
	glutCreateWindow("Example1");	// ������ ���� (������ �̸�)
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK)	// glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	} else
		std::cout << "GLEW Initialized\n";



	glutDisplayFunc(drawScene);	// ��� �Լ��� ����
	glutReshapeFunc(Reshape);	// �ٽ� �׸��� �Լ� ����
	glutIdleFunc(idleScene);			// ���̵� Ÿ�ӿ� ȣ���ϴ��Լ�������
	glutKeyboardFunc(Keyboard);			// Ű���� �Է� �ݹ��Լ�
	glutGetModifiers();				// ��Ʈ�� ��Ʈ ����Ʈ Ȯ��
	glutKeyboardUpFunc(Keyupboard);		// Ű���� ���� �ݹ��Լ�
	glutSpecialFunc (SpecialKeyboard); // Ư��Ű �Է� �ݹ��Լ�
	glutMouseFunc (Mouse);				// ���콺 �Է� �ݹ��Լ� ���콺�� ������ �����϶�
	glutMotionFunc (Motion);			// ���콺 ������ �ݹ��Լ� ���콺�� ���� �����϶�
	glutMainLoop();	// �̺�Ʈ ó�� ����
}
GLvoid drawScene()
{
	glClearColor(f,l,o,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡���Եȴ�.

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
	b=false;
}
void Keyboard(unsigned char key,int x,int y)
{
	switch(key) {
	case 'a':
	
		if(arr.size()<30&&!b){
			GLfloat x=did(gen);
			GLfloat y=did(gen);
			arr.push_back({x,y,x+0.2f,y+0.2f,dis(gen),dis(gen),dis(gen)});
			b=true;
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
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN&&!left_mouse){
		left_mouse=true;
		for(int i=0;i<arr.size();++i){
			if(arr[i].x1<(((float)x/(SIZE/2))-1.0f)&&arr[i].y1<(((SIZE - (float)y)/(SIZE/2)) -1.0f)&&arr[i].x2>(((float)x/(SIZE/2))-1.0f)&&arr[i].y2>(((SIZE - (float)y)/(SIZE/2)) -1.0f)){
				++star;
				a.push_back(arr[i].x1);
				a.push_back(arr[i].x2);
				c.push_back(arr[i].y1);
				c.push_back(arr[i].y2);
				_i.push_back(i);

			}

		}

		if(star>=2){

			sort(a.begin(),a.end());
			sort(c.begin(),c.end());

			for(int i=arr.size()-1;i>=0;--i){

				if(_i.empty()||star==0)break;
				if(i==_i.back()){
					arr.erase(arr.begin()+i);
					_i.pop_back();
					//cout<<_i.size()<<endl;

				}
			}
			arr.push_back({a.front(),c.front(),a.back(),c.back(),dis(gen),dis(gen),dis(gen)});
		}
		a.clear();c.clear();_i.clear();star=0;

	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP&&left_mouse){
		left_mouse=false;
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		for(int i=0;i<arr.size();++i){
			if(arr[i].x1<(((float)x/(SIZE/2))-1.0f)&&arr[i].y1<(((SIZE - (float)y)/(SIZE/2)) -1.0f)&&arr[i].x2>(((float)x/(SIZE/2))-1.0f)&&arr[i].y2>(((SIZE - (float)y)/(SIZE/2)) -1.0f)){
				for(int j=0;j<2;++j){
					GLfloat x=did(gen);
					GLfloat y=did(gen);
					arr.push_back({x,y,x+0.2f,y+0.2f,dis(gen),dis(gen),dis(gen)});
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
	if(left_mouse == true)
	{
		for(int i=0;i<arr.size();++i){
			cout<<arr.front().x1<<' '<<arr.front().y1<<" "<<arr.front().x2<<" "<<arr.front().y2;
			if(arr[i].x1<(((float)x/(SIZE/2))-1.0f)&&arr[i].y1<(((SIZE - (float)y)/(SIZE/2)) -1.0f)&&arr[i].x2>(((float)x/(SIZE/2))-1.0f)&&arr[i].y2>(((SIZE - (float)y)/(SIZE/2)) -1.0f)){
				GLfloat t1=(arr[i].x2-arr[i].x1)/2;
				GLfloat t2=(arr[i].y2-arr[i].y1)/2;
				arr[i].x1=(((float)x/(SIZE/2))-1.0f)-t1;
				arr[i].y1=(((SIZE - (float)y)/(SIZE/2)) -1.0f)-t2;
				arr[i].x2=(((float)x/(SIZE/2))-1.0f)+t1;
				arr[i].y2=(((SIZE - (float)y)/(SIZE/2)) -1.0f)+t2;
				//cout<<arr[i].x1<<' '<<arr[i].y1<<" "<<arr[i].x2<<" "<<arr[i].y2;
				break;
			}

		}
		glutPostRedisplay ();
	}
}
int glutGetModifiers (){ //��Ʈ�� ��Ʈ ����Ʈ Ȯ��
	return 0;

}
void TimerFunction (int value)
{
	glutPostRedisplay ();
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
//	//--- button: �޴��� �����ڰ�
//	switch(button) {
//	case 1: ��;
//		break;
//	case 2: ��;
//		break;
//	case 11: ��;
//		break;
//	case 111:��;
//	}
//	//--- Teapot �� ���
//   //--- Torus �� ���
//   //--- Red �� ���
//   //--- Wire�� ���
//	glutPostRedisplay ();
//}