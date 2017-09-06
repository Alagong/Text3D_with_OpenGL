//ʹ��FreeType2���������ļ����������ߵĻ��Ƶ���Ϣȡ��

#ifndef FREETYPE_FONT_H_
#define FREETYPE_FONT_H_

#include <iostream>
#include <fstream>
#include <vector>

//glm
#include <glm\glm.hpp>

//FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "Primitive.h"
#include "Geometry.h"

namespace FreeType
{
	//������������outline�ֽ�֮��Ķ����������������
	//�Լ�ÿ��contour��Ӧ�Ļ��Ƶ�Ԫ
	struct Char3DInfo
	{
		//��Ա����
		Vec3Array vertices;                      //��ǰ�����ߵĶ�������
		ElementArray current_indices;            //��ǰ�����ߵ���������
		std::vector<ElementArray> contour_list;  //�������б�ÿ����������Ҫ��Ӧ��ͬ���������飬���ǹ���ͬһ����������
		glm::vec3 previous;                      //��ǰ���Ƶ��ǰһ������
		GLuint num_steps;                        //���������߻��Ʋ�������
		GLfloat coord_scale;                     //��ΪFreeType�в���1/64���ص�λ�������β��������ֵΪ1/64

		//Ĭ�Ϲ��캯��
		Char3DInfo() : num_steps(10), coord_scale(1.0 / 64.0) {}

		//��ɵ�ǰ�����ߵ����ݲɼ�
		void completeCurrentGeom()
		{
			if (!vertices.empty() && !current_indices.empty())
				contour_list.push_back(current_indices);
			current_indices.clear();
		}
		//��ȡ������ɵ����μ����壬ÿһ�����ζ�Ӧһ��������
		Geometry get()
		{
			completeCurrentGeom();
			return Geometry(vertices, contour_list);
		}

		//��ǰ���������������������Ӷ��㼰������
		void addVertex(glm::vec3 pos)
		{
			previous = pos;
			pos *= coord_scale;
			//�������������ͬ�Ķ���
			if (!vertices.empty() && vertices.back() == pos)
				return;
			//����һ��contour�з��������ظ��㣬�����ظ����
			if (!current_indices.empty() && vertices[current_indices[0]] == pos)
				current_indices.push_back(current_indices[0]);
			else
			{
				current_indices.push_back(vertices.size());
				vertices.push_back(pos);
			}

		}
		//��ʼ�����µ�������
		void moveTo(const glm::vec2 &pos)
		{
			completeCurrentGeom();
			addVertex(glm::vec3(pos.x, pos.y, 0));
		}
		//����ֱ�߶�
		void lineTo(const glm::vec2 &pos)
		{
			addVertex(glm::vec3(pos.x, pos.y, 0));
		}
		//���ƶ��α���������
		void conicTo(const glm::vec2 &control, const glm::vec2 &pos)
		{
			glm::vec3 p0 = previous;
			glm::vec3 p1 = glm::vec3(control.x, control.y, 0);
			glm::vec3 p2 = glm::vec3(pos.x, pos.y, 0);

			GLfloat dt = 1.0 / num_steps;
			GLfloat u = 0;
			for (unsigned int i = 0; i <= num_steps; ++i)
			{
				GLfloat w = 1;   //w����ֻ��һ����������
				GLfloat bs = 1.0 / ((1 - u)*(1 - u) + 2 * (1 - u)*u*w + u*u);
				//ע�⣺����ֱ��copy��OSG����Ĺ�ʽ��bsò��ֻ��һ������������ԭ���������߷�����û�����bs��B-Spline����
				//���ҵ�w��ֵΪ1ʱ��bs��Զ����1
				glm::vec3 p = (p0*((1 - u)*(1 - u)) + p1*(2 * (1 - u)*u*w) + p2*(u*u)) * bs;
				addVertex(p);

				u += dt;
			}
		}
		//�������α���������
		void cubicTo(const glm::vec2 &control1, const glm::vec2 &control2, const glm::vec2 &pos)
		{
			glm::vec3 p0 = previous;
			glm::vec3 p1 = glm::vec3(control1.x, control1.y, 0);
			glm::vec3 p2 = glm::vec3(control2.x, control2.y, 0);
			glm::vec3 p3 = glm::vec3(pos.x, pos.y, 0);

			GLfloat cx = 3 * (p1.x - p0.x);
			GLfloat bx = 3 * (p2.x - p1.x) - cx;
			GLfloat ax = p3.x - p0.x - cx - bx;
			GLfloat cy = 3 * (p1.y - p0.y);
			GLfloat by = 3 * (p2.y - p1.y) - cy;
			GLfloat ay = p3.y - p0.y - cy - by;

			GLfloat dt = 1.0 / num_steps;
			GLfloat u = 0;
			for (unsigned int i = 0; i <= num_steps; ++i)
			{
				glm::vec3 p = glm::vec3(ax*u*u*u + bx*u*u + cx*u + p0.x, ay*u*u*u + by*u*u + cy*u + p0.y, 0);
				addVertex(p);

				u += dt;
			}
		}
	};  //Char3DInfo�������

