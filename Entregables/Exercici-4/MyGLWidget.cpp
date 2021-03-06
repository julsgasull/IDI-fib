#include "MyGLWidget.h"

#include <iostream>

/*----------------------------------------------------*/
/*------------- CONSTRUCTORA/DESTRUCTORA -------------*/
/*----------------------------------------------------*/

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
  posM1 = glm::vec3(-1.5,-2.0, 1.5);
  posM2 = glm::vec3( 1.5,-2.0, 1.5);
}
MyGLWidget::~MyGLWidget ()
{
  if (program != NULL) delete program;
}

/*----------------------------------------------------*/
/*------------------- INICIALITZAR -------------------*/
/*----------------------------------------------------*/
void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  createBuffersModel();
  createBuffersTerraIParet();

  iniEscena();
  //iniCamera();    //cridat a iniEscena( )
  iniFocus ();
}
void MyGLWidget::createBuffersModel ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  patr.load("./models/Patricio.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel ();

  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Patr);
  glBindVertexArray(VAO_Patr);

  // Creació dels buffers del model patr
  GLuint VBO_Patr[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Patr);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3, patr.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}
void MyGLWidget::calculaCapsaModel ()
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = patr.vertices()[0];
  miny = maxy = patr.vertices()[1];
  minz = maxz = patr.vertices()[2];
  for (unsigned int i = 3; i < patr.vertices().size(); i+=3)
  {
    if (patr.vertices()[i+0] < minx) minx = patr.vertices()[i+0];
    if (patr.vertices()[i+0] > maxx) maxx = patr.vertices()[i+0];
    if (patr.vertices()[i+1] < miny) miny = patr.vertices()[i+1];
    if (patr.vertices()[i+1] > maxy) maxy = patr.vertices()[i+1];
    if (patr.vertices()[i+2] < minz) minz = patr.vertices()[i+2];
    if (patr.vertices()[i+2] > maxz) maxz = patr.vertices()[i+2];
  }
  escala1 = 2.0/(maxy-miny);
  escala2 = 1.0/(maxy-miny);
  centreBasePatr[0] = (minx+maxx)/2.0; centreBasePatr[1] = miny; centreBasePatr[2] = (minz+maxz)/2.0;
}
void MyGLWidget::createBuffersTerraIParet ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[12] =
  {
  	glm::vec3(-2.0,-2.0, 2.0),
  	glm::vec3( 2.0,-2.0, 2.0),
  	glm::vec3(-2.0,-2.0,-2.0),
  	glm::vec3(-2.0,-2.0,-2.0),
  	glm::vec3( 2.0,-2.0, 2.0),
  	glm::vec3( 2.0,-2.0,-2.0),
  	glm::vec3(-2.0,-2.0,-2.0),
  	glm::vec3( 2.0,-2.0,-2.0),
  	glm::vec3(-2.0, 2.0,-2.0),
  	glm::vec3(-2.0, 2.0,-2.0),
  	glm::vec3( 2.0,-2.0,-2.0),
  	glm::vec3( 2.0, 2.0,-2.0)
  };

  // VBO amb la normal de cada vèrtex
  glm::vec3 norm1 (0,1,0);
  glm::vec3 norm2 (0,0,1);
  glm::vec3 normterra[12] =
  {
  	norm1, norm1, norm1, norm1, norm1, norm1, // la normal (0,1,0) per als primers dos triangles
  	norm2, norm2, norm2, norm2, norm2, norm2  // la normal (0,0,1) per als dos últims triangles
  };

  // Definim el material del terra
  glm::vec3 amb  ( 0.2, 0.0, 0.2);
  glm::vec3 diff ( 0.2, 0.2, 0.6);
  glm::vec3 spec ( 0.8, 0.8, 0.8);
  float shin = 100;

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matambterra[12] =
  {
    amb, amb, amb,
    amb, amb, amb,
    amb, amb, amb,
    amb, amb, amb
  };
  glm::vec3 matdiffterra[12] =
  {
    diff, diff, diff,
    diff, diff, diff,
    diff, diff, diff,
    diff, diff, diff
  };
  glm::vec3 matspecterra[12] =
  {
    spec, spec, spec,
    spec, spec, spec,
    spec, spec, spec,
    spec, spec, spec
  };
  float matshinterra[12] =
  {
    shin, shin, shin,
    shin, shin, shin,
    shin, shin, shin,
    shin, shin, shin
  };

  // Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[6];
  glGenBuffers(6, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matambterra), matambterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffterra), matdiffterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspecterra), matspecterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshinterra), matshinterra, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}
