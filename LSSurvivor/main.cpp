/**
@ Noms: Alex Cordón Vila (ls25707) i Marçal Gomez Lavall (27345)
@ Dia: 9-2-2014
**/


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "GL\glut.h"
#include "windows.h"
#include "Mmsystem.h"
//#include "glut.h"
#include "escenari.h"
#include "parserAse.h"
#include "math.h"
#include "projectil.h"
#include "TexturesDevil.h"
//Permet definir el temps que tarda un enemic en reapareixer a un dispensador
#define TEMPSAPARICIO 3
#define MAXESCENARIS 5
//Mida de la finestra grafica
#define WIDTH 600
#define HEIGHT 600
//Variables globals
//Redefinicio del tipus
Escenari escenari[6];
Pantalla pantalla[6];
bool DEBUG = false;
int numEnemics = 0;
//Variable de temps 
int movement_timer = 0;
//Posició del punter del mouse
float posMouseX;
float posMouseY;
//POSICIO JUGADOR
float posX;
float posY;
float posEnemX;
float posEnemY;
char tecles[256];
float temps = 0;
//Variables per el calcul del Rellotge
int currentTime = 0;
int timeAcum = 0;
//Flag que protegeix la inicialitzacio de la posició del enemics (a actualitzaEnemics)
int flag = 0;
//Variables de rotació per al mostraModel
float rotJugador;
float rotEnemic;
//Es posa a 1 si estas mort
int flagDead = 0;
float rotCamera;

//Compte enrere d'enemics 
int numEnemicsActius = 0;
//Comptador d'enemics morts
int killedEnemyCounter = 0;
//Comptador de bales
int ShotCount = 0;
int puntsJugador = 0;
//Hi guardem el numero d'enemics que surten despres dels inicials 
int numRespawns=0;
//Comptador d'aranyes mortes
int VidesJugador = 3;
int pantallaactual = 0;
//flag per saber si ha arribat al final del joc
int winner = 0;
//Declaracio dels models 3D
File modelPlayer;
File modelEnemy;
File modelColumn;
File modelParet1;
File modelParet2;
File modelParet3;
File modelParet4;
File modelProjectil;
//Array d'enemics
ArrayProjectils llistaProjectils;
//Modes de llum
int lightMode = 2;
//Modes de camera
int camMode = 1;
//Angle per la rotacio en mode primera i segona persona
int angle = 0;
//Textures per als models 3D del joc
GLuint texturaJugador;
GLuint texturaEnemic;
GLuint texturaTerra;
GLuint texturaParet;
static GLint fogMode;
bool fogEnable = false;
void RenderString(float x, float y, char* string){
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-10, 10, -10, 10, -100, 100);
	glDisable(GL_LIGHTING);

	glColor3f(0, 1, 0);
	//Definim la posicio del raster
	int length;
	length = (int)strlen(string);
	glRasterPos2f(x, y);
	for (int i = 0; i < length; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
	if (lightMode != 0){
		glEnable(GL_LIGHTING);
	}
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void incicialitzacions(){
	float initX = -8.4;
	float initY = -7.4;
	if (VidesJugador == 0){
		VidesJugador = 3;
	}
	//Inicialitzacio de posicións sobre escenari
	posX = initX + pantalla[pantallaactual].lsSurvivor.posicioX + 0.5;
	posY = initY + pantalla[pantallaactual].lsSurvivor.posicioY - 0.5;
	//Inicialització del model 3D del jugador
	pantalla[pantallaactual].lsSurvivor.actiu = true;
	//Inicialització de reaparició d'enemics
	numEnemicsActius = pantalla[pantallaactual].numEnemics - pantalla[pantallaactual].numDispensadors;
	//numRespawns = pantalla[pantallaactual].numDispensadors - 1;
	numRespawns = pantalla[pantallaactual].numDispensadors;

	killedEnemyCounter = 0;
	angle = 90;
}
void CalculaBoundingBox(File *model){
	model->boundingBox.max.x = -100000;
	model->boundingBox.max.y = -100000;
	model->boundingBox.max.z = -100000;
	model->boundingBox.min.x = 100000;
	model->boundingBox.min.y = 100000;
	model->boundingBox.min.z = 100000;
	int numMeshes = getNumMeshes(*model);
	for (int m = 0; m<numMeshes; m++){
		int numVertex = getNumVertex(*model, m);
		for (int v = 0; v<numVertex; v++){
			Point3D *vertex = model->s_MeshList.s_vMesh[m]->s_vVertex[v];
			//Operador ternari
			int x = 10 > 5 ? 1 : 0;
			//L'operador ternari equival a:
			/*
			if(10>5){
			x=1;
			}else{
			x=0;
			}
			*/
			//MAXIMS
			//Per x:
			if (vertex->s_fXCoordinate > model->boundingBox.max.x){
				model->boundingBox.max.x = vertex->s_fXCoordinate;
			}
			//Per y:
			if (vertex->s_fYCoordinate > model->boundingBox.max.y){
				model->boundingBox.max.y = vertex->s_fYCoordinate;
			}
			if (vertex->s_fZCoordinate > model->boundingBox.max.z){
				model->boundingBox.max.z = vertex->s_fZCoordinate;
			}
			//Falta per z

			//MINIMS:
			if (vertex->s_fXCoordinate < model->boundingBox.min.x){
				model->boundingBox.min.x = vertex->s_fXCoordinate;
			}
			if (vertex->s_fYCoordinate < model->boundingBox.min.y){
				model->boundingBox.min.y = vertex->s_fYCoordinate;
			}
			if (vertex->s_fZCoordinate < model->boundingBox.min.z){
				model->boundingBox.min.z = vertex->s_fZCoordinate;
			}

		}
	}
}

void CalculaNormalsCara(File *modelPlayer){

	for (int i = 0; i<getNumMeshes(*modelPlayer); i++){
		modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNFace.clear();

		for (int j = 0; j<getNumFaces(*modelPlayer, i); j++){
			Point3D v1 = getVertex(*modelPlayer, i, j, 1);
			Point3D v2 = getVertex(*modelPlayer, i, j, 2);
			Point3D v3 = getVertex(*modelPlayer, i, j, 3);
			//Falta incloure "z" i calcul de normals

			Point3D v1v2;
			v1v2.s_fXCoordinate = v1.s_fXCoordinate - v2.s_fXCoordinate;
			v1v2.s_fYCoordinate = v1.s_fYCoordinate - v2.s_fYCoordinate;
			v1v2.s_fZCoordinate = v1.s_fZCoordinate - v2.s_fZCoordinate;
			Point3D v1v3;
			v1v3.s_fXCoordinate = v1.s_fXCoordinate - v3.s_fXCoordinate;
			v1v3.s_fYCoordinate = v1.s_fYCoordinate - v3.s_fYCoordinate;
			v1v3.s_fZCoordinate = v1.s_fZCoordinate - v3.s_fZCoordinate;


			Point3D normal;
			//Calcul producte vectorial v1v2 * v3

			normal.s_fXCoordinate = (v1v2.s_fYCoordinate * v1v3.s_fZCoordinate) - (v1v2.s_fZCoordinate*v1v3.s_fYCoordinate);
			normal.s_fYCoordinate = (v1v2.s_fZCoordinate * v1v3.s_fXCoordinate) - (v1v2.s_fXCoordinate*v1v3.s_fZCoordinate);
			normal.s_fZCoordinate = (v1v2.s_fXCoordinate * v1v3.s_fYCoordinate) - (v1v2.s_fYCoordinate*v1v3.s_fXCoordinate);


			float modul;
			//On hi ha X Y i Z hi hem de posar respectivament "modelPlayer.s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fXCoordinate"
			float X = normal.s_fXCoordinate;
			float Y = normal.s_fYCoordinate;
			float Z = normal.s_fZCoordinate;

			modul = sqrt(X*X + Y*Y + Z*Z);

			normal.s_fXCoordinate /= modul;
			normal.s_fYCoordinate /= modul;
			normal.s_fZCoordinate /= modul;

			addNormal(*modelPlayer, i, normal.s_fXCoordinate, normal.s_fYCoordinate, normal.s_fZCoordinate);

		}
	}
}

void CalculaNormalVertex(File *modelPlayer){
	for (int i = 0; i< getNumMeshes(*modelPlayer); i++){
		modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.clear();

		int numVertex = getNumVertex(*modelPlayer, i);

		for (int k = 0; k < numVertex; k++){
			Point3D *normalVertex = (Point3D*)malloc(sizeof(Point3D));
			normalVertex->s_fXCoordinate = 0;
			normalVertex->s_fYCoordinate = 0;
			normalVertex->s_fZCoordinate = 0;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.push_back(normalVertex);
		}


		for (int j = 0; j < getNumFaces(*modelPlayer, i); j++){
			Point3D normal;
			normal = getNormal(*modelPlayer, i, j);
			//modelPlayer.s_MeshList.s_vMesh.at(i)->s_vNFace.at(j);
			//La fila anterior es equivalent a: normal = getNormal(modelPlayer,i,j);

			int v1 = modelPlayer->s_MeshList.s_vMesh.at(i)->s_vFace.at(j)->s_uiVertexA;
			//la posicio de les normals els vertex a l'array de normals equival a la posicio del vertex a l'array de vertex
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v1)->s_fXCoordinate += normal.s_fXCoordinate;;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v1)->s_fYCoordinate += normal.s_fYCoordinate;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v1)->s_fZCoordinate += normal.s_fZCoordinate;

			int v2 = modelPlayer->s_MeshList.s_vMesh.at(i)->s_vFace.at(j)->s_uiVertexA;
			//la posicio de les normals els vertex a l'array de normals equival a la posicio del vertex a l'array de vertex
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v2)->s_fXCoordinate += normal.s_fXCoordinate;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v2)->s_fYCoordinate += normal.s_fYCoordinate;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v2)->s_fZCoordinate += normal.s_fZCoordinate;

			int v3 = modelPlayer->s_MeshList.s_vMesh.at(i)->s_vFace.at(j)->s_uiVertexC;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v3)->s_fXCoordinate += normal.s_fXCoordinate;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v3)->s_fYCoordinate += normal.s_fYCoordinate;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(v3)->s_fZCoordinate += normal.s_fZCoordinate;

		}
		for (int k = 0; k<numVertex; k++){
			Point3D normalVertex;
			float modul;
			//On hi ha X Y i Z hi hem de posar respectivament "modelPlayer.s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fXCoordinate"
			float X = modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fXCoordinate;
			float Y = modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fYCoordinate;
			float Z = modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fZCoordinate;

			modul = sqrt(X*X + Y*Y + Z*Z);

			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fXCoordinate /= modul;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fYCoordinate /= modul;
			modelPlayer->s_MeshList.s_vMesh.at(i)->s_vNVertex.at(k)->s_fZCoordinate /= modul;

		}
	}
}