	//���ĸ���������FT_Outline_Decompose�е�FT_Funcs����
	//���������ص�Char3DInfo������ִ�������ֽ���ĸ�ͬ������
	//����ֵ0������ɹ�
	int moveTo(const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->moveTo(glm::vec2(to->x, to->y));
		return 0;
	}
	int lineTo(const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->lineTo(glm::vec2(to->x, to->y));
		return 0;
	}
	int conicTo(const FT_Vector* control, const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->conicTo(glm::vec2(control->x, control->y), glm::vec2(to->x, to->y));
		return 0;
	}
	int cubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->cubicTo(
			glm::vec2(control1->x, control1->y),
			glm::vec2(control2->x, control2->y),
			glm::vec2(to->x, to->y));
		return 0;
	}
}   //�����ռ�FreeType�ر�



class FreeTypeFont
{
public:
	FreeTypeFont(wchar_t code, GLchar *font_file);
	~FreeTypeFont()
	{
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
	Geometry getGlyph3D();
	GLuint advanceX() const { return AdvanceX; }

private:
	wchar_t charcode;    //�ַ�����
	FT_Library ft;
	FT_Face face;
	GLuint AdvanceX;
	FreeType::Char3DInfo char3d;
};

FreeTypeFont::FreeTypeFont(wchar_t code, char *font_file)
{
	//��ʼ��FreeType
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	//�½�һ��face
	if (FT_New_Face(ft, font_file, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	//�������ִ�С
	FT_Set_Pixel_Sizes(face, 0, 24);

	charcode = code;
	//���������Ի�ȡ������ȣ��������������޷���ȡ������ȵģ�
	FT_Load_Char(face, charcode, FT_LOAD_RENDER);
	AdvanceX = (face->glyph->advance.x) >> 6;
}

Geometry FreeTypeFont::getGlyph3D()
{
	//��������
	FT_Error error = FT_Load_Char(face, charcode, FT_LOAD_DEFAULT);
	if (error)
		std::cout << "FT_Load_Char(...) error 0x" << std::hex << error << std::dec << std::endl;
	if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
		std::cout << "FreeTypeFont3D::getGlyph : not a vector font" << std::endl;

	FT_Outline outline = face->glyph->outline;
	FT_Outline_Funcs funcs;
	funcs.conic_to = (FT_Outline_ConicToFunc)&FreeType::conicTo;
	funcs.line_to  = (FT_Outline_LineToFunc )&FreeType::lineTo;
	funcs.cubic_to = (FT_Outline_CubicToFunc)&FreeType::cubicTo;
	funcs.move_to  = (FT_Outline_MoveToFunc )&FreeType::moveTo;
	funcs.shift = 0;
	funcs.delta = 0;

	//�ֽ����������ߣ�ע��ص�����
	FT_Error _error = FT_Outline_Decompose(&outline, &funcs, &char3d);
	if (_error)
		std::cout << "FreeTypeFont3D::getGlyph : - outline decompose failed ..." << std::endl;
	
	//�������μ�����
	return char3d.get();
}

#endif