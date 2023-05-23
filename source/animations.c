/*!\file animations.c
 *
 * \brief Votre espace de liberté : c'est ici que vous pouvez ajouter
 * vos fonctions de transition et d'animation avant de les faire
 * référencées dans le tableau _animations du fichier \ref window.c
 *
 * Des squelettes d'animations et de transitions sont fournis pour
 * comprendre le fonctionnement de la bibliothèque. En bonus des
 * exemples dont un fondu en GLSL.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date April 12, 2023
 */

#include "header/audioHelper.h"
#include "header/assimp.h"
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <assert.h>
#include <stdlib.h>
#include <SDL_image.h>

/*!\brief dimensions initiales de la fenêtre */
static GLfloat _dim[] = {1024, 768};
static GLuint _cubeId = 0;
static GLuint _roadId = 0;
static GLuint _pId = 0;
static GLuint _pId2 = 0;
static GLuint _pId3 = 0;
static GLuint _pId4 = 0;
static GLuint _pIdmodel3d = 0;

static GLuint _model1 = 0;
static GLuint _model2 = 0;

static GLfloat _PositionCube1[3] = {-1.0f, 0.0f, -5.5f};
static GLfloat _PositionCube2[3] = {4.0f, 0.0f, -5.5f};
static GLfloat _scaleBordure[3] = {0.1f, 0.1f, 1.0f};

static GLfloat lum[4] = {0.0, 10.0, 0.0, 1.0};

static GLfloat _atemp = 0.0f;

void Roaddraw(float anim_z);
void podiumDraw(void);
void gestion_voiture(void);
void gestion_model2(void);


static double get_dt(void) {
	static double t0 = 0.0f;
	double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
	t0 = t;
	return dt;
}

void p1(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a;
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      a = 0.0f;
      
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gestion_voiture();
    assimpDrawScene(_model1);
    gl4duRotatef(180.0f, 0, 0.1, 0);
    gl4duSendMatrices();

    // On s'addresse a la view
    gl4duBindMatrix("projectionView");
    gl4duLoadIdentityf();
    gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
    gl4duLookAtf((5.0f * cos(a*0.1)), 4.0f, (-5.0f * sin(a*0.1)), _PositionCube1[0], _PositionCube1[1], _PositionCube1[2], 0.0f, 1.0f, 0.0f);
    gl4duSendMatrices();

    //avant le draw modification de la matrice en question 
    glUseProgram(0);
    a += 2.0f * M_PI * get_dt();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return;
  }
}

void p2(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a;
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      a = 0.0f;
      glUniform1f(glGetUniformLocation(_pId, "weight"), 1.1f);
      glUniform1f(glGetUniformLocation(_pId2, "weight"), 1.1f);
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
      a += 2.0f * M_PI * get_dt();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      gestion_model2();
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf((5.0f * cos(a*0.1)) + _PositionCube2[0], 4.0f, (-5.0f * sin(a*0.1)) + _PositionCube2[2], _PositionCube2[0], _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();
      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      return;
  }
}

/// @brief / td pour tout droit animation prévue pour le ligne droite
/// @param state 
void td(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a;
  static GLfloat b;
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      a = 0.0f;
      b = 0.0f;
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
      a += 2.0f * M_PI * get_dt();
      b += 2.0f * M_PI * get_dt();
      _atemp = a;
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      gestion_voiture();
      gl4duTranslatef(0.0f, 0.0f, cos(a*0.2)/2);
      assimpDrawScene(_model1);
      gl4duSendMatrices();

      
      gestion_model2();
      gl4duTranslatef(sin(a*0.2)/2, 0.0f, 0.0f);
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      Roaddraw(a*0.5);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf((5.0f * sin(a*0.1)) + 1.5f, 4.0f, 5.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();

      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
      return;
  }
}