void carregaModels(){
	LoadFile("resources/models/LSSurvivor.ASE", &modelPlayer);
	CalculaBoundingBox(&modelPlayer);
	LoadFile("resources/models/spiderBot.ASE", &modelEnemy);
	CalculaBoundingBox(&modelEnemy);
	//Aquest model no s'ha utilitzat
	LoadFile("resources/models/column.ASE", &modelColumn);
	CalculaBoundingBox(&modelColumn);
	LoadFile("resources/models/paret1.ASE", &modelParet1);
	CalculaBoundingBox(&modelParet1);
	LoadFile("resources/models/paret2.ASE", &modelParet2);
	CalculaBoundingBox(&modelParet2);
	//No s'ha utilitzat aquest model
	LoadFile("resources/models/paret3.ASE", &modelParet3);
	CalculaBoundingBox(&modelParet3);
	LoadFile("resources/models/paret4.ASE", &modelParet4);
	CalculaBoundingBox(&modelParet4);
	LoadFile("resources/models/projectil.ASE", &modelProjectil);
	CalculaBoundingBox(&modelProjectil);
	CalculaNormalsCara(&modelPlayer);
	CalculaNormalVertex(&modelPlayer);

	CalculaNormalsCara(&modelEnemy);
	CalculaNormalVertex(&modelEnemy);

	CalculaNormalsCara(&modelColumn);
	CalculaNormalVertex(&modelColumn);

	CalculaNormalsCara(&modelParet1);
	CalculaNormalVertex(&modelParet1);

	CalculaNormalsCara(&modelParet2);
	CalculaNormalVertex(&modelParet2);

	CalculaNormalsCara(&modelParet3);
	CalculaNormalVertex(&modelParet3);

	CalculaNormalsCara(&modelParet4);
	CalculaNormalVertex(&modelParet4);

	CalculaNormalsCara(&modelProjectil);
	CalculaNormalVertex(&modelProjectil);




	carregaTextura(&texturaJugador, "resources/Models2/LSSurvivor.png");
	carregaTextura(&texturaTerra, "resources/Models2/ground3.png");
	carregaTextura(&texturaEnemic, "resources/Models2/spiderBot.png");

}
void PintaBoundingBox(BB box){
	glPushMatrix();

	glBegin(GL_LINES);
	glVertex3f(box.max.x, box.max.y, box.max.z);
	glVertex3f(box.max.x, box.min.y, box.max.z);

	glVertex3f(box.max.x, box.min.y, box.max.z);
	glVertex3f(box.min.x, box.min.y, box.max.z);


	glVertex3f(box.min.x, box.min.y, box.max.z);
	glVertex3f(box.min.x, box.max.y, box.max.z);


	glVertex3f(box.min.x, box.max.y, box.max.z);
	glVertex3f(box.max.x, box.max.y, box.max.z);

	glEnd();

}

