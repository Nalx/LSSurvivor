
#include "escenari.h"



/**
* Funció que carrega de fitxer la definició d'un mapa i la guarda a l'estructura de dades Pantalla
* @param cpFileName és el nom del fitxer
* @param escenari és l'estructura de dades on es guardarà la informació
* @return true si l'ha carregat correctament, false en cas contrari
*/

bool Escenari::carregaEscenari(char * cpFileName, Pantalla *escenari)
{
	FILE *f;
	//A kk hi guardem informació prescindible
	char kk[50];
	int i=0;
	f = fopen( cpFileName, "r" );
	if ( f==NULL ){
		printf( "Error de lectura" );
	}else{
		printf("			LS Survival:			\n");
		printf("		      - - - - - - - -			\n");
		//Lectura de "mapa:"
		fscanf( f,"%s",kk );
		//Lectura del nom
		fgets((*escenari).nomMapa,50,f); 
		//Lectura de "mida:"
		fscanf(f,"%s",kk);
		//Lectura dels valors X i Y de mida
		fscanf(f,"%d",&(*escenari).pantalla.midaX);
		fscanf(f,"%d",&(*escenari).pantalla.midaY);
		//lectura de "Columnes:"
		fscanf(f,"%s",kk);
		//Lectura del nombre de columnes
		fscanf(f,"%d",&escenari->numColumnes );
		(*escenari).columnes=(Columna*)malloc(escenari->numColumnes*(sizeof(Columna)));
		//lectura de dades sobre columnes
		while (i<escenari->numColumnes){
			fscanf(f,"%d", &escenari->columnes[i].posicioX);
			fscanf(f,"%d", &escenari->columnes[i].posicioY);
			fscanf(f,"%d", &escenari->columnes[i].tipusColumna);
			fscanf(f,"%d", &escenari->columnes[i].posicioRotacio);
			i++;
		}
		//Lectura de "LsSurvivor"
		fscanf(f,"%s",kk);
		//Lectura de coordenades de LsSurvivor
		fscanf(f,"%d", &escenari->lsSurvivor.posicioX );
		fscanf(f,"%d", &escenari->lsSurvivor.posicioY);
		//Lectura de "Dispensadors"
		fscanf(f,"%s",kk);
		//Lectura de dades dels Dispensadors
		fscanf(f,"%d", &(*escenari).numDispensadors);
		(*escenari).dispensadors=(DispensadorEnemics*)malloc((*escenari).numDispensadors*(sizeof(DispensadorEnemics)));
		i=0;
		while(i < (*escenari).numDispensadors){
			fscanf(f,"%d", &(*escenari).dispensadors[i].posicioX);
			fscanf(f,"%d", &(*escenari).dispensadors[i].posicioY);
			fscanf(f,"%d", &(*escenari).dispensadors[i].angleInclinacio);
			i++;
		}
		//Lectura de "Dificultat:"
		fscanf(f,"%s",kk);
		//Lectura del nivell de dificultat
		fscanf(f,"%d", &(*escenari).dificultat);
		//Lectura de "NumEnemics:"
		fscanf(f,"%s",kk);
		//Lectura del nombre d'enemics
		fscanf(f,"%d", &(*escenari).numEnemics );
		(*escenari).enemics = (Enemic*)malloc((*escenari).numEnemics*(sizeof(Enemic)));
		//Inicialitzem els enemics
		for (int i =0; i<(*escenari).numEnemics; i++){
		(*escenari).enemics[i].posicioX = 0;
		(*escenari).enemics[i].posicioY = 0;
		(*escenari).enemics[i].angle=0;
		(*escenari).enemics[i].actiu= false;
	}
	}
	printf( "Nom de la pantalla: %s\n",(*escenari).nomMapa);
	printf( "Num dispensadors: %d\n", (*escenari).numDispensadors );
	printf("Num Enemics: %d\n", (*escenari).numEnemics);
	printf("Num Columnes: %d\n",escenari->numColumnes);
	printf("Nivell de dificultat: %d\n", (*escenari).dificultat);
	printf("Mida: %d x %d\n",(*escenari).pantalla.midaX,(*escenari).pantalla.midaY );
	printf("\nTerra          -> 0"); 
	printf("\nLSSurvivor     -> 1"); 
	printf("\nColumna        -> 2"); 
	printf("\nDispensador    -> 3"); 
	printf("\nParet          -> 4\n\n");
	//Restem una unitat per fer obtenir la mida de la matriu demanada
	(*escenari).pantalla.midaX=(*escenari).pantalla.midaX-1;
	(*escenari).pantalla.midaY=(*escenari).pantalla.midaY-1;
	return true;
}


/**
* Funció que, a partir de la informació sobre les posicions dels elements, calcula la seva disposició en una matriu per facilitar la gestió dels elements
* @param escenari és l'estructura de dades on es guardarà la informació
*/

void Escenari::ompleMatriu(Pantalla *escenari)
{
	int X = 0;
	int Y = 0;
	int C = 0; //Comptador
	
	//Creem la base de 0
	while (Y < escenari->pantalla.midaY ){
		while (X < escenari->pantalla.midaX){
			escenari->pantalla.matriuPantalla[X][Y] = 0;
			X++;
		}
		Y++;
		X=0;
	}
	Y=0;
	//Creem el marc de parets amb 4
	X=0;
	while( Y <= escenari->pantalla.midaY ) {
			escenari->pantalla.matriuPantalla[X][Y] = 4;
			Y++;
	}
	Y= escenari->pantalla.midaY;
	while( X <= escenari->pantalla.midaX ) {
		escenari->pantalla.matriuPantalla[X][Y] = 4;
		X++;
	}
	X = escenari->pantalla.midaX;
	Y= 0;
	while( Y <= escenari->pantalla.midaY ) {
		escenari->pantalla.matriuPantalla[X][Y] = 4;
		Y++;
	}
	X=0;
	Y=0;
	while( X <= escenari->pantalla.midaX ) {
			escenari->pantalla.matriuPantalla[X][Y] = 4;
			X++;
	}

	Y, X = 0;
		while( Y <=	escenari->pantalla.midaY ) {
			//printf (" C= %d\n", C);
			while( X <= escenari->pantalla.midaX ) {
				C = 0;
				while (C <= escenari->numColumnes) {
					if ((escenari->columnes[C].posicioX == X) && (escenari->columnes[C].posicioY == Y)) {
						//Posem un 2 si hi ha columna
						escenari->pantalla.matriuPantalla[X][Y] = 2;		
					}
					C++;
				}
				if ((escenari->lsSurvivor.posicioX== X) && (escenari->lsSurvivor.posicioY == Y)) {
					//Posem un 1 si hi ha el lsSurvivor
					escenari->pantalla.matriuPantalla[X][Y] = 1;
				}
				C = 0;
				while (C <= escenari->numDispensadors) {
					if ((escenari->dispensadors[C].posicioX == X) && (escenari->dispensadors[C].posicioY == Y)) {
						//Posem un 3 si hi ha un dispensador
						escenari->pantalla.matriuPantalla[X][Y] = 3;		
					}
					C++;
				}
				X++;
			}
			Y++;
			X = 0;
		}
	
	C= 0;
	Y= 0;
	while( Y <=	escenari->pantalla.midaY ) {
		while( X <= escenari->pantalla.midaX ) {
			printf ("%d", escenari->pantalla.matriuPantalla[X][Y]);
			X++;
		}
		printf ("\n");
		Y++;
		X=0;
	}

	
}	
