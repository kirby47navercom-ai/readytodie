//1-8


//--- vertex shader: vertex.glsl 파일에 저장
//--- in_Position: attribute index 0
//--- in_Color: attribute index 1
layout (location = 0) in vec3 in_Position; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 in_Color; //--- 컬러 변수: attribute position 1
out vec3 out_Color; //--- 프래그먼트 세이더에게 전달
void main(void)
{
gl_Position = vec4 (in_Position.x, in_Position.y, in_Position.z, 1.0);
out_Color = in_Color;
}

/*
#version 330 core
layout (location = 0) in vec3 vPos; //--- attribute로 설정된 위치 속성: 인덱스 0
void main()
{
gl_Position = vec4 (vPos.x, vPos.y, vPos.z, 1.0);
}
*/


/*
#version 330 core
layout (location = 0) in vec3 vPos; //--- 위치 변수: attribute position 0
layout (location = 1) in vec3 vColor; //--- 컬러 변수: attribute position 1
out vec3 outColor; //--- 컬러를 fragment shader로 출력
void main()
{
gl_Position = vec4 (vPos, 1.0);
outColor = vColor; //--- vertex data로부터 가져온 컬러 입력을 ourColor에 설정
} 
*/




/*
#version 330 core
void main()
{
	//gl_Position = vec4(0.5, 0.0, 0.0, 1.0); //------(0.5, 0.0, 0.0) 으로 좌표값 고정
	
	const vec4 vertex[3] = vec4[3] (vec4(-0.25, -0.25, 0.5, 1.0),
	vec4(0.25, -0.25, 0.5, 1.0),
	vec4(0.0, 0.25, 0.5, 1.0));

	//const vec4 vertex[4] = vec4[4] (vec4(-0.25, -0.25, 0.5, 1.0),
// vec4(0.25, -0.25, 0.5, 1.0),
// vec4(0.0, 0.25, 0.5, 1.0),
// vec4(0.0, -0.5, 0.5, 1.0));

	gl_Position = vertex [gl_VertexID];


}
*/