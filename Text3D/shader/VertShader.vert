#version 330 core

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_normal;

out vec3 Normal;   //�������������������
out vec3 FragPos;  //���������е�ƬԪ��λ�ã���������ÿ��ƬԪ�Ϲ��յķ���

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vert_position, 1.0f);
    Normal = vert_normal;
    FragPos = vec3(model_matrix * vec4(vert_position, 1.0f));
}