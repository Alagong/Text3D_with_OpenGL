#include <iostream>
#include <map>
#include <vector>
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/MyShader.h"
#include "../include/FreeTypeFont.h"
#include "../include/Tessellator.h"
#include "../include/FREEcamera.h"
#include "../include/Geometry.h"

//���ڳߴ�
const GLfloat WIDTH = 1024, HEIGHT = 768;

//����ĳ�ʼ������
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);      //λ��
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);   //����
Free_Camera camera(cameraPos, cameraFront);             //���ǵ��������
//���������ȫ�ֱ���
bool keys[1024];                           //��¼����Щ��������
bool firstMouse = true;                    //����Ƿ��ǵ�һ�ν��봰��
GLfloat posX = WIDTH / 2; 
GLfloat posY = HEIGHT / 2;            //����ʼλ��
GLfloat lastFrame = 0.0, deltaTime = 0.0;  //����ÿ֡��ʱ�䣬����ƽ��֡��

//��Դ��λ��
glm::vec3 light_pos = glm::vec3(5.0, 2.0, 5.0);


//�����ص�
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
//��껬���任��ͷ����
void MouseCallback(GLFWwindow *window, double x_pos, double y_pos);
//���������ž�ͷ
void ScrollCallback(GLFWwindow *window, double x_offset, double y_offset);
//ִ���������
void DoMovement();

//��ʼ������ѡ��
void InitWindowOption();

//��������3D���ּ�����Ķ�������͸���������
void computeGlyphGeometry(Geometry &glyph, float width);

int main()
{
	InitWindowOption();

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Window", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	//��ʼ��GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	glViewport(0, 0, WIDTH, HEIGHT);
	
	//����Ҫ��ʾ�����ּ�����
	std::vector<GLuint> advances;
	std::vector<Geometry> geometries;

	std::wstring text = L"����IPOC";

	for (const auto &c : text)
	{
		//��ȡ������Ϣ
		FreeTypeFont temp_char(c, "../fonts/stxinwei.ttf");
		//���ÿ�����εĲ������
		advances.push_back(temp_char.advanceX());
		//��������Ϣ������Ƶ�Ԫ
		Geometry glyph = temp_char.getGlyph3D();
		//����3D����ģ��
		computeGlyphGeometry(glyph, 3.0);

		geometries.push_back(glyph);
		//���������
		//glyph.output();
	}

	MyShader text_shader("shader/VertShader.vert", "shader/FragShader.frag");

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	text_shader.Use();

	//Game loop
	while (!glfwWindowShouldClose(window))
	{
		//��ȡÿ֡����ʱ������ƽ�ⲻͬ������֡��
		GLfloat current_time = glfwGetTime();
		deltaTime = current_time - lastFrame;
		lastFrame = current_time;

		glfwPollEvents();
		//ִ���������
		DoMovement();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		

		/************************************************����������Ĳ��ʺ͹�Դ����ɫ�͹�Դ������ǿ��************************************************/
		//��ɫ��Դ
		glm::vec3 LightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 diffuse_color = LightColor * glm::vec3(0.5f);
		glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f);
		glm::vec3 specular_color = LightColor * glm::vec3(1.0f);
		//����
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "material.ambient"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "material.diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "material.specular"), 0.5f, 0.5f, 0.5f);
		glUniform1f(glGetUniformLocation(text_shader.shader_program, "material.shininess"), 500.0f);
		//��Դ��ɫ
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "light_color"), 1.0, 1.0, 1.0);
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "light.ambient"), ambient_color.x, ambient_color.y, ambient_color.z);
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "light.diffuse"), diffuse_color.x, diffuse_color.y, diffuse_color.z);
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "light.specular"), specular_color.x, specular_color.y, specular_color.z);
		//����Դλ�ò���������ɫ�����ڼ���������ϵ��
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "light.position"), camera.position.x, camera.position.y, camera.position.z);
		//�������λ�ò���������ɫ�����ڼ��㾵�淴��ϵ��
		glUniform3f(glGetUniformLocation(text_shader.shader_program, "view_pos"), camera.position.x, camera.position.y, camera.position.z);

		int advance = 0;

		for (unsigned int i = 0; i < geometries.size(); advance += advances[i], ++i)
		{
			glm::mat4 model_mat;
			model_mat = glm::translate(model_mat, glm::vec3(-0.9 + advance * 0.05f, -0.6, 0.0));
			model_mat = glm::scale(model_mat, glm::vec3(0.05f, 0.05f, 0.05f));
			glUniformMatrix4fv(glGetUniformLocation(text_shader.shader_program, "model_matrix"), 1, GL_FALSE, glm::value_ptr(model_mat));
			//�ӵ����
			glm::mat4 view_mat;
			view_mat = camera.GetViewMatrix();
			glUniformMatrix4fv(glGetUniformLocation(text_shader.shader_program, "view_matrix"), 1, GL_FALSE, glm::value_ptr(view_mat));
			//ͶӰ����
			glm::mat4 proj_mat;
			proj_mat = glm::perspective(glm::radians(camera.zoom), WIDTH / HEIGHT, 0.1f, 100.0f);
			glUniformMatrix4fv(glGetUniformLocation(text_shader.shader_program, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(proj_mat));

			geometries[i].draw(text_shader);
		}

		//����֡���棨˫���棩
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void InitWindowOption()
{
	//��ʼ��GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = GLFW_TRUE;
		if (action == GLFW_RELEASE)
			keys[key] = GLFW_FALSE;
	}
}
void DoMovement()
{
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
void MouseCallback(GLFWwindow *window, double x_pos, double y_pos)
{
	if (firstMouse)
	{
		posX = x_pos;
		posY = y_pos;
		firstMouse = false;
	}
	GLfloat x_offset = x_pos - posX;
	GLfloat y_offset = posY - y_pos;
	posX = x_pos;
	posY = y_pos;

	camera.ProcessMouseMovement(x_offset, y_offset);
}
void ScrollCallback(GLFWwindow *window, double x_offset, double y_offset)
{
	camera.ProcessMouseScroll(y_offset);
}