void PintaModel(File model, int idTextura){
	if (DEBUG){
		PintaBoundingBox(model.boundingBox);
	}

	if (idTextura != -1){
		glEnable(GL_TEXTURE_2D);
	}
	if (idTextura == texturaJugador){
		glBindTexture(GL_TEXTURE_2D, texturaJugador);
	}
	if (idTextura == texturaEnemic){
		glBindTexture(GL_TEXTURE_2D, texturaEnemic);
	}
	if (idTextura == texturaParet){
		glBindTexture(GL_TEXTURE_2D, texturaParet);
	}

	glBegin(GL_TRIANGLES);

	int numMalles = getNumMeshes(model);
	for (int m = 0; m<numMalles; m++){
		if (lightMode == 0 || lightMode == 1 || lightMode == 2){
			Color color = getColor(model, m);
			glColor3f(1, 1, 1);
		}
		else{
			Color difus = getDifuse(model, m);
			Color ambient = getAmbient(model, m);

			float mat_difus[4];
			mat_difus[0] = difus.s_fRed;
			mat_difus[1] = difus.s_fGreen;
			mat_difus[2] = difus.s_fBlue;
			mat_difus[3] = 0;

			float mat_ambient[4];
			mat_ambient[0] = ambient.s_fRed;
			mat_ambient[1] = ambient.s_fGreen;
			mat_ambient[2] = ambient.s_fBlue;
			mat_ambient[3] = 0;

			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_difus);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
		}

		int numCares = getNumFaces(model, m);
		for (int c = 0; c < numCares; c++){
			Point3D v1 = getVertex(model, m, c, 1);
			Point3D v2 = getVertex(model, m, c, 2);
			Point3D v3 = getVertex(model, m, c, 3);


			Point3D t1;
			Point3D t2;
			Point3D t3;

			Point3D n3;
			Point3D n2;
			Point3D n1;
			if (idTextura != -1){

				int indexA = model.s_MeshList.s_vMesh[m]->s_vTFace[c]->s_uiVertexA;
				t1.s_fXCoordinate = model.s_MeshList.s_vMesh[m]->s_vTVertex[indexA]->s_fXCoordinate;
				t1.s_fYCoordinate = model.s_MeshList.s_vMesh[m]->s_vTVertex[indexA]->s_fYCoordinate;

				int indexB = model.s_MeshList.s_vMesh[m]->s_vTFace[c]->s_uiVertexB;
				t2.s_fXCoordinate = model.s_MeshList.s_vMesh[m]->s_vTVertex[indexB]->s_fXCoordinate;
				t2.s_fYCoordinate = model.s_MeshList.s_vMesh[m]->s_vTVertex[indexB]->s_fYCoordinate;

				int indexC = model.s_MeshList.s_vMesh[m]->s_vTFace[c]->s_uiVertexC;
				t3.s_fXCoordinate = model.s_MeshList.s_vMesh[m]->s_vTVertex[indexC]->s_fXCoordinate;
				t3.s_fYCoordinate = model.s_MeshList.s_vMesh[m]->s_vTVertex[indexC]->s_fYCoordinate;
			}

			if (lightMode == 1){
				Point3D normal = getNormal(model, m, c);
				glNormal3f(normal.s_fXCoordinate, normal.s_fYCoordinate, normal.s_fZCoordinate);
			}

			if (lightMode == 2){
				n1 = getVertexNormal(model, m, c, 1);
				n2 = getVertexNormal(model, m, c, 2);
				n3 = getVertexNormal(model, m, c, 3);

			}

			if (lightMode == 2) glNormal3f(n1.s_fXCoordinate, n1.s_fYCoordinate, n1.s_fZCoordinate);
			if (idTextura != -1) glTexCoord2f(t1.s_fXCoordinate, 1 - t1.s_fYCoordinate);
			glVertex3f(v1.s_fXCoordinate, v1.s_fYCoordinate, v1.s_fZCoordinate);

			if (lightMode == 2) glNormal3f(n2.s_fXCoordinate, n2.s_fYCoordinate, n2.s_fZCoordinate);
			if (idTextura != -1) glTexCoord2f(t2.s_fXCoordinate, 1 - t2.s_fYCoordinate);
			glVertex3f(v2.s_fXCoordinate, v2.s_fYCoordinate, v2.s_fZCoordinate);

			if (lightMode == 2) glNormal3f(n3.s_fXCoordinate, n3.s_fYCoordinate, n3.s_fZCoordinate);
			if (idTextura != -1) glTexCoord2f(t3.s_fXCoordinate, 1 - t3.s_fYCoordinate);
			glVertex3f(v3.s_fXCoordinate, v3.s_fYCoordinate, v3.s_fZCoordinate);
		}
	}

	glEnd();
	glDisable(GL_TEXTURE_2D);
	if (idTextura != -1){
		glDisable(GL_TEXTURE_2D);
	}

}

