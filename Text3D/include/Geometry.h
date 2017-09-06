//ÿһ��Geometry��Ӧһ������
//Geometry����Ψһ�Ķ�������Ͷ����������
//ÿ�����������Ӧ��ͬ��������

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Primitive.h"
#include "TriangleIndexFunctor.h"

class Geometry
{
public:
	//���캯��
	Geometry() = default;
	Geometry(Vec3Array& va, std::vector<ElementArray>& el)
	{
		_vertices = va;
		_indices = el;
	}
	~Geometry() 
	{
		//for (auto &p : _primitive_list)
			//p.deleteBuffers();
	}

	//��ȡ��������
	Vec3Array* getVertexArray() { return &_vertices; }

	//��ȡ�����б�
	std::vector<ElementArray>& getPrimitiveSetList() { return _indices; }

	//��ȡ�����б�Ĵ�С��Ҳ���Ǽ������������ߵ�����
	unsigned int getNumPrimitiveSets() { return _indices.size(); }

	//ɾ��ָ����Χ�ڵ���������
	bool removePrimitiveSet(unsigned int b, unsigned int numToRemove);

	void addNormalArray(const Vec3Array &normals)
	{
		_normals.push_back(normals);
	}

	Vec3Array getNormalArray();

	ElementArray getIndices();

	//�����������
	void addPrimitiveSet(ElementArray* element) { _indices.push_back(*element); }

	//��ӻ���ͼԪ����
	void addMode(GLenum mode) { _mode_list.push_back(mode); }
	//���ͼԪ�б�
	void clearModeList() { _mode_list.clear(); }

	//ʹ��һ���������ɺ�������������������GL_TRIANGLES����������
	void accept(PrimitiveIndexFunctor& functor) const;

	//����
	void draw(MyShader shader);

	//������ݵ��ļ�
	void output();

	void updataPrimitives()
	{
		/*if (_primitive_list.empty())
		{
		for (int i = 0; i < _indices.size(); ++i)
		_primitive_list.push_back(Primitive(GL_TRIANGLES, _vertices, _indices[i], _normals[i]));
		}*/
		p = Primitive(GL_TRIANGLES, _vertices, getIndices(), getNormalArray());
	}

private:
	Vec3Array _vertices;                       //��������
	std::vector<ElementArray> _indices;        //�����б�
	std::vector<Vec3Array> _normals;                        //����������
	//���ͼԪ�б��������洢����gluTessϸ�ֺ����ɵ�����ͼԪ����
	//gluTess��ϸ�ֳ��������������ͼԪ���ͣ��ֱ���GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN
	//ÿ�����Ͷ�Ӧ��������ͬ��������Ҫ��ϸ�ֺ�ʹ�������Լ���GL_TRIANGLES��������������������ͳһ
	std::vector<GLenum> _mode_list;            
	//std::vector<Primitive> _primitive_list;    //����ͼԪ
	Primitive p;
	//����ͼԪ�б��������Ӧ���ڻ���ͼԪ֮ǰ����
	
};

//�����ռ����ɺõ�GL_TRIANGLES����
struct CollectTriangleIndicesFunctor
{
	CollectTriangleIndicesFunctor() {}

	typedef std::vector<unsigned int> Indices;
	Indices _indices;

	void operator() (unsigned int p1, unsigned int p2, unsigned int p3)
	{
		if (p1 == p2 || p2 == p3 || p1 == p3)
		{
			return;
		}

		_indices.push_back(p1);
		_indices.push_back(p3);
		_indices.push_back(p2);

	}
};

#endif