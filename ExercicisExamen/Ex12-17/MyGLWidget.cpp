#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
  franges = false;
  R = 0.8; G = 0.0; B = 0.8;  // Ex17: color inicial terra (rosa)
}
MyGLWidget::~MyGLWidget ()
{
  if (program != NULL) delete program;
}

/*-----------------------------------------------*/
/*------------------- INICIAR -------------------*/
/*-----------------------------------------------*/
void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable (GL_DEPTH_TEST);
  carregaShaders(); createBuffersPatricio(); createBuffersTerraIParet();

  iniEscena(); /*iniCamera();*/ iniFocus();

  // posició inicial patricio
  posPatricio1 = glm::vec3(-2.0, -1.0, 0.0);
}
void MyGLWidget::createBuffersPatricio ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  patr.load("./models/Patricio.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaPatricio ();

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
void MyGLWidget::createBuffersTerraIParet ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[12] =
  {
    glm::vec3(-2.0,-1.0, 2.0),
    glm::vec3( 2.0,-1.0, 2.0),
    glm::vec3(-2.0,-1.0,-2.0),
    glm::vec3(-2.0,-1.0,-2.0),
    glm::vec3( 2.0,-1.0, 2.0),
    glm::vec3( 2.0,-1.0,-2.0),
    glm::vec3(-2.0,-1.0,-2.0),
    glm::vec3( 2.0,-1.0,-2.0),
    glm::vec3(-2.0, 1.0,-2.0),
    glm::vec3(-2.0, 1.0,-2.0),
    glm::vec3( 2.0,-1.0,-2.0),
    glm::vec3( 2.0, 1.0,-2.0)
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
  glm::vec3 amb( 0.2, 0.0, 0.2);
  glm::vec3 diff(R, G, B);
  glm::vec3 spec( 0.8, 0.8, 0.8);
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
  glGenVertexArrays(1, &VAO_Terra); glBindVertexArray(VAO_Terra);

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
  QOpenGLShader fs (QOpenGLShader::Fragment, this); QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/fragshad.frag"); vs.compileSourceFile("shaders/vertshad.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  program->addShader(&fs); program->addShader(&vs);   // Li afegim els shaders corresponents
  program->link();                                    // Linkem el program
  program->bind();                                    // Indiquem que aquest és el program que volem usar

  // Obtenim identificadora per als atributs del vertex shader
  vertexLoc   = glGetAttribLocation (program->programId(), "vertex");
  normalLoc   = glGetAttribLocation (program->programId(), "normal");
  matambLoc   = glGetAttribLocation (program->programId(), "matamb");
  matdiffLoc  = glGetAttribLocation (program->programId(), "matdiff");
  matspecLoc  = glGetAttribLocation (program->programId(), "matspec");
  matshinLoc  = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc        = glGetUniformLocation (program->programId(), "TG");
  projLoc         = glGetUniformLocation (program->programId(), "proj");
  viewLoc         = glGetUniformLocation (program->programId(), "view");
  frangesLoc      = glGetUniformLocation (program->programId(), "franges");       // Ex9: franges blanques i negres
  posFocusLoc     = glGetUniformLocation (program->programId(), "posFocus");      // Ex14: canviar posició focus sobre patricio1
}
void MyGLWidget::iniEscena ()
{
  // valors calculats previament
  Pmin = glm::vec3(-2.0,-1.0,-2.0);
  Pmax = glm::vec3( 2.0, 1.5, 2.0);
  angleEuler = glm::vec3(0.0, 0.0, 0.0);
  perspectiva = true;
  cameraPatricio  = false;
  iniCamera ();
  iniFocus ();
}
void MyGLWidget::iniCamera ()
{
  /*--------------------- VIEW ---------------------*/
  // VRP
  VRP[0] = (Pmin[0]+Pmax[0]) / 2.0;
  VRP[1] = (Pmin[1]+Pmax[1]) / 2.0,
  VRP[2] = (Pmin[2]+Pmax[2]) / 2.0;
  // radi (igual amb Pmax)
  radi = sqrt
  (
    (Pmin[0]-VRP[0])*(Pmin[0]-VRP[0]) +
    (Pmin[1]-VRP[1])*(Pmin[1]-VRP[1]) +
    (Pmin[2]-VRP[2])*(Pmin[2]-VRP[2])
  );
  // d (> r)
  dist = 2*radi;
  // OBS
  OBS = VRP + dist*glm::vec3(0.0, 0.0, 1.0);
  // UP (per defecte)
  UP = glm::vec3(0.0, 1.0, 0.0);
  // funció
  viewTransform();

  /*--------------------- PROJECTION ---------------------*/
  // FOV (canvia al resize)
  alfaIni = asin(radi/dist); FOV = 2 * alfaIni;
  // raV al resize
  // Znear i Zfar
  znear = dist - radi; zfar  = dist + radi;
  // funció // projectTransform(); --> no fa falta ja que la crido al resizeGL

  // ortho
  right = radi; left = -radi; top = radi; bottom = -radi;
}
void MyGLWidget::iniFocus ()
{
  // posició focus
  posFocus = glm::vec3(posPatricio1.x, 0.5, posPatricio1.z);
  posicioFocus();
}
void MyGLWidget::posicioFocus ()
{
  glUniform3fv (posFocusLoc, 1, &posFocus[0]);
}
void MyGLWidget::setCameraPatricio ()
{
  // viewTransform values
  OBS = glm::vec3(posPatricio1.x, 0.5, posPatricio1.z);
  VRP = glm::vec3(2.0, 0.25, 1.0);   // centre Patricio2
  UP  = glm::vec3(0.0, 1.00, 0.0);

  // projectTransform values
  znear       = 0.1;
  zfar        = 6;
  FOV         = (float)M_PI/2.0;  // 90º
  alfaIni     = FOV/2;

  viewTransform(); projectTransform();
}
void MyGLWidget::setCameraInicial ()
{
  // viewTransform values
  VRP = centreEsc;

  // projectTransform values
  znear       = dist - radi;
  zfar        = dist + radi;
  alfaIni     = asin(radi/dist);
  FOV         = 2*alfaIni;

  viewTransform(); projectTransform();
}


/*-----------------------------------------------*/
/*-------------------- PAINT --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::paintGL ()
{
  // glViewport (0, 0, width(), height()); // Aquesta crida no caldria perquè Qt la fa de forma automàtica amb aquests paràmetres

  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // TERRA
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLES, 0, 12);

  // PATRICIO 1
  glBindVertexArray (VAO_Patr);
  modelTransformPatricio1 ();
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  // PATRICIO 2
  glBindVertexArray (VAO_Patr);
  modelTransformPatricio2 ();
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  // Descactivar
  glBindVertexArray(0);
}

/*-----------------------------------------------*/
/*------------------- RESIZE --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::resizeGL (int w, int h)
{
  glViewport (0, 0, width(), height()); // Aquesta crida no caldria perquè Qt la fa de forma automàtica amb aquests paràmetres
  // Aquí anirà el codi que cal fer quan es redimensiona la finestra

  //glViewport (0, 0, w, h);	 		// viewport inicial
  raV = float (w) / float (h);		// raV

  if (perspectiva)
  {
    if (raV < 1.0)  FOV = 2.0 * atan(tan(alfaIni)/raV);
    else            FOV = 2.0 * alfaIni;
  }

  else  // ortogonal
  {
    if (raV > 1.0)  { left    = -radi*raV;  right   =  radi*raV; }
    else            { bottom  = -radi/raV;  top     =  radi/raV; }
  }
  projectTransform();	    			// funció
}

/*-----------------------------------------------*/
/*----------------- TRANSFORM -------------------*/
/*-----------------------------------------------*/
void MyGLWidget::modelTransformPatricio1 ()
{
  glm::mat4 TG(1.f);                                            // Matriu de transformació
  TG = glm::translate(TG, posPatricio1);                        // Situar al punt (-2, -1, 0)
  TG = glm::rotate(TG, float(M_PI/2.0), glm::vec3(0,1,0));      // Girar mirant eix X+
  TG = glm::scale(TG, glm::vec3(escala1, escala1, escala1));    // Escalar a altura 1
  TG = glm::translate(TG, -centreBasePatr);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::modelTransformPatricio2 ()
{
  glm::mat4 TG(1.f);                                            // Matriu de transformació
  TG = glm::translate(TG, glm::vec3(2.0,-1.0, 1.0));            // Situar al punt (2, -1, 1)
  TG = glm::rotate(TG, -float(M_PI/2.0), glm::vec3(0,1,0));     // Girar mirant eix X-
  TG = glm::scale(TG, glm::vec3(escala2, escala2, escala2));    // Escalar a altura 2.5
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
  glm::mat4 Proj (1.0f); // Matriu de projecció
  if (perspectiva or cameraPatricio)  Proj = glm::perspective (FOV, raV, znear, zfar);
  else                                Proj = glm::ortho (left, right, bottom, top, znear, zfar);
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}
void MyGLWidget::viewTransform ()
{
  glm::mat4 View (1.0f); // Matriu de posició i orientació
  if (cameraPatricio) View = glm::lookAt(OBS, VRP, UP);
	else
  {
  	View = glm::translate  (View, glm::vec3(0.0, 0.0, -dist));
  	View = glm::rotate     (View, -angleEuler[2], glm::vec3(0.0, 0.0, 1.0));	// z phy (y rara)
  	View = glm::rotate     (View,  angleEuler[0], glm::vec3(1.0, 0.0, 0.0)); 	// x theta (0)
  	View = glm::rotate     (View, -angleEuler[1], glm::vec3(0.0, 1.0, 0.0)); 	// y psi (tridente)
  	View = glm::translate  (View, -VRP);
  }
	glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

/*-----------------------------------------------*/
/*-------------------- MODEL --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::calculaCapsaPatricio ()
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
  escala1 = 1.0/(maxy-miny);
  escala2 = 2.5/(maxy-miny);
  centreBasePatr[0] = (minx+maxx)/2.0; centreBasePatr[1] = miny; centreBasePatr[2] = (minz+maxz)/2.0;
}


/*-----------------------------------------------*/
/*------------------- EVENTS --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::keyPressEvent (QKeyEvent* event)
{
  makeCurrent();
  switch (event->key())
  {
    case Qt::Key_O:  // canvia òptica entre perspectiva i axonomètrica
      if (not cameraPatricio)
      {
        perspectiva = !perspectiva;
        projectTransform ();
      }
      break;
    case Qt::Key_X: // Ex9: pintar escena franges B/W
      franges = !franges; glUniform1i(frangesLoc, franges);
      break;
    case Qt::Key_A: // Ex13: moure Patricio1 (dreta)
      if (posPatricio1.x < 2.0)
      {
        // Ex13: moure Patricio1 (esquerre)
        posPatricio1.x += 0.1; modelTransformPatricio1();
        // Ex14: moure focus igual que Patricio1
        posFocus.x += 0.1; glUniform3fv (posFocusLoc, 1, &posFocus[0]);
        // Ex15: moure camera igual que Patricio1 (si esta a sobre)
        if (cameraPatricio) { OBS[0] += 0.1; viewTransform(); }
      }
      break;
    case Qt::Key_D:
      if (posPatricio1.x > -2.0)
      {
        // Ex13: moure Patricio1 (esquerre)
        posPatricio1.x -= 0.1; modelTransformPatricio1();
        // Ex14: moure focus igual que Patricio
        posFocus.x -= 0.1; glUniform3fv (posFocusLoc, 1, &posFocus[0]);
        // Ex15: moure camera igual que Patricio1 (si esta a sobre)
        if (cameraPatricio) { OBS[0] -= 0.1; viewTransform(); }
      }
      break;
    case Qt::Key_C:
      // Ex15: canviar posicio càmera
      cameraPatricio = !cameraPatricio;
      // variar valors segons càmera

      // Ex16: checkbox per canviar posició camera
      if (cameraPatricio) { setCameraPatricio(); emit checkCameraPatricio();   }
      else                { setCameraInicial();  emit uncheckCameraPatricio(); }

      break;
    default: event->ignore(); break;
  }
  update();
}
void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x(); yClick = e->y();

  if (not cameraPatricio)
  {
    if (e->button() & Qt::LeftButton && !(e->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier)))
    {
      DoingInteractive = ROTATE;
    }
    if (e->buttons() & Qt::RightButton && !(e->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier)))
    {
      DoingInteractive = ZOOM;
    }
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
    angleEuler[1] += (e->x() - xClick) * (float)M_PI / 180.0;
    angleEuler[0] += (e->y() - yClick) * (float)M_PI / 180.0;
    viewTransform ();
  } else
  {
    if (DoingInteractive == ZOOM)
    {
      int ynew = e->y();
      if ((yClick > ynew) and (FOV-(0.1/(ynew-yClick)) > 0.0 + 0.3)) /*0.3 per error al restar*/
      {
        FOV -= 0.1/(yClick-ynew);   // si pujo mouse -> zoom in
        updateFOV (int((FOV*(float)180) / float(M_PI)));
      }
      if ((yClick < ynew) and (FOV+(0.1/(ynew-yClick)) < (float)M_PI))
      {
        FOV += 0.1/(ynew-yClick);   // si baixo mouse -> zoom out
        updateFOV (int((FOV*(float)180) / float(M_PI)));
      }
  		projectTransform();
  		yClick = ynew;
  		alfaIni = FOV/2;

      emit updateFOV(FOV*180.0/(float)M_PI);
    }
  }
  xClick = e->x(); yClick = e->y();
  update ();
}

/*-----------------------------------------------*/
/*-------------------- SLOTS --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::changeFOV (int i)              // Ex4: canviar FOV amb slider
{
    makeCurrent();
    FOV = ((float)i * (float)M_PI) / (float)180;
    projectTransform();
    update();
}
void MyGLWidget::changeToCameraPatricio(bool b) // Ex16: checkbox per canviar posició camera
{
  makeCurrent();
  cameraPatricio = b;
  if (cameraPatricio) setCameraPatricio();
  else                setCameraInicial();
  update();
}
void MyGLWidget::setRedColorFloor(int red)      // Ex17: canviar color terra amb sliders
{
  makeCurrent();
  R = float(red)/100.0;
  createBuffersTerraIParet();
  update();
}
void MyGLWidget::setGreenColorFloor(int green)  // Ex17: canviar color terra amb sliders
{
  makeCurrent();
  G = float(green)/100.0;
  createBuffersTerraIParet();
  update();
}
void MyGLWidget::setBlueColorFloor(int blue)    // Ex17: canviar color terra amb sliders
{
  makeCurrent();
  B = float(blue)/100.0;
  createBuffersTerraIParet();
  update();
}
