#include "TexturesDevil.h"
#include <iostream>

void carregaTextura(GLuint *textura, char nomFoto[100])
{

	ILboolean success;	

    //carrega imatge a memoria
	ILuint idTextura;
	ilGenImages(1,&idTextura);
	ilBindImage(idTextura);

	success = iluLoadImage((ILstring) nomFoto);
			
	if(!success){
		ILenum error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		//exit(-1);
	}else{
		int width = ilGetInteger (IL_IMAGE_WIDTH);
		int height = ilGetInteger (IL_IMAGE_HEIGHT);

		printf("WIDTH: %d\n",width);
		printf("HEGITH: %d\n",height);
		unsigned char *pixmap = new unsigned char [ width * height * 3 ]; 
		ilCopyPixels(0,0,0,width,height,1, IL_RGB,IL_UNSIGNED_BYTE,pixmap);
		ilDeleteImage(idTextura);
		ilBindImage(0);
	
		glGenTextures(1,textura);
		glBindTexture(GL_TEXTURE_2D, *textura );


		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_LINEAR);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER , GL_LINEAR);
		glTexImage2D( GL_TEXTURE_2D  , 0 , GL_RGB , width , height , 0 , GL_RGB , GL_UNSIGNED_BYTE , pixmap );
		delete pixmap;

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); 
	}
}	