void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag");
  vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc   = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc   = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc   = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc  = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc  = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc  = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc    = glGetUniformLocation (program->programId(), "TG");
  projLoc     = glGetUniformLocation (program->programId(), "proj");
  viewLoc     = glGetUniformLocation (program->programId(), "view");

  posFocusLoc0 = glGetUniformLocation (program->programId(), "posFocus0");
  posFocusLoc1 = glGetUniformLocation (program->programId(), "posFocus1");
  posFocusLoc2 = glGetUniformLocation (program->programId(), "posFocus2");
  focusTypeLoc = glGetUniformLocation (program->programId(), "focusType");
}
void MyGLWidget::iniEscena ()
{
  // valors calculats previament
  Pmin        = glm::vec3(-2.0,-2.0,-2.0);
  Pmax        = glm::vec3( 2.0, 2.0, 2.0);
  angleEuler  = glm::vec3(0.0, 0.0, 0.0);
  perspectiva = true;
  iniCamera ();
}
void MyGLWidget::iniCamera ()
{
	/*--------------------- VIEW ---------------------*/
	VRP[0] = (Pmin[0]+Pmax[0]) / 2.0;            // VRP [0]
	VRP[1] = (Pmin[1]+Pmax[1]) / 2.0,            // VRP [1]
	VRP[2] = (Pmin[2]+Pmax[2]) / 2.0;            // VRP [2]
	radi = sqrt                                  // radi (igual amb Pmax)
  (
    (Pmin[0]-VRP[0])*(Pmin[0]-VRP[0]) +
		(Pmin[1]-VRP[1])*(Pmin[1]-VRP[1]) +
		(Pmin[2]-VRP[2])*(Pmin[2]-VRP[2])
  );
	dist = 2*radi;                               // d (> r)
	OBS = VRP + dist*glm::vec3(0.0, 0.0, 1.0); 	 // OBS
	UP = glm::vec3(0.0, 1.0, 0.0);   	           // UP (per defecte)
	viewTransform();                             // funció

	/*--------------------- PROJECTION ---------------------*/
	alfaIni = asin(radi/dist); FOV = 2 * alfaIni;  	// FOV (canvia al resize)
	// raV al resize
	znear = dist - radi; zfar  = dist + radi;        // Znear i Zfar
	// funció // projectTransform(); --> no fa falta ja que la crido al resizeGL
	right = radi; left = -radi; top = radi; bottom = -radi;  // ortho
}
void MyGLWidget::iniFocus ()
{
  focusT = 0;
  posF0 = glm::vec3( 0.0, 0.0, 0.0);
  posF1 = glm::vec3(-1.5, 1.0, 1.5);
  posF2 = glm::vec3( 1.5, 1.0, 1.5);
  posicioFocus();
}
void MyGLWidget::posicioFocus ()
{
  glUniform3fv (posFocusLoc0, 1, &posF0[0]);
  glUniform3fv (posFocusLoc1, 1, &posF1[0]);
  glUniform3fv (posFocusLoc2, 1, &posF2[0]);
}
void MyGLWidget::changeFocusType ()
{
  glUniform1i(focusTypeLoc, focusT);
}



/*----------------------------------------------------*/
/*----------------------- PAINT ----------------------*/
/*----------------------------------------------------*/
void MyGLWidget::paintGL ()
{
  GLint vp[4]; glGetIntegerv(GL_VIEWPORT, vp);
  int RealWidth = vp[2]; int RealHeight = vp[3];
  glViewport (0, 0, RealWidth, RealHeight); // Aquesta crida no caldria perquè Qt la fa de forma automàtica amb aquests paràmetres

  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem el VAO per a pintar el terra
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLES, 0, 12);

  // Activem el VAO per a pintar els Patricios
  glBindVertexArray (VAO_Patr);
  modelTransformModel1 ();
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);
  modelTransformModel2 ();
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  glBindVertexArray(0);
}

/*----------------------------------------------------*/
/*---------------------- RESIZE ----------------------*/
/*----------------------------------------------------*/
void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, w, h);
  raV = float (w) / float (h);		// raV
  if (perspectiva)
  {
    if (raV < 1.0) FOV = 2.0*atan(tan(alfaIni)/raV);
    else FOV = alfaIni*2;
  } else
  {
    if (raV > 1.0)
    {
      left  = -radi*raV;
      right =  radi*raV;
    } else
    {
      bottom = -radi/raV;
      top    =  radi/raV;
    }
  }
  projectTransform();	    			// funció
}

