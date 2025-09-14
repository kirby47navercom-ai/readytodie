#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h> 
#include ".h" 

int x, y;
GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void main(int argc, char** argv)
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
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
	glutIdleFunc(idleScene);			// ���̵� Ÿ�ӿ� ȣ���ϴ��Լ�������
	glutKeyboardFunc(Keyboard);			// Ű���� �Է� �ݹ��Լ�
	glutMainLoop();
}
GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡���Եȴ�.


	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	//--- �ʿ��� ������� include
	//--- ������ ����ϰ� �ݹ��Լ� ����
   // glut �ʱ�ȭ
   // ���÷��� ��� ����
   // �������� ��ġ ����
   // �������� ũ�� ����
   // ������ ���� (������ �̸�)
	// glew �ʱ�ȭ
   // ��� �Լ��� ����
   // �ٽ� �׸��� �Լ� ����
   // �̺�Ʈ ó�� ����
   //--- �ݹ� �Լ�: ��� �ݹ� �Լ�
   // �������� ��blue�� �� ����
   // ������ ������ ��ü��ĥ�ϱ�
   // ȭ�鿡 ����ϱ�
   //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
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