#include "header/audioHelper.h"
#include "header/assimp.h"
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <fftw3.h>


#define NB_E 1024

/*!\brief dimensions initiales de la fenêtre */
static GLfloat _dim[] = {1024, 768};

static int _hauteurs[NB_E];

static GLuint _pId = 0;
static GLuint _pId2 = 0;
static GLuint _pId3 = 0;
static GLuint _pId4 = 0;
static GLuint _pIdmodel3d = 0;
static GLuint _pIdTexte = 0;
static GLuint _pIdSp = 0;

/// @brief voiture
static GLuint _model1 = 0;
/// @brief adversaire wtf 
static GLuint _model2 = 0;
/// @brief Panneau ou l'on peut écrire 
static GLuint _quad = 0;
/// @brief sert pour le podium
static GLuint _cubeId = 0;
/// @brief route
static GLuint _roadId = 0;
/// @brief ligne de depart
static GLuint _depId = 0;
/// @brief pilone bord de route
static GLuint _pilo = 0;


/// @brief stocke le texte
static GLuint _textId_1 = 0;
static GLuint _textId_2 = 0;
static GLuint _textId_VS = 0;
static GLuint _textId_FINISH = 0;
static GLuint _textId_WINNER = 0;

static GLfloat _PositionCube1[3] = {-1.0f, 0.0f, -5.5f};
static GLfloat _PositionCube2[3] = {4.0f, 0.0f, -5.5f};
static GLfloat _scaleBordure[3] = {0.1f, 0.1f, 1.0f};

static GLfloat lum[4] = {0.0, 10.0, 0.0, 1.0};

void DepArr(float z, float x);
void Roaddraw(float anim_z);
void Spectredraw();
void SpectredrawL();
void SpectredrawR();
void podiumDraw(void);
void gestion_voiture(void);
void gestion_model2(void);
void backgroundcolor(void);

static void initText(GLuint * ptId, const char * text);
void PlayertextDraw(GLuint textId, GLfloat a, GLfloat Position[3]);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

