#include "../include/MyShader.h"

MyShader::MyShader(const GLchar *vert_path, const GLchar *frag_path)
{
	//������δ������ڽ��ļ��е���ɫ��Դ����ת������
	//�ٽ���ת����C-�ַ�����ΪglShaderSource()�Ĳ���
	std::string vertex_shader_str;     //���ڴ洢�ļ�����string����
	std::string fragment_shader_str;
	std::ifstream vert_file;           //�ļ���
	std::ifstream frag_file;

	//����ɫ��Դ�����ļ�������ļ�������badbit�����׳��쳣
	vert_file.exceptions(std::ifstream::badbit);
	frag_file.exceptions(std::ifstream::badbit);
	try
	{
		vert_file.open(vert_path);
		frag_file.open(frag_path);
		//��ȡ�ļ�����string����
		std::stringstream vert_stream, frag_stream;
		vert_stream << vert_file.rdbuf();
		frag_stream << frag_file.rdbuf();
		vert_file.close();
		frag_file.close();
		vertex_shader_str = vert_stream.str();
		fragment_shader_str = frag_stream.str();
	}
	catch (std::ifstream::failure erro)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	//��string����ת��ΪC-�ַ������͵���ɫ��Դ����
	const GLchar *vert_shader_source = vertex_shader_str.c_str();
	const GLchar *frag_shader_source = fragment_shader_str.c_str();

	//������ɫ������
	GLuint vertex_shader;
	GLint success;
	GLchar info_log[512];
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vert_shader_source, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	GLuint fragment_shader;
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &frag_shader_source, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
	}
	//������ɫ������shader_program����Use()����
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}
	//���ӵ�program֮��Ϳ��԰���ɫ������ɾ�������ͷ��ڴ���
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}