void pintaQuadrat(float xPOS, float yPOS, bool pintaQuadrat, float rQuadrat, float gQuadrat, float bQuadrat, float mida, bool pintaLinia, float rLinia, float gLinia, float bLinia, float alfaQuadrat, int idTextura){
	if (idTextura != -1){
		glEnable(GL_TEXTURE_2D);

	}
	if (idTextura == texturaTerra){
		glBindTexture(GL_TEXTURE_2D, texturaTerra);
	}
	if (lightMode == 0 || lightMode == 1 || lightMode == 2){

		glNormal3f(0, 0, 1);
	}
	if (lightMode == 0 || lightMode == 1 || lightMode == 2){
		switch (pintaQuadrat){
		case true:

			glColor4f(rQuadrat, gQuadrat, bQuadrat, alfaQuadrat);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//glBindTexture(GL_TEXTURE_2D, texturaTerra);
			glBegin(GL_QUADS);

			glTexCoord2f(0, 1);
			glVertex2f(xPOS, yPOS);
			glTexCoord2f(0, 0);
			glVertex2f(xPOS, yPOS - mida);
			glTexCoord2f(1, 0);
			glVertex2f(xPOS + mida, yPOS - mida);
			glTexCoord2f(1, 1);
			glVertex2f(xPOS + mida, yPOS);
			glEnd();

			glDisable(GL_TEXTURE_2D);

			break;
		}

		glDisable(GL_TEXTURE_2D);
		if (idTextura != -1){
			glDisable(GL_TEXTURE_2D);
		}
	}

	//Pinta linia de perimetre
	switch (pintaLinia){
	case true:
		glColor3f(rLinia, gLinia, bLinia);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_LINES);
		glVertex3f(xPOS, yPOS, 0.01);
		glVertex3f(xPOS, yPOS - mida, 0.01);
		glVertex3f(xPOS, yPOS - mida, 0.01);
		glVertex3f(xPOS + mida, yPOS - mida, 0.01);
		glVertex3f(xPOS + mida, yPOS - mida, 0.01);
		glVertex3f(xPOS + mida, yPOS, 0.01);
		glVertex3f(xPOS + mida, yPOS, 0.01);
		glVertex3f(xPOS, yPOS, 0.01);
		glEnd();
		break;
	}
}
//Es dibuixen tots els models per a cada cas
void MostraModels(Pantalla){
	int c = 0, d = 0, i = 0;
	int count = 0;
	float initX = -8.4;
	float initY = -7.4;

	if (camMode == 3){
		//Mostra Jugador
		if (pantalla[pantallaactual].lsSurvivor.actiu == true){
			glPushMatrix();
			//glColor3f(0,0,1);
			glTranslatef(posX, posY, 0);
			glRotatef(rotJugador, 0, 0, 1);
			glScalef(1, 1, 1);
			PintaModel(modelPlayer, texturaJugador);
			glPopMatrix();
		}

	}
	//Mostra enemics
	for (int i = 0; i < pantalla[pantallaactual].numEnemics; i++){
		if (pantalla[pantallaactual].enemics[i].actiu){
			glPushMatrix();
			glTranslatef(pantalla[pantallaactual].enemics[i].posicioX, pantalla[pantallaactual].enemics[i].posicioY, 0);
			glRotatef(rotEnemic, 0, 0, 1);
			//glColor3f(1,0,0);
			PintaModel(modelEnemy, texturaEnemic);
			glPopMatrix();
		}
	}

	//Pinta Projectil

	for (int i = 0; i<llistaProjectils.numProjectils; i++){
		glPushMatrix();
		glColor3f(0, 1, 0);
		glTranslatef(getXProjectil(llistaProjectils, i), getYProjectil(llistaProjectils, i), 0.4);
		glScalef(0.5, 0.5, 0.5);
		PintaModel(modelProjectil, -1);
		glPopMatrix();
	}
	//Pinta parets laterals
	for (int j = pantalla[pantallaactual].pantalla.midaY; j >= 0; j--){
		for (int i = pantalla[pantallaactual].pantalla.midaX; i >= 0; i--){
			if (pantalla[pantallaactual].pantalla.matriuPantalla[i][j] == 4){
				//Paret
				if (count == 0){
					glPushMatrix();
					//glColor3f(0,0,1);
					glTranslatef(initX + i + 0.5, initY + j - 0.5, 0);
					PintaModel(modelParet2, -1);
					glPopMatrix();
				}
				if (count == 16){
					glPushMatrix();
					//glColor3f(0,0,1);
					glTranslatef(initX + i + 0.5, initY + j - 0.5, 0);
					glRotatef(-270, 0, 0, 1);
					PintaModel(modelParet2, -1);
					glPopMatrix();
				}
				if (count == 47){
					glPushMatrix();
					//glColor3f(0,0,1);
					glTranslatef(initX + i + 0.5, initY + j - 0.5, 0);
					glRotatef(-90, 0, 0, 1);
					PintaModel(modelParet2, -1);
					glPopMatrix();
				}
				if (count == 63){
					glPushMatrix();
					//glColor3f(0,0,1);
					glTranslatef(initX + i + 0.5, initY + j - 0.5, 0);
					glRotatef(-180, 0, 0, 1);
					PintaModel(modelParet2, -1);
					glPopMatrix();
				}
				if (count != 0 && count != 16 && count != 47 && count != 63){
					if ((count>0 && count<16) || (count>47 && count<63)){
						glPushMatrix();
						//	glColor3f(0,0,1);
						glTranslatef(initX + i + 0.5, initY + j - 0.5, 0);
						glRotatef(90, 0, 0, 1);
						//glScalef(-5,1,-3);
						PintaModel(modelParet1, -1);
						glPopMatrix();
					}
					else{
						glPushMatrix();
						//	glColor3f(0,0,1);
						glTranslatef(initX + i + 0.5, initY + j - 0.5, 0);
						glRotatef(0, 0, 0, 1);
						//glScalef(-5,1,-3);
						PintaModel(modelParet1, -1);
						glPopMatrix();
					}
				}
				count++;
			}
		}


	}

	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_COLOR_MATERIAL);
	GLfloat redDiffuseMaterial[] = { 1, 1, 0.5, 0.5 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, redDiffuseMaterial);



	//Pintar OBJECTES que volem que siguin TRANSPARENTS
	//Mostrem els models de les columnes 
	for (int d = 0; d<pantalla[pantallaactual].numColumnes; d++){
		if (pantalla[pantallaactual].columnes[d].tipusColumna == 1){
			glPushMatrix();
			//glColor3f(0,1,0);
			glTranslatef(initX + pantalla[pantallaactual].columnes[d].posicioX + 0.5, initY + pantalla[pantallaactual].columnes[d].posicioY - 0.5, 0);
			glRotatef(pantalla[pantallaactual].columnes[d].posicioRotacio, 0, 0, 1);
			PintaModel(modelParet1, -1);
			glPopMatrix();

		}
		if (pantalla[pantallaactual].columnes[d].tipusColumna == 2){
			glPushMatrix();
			//glColor3f(0.5,0,1);
			glTranslatef(initX + pantalla[pantallaactual].columnes[d].posicioX + 0.5, initY + pantalla[pantallaactual].columnes[d].posicioY - 0.5, 0);
			glRotatef(pantalla[pantallaactual].columnes[d].posicioRotacio, 0, 0, 1);
			PintaModel(modelParet2, -1);
			glPopMatrix();
		}
		if (pantalla[pantallaactual].columnes[d].tipusColumna == 3){
			glPushMatrix();
			//glColor3f(0.5,1,1);
			glTranslatef(initX + pantalla[pantallaactual].columnes[d].posicioX + 0.5, initY + pantalla[pantallaactual].columnes[d].posicioY - 0.5, 0);
			glRotatef(pantalla[pantallaactual].columnes[d].posicioRotacio, 0, 0, 1);
			PintaModel(modelParet3, -1);
			glPopMatrix();
		}
		if (pantalla[pantallaactual].columnes[d].tipusColumna == 4){
			glPushMatrix();
			//glColor3f(1,0,0);
			glTranslatef(initX + pantalla[pantallaactual].columnes[d].posicioX + 0.5, initY + pantalla[pantallaactual].columnes[d].posicioY - 0.5, 0);
			glRotatef(pantalla[pantallaactual].columnes[d].posicioRotacio, 0, 0, 1);
			PintaModel(modelParet4, -1);
			glPopMatrix();
		}
	}

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);







	if (camMode == 1 || camMode == 2){
		//Mostra Jugador
		if (pantalla[pantallaactual].lsSurvivor.actiu == true){
			glPushMatrix();
			//glColor3f(0,0,1);
			glTranslatef(posX, posY, 0);
			glRotatef(rotJugador, 0, 0, 1);
			glScalef(1, 1, 1);
			PintaModel(modelPlayer, texturaJugador);
			glPopMatrix();
		}

	}

}
void actualitzaProjectils(){
	for (int i = 0; i<llistaProjectils.numProjectils; i++){

		float dirX = 0.0;
		float dirY = 0.0;

		dirX = cos(getAngleProjectil(llistaProjectils, i)*3.1416 / 180);
		dirY = sin(getAngleProjectil(llistaProjectils, i)*3.1416 / 180);

		float factorVelocitat = 0.2;
		dirX *= factorVelocitat;
		dirY *= factorVelocitat;
		llistaProjectils.DadesProjectil[i].posicioX += dirX;
		llistaProjectils.DadesProjectil[i].posicioY += dirY;

	}

}
void PintaTerra(){

	float initX = -8.4;
	float initY = -7.4;
	for (int j = pantalla[pantallaactual].pantalla.midaY; j >= 0; j--){
		for (int i = pantalla[pantallaactual].pantalla.midaX; i >= 0; i--){
			int valor = pantalla[pantallaactual].pantalla.matriuPantalla[i][j];
			switch (valor){
				//Terra
			case 0:
				pintaQuadrat(initX + i, initY + j, true, 0.5f, 0.35f, 0.05f, 1, true, 0, 0, 0, 1, texturaTerra);
				break;
				//LsSurvivor
			case 1:

				pintaQuadrat(initX + i, initY + j, true, 0.0, 0.5, 0.0, 1, true, 0, 0, 0, 1, texturaTerra);
				break;
				//Columna
			case 2:
				pintaQuadrat(initX + i, initY + j, true, 1.0, 1.0, 0.0, 1, true, 0, 0, 0, 1, texturaTerra);

				break;
				//Dispensador d'enemics
			case 3:
				pintaQuadrat(initX + i, initY + j, true, 1, 0, 0, 1, true, 0, 0, 0, 1, texturaTerra);

				break;
				//Paret
			case 4:
				pintaQuadrat(initX + i, initY + j, true, 0.2, 0.2, 0.2, 1, true, 0, 0, 0, 1, texturaTerra);
				break;

			}
		}

	}

}