static double get_dt(void) {
	static double t0 = 0.0f;
	double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
	t0 = t;
	return dt;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void player1(int state) {
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
    
    PlayertextDraw(_textId_1 ,a, _PositionCube1);

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
    backgroundcolor();
    return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void player2(int state) {
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
      PlayertextDraw(_textId_2, a, _PositionCube2);

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
      backgroundcolor();
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
      a += 2.0f * M_PI * get_dt();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      PlayertextDraw(_textId_WINNER, a, _PositionCube2);

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
      backgroundcolor();
      return;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void versus(int state) {
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
    
    glUseProgram(_pIdTexte);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textId_VS);
    glUniform1i(glGetUniformLocation(_pIdTexte, "inv"), 1);
    glUniform1i(glGetUniformLocation(_pIdTexte, "tex"), 0);
    gl4duBindMatrix("modelCar");
    gl4duLoadIdentityf();
    gl4duTranslatef(0.0f, 0.0f, -5.0f);
    gl4dgDraw(_quad);
    gl4duSendMatrices();

	  glEnable(GL_DEPTH_TEST);

    // On s'addresse a la view
    gl4duBindMatrix("projectionView");
    gl4duLoadIdentityf();
    gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
    gl4duLookAtf(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, -5.0f, 0.0f, 1.0f, 0.0f);
    gl4duSendMatrices();

    //avant le draw modification de la matrice en question 
    glUseProgram(0);
    a += 2.0f * M_PI * get_dt();
    backgroundcolor();
    return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void depart1(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      a += 2.0f * M_PI * get_dt();
      /* JOUER L'ANIMATION */
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      gestion_voiture();
      assimpDrawScene(_model1);
      gl4duSendMatrices();
      
      gestion_model2();
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      DepArr(0.0f, 0.0f);
      gl4duSendMatrices();

      Spectredraw();

      Roaddraw(0.0f);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf(0, 4.0f, 5.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();

      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      backgroundcolor();
      return;
  }
}

void depart2(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      a += 2.0f * M_PI * get_dt();
      /* JOUER L'ANIMATION */
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      gestion_voiture();
      // gl4duTranslatef(0.0f, 0.0f, cos(a*0.2));
      assimpDrawScene(_model1);
      gl4duSendMatrices();
      
      gestion_model2();
      // gl4duTranslatef(0.0f, 0.0f, cos(a*0.1));
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      DepArr(a*2, 0.0f);
      gl4duSendMatrices();

      Spectredraw();

      Roaddraw(a);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf(0, 4.0f, 5.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();

      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      backgroundcolor();
      return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void arrivee1(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
      a += 2.0f * M_PI * get_dt();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      gestion_voiture();
      gl4duTranslatef(0.0f, 0.0f, cos(a*0.2)/2);
      assimpDrawScene(_model1);
      gl4duSendMatrices();

      
      gestion_model2();
      gl4duTranslatef(sin(a*0.2)/2, 0.0f, 0.0f);
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      DepArr(a*2.5, 200);
      gl4duSendMatrices();
      
      Spectredraw();

      Roaddraw(a*0.5);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf((5.0f * sin(a*0.1)) + 1.5f, 4.0f, 5.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();

      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      backgroundcolor();
      return;
  }
}

void arrivee2(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
      a += 2.0f * M_PI * get_dt();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      gestion_voiture();
      gl4duTranslatef(0.0f, 0.0f, sin(a*0.1)/3);
      assimpDrawScene(_model1);
      gl4duSendMatrices();

      
      gestion_model2();
      gl4duTranslatef(sin(a*0.1), 0.0f, 0.0f);
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      DepArr(0, 0);
      gl4duSendMatrices();

      Spectredraw();
      
      Roaddraw(0);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf(10.0f, 2.0f, -13.0f , 1.5f, -0.9999f, -13.0f, 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();

      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      backgroundcolor();
      return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief / td pour tout droit animation prévue pour le ligne droite
/// @param state 
void toutdroit(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
      a += 2.0f * M_PI * get_dt();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      gestion_voiture();
      gl4duTranslatef(0.0f, 0.0f, cos(a*0.2)/2);
      assimpDrawScene(_model1);
      gl4duSendMatrices();

      
      gestion_model2();
      gl4duTranslatef(sin(a*0.2)/2, 0.0f, 0.0f);
      assimpDrawScene(_model2);
      gl4duSendMatrices();

      Spectredraw();

      Roaddraw(a*0.5);
      gl4duSendMatrices();

      gl4duBindMatrix("projectionView");
      gl4duLoadIdentityf();
      gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _dim[1]) / _dim[0], (1.0f * _dim[1]) / _dim[0], 1.0f, 1000.0f);
      gl4duLookAtf((5.0f * sin(a*0.1)) + 1.5f, 4.0f, 5.0f , _PositionCube2[0]/2, _PositionCube2[1], _PositionCube2[2], 0.0f, 1.0f, 0.0f);
      gl4duSendMatrices();

      //avant le draw modification de la matrice en question 
      glUseProgram(0);
      backgroundcolor();
      return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief / vd pour virage droite animation prévue pour le ligne droite
/// @param state 
void viraged(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
    a += 2.0f * M_PI * get_dt();
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

    SpectredrawR();

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
    backgroundcolor();
    return;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief / vd pour virage droite animation prévue pour le ligne droite
/// @param state 
void virageg(int state){
  /* INITIALISEZ VOS VARIABLES */
  static GLfloat a = 0.0f;
  int l = ahGetAudioStreamLength(), i;
  short * s = (short *)ahGetAudioStream();
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      for(i = 0; i < l / 4; ++i)
        _hauteurs[i] = s[2 * i] >> 10;
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
    a += 2.0f * M_PI * get_dt();
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

    SpectredrawL();

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
    backgroundcolor();
    return;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Spectredraw(){
  int i, j;
  // GLfloat r,g,b;
  // r = (rand()%100)/100;
  // g = (rand()%100)/100;
  // b = (rand()%100)/100;
  // GLfloat * R = &r , *G = &g , *B = &b;

  glUseProgram(_pIdSp);
    gl4duBindMatrix("modelView");
    // ligne de gauche 
    gl4duLoadIdentityf();
    gl4duTranslatef(-12.5f, 0.0f, 100.0f);
    gl4duRotatef(90, 0, 1.0f, 0);
    for(i = 0; i < NB_E; ++i) {
      // glUniform4fv(glGetUniformLocation(_pIdSp, "R"), 1, R);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "G"), 1, G);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "B"), 1, B);
      GLfloat x = 500.0f * (i / (NB_E - 1.0f)) - 1.0f;
      gl4duPushMatrix();
      gl4duTranslatef(x, 0.0f, 0.0f);
      for(j = 0; j <= abs(_hauteurs[i]); ++j) {
        GLfloat y = (_hauteurs[i] < 0 ? -j : j);
        gl4duPushMatrix();
        gl4duTranslatef(0.0f, y, 0.0f);
        gl4duSendMatrices();
        gl4duPopMatrix();
        gl4dgDraw(_cubeId);
      }
      gl4duPopMatrix();
    }

    // ligne de droite
    gl4duLoadIdentityf();
    gl4duTranslatef(15.0f, 0.0f, 100.0f);
    gl4duRotatef(90, 0, 1.0f, 0);
    for(i = 0; i < NB_E; ++i) {
      // glUniform4fv(glGetUniformLocation(_pIdSp, "R"), 1, R);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "G"), 1, G);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "B"), 1, B);
      GLfloat x = 500.0f * (i / (NB_E - 1.0f)) - 1.0f;
      gl4duPushMatrix();
      gl4duTranslatef(x, 0.0f, 0.0f);
      for(j = 0; j <= abs(_hauteurs[i]); ++j) {
        GLfloat y = (_hauteurs[i] < 0 ? -j : j);
        gl4duPushMatrix();
        gl4duTranslatef(0.0f, y, 0.0f);
        // gl4duScalef(0.02f, 0.02f, 0.02f);
        gl4duSendMatrices();
        gl4duPopMatrix();
        gl4dgDraw(_cubeId);
      }
      gl4duPopMatrix();
    }
    
} 

void SpectredrawL(){
  int i, j;
  // GLfloat r,g,b;
  // r = (rand()%100)/100;
  // g = (rand()%100)/100;
  // b = (rand()%100)/100;
  // GLfloat * R = &r , *G = &g , *B = &b;

  glUseProgram(_pIdSp);
    gl4duBindMatrix("modelView");
    // ligne de gauche 
    gl4duLoadIdentityf();
    gl4duTranslatef(-12.5f, 0.0f, 100.0f);
    gl4duRotatef(90, 0, 1.0f, 0);
    for(i = 0; i < NB_E; ++i) {
      // glUniform4fv(glGetUniformLocation(_pIdSp, "R"), 1, R);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "G"), 1, G);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "B"), 1, B);
      GLfloat x = 500.0f * (i / (NB_E - 1.0f)) - 1.0f;
      gl4duPushMatrix();
      gl4duTranslatef(x, 0.0f, 0.0f);
      for(j = 0; j <= abs(_hauteurs[i]); ++j) {
        GLfloat y = (_hauteurs[i] < 0 ? -j : j);
        gl4duPushMatrix();
        gl4duTranslatef(0.0f, y, 0.0f);
        // gl4duScalef(0.02f, 0.02f, 0.02f);
        gl4duSendMatrices();
        gl4duPopMatrix();
        gl4dgDraw(_cubeId);
      }
      gl4duPopMatrix();
    }
} 

void SpectredrawR(){
  int i, j;
  // GLfloat r,g,b;
  // r = (rand()%100)/100;
  // g = (rand()%100)/100;
  // b = (rand()%100)/100;
  // GLfloat * R = &r , *G = &g , *B = &b;

  glUseProgram(_pIdSp);

    gl4duBindMatrix("modelView");
    // ligne de droite
    gl4duLoadIdentityf();
    gl4duTranslatef(15.0f, 0.0f, 100.0f);
    gl4duRotatef(90, 0, 1.0f, 0);
    for(i = 0; i < NB_E; ++i) {
      // glUniform4fv(glGetUniformLocation(_pIdSp, "R"), 1, R);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "G"), 1, G);
      // glUniform4fv(glGetUniformLocation(_pIdSp, "B"), 1, B);
      GLfloat x = 500.0f * (i / (NB_E - 1.0f)) - 1.0f;
      gl4duPushMatrix();
      gl4duTranslatef(x, 0.0f, 0.0f);
      for(j = 0; j <= abs(_hauteurs[i]); ++j) {
        GLfloat y = (_hauteurs[i] < 0 ? -j : j);
        gl4duPushMatrix();
        gl4duTranslatef(0.0f, y, 0.0f);
        // gl4duScalef(0.02f, 0.02f, 0.02f);
        gl4duSendMatrices();
        gl4duPopMatrix();
        gl4dgDraw(_cubeId);
      }
      gl4duPopMatrix();
    }
    
} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DepArr(float z, float x){

  glUseProgram(_pId);
  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duTranslatef(-10.0f + 1.5f, 0.0f, -13.0f + z - x);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);
  gl4duTranslatef(0.0f, 2.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);
  gl4duTranslatef(0.0f, 2.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);
  gl4duTranslatef(0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);

  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duTranslatef(10.0f + 1.5f, 0.0f, -13.0f + z - x);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);
  gl4duTranslatef(0.0f, 2.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);
  gl4duTranslatef(0.0f, 2.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);
  gl4duTranslatef(0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  gl4dgDraw(_pilo);

  glUseProgram(_pId4);
  SDL_Surface* image = IMG_Load("images/grille.jpg");
  SDL_Surface* surface = SDL_CreateRGBSurface(0, image->w, image->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);

  gl4duBindMatrix("modelRoad");
  gl4duLoadIdentityf();
  gl4duTranslatef(1.5f, -0.9999f, -13.0f + z - x);
  gl4duScalef(10, 2, 2);
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
  
  gl4dgDraw(_depId);
  glBindTexture(GL_TEXTURE_2D, _tex_id);

  gl4duBindMatrix("modelRoad");
  gl4duLoadIdentityf();
  gl4duTranslatef(1.5f, 5.0f, -13.0f + z - x);
  gl4duScalef(10, 1, 1);
  gl4duSendMatrices();

  gl4dgDraw(_depId);
  glBindTexture(GL_TEXTURE_2D, _tex_id);

  glUseProgram(_pIdTexte);
  gl4duBindMatrix("modelRoad");
  gl4duLoadIdentityf();

  glUseProgram(_pIdTexte);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textId_FINISH);
  glUniform1i(glGetUniformLocation(_pIdTexte, "inv"), 1);
  glUniform1i(glGetUniformLocation(_pIdTexte, "tex"), 0);
  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duTranslatef(1.5f, 5.0f, -12.9f + (z * 4));
  gl4duScalef(3.0f, 1.0f, 1.0f);
  gl4dgDraw(_quad);
  gl4duSendMatrices();
	glEnable(GL_DEPTH_TEST);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PlayertextDraw(GLuint textId, GLfloat a, GLfloat Position[3]){

  glUseProgram(_pIdTexte);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textId);
  glUniform1i(glGetUniformLocation(_pIdTexte, "inv"), 1);
  glUniform1i(glGetUniformLocation(_pIdTexte, "tex"), 0);
  gl4duBindMatrix("modelCar");
  gl4duLoadIdentityf();
  gl4duTranslatef(Position[0], Position[1] + 2.5f, Position[2]);
  if (textId == _textId_2){
    gl4duRotatef(180.0f, 0, 1, 0);
  }
  if (textId == _textId_WINNER){
    gl4duRotatef(190.0f, 0, 1, 0);
  }
  gl4dgDraw(_quad);
  gl4duSendMatrices();

	glEnable(GL_DEPTH_TEST);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void initText(GLuint * ptId, const char * text) {
  static int firstTime = 1;
  SDL_Color c = {255, 255, 255, 255};
  SDL_Surface * d, * s;
  TTF_Font * font = NULL;
  if(firstTime) {
    /* initialisation de la bibliothèque SDL2 ttf */
    if(TTF_Init() == -1) {
      fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
      exit(2);
    }
    firstTime = 0;
  }
  if(*ptId == 0) {
    /* initialisation de la texture côté OpenGL */
    glGenTextures(1, ptId);
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  /* chargement de la font */
  if( !(font = TTF_OpenFont("texture/DejaVuSans-Bold.ttf", 128)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  /* création d'une surface SDL avec le texte */
  d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 2048);
  if(d == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors du TTF_RenderText\n");
    return;
  }
  /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(s);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  /* transfert vers la texture OpenGL */
  glBindTexture(GL_TEXTURE_2D, *ptId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  // fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);
  SDL_FreeSurface(s);
  TTF_CloseFont(font);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void backgroundcolor(void){
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void animationsInit(void) {
  srand(0000);
  initText(&_textId_1, "Player 1");
  initText(&_textId_2, "Player 2");
  initText(&_textId_WINNER, " VAINQUEUR ");
  initText(&_textId_VS, "Vs");
  initText(&_textId_FINISH, " DEPART ");

  _pId = gl4duCreateProgram("<vs>shaders/player.vs", "<fs>shaders/hello.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/player.vs", "<fs>shaders/player.fs", NULL);
  _pId3 = gl4duCreateProgram("<vs>shaders/road.vs", "<fs>shaders/road.fs", NULL);
  _pId4 = gl4duCreateProgram("<vs>shaders/roadsol.vs", "<fs>shaders/roadsol.fs", NULL);
  _pIdTexte = gl4duCreateProgram("<vs>shaders/credits.vs", "<fs>shaders/credits.fs", NULL);
  _pIdmodel3d = gl4duCreateProgram("<vs>shaders/model.vs", "<fs>shaders/model.fs", NULL);

  glUseProgram(_pIdmodel3d);
  glUniform1i(glGetUniformLocation(_pIdmodel3d, "tex"), 0);
  glUniform4fv(glGetUniformLocation(_pIdmodel3d, "lumpos"), 1, lum);

  SDL_GL_SetSwapInterval(1);
  _pIdSp = gl4duCreateProgram("<vs>shaders/spectre.vs", "<fs>shaders/spectre.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelView");

  gl4duGenMatrix(GL_FLOAT, "projectionView");

  _model1 = assimpGenScene("texture/Chevrolet_Camaro_SS_High.obj");
  _model2 = assimpGenScene("texture/bath.obj");

  if(!_cubeId)
    _cubeId = gl4dgGenCubef();
  if(!_roadId)
    _roadId = gl4dgGenQuadf();
  if(!_depId)
    _depId = gl4dgGenQuadf();
  if(!_quad)
    _quad = gl4dgGenQuadf();
  if (!_pilo)
  _pilo = gl4dgGenCylinderf(8, GL_TRUE);
}
