//����һ��Free�������
//Free��������ڿռ��������ƶ�
//��ͷ������Ϊ-90�ȵ�90�ȣ�ƫ����Ϊ0��360�ȣ�û�з�ת����
//����е���ѧ����ʹ��GLM�����

#ifndef FREECAMERA_H_
#define FREECAMERA_H_
//��Ҫʹ�����Ǻ���
#include <cmath>

#include <gl/glew.h>

//opengl��ѧ��
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//��ʾ����ƶ��Ķ���������ʹ��һ���Զ����ö�������β�ͬ����ϵͳ֮�䷽���ʾ�Ĳ���
enum CameraMovement { FORWARD /*ǰ��*/, BACKWARD /*����*/, LEFT /*��ƽ��*/, RIGHT /*��ƽ��*/ };

//����е�Ĭ�ϲ���
const GLfloat YAW = -90.0f;         //ƫ���ǳ�ʼֵ
const GLfloat PITCH = 0.0f;         //�����ǳ�ʼֵ
const GLfloat SPEED = 2.0f;         //���������Ϊ��ƽ��֡��
const GLfloat SENSITIVITY = 0.10f;  //���������
const GLfloat ZOOM = 45.0f;         //��׶���ӳ���

class Free_Camera
{
public:
	//�������
	glm::vec3 position;         //���λ��
	glm::vec3 front;            //�������
	glm::vec3 up;               //������Ϸ���
	glm::vec3 right;            //������ҷ���
	glm::vec3 world_up;         //�����Ϸ���һ����ǣ�0.0, 1.0, 0.0��
	//ŷ����
	GLfloat yaw;        
	GLfloat pitch;
	//��������ò���
	GLfloat move_speed;
	GLfloat mouse_sensitivity;
	GLfloat zoom;

public:
	//���캯��
	Free_Camera(glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 Up = glm::vec3(0.0, 1.0, 0.0))
		: position(Position), front(Front), up(Up), world_up(Up)
	{
		yaw = YAW;
		pitch = PITCH;
		move_speed = SPEED;
		mouse_sensitivity = SENSITIVITY;
		zoom = ZOOM;
		UpdataCameraVectors();
	}

	//�����ӵ���󣬴��ӵ���󽫱�������ɫ����ʹ��
	//��Ҳ������ı���
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	//������̲�����Wǰ����S���ˣ�A��ƽ�ƣ�D��ƽ��
	void ProcessKeyboard(CameraMovement direction, GLfloat delta_time)
	{
		GLfloat velocity = move_speed * delta_time;
		if (direction == FORWARD)
			position += velocity * glm::normalize(glm::vec3(front.x, front.y, front.z));
		if (direction == BACKWARD)
			position -= velocity * glm::normalize(glm::vec3(front.x, front.y, front.z));
		if (direction == LEFT)
			position -= velocity * right;
		if (direction == RIGHT)
			position += velocity * right;
	}

	//������������Ĭ�����Ƹ����ǵĽǶȲ�����-90�Ⱥ�90��
	void ProcessMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch = true)
	{
		//ʹ��������
		x_offset *= mouse_sensitivity;
		y_offset *= mouse_sensitivity;

		pitch += y_offset;
		yaw += x_offset;

		if (constrain_pitch)
		{
			if (pitch > 89.0)
				pitch = 89.0;
			if (pitch < -89.0)
				pitch = -89.0;
		}
		UpdataCameraVectors();
	}

	//������ֲ������������ϻ���
	void ProcessMouseScroll(GLfloat y_offset)
	{
		if (zoom >= 1.0 && zoom <= 45.0)
			zoom -= y_offset;
		if (zoom <= 1.0)
			zoom = 1.0;
		if (zoom >= 45.0)
			zoom = 45.0;
	}

private:
	
	//��������ĳ���
	void UpdataCameraVectors()
	{
		//���ݵ�ǰ��ŷ���Ǽ�������ĳ���
		glm::vec3 current_front;
		current_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		current_front.y = sin(glm::radians(pitch));
		current_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(current_front);

		//�����������ϵ
		right = glm::normalize(glm::cross(front, world_up));
		up = glm::normalize(glm::cross(right, front));
	}

};

#endif