//Control de teclat
void Comprovatecles(){

	float posXAnt = posX;
	float posYAnt = posY;
	if (VidesJugador > 0){
		//Modes de llum
		if (tecles['1']){
			glDisable(GL_LIGHTING);
			lightMode = 0;
		}

		if (tecles['2']){
			glEnable(GL_LIGHTING);
			glShadeModel(GL_FLAT);
			lightMode = 1;
		}

		if (tecles['3']){
			glEnable(GL_LIGHTING);
			glShadeModel(GL_SMOOTH);
			lightMode = 2;
		}

		//Modes de camera
		if (tecles['7']){
			camMode = 1;
		}
		if (tecles['8']){
			camMode = 2;
		}
		if (tecles['9']){
			camMode = 3;
		}
		//Desplaçament lareral en 1ra i 3ra persona
		if (camMode != 3){
			if (tecles['q']){
				//posY = posY - 0.1;


				float dirX = 0.0;
				float dirY = 0.0;
				dirX = cos((angle + 90)*3.1416 / 180);
				dirY = sin((angle - 90)*3.1416 / 180);

				float factorVelocitat = 0.2;
				dirX *= factorVelocitat;
				dirY *= factorVelocitat;
				posY -= dirY;
				posX += dirX;




			}
			if (tecles['e']){
				float dirX = 0.0;
				float dirY = 0.0;
				dirX = cos((angle + 90)*3.1416 / 180);
				dirY = sin((angle - 90)*3.1416 / 180);

				float factorVelocitat = 0.2;
				dirX *= factorVelocitat;
				dirY *= factorVelocitat;
				posY += dirY;
				posX -= dirX;

			}
		}
		if (tecles['w']){
			if (camMode == 1 || camMode == 2){
				float dirX = 0.0;
				float dirY = 0.0;
				dirX = cos(angle*3.1416 / 180);
				dirY = sin(angle*3.1416 / 180);

				float factorVelocitat = 0.2;
				dirX *= factorVelocitat;
				dirY *= factorVelocitat;
				posX += dirX;
				posY += dirY;
			}
			if (camMode == 3){
				posY = posY + 0.1;
			}
		}
		if (tecles['a'] && !tecles['s']){

			if (camMode == 3){
				posX = posX - 0.1;
			}
			if (camMode == 1 || camMode == 2){
				posMouseY -= 3;
				angle = angle + 5;

			}
			if (camMode < 1){
				posX += 0.1;

			}
		}
		if (tecles['s']){
			if (camMode == 1 || camMode == 2){
				float dirX = 0.0;
				float dirY = 0.0;
				dirX = cos(angle*3.1416 / 180);
				dirY = sin(angle*3.1416 / 180);

				float factorVelocitat = 0.2;
				dirX *= factorVelocitat;
				dirY *= factorVelocitat;
				posX -= dirX;
				posY -= dirY;
			}

			if (camMode == 3){
				posY = posY - 0.1;
			}



		}
		if (camMode != 3){
			if (tecles['s'] && tecles['d']){
				posMouseX += 3;
				angle = angle + 5;
			}
			if (tecles['s'] && tecles['a']){
				posMouseX += 3;
				angle = angle - 5;
			}
		}
		else{
			if (tecles['s'] && tecles['a']){
				posX = posX - 0.1;
			}
			if (tecles['s'] && tecles['d']){
				posX = posX + 0.1;
			}
		}

		if (tecles['d'] && !tecles['s']){

			if (camMode == 3){
				posX = posX + 0.1;
			}
			if (camMode == 1 || camMode == 2){
				posMouseX += 3;
				angle = angle - 5;
			}

			if (camMode == 1 || camMode == 2) rotCamera += 3;
			if (camMode < 1) posX += 0.1;
		}

		if (tecles['v']){
			//Crea Projectils
			creaProjectil(llistaProjectils, posX, posY, rotJugador);
			PlaySound(TEXT("Shot8b.wav"), NULL, SND_ASYNC);
		}
	}
	//Xoc Jugador vs Parets Laterals
	if (posX<-7.5 || posX>7.7){
		posX = posXAnt;
	}
	if (posY < -7.5 || posY>7.7){
		posY = posYAnt;
	}
	//XOC JUGADOR PARET
	BB BBjugador = modelPlayer.boundingBox;
	BBjugador.max.x += posX;
	BBjugador.max.y += posY;
	BBjugador.min.x += posX;
	BBjugador.min.y += posY;
	for (int i = 0; i < pantalla[pantallaactual].numColumnes; i++){
		BB BBcolumna1 = modelParet1.boundingBox;

		BB BBcolumna4 = modelParet4.boundingBox;
		//Declarem la bounding box auxiliar de cada tipus de columna
		switch (pantalla[pantallaactual].columnes[i].tipusColumna){
		case 1:
			BBcolumna1.max.x += pantalla[pantallaactual].columnes[i].posicioX - 8;
			BBcolumna1.max.y += pantalla[pantallaactual].columnes[i].posicioY - 8;
			BBcolumna1.min.x += pantalla[pantallaactual].columnes[i].posicioX - 8;
			BBcolumna1.min.y += pantalla[pantallaactual].columnes[i].posicioY - 8;
			//Adaptem la BB de la columna 1
			if (pantalla[pantallaactual].columnes[i].posicioRotacio == 90 || pantalla[pantallaactual].columnes[i].posicioRotacio == 270){
				float quantiat = (1 - (BBcolumna1.max.x - BBcolumna1.min.x)) / 2;
				BBcolumna1.max.x += quantiat;
				BBcolumna1.max.y -= quantiat;
				BBcolumna1.min.x -= quantiat;
				BBcolumna1.min.y += quantiat;
			}

			if (BBcolumna1.Intersect(BBjugador)){

				//if(!(BBcolumna1.max.x<BBjugador.min.x || BBcolumna1.min.x > BBjugador.max.x)){ 
				posX = posXAnt;
				//}else {
				posY = posYAnt;
				//}
			}
			break;

		case 4:
			BBcolumna4.max.x += pantalla[pantallaactual].columnes[i].posicioX - 8;
			BBcolumna4.max.y += pantalla[pantallaactual].columnes[i].posicioY - 8;
			BBcolumna4.min.x += pantalla[pantallaactual].columnes[i].posicioX - 8;
			BBcolumna4.min.y += pantalla[pantallaactual].columnes[i].posicioY - 8;

			//Adaptem la BB de la columna 4
			if (pantalla[pantallaactual].columnes[i].posicioRotacio == 90 || pantalla[pantallaactual].columnes[i].posicioRotacio == 270){
				float quantiat = (1 - (BBcolumna4.max.x - BBcolumna4.min.x)) / 2;
				BBcolumna4.max.x += quantiat;
				BBcolumna4.max.y -= quantiat;
				BBcolumna4.min.x -= quantiat;
				BBcolumna4.min.y += quantiat;
			}

			if (BBcolumna4.Intersect(BBjugador)){
				//if(!(BBcolumna1.max.x<BBjugador.min.x || BBcolumna1.min.x > BBjugador.max.x)){ 
				posX = posXAnt;
				//}else {
				posY = posYAnt;
				//}
			}

			break;

		}

	}
}

