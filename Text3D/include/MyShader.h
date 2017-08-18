#ifndef MYSHADER_H_
#define MYSHADER_H_

#include <iostream>
#include <fstream>    //���ļ��ж�ȡ��ɫ��Դ����
#include <sstream>
#include <string>

#include <gl/glew.h>  //����Ϊ�˰������б�Ҫ��openglͷ�ļ�

class MyShader
{
public:
	//��ɫ���������

	GLuint shader_program;
	//���캯�����������������캯��������������
	//��һ���Ƕ�����ɫ�����ļ�·�����ڶ�����ƬԪ��ɫ�����ļ�·��
	MyShader(const GLchar *vert_path, const GLchar *frag_path);
	~MyShader() {}

	//�ú�������ʹ����ɫ������
	void Use()
	{
		glUseProgram(shader_program);
	}
};

#endif