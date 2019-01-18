#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget  (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
  angleTeclaR = 0.0;
}
MyGLWidget::~MyGLWidget ()
{
  if (program != NULL) delete program;
}

/*-----------------------------------------------*/
/*------------------- INICIAR -------------------*/
/*-----------------------------------------------*/
void MyGLWidget::initializeGL             ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable (GL_DEPTH_TEST);
  carregaShaders();
  createBuffersPatricio ();
  createBuffersVaca ();
  createBuffersTerraIParet();

  iniEscena();
  // iniCamera(); cridat a iniEscena
}
void MyGLWidget::createBuffersPatricio    ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  patr.load("./models/Patricio.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModelPatricio ();

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
void MyGLWidget::createBuffersVaca        ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  vaca.load("./models/cow.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModelVaca ();

  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Vaca);
  glBindVertexArray(VAO_Vaca);

  // Creació dels buffers del model patr
  GLuint VBO_Vaca[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Vaca);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Vaca[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Vaca[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Vaca[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Vaca[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Vaca[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3*3, vaca.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Vaca[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vaca.faces().size()*3, vaca.VBO_matshin(), GL_STATIC_DRAW);

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
  glm::vec3 amb(0.2,0,0.2);
  glm::vec3 diff(0.8,0,0.8);
  glm::vec3 spec(0,0,0);
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
void MyGLWidget::carregaShaders           ()
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
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc      = glGetUniformLocation (program->programId(), "TG");
  projLoc       = glGetUniformLocation (program->programId(), "proj");
  viewLoc       = glGetUniformLocation (program->programId(), "view");
  pintaVacaLoc  = glGetUniformLocation (program->programId(), "pintaVaca");
  teclaXLoc     = glGetUniformLocation (program->programId(), "canviFranjes");
  teclaLLoc     = glGetUniformLocation (program->programId(), "teclaL");
}
void MyGLWidget::iniEscena                ()
{
  // valors calculats previament
  Pmin = glm::vec3(-2.0,-1.0,-2.0);
  Pmax = glm::vec3( 2.0, 1.0, 2.0);

  angleEuler = glm::vec3(0.0, 0.0, 0.0);
  perspectiva = true;
  teclaX = 0; glUniform1i(teclaXLoc, teclaX);
  teclaL = 1; glUniform1i(teclaLLoc, teclaL);
  iniCamera ();
}
void MyGLWidget::iniCamera                ()
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
	// funció 	viewTransform();


	/*--------------------- PROJECTION ---------------------*/
	// FOV (canvia al resize)
	alfaIni = asin(radi/dist); FOV = 2 * alfaIni;
	// raV al resize
	// Znear i Zfar
	znear = dist - radi; zfar  = dist + radi;
	// funció // projectTransform(); --> no fa falta ja que la crido al resizeGL

  // ortho
	right = radi; left = -radi; top = radi; bottom = -radi;

  // Exercici 7
  OBS = glm::vec3(-1.0, 1.0,-1.0);
  VRP = glm::vec3( 1.0,-0.5+0.125,0.0);
  FOV = (float)((float)M_PI/(float)3.0);
  znear = 0.1; zfar = 7.0;
  viewTransform();

}

/*-----------------------------------------------*/
/*-------------------- PAINT --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::paintGL ()
{
  //glViewport (0, 0, width(), height()); // Aquesta crida no caldria perquè Qt la fa de forma automàtica amb aquests paràmetres

  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Activem el VAO per a pintar el terra
  glBindVertexArray (VAO_Terra); modelTransformTerra (); glDrawArrays(GL_TRIANGLES, 0, 12);

  // Pintar Vaca
  glUniform1i(pintaVacaLoc, 1); //Ex8
  glBindVertexArray (VAO_Vaca); modelTransformVaca (); glDrawArrays(GL_TRIANGLES, 0, vaca.faces().size()*3);

  // Pintar Patricio
  glUniform1i(pintaVacaLoc, 0); //Ex8
  glBindVertexArray (VAO_Patr); modelTransformPatricio (); glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

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

  // if (perspectiva)
  // {
  //   if (raV < 1.0) FOV = 2.0*atan(tan(alfaIni)/raV);
  //   else FOV = alfaIni*2;
  // } else
  // {
  //   if (raV > 1.0)
  //   {
  //     left  = -radi*raV;
  //     right =  radi*raV;
  //   } else
  //   {
  //     bottom = -radi/raV;
  //     top    =  radi/raV;
  //   }
  // }
  projectTransform();	    			// funció
}

/*-----------------------------------------------*/
/*----------------- TRANSFORM -------------------*/
/*-----------------------------------------------*/
void MyGLWidget::modelTransformPatricio ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::rotate    (TG, angleTeclaR, glm::vec3(0.0, 1.0, 0.0));
  TG = glm::translate (TG, glm::vec3( 1.0, 0.125-0.5, 0.0)); // (1,-0.5,0)
  TG = glm::scale     (TG, glm::vec3(escalaPatr, escalaPatr, escalaPatr));
  TG = glm::translate (TG, -centrePatr);
  glUniformMatrix4fv  (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::modelTransformVaca     ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  TG = glm::rotate    (TG, angleTeclaR, glm::vec3(0.0, 1.0, 0.0));
  TG = glm::translate (TG, glm::vec3( 1.0, 0.25-1.0, 0.0)); // (1,-1,0)
  TG = glm::scale     (TG, glm::vec3(escalaVaca, escalaVaca, escalaVaca));
  TG = glm::rotate    (TG, -(float)((float)M_PI/(float)2.0), glm::vec3(0.0, 1.0, 0.0));
  TG = glm::rotate    (TG, -(float)((float)M_PI/(float)2.0), glm::vec3(1.0, 0.0, 0.0));
  TG = glm::translate (TG, -centreVaca);
  glUniformMatrix4fv  (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::modelTransformTerra    ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}
void MyGLWidget::projectTransform       ()
{
  glm::mat4 Proj (1.0f);
	if (perspectiva) Proj = glm::perspective (FOV, raV, znear, zfar);
	else             Proj = glm::ortho (left, right, bottom, top, znear, zfar);
	glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}
void MyGLWidget::viewTransform          ()
{
  glm::mat4 View = glm::lookAt (OBS, VRP, UP);
	// glm::mat4 View (1.0f);
	// View = glm::translate  (View, glm::vec3(0.0, 0.0, -dist));
	// View = glm::rotate     (View, -angleEuler[2], glm::vec3(0.0, 0.0, 1.0));	// z phy (y rara)
	// View = glm::rotate     (View,  angleEuler[0], glm::vec3(1.0, 0.0, 0.0)); 	// x theta (0)
	// View = glm::rotate     (View, -angleEuler[1], glm::vec3(0.0, 1.0, 0.0)); 	// y psi (tridente)
	// View = glm::translate  (View, -VRP);
	glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

/*-----------------------------------------------*/
/*-------------------- CAPSA --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::calculaCapsaModelPatricio  ()
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
  escalaPatr = 0.25/(maxy-miny);
  centrePatr[0] = (minx+maxx)/2.0; centrePatr[1] = (miny+maxy)/2.0; centrePatr[2] = (minz+maxz)/2.0;
  centrePatrCurrent = centrePatr;
}
void MyGLWidget::calculaCapsaModelVaca      ()
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = vaca.vertices()[0];
  miny = maxy = vaca.vertices()[1];
  minz = maxz = vaca.vertices()[2];
  for (unsigned int i = 3; i < vaca.vertices().size(); i+=3)
  {
    if (vaca.vertices()[i+0] < minx) minx = vaca.vertices()[i+0];
    if (vaca.vertices()[i+0] > maxx) maxx = vaca.vertices()[i+0];
    if (vaca.vertices()[i+1] < miny) miny = vaca.vertices()[i+1];
    if (vaca.vertices()[i+1] > maxy) maxy = vaca.vertices()[i+1];
    if (vaca.vertices()[i+2] < minz) minz = vaca.vertices()[i+2];
    if (vaca.vertices()[i+2] > maxz) maxz = vaca.vertices()[i+2];
  }
  escalaVaca = 0.5 /(maxz-minz);    // vaca tumbada
  centreVaca[0] = (minx+maxx)/2.0; centreVaca[1] = (miny+maxy)/2.0; centreVaca[2] = (minz+maxz)/2.0;
}

/*-----------------------------------------------*/
/*------------------- EVENTS --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::keyPressEvent      (QKeyEvent* event)
{
  makeCurrent();
  switch (event->key())
  {
    case Qt::Key_O: // canvia òptica entre perspectiva i axonomètrica
    {
      perspectiva = !perspectiva;
      projectTransform ();
      break;
    }
    case Qt::Key_R:
    {  // canvia òptica entre perspectiva i axonomètrica
      angleTeclaR -= M_PI/6.0;
      glm::mat4 TG(1.f);  //glm::vec4 rotaVRP = glm::rotate(TG, float(-M_PI/6.0f), glm::vec3(0,1,0))*glm::vec4(VRP, 1.0f);
      glm::vec3 rotaVRP(1*cos(-angleTeclaR), -0.375, 1*sin(-angleTeclaR));  //VRP = glm::vec3(rotaVRP);
      VRP[0] = rotaVRP[0];  VRP[1] = rotaVRP[1];  VRP[2] = rotaVRP[2];
      viewTransform();
      //modelTransformPatricio(); //modelTransformVaca();
      break;
    }
    case Qt::Key_X:
    {
      if (teclaX == 1)
      {
		teclaX = 0;
		emit signalSetFocusCam ( );
	  } else
	  {
		teclaX = 1;
		emit signalSetFocusEscena ( );
	  }                
      glUniform1i (teclaXLoc, teclaX);
      break;
    }
    case Qt::Key_L: //Focus cam / escena
    {
      if (teclaL == 1)  
      {
		teclaL = 0;
		emit signalSetFocusCam ( );
	  } else
	  {
	    teclaL = 1;
	    emit signalSetFocusEscena ( );
	  }              
      glUniform1i (teclaLLoc, teclaL);
      break;
    }
    default: event->ignore(); break;
  }
  update();
}
void MyGLWidget::mousePressEvent    (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton && ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}
void MyGLWidget::mouseReleaseEvent  (QMouseEvent *)
{
  DoingInteractive = NONE;
}
void MyGLWidget::mouseMoveEvent     (QMouseEvent *e)
{
  makeCurrent();
  // MOURE CLICK ESQUERRA
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleEuler[1] += (e->x() - xClick) * (float)M_PI / 180.0;
    angleEuler[0] += (e->y() - yClick) * (float)M_PI / 180.0;
    viewTransform ();
  }

  // ZOOM CLICK DRET
	if (e->buttons() == Qt::RightButton && !(e->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier)))
	{
		int ynew = e->y();
    if ((yClick > ynew) and (FOV-(0.1/(ynew-yClick)) > 0.0 + 0.3)) /*0.3 per error al restar*/
    {
      FOV -= 0.1/(yClick-ynew);   // si pujo mouse -> zoom in
      signalChangeFOV (int((FOV*(float)180) / float(M_PI)));
    }
    if ((yClick < ynew) and (FOV+(0.1/(ynew-yClick)) < (float)M_PI))
    {
      FOV += 0.1/(ynew-yClick);   // si baixo mouse -> zoom out
      signalChangeFOV (int((FOV*(float)180) / float(M_PI)));
    }
		projectTransform();
		yClick = ynew;
		alfaIni = FOV/2;
	}

  xClick = e->x();
  yClick = e->y();

  update ();
}

/*-----------------------------------------------*/
/*-------------------- SLOTS --------------------*/
/*-----------------------------------------------*/
void MyGLWidget::changeFOV    (int i)
{
	makeCurrent();
	FOV = ((float)i * (float)M_PI) / (float)180;
	projectTransform();
	update();
}
void MyGLWidget::setFocusCam  ()
{
  makeCurrent();
  teclaL = 0;
  glUniform1i(teclaLLoc, teclaL);
  update();
}
void MyGLWidget::setFocusEscena  ()
{
  makeCurrent();
  teclaL = 1;
  glUniform1i(teclaLLoc, teclaL);
  update();
}