void RotacioEnemic(){
	rotEnemic = 45;
	float VectorEpicX;
	float VectorEpicY;
	for (int i = 0; i<pantalla[pantallaactual].numEnemics; i++){
		VectorEpicX = posX - pantalla[pantallaactual].enemics[i].posicioX;
		VectorEpicY = posY - pantalla[pantallaactual].enemics[i].posicioY;
		float modul = sqrt((VectorEpicX*VectorEpicX) + (VectorEpicY*VectorEpicY));
		VectorEpicX /= modul;
		VectorEpicY /= modul;
		rotEnemic = (atan2(VectorEpicY, VectorEpicX))*(180.0 / 3.1416);
	}
}

void RotacioJugador(){
	if (camMode == 1 || camMode == 2){

		if (angle == 360 || angle == -360){
			angle = 0;
		}
		rotJugador = angle;
	}
	else{
		rotJugador = 45;
		float VectorEpicX;
		float VectorEpicY;
		//vector mouse-jugador
		VectorEpicX = (posMouseX - posX) + posX;
		VectorEpicY = (posMouseY - posY) + posY;
		//Modulo
		float modul = sqrt((VectorEpicX*VectorEpicX) + (VectorEpicY*VectorEpicY));
		//Normalizar
		VectorEpicX /= modul;
		VectorEpicY /= modul;

		//Calculo del angulo
		rotJugador = (atan2(VectorEpicY, VectorEpicX))*(180.0 / 3.1416);
	}
}
//S'utilitza per reasignar una casella aleatoria als enemics que falten per sortir
void enemyRespawn(){
	int Num = 0;
	float initX = -8.4;
	float initY = -7.4;
	//Calcul de valor aleatori
	srand(time(NULL));
	Num = (rand() % 3);
	//Fins que no surtin tots els enemics es fan reapareixer en funció de Num
	if (numEnemicsActius>0){
		
		pantalla[pantallaactual].enemics[numRespawns].posicioX = pantalla[pantallaactual].dispensadors[Num].posicioX + initX + 0.5;
		pantalla[pantallaactual].enemics[numRespawns].posicioY = pantalla[pantallaactual].dispensadors[Num].posicioY + initY - 0.5;
		pantalla[pantallaactual].enemics[numRespawns].actiu = true;
		numRespawns++;
		numEnemicsActius--;
	}

}
//Calcula el temps en segons si es crida a logica
void rellotge(){
	//Cada vegada que currentTime és 60 equival a 1s real
	currentTime++;
	if (currentTime == 60){
		timeAcum++;
		currentTime = 0;
		if (timeAcum == TEMPSAPARICIO){
			enemyRespawn();
		}
		if (timeAcum == TEMPSAPARICIO){
			timeAcum = 0;
		}
	}

}
void actualitzaEnemics(){
	//Posicion anterior del enemigo
	int posXAnt;
	int posYAnt;
	float initX = -8.4;
	float initY = -7.4;
	//Inicialització dels enemics a les seves caselles a la primera aparicio
	if (flag == 0){
		for (int i = 0; i<pantalla[pantallaactual].numDispensadors; i++){
			pantalla[pantallaactual].enemics[i].posicioX = pantalla[pantallaactual].dispensadors[i].posicioX + initX + 0.5;
			pantalla[pantallaactual].enemics[i].posicioY = pantalla[pantallaactual].dispensadors[i].posicioY + initY - 0.5;
			pantalla[pantallaactual].enemics[i].actiu = true;
		}
		//Força que no s'entri més d'1 cop 
		flag = 1;
	}
	for (int i = 0; i<pantalla[pantallaactual].numEnemics; i++){

		if (pantalla[pantallaactual].enemics[i].actiu){
			posXAnt = pantalla[pantallaactual].enemics[i].posicioX;
			posYAnt = pantalla[pantallaactual].enemics[i].posicioY;
			float vecX = posX - pantalla[pantallaactual].enemics[i].posicioX;
			float vecY = posY - pantalla[pantallaactual].enemics[i].posicioY;

			float modul = sqrt(vecX*vecX + vecY*vecY);

			vecX /= modul; // equivalent a vecX = vecX/modul
			vecY /= modul;

			float factorEscala = 0.06;
			vecX *= factorEscala;
			vecY *= factorEscala;

			pantalla[pantallaactual].enemics[i].posicioX += vecX;
			pantalla[pantallaactual].enemics[i].posicioY += vecY;
			//XOC ENEMIC-PARET
			BB BBenemic = modelEnemy.boundingBox; //bounding box auxiliar del enemic
			BBenemic.max.x += pantalla[pantallaactual].enemics[i].posicioX;// +0.2;
			BBenemic.max.y += pantalla[pantallaactual].enemics[i].posicioY;// +0.2;
			BBenemic.min.x += pantalla[pantallaactual].enemics[i].posicioX;
			BBenemic.min.y += pantalla[pantallaactual].enemics[i].posicioY;

			for (int d = 0; d < pantalla[pantallaactual].numColumnes; d++){
				//Declarem les BB de les columnes
				BB BBcolumna1 = modelParet1.boundingBox;

				BB BBcolumna4 = modelParet4.boundingBox;

				//Ajustem la BBcolumna
				switch (pantalla[pantallaactual].columnes[d].tipusColumna){
				case 1:
					BBcolumna1.max.x += pantalla[pantallaactual].columnes[d].posicioX - 8;
					BBcolumna1.max.y += pantalla[pantallaactual].columnes[d].posicioY - 8;
					BBcolumna1.min.x += pantalla[pantallaactual].columnes[d].posicioX - 8;
					BBcolumna1.min.y += pantalla[pantallaactual].columnes[d].posicioY - 8;
					//Adaptem la BB de la columna 1
					if (pantalla[pantallaactual].columnes[d].posicioRotacio == 90 || pantalla[pantallaactual].columnes[d].posicioRotacio == 270){
						float quantiat = (1 - (BBcolumna1.max.x - BBcolumna1.min.x)) / 2;
						BBcolumna1.max.x += quantiat;
						BBcolumna1.max.y -= quantiat;
						BBcolumna1.min.x -= quantiat;
						BBcolumna1.min.y += quantiat;
					}

					if (BBcolumna1.Intersect(BBenemic)){
						if(!(BBcolumna1.max.x<BBenemic.min.x || BBcolumna1.min.x > BBenemic.max.x)){
						pantalla[pantallaactual].enemics[i].posicioX = posXAnt;
						}else{

						pantalla[pantallaactual].enemics[i].posicioY = posYAnt;
						}
					}
					break;
				case 4:
					BBcolumna4.max.x += pantalla[pantallaactual].columnes[d].posicioX - 8;
					BBcolumna4.max.y += pantalla[pantallaactual].columnes[d].posicioY - 8;
					BBcolumna4.min.x += pantalla[pantallaactual].columnes[d].posicioX - 8;
					BBcolumna4.min.y += pantalla[pantallaactual].columnes[d].posicioY - 8;

					//Adaptem la BB de la columna 4
					if (pantalla[pantallaactual].columnes[d].posicioRotacio == 90 || pantalla[pantallaactual].columnes[d].posicioRotacio == 270){
						float quantiat = (1 - (BBcolumna4.max.x - BBcolumna4.min.x)) / 2;
						BBcolumna4.max.x += quantiat;
						BBcolumna4.max.y -= quantiat;
						BBcolumna4.min.x -= quantiat;
						BBcolumna4.min.y += quantiat;
					}

					if (BBcolumna1.Intersect(BBenemic)){
						if (!(BBcolumna1.max.x<BBenemic.min.x || BBcolumna1.min.x > BBenemic.max.x)){
						pantalla[pantallaactual].enemics[i].posicioX = posXAnt;
						}
						else{

						pantalla[pantallaactual].enemics[i].posicioY = posYAnt;
						}
					}
					break;

				}
			}
		}

	}
}
void reinicialitza(){
	flag = 0;
	incicialitzacions();
	//Penalització per reviure
	killedEnemyCounter = 0;
}