/// @brief / vd pour virage droite animation prévue pour le ligne droite
/// @param state 
void vd(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a;
  static GLfloat b;
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      a = 0.0f;
      b = 0.0f;
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
    a += 2.0f * M_PI * get_dt();
    b += 2.0f * M_PI * get_dt();
    _atemp = a;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gestion_voiture();
    gl4duTranslatef(0.0f, 0.0f, cos(a*0.2)/2);
    gl4duRotatef(40.0f + (sin(a*0.5)*2), 0.0f, 0.1f, 0.0f);
    assimpDrawScene(_model1);
    gl4duSendMatrices();

    gestion_model2();
    gl4duTranslatef(sin(a*0.2)/2, 0.0f, 0.0f);
    gl4duRotatef(40.0f + (sin(a*0.5)*2), 0.0f, 0.1f, 0.0f);
    assimpDrawScene(_model2);
    gl4duSendMatrices();

    Roaddraw(a*0.5);
    gl4duSendMatrices();

    gl4duBindMatrix("projectionView");
    gl4duLoadIdentityf();
    // (-2.0, 2.0, -1.0, 1.0, 1.0, 10.0)
    gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 3.0f, 1000.0f);
    gl4duLookAtf((5.0f * sin(a*0.1)) - 15.0f, 4.0f, 5.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);

    //envoie la matrice au gpu
    gl4duSendMatrices();
    //avant le draw modification de la matrice en question 
    glUseProgram(0);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    return;
  }
}

/// @brief / vd pour virage droite animation prévue pour le ligne droite
/// @param state 
void vg(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a;
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      a = 0.0f;
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
    a += 2.0f * M_PI * get_dt();
    _atemp = a;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    gestion_voiture();
    gl4duTranslatef(0.0f, 0.0f, cos(a*0.2)/2);
    gl4duRotatef(-40.0f + (cos(a*0.5)*2), 0.0f, 0.1f, 0.0f);
    assimpDrawScene(_model1);
    gl4duSendMatrices();

    gestion_model2();
    gl4duTranslatef(sin(a*0.2)/2, 0.0f, 0.0f);
    gl4duRotatef(-40.0f + (sin(a*0.5)*2), 0.0f, 0.1f, 0.0f);
    assimpDrawScene(_model2);
    gl4duSendMatrices();

    Roaddraw(a*0.5);
    gl4duSendMatrices();

    gl4duBindMatrix("projectionView");
    gl4duLoadIdentityf();
    gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
    gl4duLookAtf((5.0f * sin(a*0.1)) + 15.0f, 4.0f, 0.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);

    //envoie la matrice au gpu
    gl4duSendMatrices();
    //avant le draw modification de la matrice en question 
    glUseProgram(0);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    return;
  }
}

void pd(int state) {
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a;
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      a = 0.0f;
      
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    podiumDraw();

    glUseProgram(_pIdmodel3d);
    gl4duBindMatrix("modelCar");
    gl4duLoadIdentityf();
    gl4duScalef(3.0f, 3.0f, 3.0f);
    gl4duTranslatef(0.0f, 1.0f, -3.0f);
    assimpDrawScene(_model1);


    glUseProgram(_pIdmodel3d);
    gl4duBindMatrix("modelCar");
    gl4duLoadIdentityf();
    gl4duScalef(3.0f, 3.0f, 3.0f);
    gl4duTranslatef(1.2f, 2.0f, -3.2f);
    gl4duRotatef(-90, 0, 0.1, 0);
    assimpDrawScene(_model2);

    // On s'addresse a la view
    gl4duBindMatrix("projectionView");
    gl4duLoadIdentityf();
    gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
    gl4duLookAtf(2.0f, 8.0f, 0.0f, 2.5f, 4.0f, -5.0f, 0.0f, 1.0f, 0.0f);
    gl4duSendMatrices();

    //avant le draw modification de la matrice en question 
    glUseProgram(0);
    a += 2.0f * M_PI * get_dt();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    return;
  }
}

void gestion_voiture(void){
  glUseProgram(_pIdmodel3d);
  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duTranslatef(_PositionCube1[0], _PositionCube1[1], _PositionCube1[2]);
  gl4duRotatef(180, 0, 0.1, 0);
  gl4duScalef(7.5f,7.5f,7.5f);
}

void gestion_model2(void){
  glUseProgram(_pIdmodel3d);
  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duTranslatef(_PositionCube2[0], _PositionCube2[1], _PositionCube2[2]);
  gl4duRotatef(90, 0, 0.1, 0);
  gl4duScalef(5.0f,5.0f,5.0f);
}


