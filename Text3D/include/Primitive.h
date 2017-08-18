//Primitive�ǻ������ε���С��Ԫ��һ��������ÿһ��contourʹ��һ��Geometry

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include <iostream>
#include <vector>

#include <gl\glew.h>

#include <glm\glm.hpp>

#include "MyShader.h"

//�����������������
typedef std::vector<glm::vec3> Vec3Array;
typedef std::vector<GLuint> ElementArray;

//����ṹ
struct Vertex
{
	glm::vec3 position;  //λ������
	glm::vec3 normal;    //������
	//glm::vec2 texcoord;  //��������
};

class Primitive
{
private:
	GLuint VAO, VBO, EBO;
	GLenum mode;     //����ͼԪ����
	void SetupVAO();
public:
	std::vector<Vertex> vertices;
	ElementArray indices;
	Primitive() = default;
	Primitive(GLenum m, Vec3Array vec3s, ElementArray eles, Vec3Array norms);
	~Primitive() {}
	//����
	void draw(MyShader shader);

	//����������
	void deleteBuffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};

#endif