#pragma once
#include <windows.h>
#include <math.h>		
#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "GL/glew.h"
#include "qdebug.h"
#include <lpng/png.h>

#define M_PI 3.14159265358979323846
#define TYPE_PLY_RB 0
#define TYPE_PLY_RT 1
#define TYPE_TXT 2

class PlyIO
{
public:
	PlyIO(char* fileName);
	~PlyIO();
	struct Point3D//������
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat  r;
		GLfloat  g;
		GLfloat  b;
		GLfloat  alpha;
	};
	struct Face3D
	{
		int v1;
		int v2;
		int v3;
		GLfloat u[3];
		GLfloat v[3];
		GLfloat normal[3];
	};
	struct  Line3D//������(�����˵�)
	{
		Point3D pointone;
		Point3D pointtwo;
	};
	struct  Triangle3D//����������(�����˵�)
	{
		Point3D pointone;
		Point3D pointtwo;
		Point3D pointthree;
		GLfloat normal[3]; //��ķ�����
	};
	char* fileName;
	bool available;
	Point3D * vertex;
	Face3D * faces;
	std::vector<Line3D> Lines;
	std::vector<Triangle3D> Triangles;

	GLfloat x_max, x_min, y_max, y_min, z_max, z_min;//�洢���ݵ����¡����ϡ����¡���������

	int vertex_N, face_N;   //����������

	std::string textureFileName;
	std::string workDir;
	GLuint textureID;
	int width, height; //��¼ͼƬ����͸�
	GLubyte* rgba;
	png_byte color_type; //ͼƬ�����ͣ����ܻ������Ƿ��ǿ�����ͨ����
	bool open();
	Point3D CalTexture(GLfloat x, GLfloat y) {
		Point3D Caltexture;
		Caltexture.x = (2 - x) / (2 - 1);
		Caltexture.y = (2 - y) / (2 - 1);
		return Caltexture;
	}
	void calculateNormal(Face3D &face);
	GLvoid render();
	void initPng();
	GLuint CreateTextureFromPng();
	void CreateTextureFromBmp();
private:
	std::vector<int> vertex_types, face_types;
	int type;
	FILE *file;
	int getPlyFileType();
	std::string vectostring(std::vector<char> vec);
	std::string readValueRB();
	int getTypeBytesLength(std::string type);
	bool readHeaderRB();


};

