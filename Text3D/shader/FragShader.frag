#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 final_color;

uniform vec3 view_pos;  //�����λ��
uniform vec3 light_color;  //��Դ����ɫ

//��Դ������
struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

//����Ĳ���
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

void main()
{   
    //��������еĻ�����
    vec3 ambient = light_color * material.ambient * light.ambient;

    //��������е��������
    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(light.position - FragPos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * (light_color * material.diffuse) * light.diffuse;
    
    //��������еľ��淴���
    vec3 view_dir = normalize(view_pos - FragPos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = spec * (light_color * material.specular) * light.specular;

    vec3 result = ambient + diffuse + specular;
    final_color = vec4(result, 1.0);
}    