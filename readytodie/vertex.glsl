#version 330 core
void main()
{
	//gl_Position = vec4(0.5, 0.0, 0.0, 1.0); //------(0.5, 0.0, 0.0) 으로 좌표값 고정
	
	const vec4 vertex[3] = vec4[3] (vec4(-0.25, -0.25, 0.5, 1.0),
	vec4(0.25, -0.25, 0.5, 1.0),
	vec4(0.0, 0.25, 0.5, 1.0));

	gl_Position = vertex [gl_VertexID];


}