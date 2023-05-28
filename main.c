#include "header/animations.h"
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>

static void init(void);
static void sortie(void);
static void resize(int w, int h);



/*!\brief dimensions initiales de la fenêtre */
static GLfloat _dim[] = {1024, 768};

/*!\brief tableau contenant les animations sous la forme de timeline,
 * ce tableau se termine toujours par l'élémént {0, NULL, NULL,
 * NULL} */
static GL4DHanime _animations[] = {
  { 4500, player1, NULL, NULL },
  { 1500, versus, NULL, NULL },
  { 4500, player2, NULL, NULL },
  { 3500, depart1, NULL, NULL },
  { 2000, depart2, NULL, NULL },
  { 8000, toutdroit, NULL, NULL },
  { 8000, viraged, NULL, NULL },
  { 6000, toutdroit, NULL, NULL },
  { 8000, virageg, NULL, NULL },
  { 10000, arrivee1, NULL, NULL },
  { 2000, arrivee2, NULL, NULL },
  { 6000, pd, NULL, NULL },
  {    0, NULL, NULL, NULL } /* Toujours laisser à la fin */
};

int main(int argc, char ** argv) {
	if(!gl4duwCreateWindow(argc, argv, "Ateliers API8 - démo", 
			 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _dim[0], _dim[1],
			 SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
	init();
	atexit(sortie);
	gl4duwDisplayFunc(gl4dhDraw);
  	ahInitAudio("audio/RunningintheKazoo.ogg");
	gl4duwMainLoop();
	return 0;
}

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gl4dhInit(_animations, _dim[0], _dim[1], animationsInit);
	gl4duGenMatrix(GL_FLOAT, "modelCar");
	gl4duGenMatrix(GL_FLOAT, "modelRoad");
	gl4duGenMatrix(GL_FLOAT, "projectionView");
	resize(_dim[0], _dim[1]);
	// active la synchro vertical
	SDL_GL_SetSwapInterval(1);
	glEnable(GL_DEPTH_TEST);

	// On s'addresse a projectionView
	gl4duBindMatrix("projectionView");
	// projectionView = Identity et identity est égale a 1 ne sert a rien toujopur load 
	gl4duLoadIdentityf();
	gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glLineWidth(3.0f);
	// glEnable(GL_LINE_SMOOTH);
}

static void resize(int w, int h) {
	_dim[0] = w; _dim[1] = h;
	glViewport(0, 0, _dim[0], _dim[1]);
	gl4duBindMatrix("projectionView");
	gl4duLoadIdentityf();
	gl4duFrustumf(-0.5, 0.5, -0.5 * _dim[0] / _dim[0], 0.5 * _dim[1] / _dim[0], 1.0, 1000.0);
	gl4duBindMatrix("modelCar");
}

void sortie(void) {
  	ahClean();
	gl4duClean(GL4DU_ALL);
}