void podiumDraw(void){
  GLfloat scale = 2.0f;

  glUseProgram(_pId);
  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duScalef(scale, scale, scale);
  gl4duTranslatef(0.0f, 0.0f, -5.0f);
  gl4dgDraw(_cubeId);
  gl4duSendMatrices();
  gl4duTranslatef(scale*2, 0.0f, 0.0f);
  gl4dgDraw(_cubeId);
  gl4duSendMatrices();
  gl4duTranslatef(-scale, 0.0f, 0.0f);
  gl4dgDraw(_cubeId);
  gl4duSendMatrices();
  gl4duTranslatef(0.0f, scale-(scale/4), 0.0f);
  gl4dgDraw(_cubeId);
  gl4duSendMatrices();
}

void Roaddraw(float anim_z) {
  anim_z = (-anim_z);

  glUseProgram(_pId4);
  SDL_Surface* image = IMG_Load("images/route.jpg");
  SDL_Surface* surface = SDL_CreateRGBSurface(0, image->w, image->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);

  gl4duBindMatrix("modelRoad");
  gl4duLoadIdentityf();
  gl4duTranslatef(1.5f, -1.001f, -1.0f);
  gl4duScalef(10, 20, 1000);
  gl4duRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  gl4duSendMatrices();

  SDL_BlitSurface(image, NULL, surface, NULL);
  SDL_FreeSurface(image);

  GLuint _tex_id;
  glGenTextures(1, &_tex_id);
  glBindTexture(GL_TEXTURE_2D, _tex_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
  SDL_FreeSurface(surface);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  gl4dgDraw(_roadId);
  glBindTexture(GL_TEXTURE_2D, _tex_id);

  glUseProgram(_pId3);
  gl4duBindMatrix("modelRoad");
  
  // ligne de milieu
  gl4duLoadIdentityf();
  gl4duScalef(_scaleBordure[0], _scaleBordure[1], _scaleBordure[2]);
  gl4duTranslatef(15.0f, -10.0f, 100.0f);
  gl4duRotatef(-90.0f, 0.1f, 0.0f,0.0f);
  gl4duSendMatrices();
  for (int i = -10; i < 400; i++)
  {
    gl4duTranslatef(0.0f , i + anim_z*2, 0.0f);
    gl4duSendMatrices();
    gl4dgDraw(_roadId);
    gl4duPopMatrix();
  }
  // ligne du gauche
  gl4duLoadIdentityf();
  gl4duScalef(_scaleBordure[0], _scaleBordure[1], _scaleBordure[2]*3);
  gl4duTranslatef(-60.0f, -10.0f, _scaleBordure[2]);
  gl4duRotatef(-90.0f, 0.1f, 0.0f,0.0f);
  gl4duSendMatrices();
  gl4duPopMatrix();
  for (int i = -10; i < 400; i++)
  {
    gl4duTranslatef(0.0f , i + anim_z, 0.0f);
    gl4duSendMatrices();
    gl4dgDraw(_roadId);
    gl4duPopMatrix();
  }
  // ligne de droite
  gl4duLoadIdentityf();
  gl4duScalef(_scaleBordure[0], _scaleBordure[1], _scaleBordure[2]*3);
  gl4duTranslatef(80.0f, -10.0f, _scaleBordure[2]);
  gl4duRotatef(-90.0f, 0.1f, 0.0f,0.0f);
  gl4duSendMatrices();
  for (int i = -10; i < 400; i++)
  {
    gl4duTranslatef(0.0f , i + anim_z, 0.0f);
    gl4duSendMatrices();
    gl4dgDraw(_roadId);
    gl4duPopMatrix();
  }
}

void animationsInit(void) {
  _pId = gl4duCreateProgram("<vs>shaders/player.vs", "<fs>shaders/hello.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/player.vs", "<fs>shaders/player.fs", NULL);
  _pId3 = gl4duCreateProgram("<vs>shaders/road.vs", "<fs>shaders/road.fs", NULL);
  _pId4 = gl4duCreateProgram("<vs>shaders/roadsol.vs", "<fs>shaders/roadsol.fs", NULL);
  _pIdmodel3d = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);

  glUseProgram(_pIdmodel3d);
  glUniform1i(glGetUniformLocation(_pIdmodel3d, "tex"), 0);
  glUniform4fv(glGetUniformLocation(_pIdmodel3d, "lumpos"), 1, lum);

  _model1 = assimpGenScene("texture/Chevrolet_Camaro_SS_Low.obj");
  _model2 = assimpGenScene("texture/bath.obj");

  if(!_cubeId)
    _cubeId = gl4dgGenCubef();
  if(!_roadId)
    _roadId = gl4dgGenQuadf();
}