void comprovaColisions(){
	BB BBjugador = modelPlayer.boundingBox;
	BBjugador.max.x += posX;
	BBjugador.max.y += posY;
	BBjugador.min.x += posX;
	BBjugador.min.y += posY;
	//Xoc Jugador-Enemic
	for (int i = 0; i<pantalla[pantallaactual].numEnemics; i++){
		if (pantalla[pantallaactual].enemics[i].actiu){
			BB BBenemic = modelEnemy.boundingBox;
			BBenemic.max.x += pantalla[pantallaactual].enemics[i].posicioX;
			BBenemic.max.y += pantalla[pantallaactual].enemics[i].posicioY;
			BBenemic.min.x += pantalla[pantallaactual].enemics[i].posicioX;
			BBenemic.min.y += pantalla[pantallaactual].enemics[i].posicioY;
			if (BBjugador.Intersect(BBenemic) && VidesJugador != 0){
				pantalla[pantallaactual].enemics[i].actiu = false;
				killedEnemyCounter++;
				PlaySound(TEXT("Au.wav"), NULL, SND_ASYNC);
				VidesJugador -= 1;
				if (VidesJugador <= 0){
					printf("----------------------------------Has mort!---------------------------------- \n");

					PlaySound(TEXT("Au.wav"), NULL, SND_ASYNC);
					PlaySound(TEXT("GameOver.wav"), NULL, SND_ASYNC);
					pantalla[pantallaactual].lsSurvivor.actiu = false;

				}
				else{
					printf("Perds una vida! Vides actuals: %d\n", VidesJugador);
				}
				if (killedEnemyCounter >= pantalla[pantallaactual].numEnemics && pantallaactual <= MAXESCENARIS){

					pantallaactual++;
					PlaySound(TEXT("NextLevel.wav"), NULL, SND_ASYNC);
					reinicialitza();
				}

			}
		}
	}
	//Xoc projectil-Paret

	for (int i = 0; i < pantalla[pantallaactual].numColumnes; i++){
		BB BBcolumna1 = modelParet1.boundingBox;

		BB BBcolumna4 = modelParet4.boundingBox;
		switch (pantalla[pantallaactual].columnes[i].tipusColumna){
		case 1:
			BBcolumna1.max.x += pantalla[pantallaactual].columnes[i].posicioX - 8;
			BBcolumna1.max.y += pantalla[pantallaactual].columnes[i].posicioY - 8;
			BBcolumna1.min.x += pantalla[pantallaactual].columnes[i].posicioX - 8;
			BBcolumna1.min.y += pantalla[pantallaactual].columnes[i].posicioY - 8;
			//Adaptem la BB de la columna 1
			if (pantalla[pantallaactual].columnes[i].posicioRotacio == 90 || pantalla[pantallaactual].columnes[i].posicioRotacio == 270){
				float quantiat = (1 - (BBcolumna1.max.x - BBcolumna1.min.x)) / 2;
				BBcolumna1.max.x += quantiat;
				BBcolumna1.max.y -= quantiat;
				BBcolumna1.min.x -= quantiat;
				BBcolumna1.min.y += quantiat;
			}
			//Declarem la bounding box auxiliar de la columna
			for (int j = 0; j < llistaProjectils.numProjectils; j++){

				BB BBprojectil = modelProjectil.boundingBox;
				BBprojectil.max.x += llistaProjectils.DadesProjectil[j].posicioX;
				BBprojectil.max.y += llistaProjectils.DadesProjectil[j].posicioY;
				BBprojectil.min.x += llistaProjectils.DadesProjectil[j].posicioX;
				BBprojectil.min.y += llistaProjectils.DadesProjectil[j].posicioY;
				if (BBprojectil.Intersect(BBcolumna1) || BBprojectil.min.y<-7.5 || BBprojectil.min.y>7.7 || BBprojectil.max.x<-7.5 || BBprojectil.max.x>7.7){


					eliminaProjectil(llistaProjectils, j);
					PlaySound(TEXT("WallShot.wav"), NULL, SND_ASYNC);
				}


			}
		}
	}
	//Xoc projectil-Enemic
	for (int i = 0; i<pantalla[pantallaactual].numEnemics; i++){
		if (pantalla[pantallaactual].enemics[i].actiu){
			BB BBenemic = modelEnemy.boundingBox;
			BBenemic.max.x += pantalla[pantallaactual].enemics[i].posicioX;
			BBenemic.max.y += pantalla[pantallaactual].enemics[i].posicioY;
			BBenemic.min.x += pantalla[pantallaactual].enemics[i].posicioX;
			BBenemic.min.y += pantalla[pantallaactual].enemics[i].posicioY;
			for (int j = 0; j<llistaProjectils.numProjectils; j++){

				BB BBprojectil = modelProjectil.boundingBox;
				BBprojectil.max.x += llistaProjectils.DadesProjectil[j].posicioX;
				BBprojectil.max.y += llistaProjectils.DadesProjectil[j].posicioY;
				BBprojectil.min.x += llistaProjectils.DadesProjectil[j].posicioX;
				BBprojectil.min.y += llistaProjectils.DadesProjectil[j].posicioY;
				if (BBprojectil.Intersect(BBenemic)){
					pantalla[pantallaactual].enemics[i].actiu = false;
					eliminaProjectil(llistaProjectils, j);
					killedEnemyCounter++;
					puntsJugador += 25;
					printf("Has mort %d enemic/s\n", killedEnemyCounter);
					PlaySound(TEXT("DeadMachine.wav"), NULL, SND_ASYNC);
				}
				if (killedEnemyCounter >= pantalla[pantallaactual].numEnemics && pantallaactual <= MAXESCENARIS){
					
					pantallaactual++;
					PlaySound(TEXT("NextLevel.wav"), NULL, SND_ASYNC);
					reinicialitza();

					
				}

			}

		}
	}

}
void LogicaJoc(){
	Comprovatecles();
	RotacioJugador();
	RotacioEnemic();
	actualitzaEnemics();
	actualitzaProjectils();
	comprovaColisions();
	rellotge();
}
void Idle(){
	int time_now = glutGet(GLUT_ELAPSED_TIME);
	if (time_now - movement_timer > 16){
		movement_timer = time_now;

		LogicaJoc();
		glutPostRedisplay();
	}
}
void initGL(){
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//defineix la posició en x i en y de la finestra.
	glutInitWindowPosition(300, 50);
	//defineix l’ample i l’alçada de la pantalla.

	glutInitWindowSize(WIDTH, HEIGHT);
	//crea la finestra, el paràmetre que rep es el nom que tindrà.
	glutCreateWindow("GFX 1: ls27345 ls25707");

	glClearColor(0, 0, 0, 0);

	//Inicialitacio de projectils
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	inicialitzaArrayProjectils(llistaProjectils);
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}
//Funcions de teclat 2
void my_key_events(unsigned char key, int x, int y){
	switch (key){
		//tecla p
	case 'p':
		DEBUG = !DEBUG;
		break;
	case 'f':
		fogEnable = !fogEnable;
		glDisable(GL_FOG);
		break;
	case 'r':
		reinicialitza();
		break;
		//tecla exit
	case 27:
		exit(0);
		break;
	}
	tecles[key] = true;
}
void my_key_events_up(unsigned char key, int x, int y){

	tecles[key] = false;
}
//Funcions de tecles especials
void inputspecialUsuari(int key){
	if (key == GLUT_KEY_UP){
		exit(1);
	}
}
//Funcions de ratolí
void movimentMouse(int x, int y){

	posMouseX = x - WIDTH / 2;
	posMouseY = -y + HEIGHT / 2;

	posMouseX = posMouseX * 20 / WIDTH;
	posMouseY = posMouseY * 20 / HEIGHT;
}
void theMouse(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON){
		if (state == GLUT_UP){
			creaProjectil(llistaProjectils, posX, posY, rotJugador);
			PlaySound(TEXT("Shot8b.wav"), NULL, SND_ASYNC);
			ShotCount++;

		}
	}
}