/*----------------------------------------------------*/
/*-------------------- TRANSFORM ---------------------*/
/*----------------------------------------------------*/
void MyGLWidget::modelTransformModel1 ()
{
  float angle = -((float)5*(float)M_PI)/(float)4;//5pi/4
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG, posM1);
  //TG = glm::translate(TG, glm::vec3(-1.5,-2.0, 1.5));
  TG = glm::rotate(TG, angle, glm::vec3(0.0, 1.0, 0.0));
  TG = glm::scale(TG, glm::vec3(escala1, escala1, escala1));
  TG = glm::translate(TG, -centreBasePatr);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::modelTransformModel2 ()
{
  float angle = ((float)5*(float)M_PI)/(float)4;//5pi/4
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::translate(TG, posM2);
  //TG = glm::translate(TG, glm::vec3(1.5, -2.0, 1.5));
  TG = glm::rotate(TG, angle, glm::vec3(0.0, 1.0, 0.0));
  TG = glm::scale(TG, glm::vec3(escala2, escala2, escala2));
  TG = glm::translate(TG, -centreBasePatr);

  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::modelTransformTerra ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj (1.0f);
	if (perspectiva) Proj = glm::perspective (FOV, raV, znear, zfar);
	else             Proj = glm::ortho (left, right, bottom, top, znear, zfar);
	glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}
void MyGLWidget::viewTransform ()
{
  // glm::mat4 View = glm::lookAt (OBS, VRP, UP);
	glm::mat4 View (1.0f);
	View = glm::translate  (View, glm::vec3(0.0, 0.0, -dist));
	View = glm::rotate     (View, -angleEuler[2], glm::vec3(0.0, 0.0, 1.0));	// z phy (y rara)
	View = glm::rotate     (View,  angleEuler[0], glm::vec3(1.0, 0.0, 0.0)); 	// x theta (0)
	View = glm::rotate     (View, -angleEuler[1], glm::vec3(0.0, 1.0, 0.0)); 	// y psi (tridente)
	View = glm::translate  (View, -VRP);
	glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

/*----------------------------------------------------*/
/*---------------------- EVENTS ----------------------*/
/*----------------------------------------------------*/
void MyGLWidget::keyPressEvent (QKeyEvent* event)
{
  makeCurrent();
  switch (event->key())
  {
    case Qt::Key_O:   // canvia òptica entre perspectiva i axonomètrica
      perspectiva = !perspectiva;
      projectTransform ();
      break;
    case Qt::Key_F:
      if      (focusT == 0) focusT = 1;
      else if (focusT == 1) focusT = 2;
      else if (focusT == 2) focusT = 0;
      changeFocusType();
      break;
    case Qt::Key_Up: // diagonals de cara als patricios
      if (focusT == 1 and (posF1[0]+0.25 <= 1.5 or posF1[2]-0.25 >= -1.5))
      { // Patricio 1 [focus, model]
        posF1[0] += 0.25; posM1[0] += 0.25;   //+X
        posF1[2] -= 0.25; posM1[2] -= 0.25;   //-Z
      } else if (focusT == 2 and (posF2[0]-0.25 >= -1.5 or posF2[0]-0.25 >= -1.5))
      { // Patricio 2 [focus, model]
        posF2[0] -= 0.25; posM2[0] -= 0.25;   //-X
        posF2[2] -= 0.25; posM2[2] -= 0.25;   //-Z
      }
      posicioFocus ();  paintGL (); // funcions
      break;
    case Qt::Key_Down:
      if (focusT == 1 and (posF1[0]-0.25 >= -1.5 or posF1[2]+0.25 <= 1.5))
      { // Patricio 1 [focus, model]
        posF1[0] -= 0.25; posM1[0] -= 0.25;   //-X
        posF1[2] += 0.25; posM1[2] += 0.25;   //+Z
      } else if (focusT == 2 and (posF2[0]+0.25 <= 1.5 or posF2[0]+0.25 <= 1.5))
      { // Patricio 2 [focus, model]
        posF2[0] += 0.25; posM2[0] += 0.25;   //+X
        posF2[2] += 0.25; posM2[2] += 0.25;   //+Z
      }
      posicioFocus ();  paintGL (); // funcions
      break;
    default: event->ignore(); break;
  }
  update();
}
void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton && ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}
void MyGLWidget::mouseReleaseEvent (QMouseEvent *)
{
  DoingInteractive = NONE;
}
void MyGLWidget::mouseMoveEvent (QMouseEvent *e)
{
  makeCurrent();
  // Aqui cal que es calculi i s'apliqui la rotacio o el zoom com s'escaigui...
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleEuler[1] += (e->x() - xClick) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}
