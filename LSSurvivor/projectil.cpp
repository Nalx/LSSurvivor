#include "projectil.h"

/**
* Inicialitza l'estructura de dades que guardar� les dades dels projectils
* @param llistaProjectils �s l'estructura de dades que contindr� els projectils
*/
void inicialitzaArrayProjectils(ArrayProjectils &llistaProjectils)
{
	llistaProjectils.numProjectils = 0;
	
}

/**
* Afegeix a l'estructura de dades un nou projectils
* @param llistaProjectils �s l'estructura de dades que contindr� els projectil
* @param x �s la posici� x del projectil
* @param y �s la posici� y del projectil
* @param angle �s l'angle del projectil
* @return true si l'ha pogut afegir, false en cas contrari
*/
bool creaProjectil(ArrayProjectils &llistaProjectils,float x, float y, float angle)
{
	bool ok=false;
	if (llistaProjectils.numProjectils<MAX_PROJECTILS) {
		llistaProjectils.DadesProjectil[llistaProjectils.numProjectils].posicioX = x;
		llistaProjectils.DadesProjectil[llistaProjectils.numProjectils].posicioY = y;
		llistaProjectils.DadesProjectil[llistaProjectils.numProjectils].angle = angle;
		llistaProjectils.numProjectils++;
		ok=true;
	}
	return ok;
}

/**
* Elimina el projectil especificat de l'estructura de dades
* @param llistaProjectils �s l'estructura de dades que contindr� els projectil
* @param numProjectil �s el projectil a eliminar
* @return true si ha anat b�, false si el projectil no existia
*/
bool eliminaProjectil(ArrayProjectils &llistaProjectils, int numProjectil){
	bool ok=false;
	if (numProjectil<llistaProjectils.numProjectils) {
		llistaProjectils.DadesProjectil[numProjectil].posicioX = llistaProjectils.DadesProjectil[llistaProjectils.numProjectils-1].posicioX;
		llistaProjectils.DadesProjectil[numProjectil].posicioY = llistaProjectils.DadesProjectil[llistaProjectils.numProjectils-1].posicioY;
		llistaProjectils.DadesProjectil[numProjectil].angle = llistaProjectils.DadesProjectil[llistaProjectils.numProjectils-1].angle;
		llistaProjectils.numProjectils--;
		ok=true;
	}
	return ok;
}

/**
* Consulta la posici� x del projectil especificat
* @param llistaProjectils �s l'estructura de dades que contindr� els projectil
* @return la posisici� x del projectil, o ERROR si la posici� no existia
*/
float getXProjectil(ArrayProjectils &llistaProjectils, int projectilConsultar){
	float x=ERROR_PROJECTIL;
	if (projectilConsultar<llistaProjectils.numProjectils) {
		x=llistaProjectils.DadesProjectil[projectilConsultar].posicioX;
	}
	return x;
}

/*
* Consulta la posici� y del projectil especificat
* @param llistaProjectils �s l'estructura de dades que contindr� els projectil
* @return la posisici� y del projectil, o ERROR si la posici� no existia
*/
float getYProjectil(ArrayProjectils &llistaProjectils, int projectilConsultar){
	float y=ERROR_PROJECTIL;
	if (projectilConsultar<llistaProjectils.numProjectils) {
		y=llistaProjectils.DadesProjectil[projectilConsultar].posicioY;
	}
	return y;
}

/**
* Consulta l'angle del projectil especificat
* @param llistaProjectils �s l'estructura de dades que contindr� els projectil
* @return l'angle del projectil, o ERROR si la posici� no existia
*/
float getAngleProjectil(ArrayProjectils &llistaProjectils, int projectilConsultar){
	float angle=ERROR_PROJECTIL;
	if (projectilConsultar<llistaProjectils.numProjectils) {
		angle=llistaProjectils.DadesProjectil[projectilConsultar].angle;
	}
	return angle;
}