void Inicialitzacio(){
	initGL();
	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);
	carregaModels();
}

void cambiaEscenari(){
	char aux[15];
	char aux2[2];
	for (int i = 1; i <= MAXESCENARIS; i++){
		strcpy(aux, "\0");
		strcpy(aux2, "\0");
		strcat(aux, "pantalla");
		itoa(i, aux2, 10);//pasem el numero a caracter
		printf(aux2);
		strcat(aux, aux2);
		strcat(aux, ".txt");
		printf(aux);
		escenari[i - 1].carregaEscenari(aux, &pantalla[i - 1]);
		escenari[i - 1].ompleMatriu(&pantalla[i - 1]);
	}



}
void pintaText(){
	char text[100];
	char text2[100];
	char text3[100];
	char text4[100];

	sprintf(text, "VIDES: %d", VidesJugador);
	RenderString(-9, 9, text);

	sprintf(text2, "ENEMICS: %d", pantalla[pantallaactual].numEnemics - killedEnemyCounter);
	RenderString(-9, 8, text2);

	sprintf(text3, "Punts: %d ", puntsJugador);
	RenderString(-9, 7, text3);
	if (pantalla[pantallaactual].numEnemics == 0 && pantallaactual == MAXESCENARIS){
		char text5[100];
		sprintf(text5, "YOU WIN!  Pres ESC to quit   \n");
		winner = 1;
		pantalla[pantallaactual].lsSurvivor.actiu = false;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderString(-4, 0, text5);

	}
	if (VidesJugador <= 0){

		sprintf(text3, "GAME OVER");
		RenderString(-3, 1, text3);
		sprintf(text4, " Press 'ESC' to quit or 'R' to respawn.");

		RenderString(-3, -2, text4);
		flagDead = 1;
	}
}
void Render(){
	//Color de fons de pantalla
	//glClearColor(0.5, 0.5, 0.5, 1);
	//glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Definim la vista de càmera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (camMode != 3){
		//per donar efectes a la camera s'utilitza molt
		//10 * cos((float)temps)
		//10 * sin((float)temps)
		gluPerspective(50, 1, 0.01, 100);
	}
	else{

		glOrtho(-10, 10, -10, 10, 0.01, 400);
		//glOrtho(-10, 10, -10, 10, 0.1, 24);

	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -20);
	//Traslladar l'escena en funcio de la vista que volguem
	if (camMode == 1){
		//Camera 3rd Person
		glTranslatef(0, -1, 18);
		glRotatef(-60, 1, 0, 0);
		glRotatef(-rotJugador + 90, 0, 0, 1);
		//glRotatef(0, 0, 0, 1);
		glTranslatef(-posX, -posY, 0);
	}
	if (camMode == 2){
		//0,-0.6,20.08
		glTranslatef(0, -0.6, 20.05);
		//glTranslatef(0, -0.3, 23);
		glRotatef(-90, 1, 0, 0);
		glRotatef(-rotJugador + 90, 0, 0, 1);
		//glRotatef(0, 0, 0, 1);
		glTranslatef(-posX, -posY, 0);
	}
	if (camMode == 3){
		//Camera Zenital seguint el jugador
		glTranslatef(-posX, -posY, 1);

	}
	//FOG
	if (fogEnable){
		glEnable(GL_FOG);
		GLfloat fogColor[4] = { 0, 0, 0, 0.1 };

		fogMode = GL_EXP;
		glFogi(GL_FOG_MODE, fogMode);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, 0.3);
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_START, 1.0);
		glFogf(GL_FOG_END, 5.0);

		glClearColor(1, 1, 1, 1);

	}

	//End FoG

	GLfloat light_position[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT1);
	GLfloat light_position2[4];
	light_position2[0] = posX;
	light_position2[1] = posY;
	light_position2[2] = 1.0;
	light_position2[3] = 1.0;
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);


	//Punt on és la llum
	//glColor3f(0, 1, 0);
	glPointSize(4.0);
	glBegin(GL_POINTS);
	//	glVertex3f(1, 1, 1);
	glEnd();

	//Pintem les linies de terra

	PintaTerra();
	/////////Mostrem tots els models 3D/////////
	glPushMatrix();
	pintaText();
	glPopMatrix();
	MostraModels(pantalla[pantallaactual]);
	glDisable(GL_TEXTURE_2D);
	//Pintem text


	//glFlush();

	glutSwapBuffers();

}


int main(int argc, char **argv){

	//FreeConsole();
	printf("			LS Survival:			\n");
	printf("		      - - - - - - - -			\n");

	system("cls");

	//Funcio que carrega la informacio d'un escenari un cop morts tots els enemics
	cambiaEscenari();
	//Posem el jugador a la casella inicial
	incicialitzacions();
	//inicialitza la Glut i negocia amb el sistema de finestres per inicialitzar-ne una.
	glutInit(&argc, argv);
	//Funcio propia que inicialitzarà els estats d’OGL quan el programa s’inicialitza.
	Inicialitzacio();
	glutDisplayFunc(Render);
	//Redibuixa la finestra
	glutPassiveMotionFunc(movimentMouse);
	glutKeyboardUpFunc(my_key_events_up);
	glutKeyboardFunc(my_key_events);
	glutMouseFunc(theMouse);
	//glutSpecialFunc(inputspecialUsuari);
	glutIdleFunc(Idle);

	//el programa anirà cridant als diferents “callbacks” (fa que el programa no s’acabi)
	glutMainLoop();

}
