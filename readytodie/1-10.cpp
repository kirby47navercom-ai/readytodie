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
uniform_real_distribution<float> rtri(0.0f,0.2f);
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
void tri90DegressRotate(float &v1x,float &v1y,float &v2x,float &v2y,float &v3x,float &v3y){
	float centerX = (v1x + v2x + v3x) / 3.0f;
	float centerY = (v1y + v2y + v3y) / 3.0f;

	float nv1x = -(v1y - centerY) + centerX;
	float nv1y =  (v1x - centerX) + centerY;
	float nv2x = -(v2y - centerY) + centerX;
	float nv2y =  (v2x - centerX) + centerY;
	float nv3x = -(v3y - centerY) + centerX;
	float nv3y =  (v3x - centerX) + centerY;

	v1x = nv1x;
	v1y = nv1y;
	v2x = nv2x;
	v2y = nv2y;
	v3x = nv3x;
	v3y = nv3y;
}

void triRotate(float &v1x,float &v1y,float &v2x,float &v2y,float &v3x,float &v3y,const float &radian){
	float d = radian * 3.141592 / 180;
	float X = (v1x + v2x + v3x) / 3.0;
	float Y = (v1y + v2y + v3y) / 3.0;

	float temp1x = (v1x - X) * cos(d) - (v1y - Y) * sin(d);
	float temp1y = (v1x - X) * sin(d) + (v1y - Y) * cos(d);
	float temp2x = (v2x - X) * cos(d) - (v2y - Y) * sin(d);
	float temp2y = (v2x - X) * sin(d) + (v2y - Y) * cos(d);
	float temp3x = (v3x - X) * cos(d) - (v3y - Y) * sin(d);
	float temp3y = (v3x - X) * sin(d) + (v3y - Y) * cos(d);

	v1x = temp1x + X;
	v1y = temp1y + Y;
	v2x = temp2x + X;
	v2y = temp2y + Y;
	v3x = temp3x + X;
	v3y = temp3y + Y;
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
vector<float> triver1;
vector<float> triver2;
vector<float> triver3;
vector<float> triver4;
vector<float> ctriver1;
vector<float> ctriver2;
vector<float> ctriver3;
vector<float> ctriver4;
vector<float> twotriver;

vector<int> triver1_count;
vector<int> triver2_count;
vector<int> triver3_count;
vector<int> triver4_count;

array<int,4> triver1_time={0,0,0,0};
array<int,4> triver2_time={0,0,0,0};
array<int,4> triver3_time={0,0,0,0};
array<int,4> triver4_time={0,0,0,0};

array<int,4> triver1_radian={0,0,0,0};
array<int,4> triver2_radian={0,0,0,0};
array<int,4> triver3_radian={0,0,0,0};
array<int,4> triver4_radian={0,0,0,0};

void updateVBO()
{
	// 1. 두 벡터를 합칠 임시 벡터 생성
	vector<GLfloat> comver;
	comver.insert(comver.end(),pointver.begin(),pointver.end());
	comver.insert(comver.end(),linever.begin(),linever.end());
	comver.insert(comver.end(),triver1.begin(),triver1.end());
	comver.insert(comver.end(),triver2.begin(),triver2.end());
	comver.insert(comver.end(),triver3.begin(),triver3.end());
	comver.insert(comver.end(),triver4.begin(),triver4.end());
	comver.insert(comver.end(),twotriver.begin(),twotriver.end());

	// 2. 합쳐진 데이터로 VBO를 새로 업데이트
	if(!comver.empty())
	{
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) * comver.size(),comver.data(),GL_DYNAMIC_DRAW);
	}
}
vector<dtd> art;
FRECT rect[4]={0,0,1.0f,1.0f,
-1.0f,0,0,1.0f,
-1.0f,-1.0f,0,0,
0,-1.0f,1.0f,0};
void Initdata()
{
	float line[]={
		0,1.0f,0.0f,1.0f,1.0f,1.0f,
		0,-1.0f,0.0f,1.0f,1.0f,1.0f,
		1.0f,0,0.0f,1.0f,1.0f,1.0f,
		-1.0f,0,0.0f,1.0f,1.0f,1.0f
	};
	linever.insert(linever.end(),line,line+24);

	float triangle[]={
		0.5f,0.6f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		0.4f,0.4f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		0.6f,0.4f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),

		-0.5f,0.6f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		-0.4f,0.4f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		-0.6f,0.4f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),

		-0.5f,-0.4f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		-0.4f,-0.6f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		-0.6f,-0.6f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),

		0.5f,-0.4f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		0.4f,-0.6f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
		0.6f,-0.6f,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
	};
	triver1.insert(triver1.end(),triangle,triangle+18);
	triver2.insert(triver2.end(),triangle+18,triangle+36);
	triver3.insert(triver3.end(),triangle+36,triangle+54);
	triver4.insert(triver4.end(),triangle+54,triangle+72);
	ctriver1.insert(ctriver1.end(),triangle,triangle+18);
	ctriver2.insert(ctriver2.end(),triangle+18,triangle+36);
	ctriver3.insert(ctriver3.end(),triangle+36,triangle+54);
	ctriver4.insert(ctriver4.end(),triangle+54,triangle+72);
	triver1_count.push_back(0);
	triver2_count.push_back(0);
	triver3_count.push_back(0);
	triver4_count.push_back(0);
}
void move1(){
	for(int i=0;i<triver1.size()/18;++i){
		if(triver1_count[i]==0){
			triver1[i*18]+=0.01f;
			triver1[i*18+1]+=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+7]+=0.01f;
			triver1[i*18+12]+=0.01f;
			triver1[i*18+13]+=0.01f;
			if(triver1[i*18+12]>1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=1;
			}
		} 
		else if(triver1_count[i]==1){
			triver1[i*18]-=0.01f;
			triver1[i*18+1]+=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+7]+=0.01f;
			triver1[i*18+12]-=0.01f;
			triver1[i*18+13]+=0.01f;
			if(triver1[i*18+13]>1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=2;
			}
		}
		else if(triver1_count[i]==2){
			triver1[i*18]-=0.01f;
			triver1[i*18+1]-=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+12]-=0.01f;
			triver1[i*18+13]-=0.01f;
			if(triver1[i*18+12]<-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=3;
			}
		} else if(triver1_count[i]==3){
			triver1[i*18]+=0.01f;
			triver1[i*18+1]-=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+12]+=0.01f;
			triver1[i*18+13]-=0.01f;
			if(triver1[i*18+13]<-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=0;
			}
		}
	}
	for(int i=0;i<triver2.size()/18;++i){
		if(triver2_count[i]==0){
			triver2[i*18]+=0.02f;
			triver2[i*18+1]+=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+7]+=0.02f;
			triver2[i*18+12]+=0.02f;
			triver2[i*18+13]+=0.02f;
			if(triver2[i*18+12]>1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=1;
			}
		} else if(triver2_count[i]==1){
			triver2[i*18]-=0.02f;
			triver2[i*18+1]+=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+7]+=0.02f;
			triver2[i*18+12]-=0.02f;
			triver2[i*18+13]+=0.02f;
			if(triver2[i*18+13]>1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=2;
			}
		} else if(triver2_count[i]==2){
			triver2[i*18]-=0.02f;
			triver2[i*18+1]-=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+12]-=0.02f;
			triver2[i*18+13]-=0.02f;
			if(triver2[i*18+12]<-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=3;
			}
		} else if(triver2_count[i]==3){
			triver2[i*18]+=0.02f;
			triver2[i*18+1]-=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+12]+=0.02f;
			triver2[i*18+13]-=0.02f;
			if(triver2[i*18+13]<-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=0;
			}
		}
	}
	for(int i=0;i<triver3.size()/18;++i){
		if(triver3_count[i]==0){
			triver3[i*18]+=0.03f;
			triver3[i*18+1]+=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+7]+=0.03f;
			triver3[i*18+12]+=0.03f;
			triver3[i*18+13]+=0.03f;
			if(triver3[i*18+12]>1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=1;
			}
		} else if(triver3_count[i]==1){
			triver3[i*18]-=0.03f;
			triver3[i*18+1]+=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+7]+=0.03f;
			triver3[i*18+12]-=0.03f;
			triver3[i*18+13]+=0.03f;
			if(triver3[i*18+13]>1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=2;
			}
		} else if(triver3_count[i]==2){
			triver3[i*18]-=0.03f;
			triver3[i*18+1]-=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+12]-=0.03f;
			triver3[i*18+13]-=0.03f;
			if(triver3[i*18+12]<-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=3;
			}
		} else if(triver3_count[i]==3){
			triver3[i*18]+=0.03f;
			triver3[i*18+1]-=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+12]+=0.03f;
			triver3[i*18+13]-=0.03f;
			if(triver3[i*18+13]<-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=0;
			}
		}
	}
	for(int i=0;i<triver4.size()/18;++i){
		if(triver4_count[i]==0){
			triver4[i*18]+=0.04f;
			triver4[i*18+1]+=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+7]+=0.04f;
			triver4[i*18+12]+=0.04f;
			triver4[i*18+13]+=0.04f;
			if(triver4[i*18+12]>1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=1;
			}
		} else if(triver4_count[i]==1){
			triver4[i*18]-=0.04f;
			triver4[i*18+1]+=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+7]+=0.04f;
			triver4[i*18+12]-=0.04f;
			triver4[i*18+13]+=0.04f;
			if(triver4[i*18+13]>1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=2;
			}
		} else if(triver4_count[i]==2){
			triver4[i*18]-=0.04f;
			triver4[i*18+1]-=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+12]-=0.04f;
			triver4[i*18+13]-=0.04f;
			if(triver4[i*18+12]<-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=3;
			}
		} else if(triver4_count[i]==3){
			triver4[i*18]+=0.04f;
			triver4[i*18+1]-=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+12]+=0.04f;
			triver4[i*18+13]-=0.04f;
			if(triver4[i*18+13]<-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=0;
			}
		}
		
	}
}
void move2(){
	for(int i=0;i<triver1.size()/18;++i){
		if(triver1_count[i]==0){
			triver1[i*18]+=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+12]+=0.01f;
			if(triver1[i*18+12]>1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=1;
			}
		}
		if(triver1_count[i]==1){
			triver1[i*18+1]-=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+13]-=0.01f;
			++triver1_time[i];
			if(triver1[i*18+1]<=-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=5;
				triver1_time[i]=0;
			}
			else if(triver1_time[i]==10){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=2;
				triver1_time[i]=0;
			}
		}
		if(triver1_count[i]==2){
			triver1[i*18]-=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+12]-=0.01f;
			if(triver1[i*18+12]<-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=3;
			}
		}
		if(triver1_count[i]==3){
			triver1[i*18+1]-=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+13]-=0.01f;
			++triver1_time[i];
			if(triver1[i*18+1]<=-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=7;
				triver1_time[i]=0;
			}
			else if(triver1_time[i]==10){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=0;
				triver1_time[i]=0;
			}
		}
		if(triver1_count[i]==4){
			triver1[i*18]+=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+12]+=0.01f;
			if(triver1[i*18+12]>1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=5;
			}
		}
		if(triver1_count[i]==5){
			triver1[i*18+1]+=0.01f;
			triver1[i*18+7]+=0.01f;
			triver1[i*18+13]+=0.01f;
			++triver1_time[i];
			if(triver1[i*18+1]>=1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=3;
				triver1_time[i]=0;
			} else if(triver1_time[i]==10){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=6;
				triver1_time[i]=0;
			}
		}
		if(triver1_count[i]==6){
			triver1[i*18]-=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+12]-=0.01f;
			if(triver1[i*18+12]<-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=7;
			}
		}
		if(triver1_count[i]==7){
			triver1[i*18+1]+=0.01f;
			triver1[i*18+7]+=0.01f;
			triver1[i*18+13]+=0.01f;
			++triver1_time[i];
			if(triver1[i*18+1]>=1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=0;
				triver1_time[i]=0;
			} else if(triver1_time[i]==10){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=4;
				triver1_time[i]=0;
			}
		}
		
	}
	for(int i=0;i<triver2.size()/18;++i){
		if(triver2_count[i]==0){
			triver2[i*18]+=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+12]+=0.02f;
			if(triver2[i*18+12]>1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=1;
			}
		}
		if(triver2_count[i]==1){
			triver2[i*18+1]-=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+13]-=0.02f;
			++triver2_time[i];
			if(triver2[i*18+1]<=-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=5;
				triver2_time[i]=0;
			} else if(triver2_time[i]==10){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=2;
				triver2_time[i]=0;
			}
		}
		if(triver2_count[i]==2){
			triver2[i*18]-=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+12]-=0.02f;
			if(triver2[i*18+12]<-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=3;
			}
		}
		if(triver2_count[i]==3){
			triver2[i*18+1]-=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+13]-=0.02f;
			++triver2_time[i];
			if(triver2[i*18+1]<=-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=7;
				triver2_time[i]=0;
			} else if(triver2_time[i]==10){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=0;
				triver2_time[i]=0;
			}
		}
		if(triver2_count[i]==4){
			triver2[i*18]+=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+12]+=0.02f;
			if(triver2[i*18+12]>1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=5;
			}
		}
		if(triver2_count[i]==5){
			triver2[i*18+1]+=0.02f;
			triver2[i*18+7]+=0.02f;
			triver2[i*18+13]+=0.02f;
			++triver2_time[i];
			if(triver2[i*18+1]>=1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=3;
				triver2_time[i]=0;
			} else if(triver2_time[i]==10){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=6;
				triver2_time[i]=0;
			}
		}
		if(triver2_count[i]==6){
			triver2[i*18]-=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+12]-=0.02f;
			if(triver2[i*18+12]<-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=7;
			}
		}
		if(triver2_count[i]==7){
			triver2[i*18+1]+=0.02f;
			triver2[i*18+7]+=0.02f;
			triver2[i*18+13]+=0.02f;
			++triver2_time[i];
			if(triver2[i*18+1]>=1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=0;
				triver2_time[i]=0;
			} else if(triver2_time[i]==10){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=4;
				triver2_time[i]=0;
			}
		}

	}
	for(int i=0;i<triver3.size()/18;++i){
		if(triver3_count[i]==0){
			triver3[i*18]+=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+12]+=0.03f;
			if(triver3[i*18+12]>1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=1;
			}
		}
		if(triver3_count[i]==1){
			triver3[i*18+1]-=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+13]-=0.03f;
			++triver3_time[i];
			if(triver3[i*18+1]<=-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=5;
				triver3_time[i]=0;
			} else if(triver3_time[i]==10){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=2;
				triver3_time[i]=0;
			}
		}
		if(triver3_count[i]==2){
			triver3[i*18]-=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+12]-=0.03f;
			if(triver3[i*18+12]<-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=3;
			}
		}
		if(triver3_count[i]==3){
			triver3[i*18+1]-=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+13]-=0.03f;
			++triver3_time[i];
			if(triver3[i*18+1]<=-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=7;
				triver3_time[i]=0;
			} else if(triver3_time[i]==10){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=0;
				triver3_time[i]=0;
			}
		}
		if(triver3_count[i]==4){
			triver3[i*18]+=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+12]+=0.03f;
			if(triver3[i*18+12]>1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=5;
			}
		}
		if(triver3_count[i]==5){
			triver3[i*18+1]+=0.03f;
			triver3[i*18+7]+=0.03f;
			triver3[i*18+13]+=0.03f;
			++triver3_time[i];
			if(triver3[i*18+1]>=1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=3;
				triver3_time[i]=0;
			} else if(triver3_time[i]==10){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=6;
				triver3_time[i]=0;
			}
		}
		if(triver3_count[i]==6){
			triver3[i*18]-=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+12]-=0.03f;
			if(triver3[i*18+12]<-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=7;
			}
		}
		if(triver3_count[i]==7){
			triver3[i*18+1]+=0.03f;
			triver3[i*18+7]+=0.03f;
			triver3[i*18+13]+=0.03f;
			++triver3_time[i];
			if(triver3[i*18+1]>=1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=0;
				triver3_time[i]=0;
			} else if(triver3_time[i]==10){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=4;
				triver3_time[i]=0;
			}
		}

	}
	for(int i=0;i<triver4.size()/18;++i){
		if(triver4_count[i]==0){
			triver4[i*18]+=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+12]+=0.04f;
			if(triver4[i*18+12]>1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=1;
			}
		}
		if(triver4_count[i]==1){
			triver4[i*18+1]-=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+13]-=0.04f;
			++triver4_time[i];
			if(triver4[i*18+1]<=-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=5;
				triver4_time[i]=0;
			} else if(triver4_time[i]==10){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=2;
				triver4_time[i]=0;
			}
		}
		if(triver4_count[i]==2){
			triver4[i*18]-=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+12]-=0.04f;
			if(triver4[i*18+12]<-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=3;
			}
		}
		if(triver4_count[i]==3){
			triver4[i*18+1]-=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+13]-=0.04f;
			++triver4_time[i];
			if(triver4[i*18+1]<=-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=7;
				triver4_time[i]=0;
			} else if(triver4_time[i]==10){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=0;
				triver4_time[i]=0;
			}
		}
		if(triver4_count[i]==4){
			triver4[i*18]+=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+12]+=0.04f;
			if(triver4[i*18+12]>1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=5;
			}
		}
		if(triver4_count[i]==5){
			triver4[i*18+1]+=0.04f;
			triver4[i*18+7]+=0.04f;
			triver4[i*18+13]+=0.04f;
			++triver4_time[i];
			if(triver4[i*18+1]>=1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=3;
				triver4_time[i]=0;
			} else if(triver4_time[i]==10){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=6;
				triver4_time[i]=0;
			}
		}
		if(triver4_count[i]==6){
			triver4[i*18]-=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+12]-=0.04f;
			if(triver4[i*18+12]<-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=7;
			}
		}
		if(triver4_count[i]==7){
			triver4[i*18+1]+=0.04f;
			triver4[i*18+7]+=0.04f;
			triver4[i*18+13]+=0.04f;
			++triver4_time[i];
			if(triver4[i*18+1]>=1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=0;
				triver4_time[i]=0;
			} else if(triver4_time[i]==10){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=4;
				triver4_time[i]=0;
			}
		}

	}
	
}
void move3(){
	for(int i=0;i<triver1.size()/18;++i){
		if(triver1_count[i]==0){
			triver1[i*18]-=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+12]-=0.01f;
			if(triver1[i*18]<-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=1;
			}
		}
		if(triver1_count[i]==1){
			triver1[i*18+1]-=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+13]-=0.01f;
			if(triver1[i*18+1]<=-1.0f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=2;
			} 
		}
		if(triver1_count[i]==2){
			triver1[i*18]+=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+12]+=0.01f;
			if(triver1[i*18]>0.6f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=3;
			}
		}
		if(triver1_count[i]==3){
			triver1[i*18+1]+=0.01f;
			triver1[i*18+7]+=0.01f;
			triver1[i*18+13]+=0.01f;
			if(triver1[i*18+1]>=0.6f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=4;
			}
		}
		if(triver1_count[i]==4){
			triver1[i*18]-=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+12]-=0.01f;
			if(triver1[i*18]<-0.6f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=5;
			}
		}
		if(triver1_count[i]==5){
			triver1[i*18+1]-=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+13]-=0.01f;
			if(triver1[i*18+1]<=-0.3f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=6;
			}
		}
		if(triver1_count[i]==6){
			triver1[i*18]+=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+12]+=0.01f;
			if(triver1[i*18]>0.4f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=7;
			}
		}
		if(triver1_count[i]==7){
			triver1[i*18+1]+=0.01f;
			triver1[i*18+7]+=0.01f;
			triver1[i*18+13]+=0.01f;
			if(triver1[i*18+1]>=0.4f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=8;
			}
		}
		if(triver1_count[i]==8){
			triver1[i*18]-=0.01f;
			triver1[i*18+6]-=0.01f;
			triver1[i*18+12]-=0.01f;
			if(triver1[i*18]<-0.4f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=9;
			}
		}
		if(triver1_count[i]==9){
			triver1[i*18+1]-=0.01f;
			triver1[i*18+7]-=0.01f;
			triver1[i*18+13]-=0.01f;
			if(triver1[i*18+1]<=-0.2f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=10;
			}
		}
		if(triver1_count[i]==10){
			triver1[i*18]+=0.01f;
			triver1[i*18+6]+=0.01f;
			triver1[i*18+12]+=0.01f;
			if(triver1[i*18]>0.2f){
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
				triver1_count[i]=11;
			}
		}
	}
	for(int i=0;i<triver2.size()/18;++i){
		if(triver2_count[i]==0){
			triver2[i*18]-=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+12]-=0.02f;
			if(triver2[i*18]<-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=1;
			}
		}
		if(triver2_count[i]==1){
			triver2[i*18+1]-=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+13]-=0.02f;
			if(triver2[i*18+1]<=-1.0f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=2;
			}
		}
		if(triver2_count[i]==2){
			triver2[i*18]+=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+12]+=0.02f;
			if(triver2[i*18]>0.6f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=3;
			}
		}
		if(triver2_count[i]==3){
			triver2[i*18+1]+=0.02f;
			triver2[i*18+7]+=0.02f;
			triver2[i*18+13]+=0.02f;
			if(triver2[i*18+1]>=0.6f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=4;
			}
		}
		if(triver2_count[i]==4){
			triver2[i*18]-=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+12]-=0.02f;
			if(triver2[i*18]<-0.6f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=5;
			}
		}
		if(triver2_count[i]==5){
			triver2[i*18+1]-=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+13]-=0.02f;
			if(triver2[i*18+1]<=-0.3f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=6;
			}
		}
		if(triver2_count[i]==6){
			triver2[i*18]+=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+12]+=0.02f;
			if(triver2[i*18]>0.4f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=7;
			}
		}
		if(triver2_count[i]==7){
			triver2[i*18+1]+=0.02f;
			triver2[i*18+7]+=0.02f;
			triver2[i*18+13]+=0.02f;
			if(triver2[i*18+1]>=0.4f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=8;
			}
		}
		if(triver2_count[i]==8){
			triver2[i*18]-=0.02f;
			triver2[i*18+6]-=0.02f;
			triver2[i*18+12]-=0.02f;
			if(triver2[i*18]<-0.4f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=9;
			}
		}
		if(triver2_count[i]==9){
			triver2[i*18+1]-=0.02f;
			triver2[i*18+7]-=0.02f;
			triver2[i*18+13]-=0.02f;
			if(triver2[i*18+1]<=-0.2f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=10;
			}
		}
		if(triver2_count[i]==10){
			triver2[i*18]+=0.02f;
			triver2[i*18+6]+=0.02f;
			triver2[i*18+12]+=0.02f;
			if(triver2[i*18]>0.2f){
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
				triver2_count[i]=11;
			}
		}
	}
	for(int i=0;i<triver3.size()/18;++i){
		if(triver3_count[i]==0){
			triver3[i*18]-=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+12]-=0.03f;
			if(triver3[i*18]<-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=1;
			}
		}
		if(triver3_count[i]==1){
			triver3[i*18+1]-=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+13]-=0.03f;
			if(triver3[i*18+1]<=-1.0f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=2;
			}
		}
		if(triver3_count[i]==2){
			triver3[i*18]+=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+12]+=0.03f;
			if(triver3[i*18]>0.6f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=3;
			}
		}
		if(triver3_count[i]==3){
			triver3[i*18+1]+=0.03f;
			triver3[i*18+7]+=0.03f;
			triver3[i*18+13]+=0.03f;
			if(triver3[i*18+1]>=0.6f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=4;
			}
		}
		if(triver3_count[i]==4){
			triver3[i*18]-=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+12]-=0.03f;
			if(triver3[i*18]<-0.6f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=5;
			}
		}
		if(triver3_count[i]==5){
			triver3[i*18+1]-=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+13]-=0.03f;
			if(triver3[i*18+1]<=-0.3f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=6;
			}
		}
		if(triver3_count[i]==6){
			triver3[i*18]+=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+12]+=0.03f;
			if(triver3[i*18]>0.4f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=7;
			}
		}
		if(triver3_count[i]==7){
			triver3[i*18+1]+=0.03f;
			triver3[i*18+7]+=0.03f;
			triver3[i*18+13]+=0.03f;
			if(triver3[i*18+1]>=0.4f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=8;
			}
		}
		if(triver3_count[i]==8){
			triver3[i*18]-=0.03f;
			triver3[i*18+6]-=0.03f;
			triver3[i*18+12]-=0.03f;
			if(triver3[i*18]<-0.4f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=9;
			}
		}
		if(triver3_count[i]==9){
			triver3[i*18+1]-=0.03f;
			triver3[i*18+7]-=0.03f;
			triver3[i*18+13]-=0.03f;
			if(triver3[i*18+1]<=-0.2f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=10;
			}
		}
		if(triver3_count[i]==10){
			triver3[i*18]+=0.03f;
			triver3[i*18+6]+=0.03f;
			triver3[i*18+12]+=0.03f;
			if(triver3[i*18]>0.2f){
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
				triver3_count[i]=11;
			}
		}
	}
	for(int i=0;i<triver4.size()/18;++i){
		if(triver4_count[i]==0){
			triver4[i*18]-=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+12]-=0.04f;
			if(triver4[i*18]<-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=1;
			}
		}
		if(triver4_count[i]==1){
			triver4[i*18+1]-=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+13]-=0.04f;
			if(triver4[i*18+1]<=-1.0f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=2;
			}
		}
		if(triver4_count[i]==2){
			triver4[i*18]+=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+12]+=0.04f;
			if(triver4[i*18]>0.6f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=3;
			}
		}
		if(triver4_count[i]==3){
			triver4[i*18+1]+=0.04f;
			triver4[i*18+7]+=0.04f;
			triver4[i*18+13]+=0.04f;
			if(triver4[i*18+1]>=0.6f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=4;
			}
		}
		if(triver4_count[i]==4){
			triver4[i*18]-=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+12]-=0.04f;
			if(triver4[i*18]<-0.6f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=5;
			}
		}
		if(triver4_count[i]==5){
			triver4[i*18+1]-=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+13]-=0.04f;
			if(triver4[i*18+1]<=-0.3f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=6;
			}
		}
		if(triver4_count[i]==6){
			triver4[i*18]+=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+12]+=0.04f;
			if(triver4[i*18]>0.4f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=7;
			}
		}
		if(triver4_count[i]==7){
			triver4[i*18+1]+=0.04f;
			triver4[i*18+7]+=0.04f;
			triver4[i*18+13]+=0.04f;
			if(triver4[i*18+1]>=0.4f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=8;
			}
		}
		if(triver4_count[i]==8){
			triver4[i*18]-=0.04f;
			triver4[i*18+6]-=0.04f;
			triver4[i*18+12]-=0.04f;
			if(triver4[i*18]<-0.4f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=9;
			}
		}
		if(triver4_count[i]==9){
			triver4[i*18+1]-=0.04f;
			triver4[i*18+7]-=0.04f;
			triver4[i*18+13]-=0.04f;
			if(triver4[i*18+1]<=-0.2f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=10;
			}
		}
		if(triver4_count[i]==10){
			triver4[i*18]+=0.04f;
			triver4[i*18+6]+=0.04f;
			triver4[i*18+12]+=0.04f;
			if(triver4[i*18]>0.2f){
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
				triver4_count[i]=11;
			}
		}
	}


}
void move4(){
	for(int i=0;i<triver1.size()/18;++i){
		triver1[i*18]+=static_cast<float>(triver1_count[i])/100000.0f*cos(static_cast<float>(triver1_radian[i])/180.0f*3.14f);
		triver1[i*18+1]+=static_cast<float>(triver1_count[i])/100000.0f*sin(static_cast<float>(triver1_radian[i])/180.0f*3.14f);
		triver1[i*18+6]+=static_cast<float>(triver1_count[i])/100000.0f*cos(static_cast<float>(triver1_radian[i])/180.0f*3.14f);
		triver1[i*18+7]+=static_cast<float>(triver1_count[i])/100000.0f*sin(static_cast<float>(triver1_radian[i])/180.0f*3.14f);
		triver1[i*18+12]+=static_cast<float>(triver1_count[i])/100000.0f*cos(static_cast<float>(triver1_radian[i])/180.0f*3.14f);
		triver1[i*18+13]+=static_cast<float>(triver1_count[i])/100000.0f*sin(static_cast<float>(triver1_radian[i])/180.0f*3.14f);

		triRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13],static_cast<float>(triver1_radian[i]));
		triver1_radian[i]=(++triver1_radian[i])%360;
		if(triver1_count[i]<1500)
		++triver1_count[i];
	}
	for(int i=0;i<triver2.size()/18;++i){
		triver2[i*18]+=static_cast<float>(triver2_count[i])/100000.0f*cos(static_cast<float>(triver2_radian[i])/180.0f*3.14f);
		triver2[i*18+1]+=static_cast<float>(triver2_count[i])/100000.0f*sin(static_cast<float>(triver2_radian[i])/180.0f*3.14f);
		triver2[i*18+6]+=static_cast<float>(triver2_count[i])/100000.0f*cos(static_cast<float>(triver2_radian[i])/180.0f*3.14f);
		triver2[i*18+7]+=static_cast<float>(triver2_count[i])/100000.0f*sin(static_cast<float>(triver2_radian[i])/180.0f*3.14f);
		triver2[i*18+12]+=static_cast<float>(triver2_count[i])/100000.0f*cos(static_cast<float>(triver2_radian[i])/180.0f*3.14f);
		triver2[i*18+13]+=static_cast<float>(triver2_count[i])/100000.0f*sin(static_cast<float>(triver2_radian[i])/180.0f*3.14f);
		triRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13],static_cast<float>(triver2_radian[i]));
		triver2_radian[i]=(triver2_radian[i]+2)%360;
		if(triver2_count[i]<1300)
			++triver2_count[i];
	}
	for(int i=0;i<triver3.size()/18;++i){
		triver3[i*18]+=static_cast<float>(triver3_count[i])/100000.0f*cos(static_cast<float>(triver3_radian[i])/180.0f*3.14f);
		triver3[i*18+1]+=static_cast<float>(triver3_count[i])/100000.0f*sin(static_cast<float>(triver3_radian[i])/180.0f*3.14f);
		triver3[i*18+6]+=static_cast<float>(triver3_count[i])/100000.0f*cos(static_cast<float>(triver3_radian[i])/180.0f*3.14f);
		triver3[i*18+7]+=static_cast<float>(triver3_count[i])/100000.0f*sin(static_cast<float>(triver3_radian[i])/180.0f*3.14f);
		triver3[i*18+12]+=static_cast<float>(triver3_count[i])/100000.0f*cos(static_cast<float>(triver3_radian[i])/180.0f*3.14f);
		triver3[i*18+13]+=static_cast<float>(triver3_count[i])/100000.0f*sin(static_cast<float>(triver3_radian[i])/180.0f*3.14f);
		triRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13],static_cast<float>(triver3_radian[i]));
		triver3_radian[i]=(triver3_radian[i]+3)%360;
		if(triver3_count[i]<1100)
			++triver3_count[i];
	}
	for(int i=0;i<triver4.size()/18;++i){
		triver4[i*18]+=static_cast<float>(triver4_count[i])/100000.0f*cos(static_cast<float>(triver4_radian[i])/180.0f*3.14f);
		triver4[i*18+1]+=static_cast<float>(triver4_count[i])/100000.0f*sin(static_cast<float>(triver4_radian[i])/180.0f*3.14f);
		triver4[i*18+6]+=static_cast<float>(triver4_count[i])/100000.0f*cos(static_cast<float>(triver4_radian[i])/180.0f*3.14f);
		triver4[i*18+7]+=static_cast<float>(triver4_count[i])/100000.0f*sin(static_cast<float>(triver4_radian[i])/180.0f*3.14f);
		triver4[i*18+12]+=static_cast<float>(triver4_count[i])/100000.0f*cos(static_cast<float>(triver4_radian[i])/180.0f*3.14f);
		triver4[i*18+13]+=static_cast<float>(triver4_count[i])/100000.0f*sin(static_cast<float>(triver4_radian[i])/180.0f*3.14f);
		triRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13],static_cast<float>(triver4_radian[i]));
		triver4_radian[i]=(triver4_radian[i]+4)%360;
		if(triver4_count[i]<900)
			++triver4_count[i];
	}


}
void Reset()
{

	for(int i = 0; i < triver1.size() / 18; ++i) {

		triRotate(triver1[i * 18],triver1[i * 18+1],triver1[i * 18+6],triver1[i * 18+7],triver1[i * 18+12],triver1[i * 18+13],360-triver1_radian[i]);



		float currentCenterX = (triver1[i * 18] + triver1[i * 18 + 6] + triver1[i * 18 + 12]) / 3.0f;
		float currentCenterY = (triver1[i * 18 + 1] + triver1[i * 18 + 7] + triver1[i * 18 + 13]) / 3.0f;

		float sourceCenterX = (ctriver1[i * 18] + ctriver1[i * 18 + 6] + ctriver1[i * 18 + 12]) / 3.0f;
		float sourceCenterY = (ctriver1[i * 18 + 1] + ctriver1[i * 18 + 7] + ctriver1[i * 18 + 13]) / 3.0f;

		float rel_v1x = ctriver1[i * 18] - sourceCenterX;
		float rel_v1y = ctriver1[i * 18 + 1] - sourceCenterY;
		float rel_v2x = ctriver1[i * 18 + 6] - sourceCenterX;
		float rel_v2y = ctriver1[i * 18 + 7] - sourceCenterY;
		float rel_v3x = ctriver1[i * 18 + 12] - sourceCenterX;
		float rel_v3y = ctriver1[i * 18 + 13] - sourceCenterY;

		triver1[i * 18]     = currentCenterX + rel_v1x;
		triver1[i * 18 + 1] = currentCenterY + rel_v1y;
		triver1[i * 18 + 6] = currentCenterX + rel_v2x;
		triver1[i * 18 + 7] = currentCenterY + rel_v2y;
		triver1[i * 18 + 12]= currentCenterX + rel_v3x;
		triver1[i * 18 + 13]= currentCenterY + rel_v3y;
	}

	for(int i = 0; i < triver2.size() / 18; ++i) {

		triRotate(triver1[i * 18],triver1[i * 18+1],triver1[i * 18+6],triver1[i * 18+7],triver1[i * 18+12],triver1[i * 18+13],360-triver1_radian[i]);
		float currentCenterX = (triver2[i * 18] + triver2[i * 18 + 6] + triver2[i * 18 + 12]) / 3.0f;
		float currentCenterY = (triver2[i * 18 + 1] + triver2[i * 18 + 7] + triver2[i * 18 + 13]) / 3.0f;

		float sourceCenterX = (ctriver2[i * 18] + ctriver2[i * 18 + 6] + ctriver2[i * 18 + 12]) / 3.0f;
		float sourceCenterY = (ctriver2[i * 18 + 1] + ctriver2[i * 18 + 7] + ctriver2[i * 18 + 13]) / 3.0f;

		float rel_v1x = ctriver2[i * 18] - sourceCenterX;
		float rel_v1y = ctriver2[i * 18 + 1] - sourceCenterY;
		float rel_v2x = ctriver2[i * 18 + 6] - sourceCenterX;
		float rel_v2y = ctriver2[i * 18 + 7] - sourceCenterY;
		float rel_v3x = ctriver2[i * 18 + 12] - sourceCenterX;
		float rel_v3y = ctriver2[i * 18 + 13] - sourceCenterY;

		triver2[i * 18] = currentCenterX + rel_v1x;
		triver2[i * 18 + 1] = currentCenterY + rel_v1y;
		triver2[i * 18 + 6] = currentCenterX + rel_v2x;
		triver2[i * 18 + 7] = currentCenterY + rel_v2y;
		triver2[i * 18 + 12] = currentCenterX + rel_v3x;
		triver2[i * 18 + 13] = currentCenterY + rel_v3y;
	}

	for(int i = 0; i < triver3.size() / 18; ++i) {

		triRotate(triver1[i * 18],triver1[i * 18+1],triver1[i * 18+6],triver1[i * 18+7],triver1[i * 18+12],triver1[i * 18+13],360-triver1_radian[i]);
		float currentCenterX = (triver3[i * 18] + triver3[i * 18 + 6] + triver3[i * 18 + 12]) / 3.0f;
		float currentCenterY = (triver3[i * 18 + 1] + triver3[i * 18 + 7] + triver3[i * 18 + 13]) / 3.0f;

		float sourceCenterX = (ctriver3[i * 18] + ctriver3[i * 18 + 6] + ctriver3[i * 18 + 12]) / 3.0f;
		float sourceCenterY = (ctriver3[i * 18 + 1] + ctriver3[i * 18 + 7] + ctriver3[i * 18 + 13]) / 3.0f;

		float rel_v1x = ctriver3[i * 18] - sourceCenterX;
		float rel_v1y = ctriver3[i * 18 + 1] - sourceCenterY;
		float rel_v2x = ctriver3[i * 18 + 6] - sourceCenterX;
		float rel_v2y = ctriver3[i * 18 + 7] - sourceCenterY;
		float rel_v3x = ctriver3[i * 18 + 12] - sourceCenterX;
		float rel_v3y = ctriver3[i * 18 + 13] - sourceCenterY;

		triver3[i * 18] = currentCenterX + rel_v1x;
		triver3[i * 18 + 1] = currentCenterY + rel_v1y;
		triver3[i * 18 + 6] = currentCenterX + rel_v2x;
		triver3[i * 18 + 7] = currentCenterY + rel_v2y;
		triver3[i * 18 + 12] = currentCenterX + rel_v3x;
		triver3[i * 18 + 13] = currentCenterY + rel_v3y;
	}

	for(int i = 0; i < triver4.size() / 18; ++i) {

		triRotate(triver1[i * 18],triver1[i * 18+1],triver1[i * 18+6],triver1[i * 18+7],triver1[i * 18+12],triver1[i * 18+13],360-triver1_radian[i]);
		float currentCenterX = (triver4[i * 18] + triver4[i * 18 + 6] + triver4[i * 18 + 12]) / 3.0f;
		float currentCenterY = (triver4[i * 18 + 1] + triver4[i * 18 + 7] + triver4[i * 18 + 13]) / 3.0f;

		float sourceCenterX = (ctriver4[i * 18] + ctriver4[i * 18 + 6] + ctriver4[i * 18 + 12]) / 3.0f;
		float sourceCenterY = (ctriver4[i * 18 + 1] + ctriver4[i * 18 + 7] + ctriver4[i * 18 + 13]) / 3.0f;

		float rel_v1x = ctriver4[i * 18] - sourceCenterX;
		float rel_v1y = ctriver4[i * 18 + 1] - sourceCenterY;
		float rel_v2x = ctriver4[i * 18 + 6] - sourceCenterX;
		float rel_v2y = ctriver4[i * 18 + 7] - sourceCenterY;
		float rel_v3x = ctriver4[i * 18 + 12] - sourceCenterX;
		float rel_v3y = ctriver4[i * 18 + 13] - sourceCenterY;

		triver4[i * 18] = currentCenterX + rel_v1x;
		triver4[i * 18 + 1] = currentCenterY + rel_v1y;
		triver4[i * 18 + 6] = currentCenterX + rel_v2x;
		triver4[i * 18 + 7] = currentCenterY + rel_v2y;
		triver4[i * 18 + 12] = currentCenterX + rel_v3x;
		triver4[i * 18 + 13] = currentCenterY + rel_v3y;
	}
}
void Set1(){
	for(int i=0;i<triver2.size()/18;++i){
			tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
		
	}
	for(int i=0;i<triver3.size()/18;++i){
		for(int j=0;j<2;++j)
			tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);

	}
	for(int i=0;i<triver4.size()/18;++i){
		for(int j=0;j<3;++j)
			tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
	}
}
void Set2(){
	for(int i=0;i<triver1.size()/18;++i){
		for(int j=0;j<3;++j)
		tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);

	}
	for(int i=0;i<triver2.size()/18;++i){
		tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);

	}
	for(int i=0;i<triver3.size()/18;++i){
		for(int j=0;j<3;++j)
			tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);

	}
	for(int i=0;i<triver4.size()/18;++i){
		
			tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
	}
}
void Set3(){
	for(int i=0;i<triver1.size()/18;++i){
		float f= triver1[i*18+1]-triver1[i*18+7];
		triver1[i*18+1]=1.0f;
		triver1[i*18+7]=1.0f-f;
		triver1[i*18+13]=1.0f-f;
		tri90DegressRotate(triver1[i*18],triver1[i*18+1],triver1[i*18+6],triver1[i*18+7],triver1[i*18+12],triver1[i*18+13]);
	}
	for(int i=0;i<triver2.size()/18;++i){
		float f= triver2[i*18+1]-triver2[i*18+7];
		triver2[i*18+1]=1.0f;
		triver2[i*18+7]=1.0f-f;
		triver2[i*18+13]=1.0f-f;
		tri90DegressRotate(triver2[i*18],triver2[i*18+1],triver2[i*18+6],triver2[i*18+7],triver2[i*18+12],triver2[i*18+13]);
	}
	for(int i=0;i<triver3.size()/18;++i){
		float f= triver3[i*18+1]-triver3[i*18+7];
		triver3[i*18+1]=1.0f;
		triver3[i*18+7]=1.0f-f;
		triver3[i*18+13]=1.0f-f;
		tri90DegressRotate(triver3[i*18],triver3[i*18+1],triver3[i*18+6],triver3[i*18+7],triver3[i*18+12],triver3[i*18+13]);
	}
	for(int i=0;i<triver4.size()/18;++i){
		float f= triver4[i*18+1]-triver4[i*18+7];
		triver4[i*18+1]=1.0f;
		triver4[i*18+7]=1.0f-f;
		triver4[i*18+13]=1.0f-f;
		tri90DegressRotate(triver4[i*18],triver4[i*18+1],triver4[i*18+6],triver4[i*18+7],triver4[i*18+12],triver4[i*18+13]);
	}
}
void Set4(){
	for(int i=0;i<triver1.size()/18;++i){
		float a= (triver1[i*18]+triver1[i*18+6]+triver1[i*18+12])/3.0f;
		float b= (triver1[i*18+1]+triver1[i*18+7]+triver1[i*18+13])/3.0f;
		triver1[i*18]=triver1[i*18]-a;
		triver1[i*18+1]=triver1[i*18+1]-b;
		triver1[i*18+6]=triver1[i*18+6]-a;
		triver1[i*18+7]=triver1[i*18+7]-b;
		triver1[i*18+12]=triver1[i*18+12]-a;
		triver1[i*18+13]=triver1[i*18+13]-b;
	}
	for(int i=0;i<triver2.size()/18;++i){
		float a= (triver2[i*18]+triver2[i*18+6]+triver2[i*18+12])/3.0f;
		float b= (triver2[i*18+1]+triver2[i*18+7]+triver2[i*18+13])/3.0f;
		triver2[i*18]=triver2[i*18]-a;
		triver2[i*18+1]=triver2[i*18+1]-b;
		triver2[i*18+6]=triver2[i*18+6]-a;
		triver2[i*18+7]=triver2[i*18+7]-b;
		triver2[i*18+12]=triver2[i*18+12]-a;
		triver2[i*18+13]=triver2[i*18+13]-b;
	}
	for(int i=0;i<triver3.size()/18;++i){
		float a= (triver3[i*18]+triver3[i*18+6]+triver3[i*18+12])/3.0f;
		float b= (triver3[i*18+1]+triver3[i*18+7]+triver3[i*18+13])/3.0f;
		triver3[i*18]=triver3[i*18]-a;
		triver3[i*18+1]=triver3[i*18+1]-b;
		triver3[i*18+6]=triver3[i*18+6]-a;
		triver3[i*18+7]=triver3[i*18+7]-b;
		triver3[i*18+12]=triver3[i*18+12]-a;
		triver3[i*18+13]=triver3[i*18+13]-b;
	}
	for(int i=0;i<triver4.size()/18;++i){
		float a= (triver4[i*18]+triver4[i*18+6]+triver4[i*18+12])/3.0f;
		float b= (triver4[i*18+1]+triver4[i*18+7]+triver4[i*18+13])/3.0f;
		triver4[i*18]=triver4[i*18]-a;
		triver4[i*18+1]=triver4[i*18+1]-b;
		triver4[i*18+6]=triver4[i*18+6]-a;
		triver4[i*18+7]=triver4[i*18+7]-b;
		triver4[i*18+12]=triver4[i*18+12]-a;
		triver4[i*18+13]=triver4[i*18+13]-b;
	}
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
	Initdata();

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
	rColor = bColor = 0;
	gColor = 0; //--- 배경색을 파랑색으로 설정
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

	int triangleCount1 = triver1.size() / 6;
	int triangleCount2 = triver2.size() / 6;
	int triangleCount3 = triver3.size() / 6;
	int triangleCount4 = triver4.size() / 6;
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
	if(triangleCount1 > 0)
	{
		glDrawArrays(GL_TRIANGLES,offset,triangleCount1);
		offset += triangleCount1;
	}
	if(triangleCount2 > 0)
	{
		glDrawArrays(GL_TRIANGLES,offset,triangleCount2);
		offset += triangleCount2;
	}
	if(triangleCount3 > 0)
	{
		glDrawArrays(GL_TRIANGLES,offset,triangleCount3);
		offset += triangleCount3;
	}
	if(triangleCount4 > 0)
	{
		glDrawArrays(GL_TRIANGLES,offset,triangleCount4);
		offset += triangleCount4;
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
	case '1':
	Reset();
	mode = 1;
	fill(triver1_count.begin(),triver1_count.end(),0);
	fill(triver2_count.begin(),triver2_count.end(),1);
	fill(triver3_count.begin(),triver3_count.end(),2);
	fill(triver4_count.begin(),triver4_count.end(),3);
	fill(triver1_radian.begin(),triver1_radian.end(),0);
	fill(triver2_radian.begin(),triver2_radian.end(),0);
	fill(triver3_radian.begin(),triver3_radian.end(),0);
	fill(triver4_radian.begin(),triver4_radian.end(),0);
	Set1();
	break;
	case '2':
	Reset();
	mode = 2;
	fill(triver1_count.begin(),triver1_count.end(),0);
	fill(triver2_count.begin(),triver2_count.end(),2);
	fill(triver3_count.begin(),triver3_count.end(),4);
	fill(triver4_count.begin(),triver4_count.end(),6);
	fill(triver1_time.begin(),triver1_time.end(),0);
	fill(triver2_time.begin(),triver2_time.end(),0);
	fill(triver3_time.begin(),triver3_time.end(),0);
	fill(triver4_time.begin(),triver4_time.end(),0);
	fill(triver1_radian.begin(),triver1_radian.end(),0);
	fill(triver2_radian.begin(),triver2_radian.end(),0);
	fill(triver3_radian.begin(),triver3_radian.end(),0);
	fill(triver4_radian.begin(),triver4_radian.end(),0);
	Set2();
	break;
	case '3':
	Reset();
	mode = 3;
	fill(triver1_count.begin(),triver1_count.end(),0);
	fill(triver2_count.begin(),triver2_count.end(),0);
	fill(triver3_count.begin(),triver3_count.end(),0);
	fill(triver4_count.begin(),triver4_count.end(),0);
	fill(triver1_time.begin(),triver1_time.end(),0);
	fill(triver2_time.begin(),triver2_time.end(),0);
	fill(triver3_time.begin(),triver3_time.end(),0);
	fill(triver4_time.begin(),triver4_time.end(),0);
	fill(triver1_radian.begin(),triver1_radian.end(),0);
	fill(triver2_radian.begin(),triver2_radian.end(),0);
	fill(triver3_radian.begin(),triver3_radian.end(),0);
	fill(triver4_radian.begin(),triver4_radian.end(),0);
	Set3();
	break;
	case '4':
	Reset();
	mode = 4;
	fill(triver1_count.begin(),triver1_count.end(),0);
	fill(triver2_count.begin(),triver2_count.end(),0);
	fill(triver3_count.begin(),triver3_count.end(),0);
	fill(triver4_count.begin(),triver4_count.end(),0);
	fill(triver1_time.begin(),triver1_time.end(),0);
	fill(triver2_time.begin(),triver2_time.end(),0);
	fill(triver3_time.begin(),triver3_time.end(),0);
	fill(triver4_time.begin(),triver4_time.end(),0);
	fill(triver1_radian.begin(),triver1_radian.end(),0);
	fill(triver2_radian.begin(),triver2_radian.end(),0);
	fill(triver3_radian.begin(),triver3_radian.end(),0);
	fill(triver4_radian.begin(),triver4_radian.end(),0);
	Set4();
	break;
	case 'c':
	triver1.clear();
	triver2.clear();
	triver3.clear();
	triver4.clear();
	triver1_count.clear();
	triver2_count.clear();
	triver3_count.clear();
	triver4_count.clear();
	mode=0;
	Initdata();
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
		for(int i=0;i<4;++i)
			if(FPtInFRect(rect[i],FPOINT(tranformx(x),tranformy(y)))){
				mode=0;
				float a1=rtri(gen);
				float a2=rtri(gen);
				if(i==0){
					triver1.erase(triver1.begin(),triver1.begin()+18);
					ctriver1.erase(ctriver1.begin(),ctriver1.begin()+18);
					triver1_count.erase(triver1_count.begin());
					triver1.insert(triver1.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver1.insert(ctriver1.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver1_count.push_back(0);
				
				} else if(i==1){
					triver2.erase(triver2.begin(),triver2.begin()+18);
					ctriver2.erase(ctriver2.begin(),ctriver2.begin()+18);
					triver2_count.erase(triver2_count.begin());
					triver2.insert(triver2.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver2.insert(ctriver2.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver2_count.push_back(0);
				} else if(i==2){
					triver3.erase(triver3.begin(),triver3.begin()+18);
					ctriver3.erase(ctriver3.begin(),ctriver3.begin()+18);
					triver3_count.erase(triver3_count.begin());
					triver3.insert(triver3.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver3.insert(ctriver3.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver3_count.push_back(0);
				} else if(i==3){
					triver4.erase(triver4.begin(),triver4.begin()+18);
					ctriver4.erase(ctriver4.begin(),ctriver4.begin()+18);
					triver4_count.erase(triver4_count.begin());
					triver4.insert(triver4.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver4.insert(ctriver4.end(),{
						tranformx(x),tranformy(y)+rtri(gen),0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver4_count.push_back(0);
				}
			}
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		for(int i=0;i<4;++i)
			if(FPtInFRect(rect[i],FPOINT(tranformx(x),tranformy(y)))){
				mode=0;
				float a1=rtri(gen);
				float a2=rtri(gen);
				float a3=rtri(gen);
				if(i==0&&triver1.size()/18<4){
					triver1.insert(triver1.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver1.insert(ctriver1.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver1_count.push_back(0);
				} else if(i==1&&triver2.size()/18<4){
					triver2.insert(triver2.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver2.insert(ctriver2.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver2_count.push_back(0);
				} else if(i==2&&triver3.size()/18<4){
					triver3.insert(triver3.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver3.insert(ctriver3.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver3_count.push_back(0);
				} else if(i==3&&triver4.size()/18<4){
					triver4.insert(triver4.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					ctriver4.insert(ctriver4.end(),{
						tranformx(x),tranformy(y)+a3,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)-a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen),
						tranformx(x)+a1,tranformy(y)-a2,0.0f,rcolor(gen),rcolor(gen),rcolor(gen)
					});
					triver4_count.push_back(0);
				}
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
	if(mode ==1){
		move1();
	}
	else if(mode ==2){
		move2();
	}
	else if(mode ==3){
		move3();
	} 
	else if(mode ==4){
		move4();
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
	if(!result)
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