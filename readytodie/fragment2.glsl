#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 materialColorDefault;
uniform bool bUseTexture;

// 조명 관련 변수(lightPos 등)는 이제 필요 없어서 삭제했습니다.

void main()
{
    vec4 finalColor;

    if (bUseTexture) {
        // 텍스처가 있으면 텍스처 색상 그대로 출력 (조명 영향 X)
        finalColor = texture(texture_diffuse1, TexCoords);
    }
    else {
        // 텍스처가 없으면 기본 색상 출력
        finalColor = vec4(materialColorDefault, 1.0);
    }

    FragColor = finalColor;
}