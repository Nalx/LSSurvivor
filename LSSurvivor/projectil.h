#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#define ERROR_PROJECTIL -10000
#define MAX_PROJECTILS 1000

struct vector
{
	float posX;
	float posY;
	float posZ;
};

struct Projectil
{	
public: 
	float			posicioX;
	float			posicioY;
	float			angle;
};

struct ArrayProjectils
{	
public: 
	Projectil DadesProjectil[MAX_PROJECTILS];
	int numProjectils;	
};

void inicialitzaArrayProjectils(ArrayProjectils &llistaProjectils);
bool creaProjectil( ArrayProjectils &llistaProjectils, float x, float y, float angle);
bool eliminaProjectil( ArrayProjectils &llistaProjectils, int numProjectil);
float getXProjectil(ArrayProjectils &llistaProjectils, int projectilConsultar);
float getYProjectil(ArrayProjectils &llistaProjectils, int projectilConsultar);
float getAngleProjectil(ArrayProjectils &llistaProjectils, int projectilConsultar);