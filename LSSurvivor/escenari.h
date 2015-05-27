#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//Estructura que representa la posició d'una columna
struct Columna
{	
public: 
	int			posicioX;
	int			posicioY;
	int			tipusColumna;
	int			posicioRotacio;
};

//Estructura que representa la posició d'una tanc enemic
struct DispensadorEnemics
{
public:
	int			posicioX;
	int			posicioY;
	int			angleInclinacio;
};

struct Enemic
{
	float posicioX;
	float posicioY;
	float angle;
	//int tempsAparicio;
	bool actiu;
};

//Estructura que representa la informació sobre el mapa en forma de matriu amb la llegenda indicada a l'enunciat de la pràctica
struct Escena
{
public: 
	int			midaX;
	int			midaY;
	int			matriuPantalla[17][17];
};

//Estructura que representa la posició del LSSurvivor
struct LsSurvivor
{
public: 
	int			posicioX;
	int			posicioY;
	//S'ha d'afegir?
	bool        actiu;
};

//Estructura que conté la informació sobre un escenari llegida de fitxer
struct Pantalla
{
public:
	char				nomMapa[50];
	Escena				pantalla;
	Columna				*columnes;
	DispensadorEnemics	*dispensadors;
	Enemic				*enemics;
	LsSurvivor			lsSurvivor;
	int					numEnemics;
	int					dificultat;
	int					numDispensadors;
	int					numColumnes;

};


class Escenari
{
	
	public: 
		bool carregaEscenari(char * cpFileName, Pantalla * escenari);
		void ompleMatriu(Pantalla *pantalla);

};
