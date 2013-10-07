#include "StatsVisualization.h"
#include "KinectInterface.h"


#include <iostream>
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Transformation.h>
#include <ngl/TransformStack.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>

#include <ngl/Random.h>
#include <ngl/Util.h>
#include <ngl/Texture.h>

//#include <boost/foreach.hpp>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.3;
//----------------------------------------------------------------------------------------------------------------------

const GLubyte indices[]=  {
                            3,2,7,2,6,7, // back
                            0,4,1,1,4,5, // front
                            3,0,2,2,0,1, // top
                            6,4,7,5,4,6, // bottom
                            3,7,4,3,4,0, // left
                            1,5,2,2,5,6  //right
                          };

const ngl::Vec3 normals[] = {
                                ngl::Vec3(0.0f,0.0f,-1.0f), //back
                                ngl::Vec3(0.0f,0.0f,1.0f),  // front
                                ngl::Vec3(0.0f,1.0f,0.0f),  // top
                                ngl::Vec3(0.0f,-1.0f,0.0f), // bottom
                                ngl::Vec3(-1.0f,0.0f,0.0f), // left
                                ngl::Vec3(1.0f,0.0f,0.0f)  // right
                            };

StatsVisualization::StatsVisualization()
{
   m_importFlag = false;
   m_vao = 0;
   m_text = 0;
}

StatsVisualization::StatsVisualization(
                   QWidget *_parent,PlayerData *_playerA, PlayerData *_playerB
        ) : QGLWidget( new CreateCoreGLContext(QGLFormat::defaultFormat()), _parent ),
        m_playerA(_playerA),m_playerB(_playerB)
{


  // set this widget to have the initial keyboard focus
  //setFocus();
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());

  m_importFlag = false;
  // Now set the initial StatsVisualization attributes to default values
  // Roate is false
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  m_origX=0;
  m_origY=0;

 // m_depthObject = _inputDepthWindow;

  m_ballPtsFilled = 0;

  m_inputBallPtsList.clear();

  m_impactPtsFilled = false;

  // by default when process stats is clicked, the first stat displayed is the impact point
  // of both the players

  m_whichPlayer = BOTH;
  m_whichStat = IMPACT_POINTS_VIEW;
  m_whichQuad = ALL;

  m_width = m_height = m_midTopX = m_midTopY = m_midBottomX = m_midBottomY = m_midLeftX = 0;
  m_midLeftY= m_midRightX = m_midRightY = 0;

  m_percentageABL = m_percentageATL = m_percentageATR = m_percentageABR = -1.0;
  m_percentageBBL = m_percentageBTL = m_percentageBTR = m_percentageBBR = -1.0;

  m_playerAImpactData.clear();
  m_playerBImpactData.clear();
  m_quadSpecABL.clear();
  m_quadSpecATL.clear();
  m_quadSpecATR.clear();
  m_quadSpecABR.clear();
  m_quadSpecBBL.clear();
  m_quadSpecBTL.clear();
  m_quadSpecBTR.clear();
  m_quadSpecBBR.clear();
  m_dataVertsA.clear();
  m_dataVertsB.clear();

  m_playerAInterpPtsMappedToRallies.clear();
  m_playerBInterpPtsMappedToRallies.clear();
  m_playerATrajectoryFromFile.clear();
  m_playerBTrajectoryFromFile.clear();

  m_speedAData.clear();
  m_speedBData.clear();
  m_playerAImpactPtsAllRalliesIn3D.clear();
  m_playerBImpactPtsAllRalliesIn3D.clear();
  m_playerARPM.clear();
  m_playerBRPM.clear();

  m_vao = 0;
  m_text = 0;

  m_3dFlag = false;
  m_speedMapBuilt = false;
  m_rpmDataBuilt = false;
  m_quadMapBuilt = false;
  m_trajectoryBuilt = false;
  // this is the exacct distance in meters as measured by the kinect
  // to the playing tt table
  m_kinectToPlayAreaInMeters = 0.76;
  m_diameterOfBAllInMeters = 0.044;
  m_finalCourtWidthWorld = m_finalCourtDepthWorld = 0.0;

  m_lightPos.set(0,2,0);
}

StatsVisualization::StatsVisualization(
                   QWidget *_parent,bool _importFlag
        ) : QGLWidget( new CreateCoreGLContext(QGLFormat::defaultFormat()), _parent ),m_importFlag(_importFlag)

{

  // set this widget to have the initial keyboard focus
  //setFocus();
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize(_parent->size());
  // Now set the initial StatsVisualization attributes to default values
  // Roate is false
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  m_origX=0;
  m_origY=0;

 // m_depthObject = _inputDepthWindow;

  m_ballPtsFilled = 0;

  m_inputBallPtsList.clear();

  m_impactPtsFilled = false;

  // by default when process stats is clicked, the first stat displayed is the impact point
  // of both the players

  m_whichPlayer = BOTH;
  m_whichStat = IMPACT_POINTS_VIEW;
  m_whichQuad = ALL;
  m_3dFlag = false;

  m_width = m_height = m_midTopX = m_midTopY = m_midBottomX = m_midBottomY = m_midLeftX = 0;
  m_midLeftY= m_midRightX = m_midRightY = 0;

  m_percentageABL = m_percentageATL = m_percentageATR = m_percentageABR = -1.0;
  m_percentageBBL = m_percentageBTL = m_percentageBTR = m_percentageBBR = -1.0;

  m_playerAImpactData.clear();
  m_playerBImpactData.clear();
  m_quadSpecABL.clear();
  m_quadSpecATL.clear();
  m_quadSpecATR.clear();
  m_quadSpecABR.clear();
  m_quadSpecBBL.clear();
  m_quadSpecBTL.clear();
  m_quadSpecBTR.clear();
  m_quadSpecBBR.clear();
  m_dataVertsA.clear();
  m_dataVertsB.clear();
  m_playerAInterpPtsMappedToRallies.clear();
  m_playerBInterpPtsMappedToRallies.clear();
  m_speedAData.clear();
  m_speedBData.clear();
  m_playerAImpactPtsAllRalliesIn3D.clear();
  m_playerBImpactPtsAllRalliesIn3D.clear();
  m_playerARPM.clear();
  m_playerBRPM.clear();

  m_vao = 0;
  m_text = 0;

  m_speedMapBuilt = false;
  m_rpmDataBuilt = false;
  m_quadMapBuilt = false;
  m_trajectoryBuilt = false;
  // this is the exacct distance in meters as measured by the kinect
  // to the playing tt table
  m_kinectToPlayAreaInMeters = 0.76;
  m_diameterOfBAllInMeters = 0.044;
  m_finalCourtWidthWorld = m_finalCourtDepthWorld = 0.0;

  m_lightPos.set(0,2,0);

  //m_text->setScreenSize(640,480);
}

StatsVisualization::~StatsVisualization()
{
//  ngl::NGLInit *Init = ngl::NGLInit::instance();
//  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
//  Init->NGLQuit();

    if(m_2DScreen)
    {
        delete m_2DScreen;
    }

    if(m_vao)
    {
        m_vao->removeVOA();
        //delete m_vao;

    }

    if(m_text)
    {
        delete m_text;
        m_text = 0;
    }
//    if(m_text1)
//    {
//        delete m_text1;
//    }
}


// This virtual function is called once before the first call to paintGL() or resizeGL(),
//and then once whenever the widget has been assigned a new QGLContext.
// This function should set up any required OpenGL context rendering flags, defining display lists, etc.

//----------------------------------------------------------------------------------------------------------------------
void StatsVisualization::initializeGL()
{

   glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
   // enable depth testing for drawing
   glEnable(GL_DEPTH_TEST);

   glEnable(GL_MULTISAMPLE);

   // Now we will create a basic Camera from the graphics library
   // This is a static camera so it only needs to be set once
   // First create Values for the camera position

   ngl::Vec3 from(0,0.2,50);
   ngl::Vec3 to(0,0,0);
   ngl::Vec3 up(0,1,0);

   m_cam= new ngl::Camera(from,to,up,ngl::PERSPECTIVE);
   // set the shape using FOV 45 Aspect Ratio based on Width and Height
   // The final two are near and far clipping planes of 0.5 and 10
   m_cam->setShape(45,(float)640/480,0.001,150,ngl::PERSPECTIVE);





   ngl::NGLInit *Init = ngl::NGLInit::instance();
   #ifdef WIN32
    glewInit();
   #endif
   Init->initGlew();

   // we create 2 shaders, one for 3D rendering, another one for 2D quad pasting

   //PER FRAGMENT ASD SHADER FOR 3D RENDER

   // now to load the shader and set the values
   // grab an instance of shader manager

   ngl::ShaderLib *shader = ngl::ShaderLib::instance();
   //shader = ngl::ShaderLib::instance();
   // we are creating a shader called PerFragADS
   shader->createShaderProgram("PerFragADS");
   // now we are going to create empty shaders for Frag and Vert
   shader->attachShader("PerFragADSVertex",ngl::VERTEX);
   shader->attachShader("PerFragADSFragment",ngl::FRAGMENT);
   // attach the source
   shader->loadShaderSource("PerFragADSVertex","shaders/PerFragASDVert.glsl");
   shader->loadShaderSource("PerFragADSFragment","shaders/PerFragASDFrag.glsl");
   // compile the shaders
   shader->compileShader("PerFragADSVertex");
   shader->compileShader("PerFragADSFragment");
   // add them to the program
   shader->attachShaderToProgram("PerFragADS","PerFragADSVertex");
   shader->attachShaderToProgram("PerFragADS","PerFragADSFragment");

   // now we have associated this data we can link the shader
   shader->linkProgramObject("PerFragADS");
   // and make it active ready to load values
   (*shader)["PerFragADS"]->use();
   // now we need to set the material and light values
   /*
    *struct MaterialInfo
    {
         // Ambient reflectivity
         vec3 Ka;
         // Diffuse reflectivity
         vec3 Kd;
         // Specular reflectivity
         vec3 Ks;
         // Specular shininess factor
         float shininess;
   };*/
   shader->setShaderParam3f("material.Ka",0.1,0.1,0.1);
   // green diffuse
   shader->setShaderParam3f("material.Kd",0.0,0.8,0.0);
   // white spec
   shader->setShaderParam3f("material.Ks",1.0,1.0,1.0);
   shader->setShaderParam1f("material.shininess",100);
   // now for  the lights values (all set to white)
   /*struct LightInfo
   {
   // Light position in eye coords.
   vec4 position;
   // Ambient light intensity
   vec3 La;
   // Diffuse light intensity
   vec3 Ld;
   // Specular light intensity
   vec3 Ls;
   };*/
   shader->setUniform("light.position",m_lightPos);
   shader->setShaderParam3f("light.La",0.1,0.1,0.1);
   shader->setShaderParam3f("light.Ld",1.0,1.0,1.0);
   shader->setShaderParam3f("light.Ls",0.9,0.9,0.9);



   // TEXTURE SHADER INSTANCE FOR 2D

     // now shader for Texture Pasting used for 2D viz.
     shader = ngl::ShaderLib::instance();
     shader->createShaderProgram("TextureShaderStats");

     shader->attachShader("TextureVertex",ngl::VERTEX);
     shader->attachShader("TextureFragment",ngl::FRAGMENT);
     shader->loadShaderSource("TextureVertex","shaders/TextureVert.glsl");
     shader->loadShaderSource("TextureFragment","shaders/TextureFrag.glsl");

     shader->compileShader("TextureVertex");
     shader->compileShader("TextureFragment");
     shader->attachShaderToProgram("TextureShaderStats","TextureVertex");
     shader->attachShaderToProgram("TextureShaderStats","TextureFragment");

     shader->linkProgramObject("TextureShaderStats");
     (*shader)["TextureShaderStats"]->use();
     shader->setShaderParam1i("tex",0);

     m_2DScreen = new ScreenQuad(640,480,"TextureShaderStats");

     ngl::Texture crowdTexture("textures/crowd.png");
     ngl::Texture tableTexture("textures/blue_ultramarine.png");

     ngl::Texture speedLegendTexture("textures/speedlegendbold.png");
     ngl::Texture rpmLegendTexture("textures/rpslegend.png");
     ngl::Texture trajectoryLegendTexture("textures/traj_map.png");
     ngl::Texture htMapLegendTexture("textures/height_map.png");
     ngl::Texture htGridMapLegendTexture("textures/grid.png");

     m_crowdTextureName = crowdTexture.setTextureGL();
     m_tableTextureName = tableTexture.setTextureGL();

     m_speedLegend = speedLegendTexture.setTextureGL();
     m_rpmLegend = rpmLegendTexture.setTextureGL();
     m_trajectoryLegend = trajectoryLegendTexture.setTextureGL();
     m_heightMapLegend = htMapLegendTexture.setTextureGL();
     m_gridXYHtMap = htGridMapLegendTexture.setTextureGL();

     //frame rate GUNDU
     m_text=new ngl::Text(QFont("Arial",25));
     m_text->setColour(1,1,1);
     //m_text->setScreenSize(640,480);

     //     m_text1=new ngl::Text(QFont("Arial",25));
     //     m_text1->setColour(1,1,1);
     //     m_text1->setScreenSize(640,480);

     m_updateTimer = startTimer(3);

}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget has been resized.
// The new size is passed in width and height.
void StatsVisualization::resizeGL(
                        int _w,
                        int _h
                       )
{
    //glViewport(0,0,_w,_h);

    m_legendWidth = _w * (200/640.0);
    m_legendHeight = _h * (200/480.0);
    //m_cam->setShape(45,(float)_w/_h,0.001,150,ngl::PERSPECTIVE);
}

// change this name to ads shader
void StatsVisualization::loadMatricesToADSShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  //(*shader)["PerFragADS"]->use();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_transformStack.getCurrentTransform().getMatrix();
  MV= m_transformStack.getCurrAndGlobal().getMatrix()*m_cam->getViewMatrix();
  MVP=MV*m_cam->getProjectionMatrix() ;
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setUniform("MVP",MVP);
  shader->setUniform("MV",MV);
  shader->setUniform("normalMatrix",normalMatrix);
  shader->setUniform("light.position",m_lightPos);

}

void StatsVisualization::loadMatricesToTextureShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  //(*shader)["TextureShaderStats"]->use();

  ngl::Mat4 MVP=m_transformStack.getCurrAndGlobal().getMatrix()*m_cam->getVPMatrix();

  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setUniform("textureFlag",true);



}

//----------------------------------------------------------------------------------------------------------------------
//This virtual function is called whenever the widget needs to be painted.
// this is our main drawing routine
void StatsVisualization::paintGL()
{


  //UPDATETIMER does not call PAINTGL
  //JUST CALLS ONCE finally WHEN FIELD VALUES ARE COMPUTED and kills the timer

  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glViewport(0,0,640,480);

  // only when 3D data is requested..
  if(m_3dFlag)
  {
    // Rotation based on the mouse position for our global transform
    ngl::Transformation trans;
    ngl::Mat4 rotX;
    ngl::Mat4 rotY;
    // create the rotation matrices
    rotX.rotateX(m_spinXFace);
    rotY.rotateY(m_spinYFace);
    // multiply the rotations
    ngl::Mat4 final=rotY*rotX;
    // add the translations
    final.m_m[3][0] = m_modelPos.m_x;
    final.m_m[3][1] = m_modelPos.m_y;
    final.m_m[3][2] = m_modelPos.m_z;
    // set this in the TX stack
    trans.setMatrix(final);
    m_transformStack.setGlobal(trans);
    //loadMatricesToADSShader();

    // if we were previously seeing a 2D stats
    // window it would have been restricted to 640*480
    // so we need to resize our opengl window
    // to render to the current window size for 3D stats
    glViewport(0,0,this->size().width(),this->size().height());
    // this resize is called to resize the ratio of the legend window
    // which will be displayed in 3d stats
    resizeGL(this->size().width(),this->size().height());

  }
  else
  {
    // for 2D stats the gl window can only be 640*480
    glViewport(0,0,640,480);
    m_text->setScreenSize(640,480);
    resizeGL(640,480);

  }


  drawEntireScene();

}

void StatsVisualization::drawEntireScene()
{

    cv::Mat drawing = cv::Mat::zeros(480,640,CV_8UC3 );

    if((m_width != 0) && (m_height != 0))
    {

        cv::rectangle(drawing,m_fieldOfPlay,cv::Scalar(92,163,69),-1);

        cv::line(drawing,cv::Point(m_midTopX,m_midTopY),cv::Point(m_midBottomX,m_midBottomY),cv::Scalar(255,0,0),1);

        cv::line(drawing,cv::Point(m_midLeftX,m_midLeftY),cv::Point(m_midRightX,m_midRightY),cv::Scalar(233,235,226),3);


        // Note for All following Switch cases and Statistics:

        // if data has been processed by the processing utility
        // then call a local function which will call playerallrallies function
        // store in player specific array and draw it if not empty

        // if impact pts have already been filled, then this will avoid
        // further calling of functions..saves time and calculations
        // same logic applies for all statistics

        switch (m_whichStat)
        {
          case IMPACT_POINTS_VIEW:
          {
            //m_3dFlag = 0;

            if(!(m_impactPtsFilled))
            {
              if(m_playerA->checkDataProcessed() && m_playerB->checkDataProcessed())
              {
                  fillAllImpactPointsBothPlayers();
              }
            }

            switch(m_whichPlayer)
            {
              case PLAYERA:
              {
                if(m_playerAImpactData.size() != 0)
                {
                    for(int i=0;i<m_playerAImpactData.size();i++)
                    {
                        cv::circle(drawing,m_playerAImpactData[i],8,cv::Scalar(255,255,255),-1);
                    }
                }
                break;
              }

              case PLAYERB:
              {
                if(m_playerBImpactData.size() != 0)
                {
                    for(int i=0;i<m_playerBImpactData.size();i++)
                    {
                        cv::circle(drawing,m_playerBImpactData[i],8,cv::Scalar(255,0,0),-1);
                    }
                }
                break;
              }
              case BOTH:
              {
                if(m_playerAImpactData.size() != 0)
                {
                    for(int i=0;i<m_playerAImpactData.size();i++)
                    {
                        cv::circle(drawing,m_playerAImpactData[i],8,cv::Scalar(255,255,255),-1);
                    }
                }

                if(m_playerBImpactData.size() != 0)
                {
                    for(int i=0;i<m_playerBImpactData.size();i++)
                    {
                        cv::circle(drawing,m_playerBImpactData[i],8,cv::Scalar(255,0,0),-1);
                    }
                }

                break;
              }

            }
            break;
          }

          case IMPACT_PERCENTAGE_DISTRIBUTION_VIEW:
          {
            // pecentage distrbn
            // to stop recalculating in every paintgl, we check against -1

            // THESE SWITCH CASES MIGHT ONLY BE USEFUL WHEN IT IS RENDERED APO-APO
            //  LIKE THE IMPACT DRAWING BUFFER...WE NEED TO FIND A WAY FOR THIS

            //threeDFlag = 0;

            if(!(m_impactPtsFilled))
            {
              if(m_playerA->checkDataProcessed() && m_playerB->checkDataProcessed())
              {
                  fillAllImpactPointsBothPlayers();
              }
            }

            switch(m_whichPlayer)
            {
              case PLAYERA:
              {

                if(!(m_quadMapBuilt) && (m_impactPtsFilled == true))
                {
                    buildQuadSpecificData();
                }

                switch(m_whichQuad)
                {
                  case BOTTOM_LEFT:
                  {

                    if(m_quadSpecABL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecABL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecABL[i],8,cv::Scalar(255,255,255),-1);
                        }


                    }
                    break;
                  }
                  case TOP_LEFT:
                  {

                    if(m_quadSpecATL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecATL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecATL[i],8,cv::Scalar(255,255,255),-1);
                        }

                    }

                    break;
                  }
                  case TOP_RIGHT:
                  {


                    if(m_quadSpecATR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecATR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecATR[i],8,cv::Scalar(255,255,255),-1);
                        }

                    }

                    break;
                  }
                  case BOTTOM_RIGHT:
                  {

                    if(m_quadSpecABR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecABR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecABR[i],8,cv::Scalar(255,255,255),-1);
                        }

                    }

                    break;
                  }
                case ALL: // draw A pts
                  {
                    if(m_playerAImpactData.size() != 0)
                    {
                        for(int i=0;i<m_playerAImpactData.size();i++)
                        {
                            cv::circle(drawing,m_playerAImpactData[i],8,cv::Scalar(255,255,255),-1);
                        }

                    }
                    break;
                  }
                }

                break;
              }

              case PLAYERB:
              {
                if(!(m_quadMapBuilt) && (m_impactPtsFilled == true))
                {
                    buildQuadSpecificData();
                }

                switch(m_whichQuad)
                {
                  case BOTTOM_LEFT:
                  {

                    if(m_quadSpecBBL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBBL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBBL[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }
                  case TOP_LEFT:
                  {

                    if(m_quadSpecBTL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBTL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBTL[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }
                  case TOP_RIGHT:
                  {

                    if(m_quadSpecBTR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBTR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBTR[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }
                  case BOTTOM_RIGHT:
                  {

                    if(m_quadSpecBBR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBBR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBBR[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }

                case ALL: //tbd // draw B pts
                  {
                    if(m_playerBImpactData.size() != 0)
                    {
                        for(int i=0;i<m_playerBImpactData.size();i++)
                        {
                            cv::circle(drawing,m_playerBImpactData[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }
                    break;
                  }
                }

                break;
              }
              case BOTH:
              {
                if(!(m_quadMapBuilt) && (m_impactPtsFilled == true))
                {
                    buildQuadSpecificData();
                }

                switch(m_whichQuad)
                {
                  case BOTTOM_LEFT:
                  {

                    if(m_quadSpecABL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecABL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecABL[i],8,cv::Scalar(255,255,255),-1);
                        }
                    }

                    if(m_quadSpecBBL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBBL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBBL[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }
                    break;
                  }
                  case TOP_LEFT:
                  {

                    if(m_quadSpecATL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecATL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecATL[i],8,cv::Scalar(255,255,255),-1);
                        }
                    }

                    if(m_quadSpecBTL.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBTL.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBTL[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }
                  case TOP_RIGHT:
                  {

                    if(m_quadSpecATR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecATR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecATR[i],8,cv::Scalar(255,255,255),-1);
                        }
                    }

                    if(m_quadSpecBTR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBTR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBTR[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }
                  case BOTTOM_RIGHT:
                  {

                    if(m_quadSpecABR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecABR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecABR[i],8,cv::Scalar(255,255,255),-1);
                        }
                    }

                    if(m_quadSpecBBR.size() != 0)
                    {
                        for(int i=0;i<m_quadSpecBBR.size();i++)
                        {
                            cv::circle(drawing,m_quadSpecBBR[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }

                    break;
                  }

                  case ALL: // TBD
                  {
                    // if ALL quads and BOTH players selected in % view, its the same as full impact points
                    if(m_playerAImpactData.size() != 0)
                    {
                        for(int i=0;i<m_playerAImpactData.size();i++)
                        {
                            cv::circle(drawing,m_playerAImpactData[i],8,cv::Scalar(255,255,255),-1);
                        }
                    }

                    if(m_playerBImpactData.size() != 0)
                    {
                        for(int i=0;i<m_playerBImpactData.size();i++)
                        {
                            cv::circle(drawing,m_playerBImpactData[i],8,cv::Scalar(255,0,0),-1);
                        }
                    }
                    break;
                  }
                }
                break;
              }

            }
            break;
          }
          case IMPACT_HEIGHT_MAP_VIEW:
          {


            //hegith map
            m_cam->setEye(ngl::Vec4(0,3.0,55,1));

            if(!(m_impactPtsFilled))
            {
              if(m_playerA->checkDataProcessed() && m_playerB->checkDataProcessed())
              {
                  fillAllImpactPointsBothPlayers();
              }
            }

            if(!(m_vao) && (m_impactPtsFilled == true))
            {
                buildPointGraphBars();
            }


            ngl::ShaderLib *shader=ngl::ShaderLib::instance();
            switch(m_whichPlayer)
            {

              case PLAYERA:
              {
                  // in this case we are going to set our data as the vertices above

                  if(m_vao)
                  {

                      drawCrowd("heightMapPlane");
                      drawTable("heightMapPlane");

                      // draw the bounding graph planes
                      drawContainingGridScaleXY();

                      (*shader)["PerFragADS"]->use();
                      shader->setShaderParam3f("material.Kd",0.5,0.5,0.5);

                      m_transformStack.pushTransform();
                      {
                        // next we bind it so it's active for setting data
                        m_vao->bind();
                        m_vao->setData(m_dataVertsA.size()*sizeof(VertData),m_dataVertsA[0].nx);

                        //sizeof(vertData) and the offset into the data structure for the first x component is 3 (nx,ny,nz)..x
                        m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(VertData),3);
                        // uv same as above but starts at 0 and is attrib 1 and only u,v so 2
                        // normal same as vertex only starts at position 0
                        m_vao->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(VertData),0);
                        m_vao->setNumIndices(m_dataVertsA.size());

                        loadMatricesToADSShader();

                        m_vao->draw();
                        // finally we have finished for now so time to unbind the VAO
                        m_vao->unbind();
                       }
                      m_transformStack.popTransform();
                   }
                  break;
                }

              case PLAYERB:
              {
                      // in this case we are going to set our data as the vertices above
                  if(m_vao)
                  {
                      drawCrowd("heightMapPlane");
                      drawTable("heightMapPlane");
                      // draw the bounding graph planes
                      drawContainingGridScaleXY();

                      (*shader)["PerFragADS"]->use();
                      shader->setShaderParam3f("material.Kd",0.8,0.0,0.0);

                      m_transformStack.pushTransform();
                      {
                        // next we bind it so it's active for setting data
                        m_vao->bind();

                        m_vao->setData(m_dataVertsB.size()*sizeof(VertData),m_dataVertsB[0].nx);
                        m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(VertData),3);
                        // normal same as vertex only starts at position 0
                        m_vao->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(VertData),0);
                        m_vao->setNumIndices(m_dataVertsB.size());
                        loadMatricesToADSShader();
                        m_vao->draw();
                        // finally we have finished for now so time to unbind the VAO
                        m_vao->unbind();
                      }
                      m_transformStack.popTransform();
                   }

                    break;
                }
                case BOTH:
                {
                    // cannot view 3d stats for both players at a time
                    break;
                }
              }
              break;
          }
          case TRAJECTORY_VIEW:
          {

            m_cam->setEye(ngl::Vec4(0,0.2,1,1));


            if(!(m_impactPtsFilled))
            {
              if(m_playerA->checkDataProcessed() && m_playerB->checkDataProcessed())
              {
                  fillAllImpactPointsBothPlayers();
              }
            }

            if(!(m_trajectoryBuilt) && (m_impactPtsFilled == true))
            {
                buildTrajectory();
            }

            // draw field in WORLD coordinates
            // as the trajectory view is in
            // METRES SCALE
            // SHOW THE SCALE SOMEWHERE---font rendering
            ngl::ShaderLib *shader=ngl::ShaderLib::instance();
            ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

            if(m_trajectoryBuilt)
            {
                drawCrowd("trajectoryPlane");
                drawTable("trajectoryPlane");
                drawContainingGridScaleXY();
            }

            switch(m_whichPlayer)
            {
              case PLAYERA:
              {

                (*shader)["PerFragADS"]->use();
                shader->setShaderParam3f("material.Kd",0.8,0.8,0.8);

                if(!(m_importFlag)) // live data
                {
                    for(int i=0;i<m_playerAInterpPtsMappedToRallies.size();i++)
                    {
                        for(int j=0;j<m_playerAInterpPtsMappedToRallies[i].size();j++)
                        {
                            if(m_playerAInterpPtsMappedToRallies[i][j].y < 0.0)
                            {
                                continue;
                            }
                            m_transformStack.pushTransform();
                            {
                                m_transformStack.setPosition(m_playerAInterpPtsMappedToRallies[i][j].x,
                                                             m_playerAInterpPtsMappedToRallies[i][j].y,
                                                             m_playerAInterpPtsMappedToRallies[i][j].z);



                                loadMatricesToADSShader();
                                prim->draw("trajectorySphere");
                            }
                            m_transformStack.popTransform();
                        }


                    }
                }
                else // imported data
                {
                    for(int j=0;j<m_playerATrajectoryFromFile.size();j = j+3)
                    {
                        if(m_playerATrajectoryFromFile[j + 1] < 0.0)
                        {
                            continue;
                        }
                        m_transformStack.pushTransform();
                        {
                            m_transformStack.setPosition(m_playerATrajectoryFromFile[j],
                                                         m_playerATrajectoryFromFile[j+1],
                                                         m_playerATrajectoryFromFile[j+2]);


                            loadMatricesToADSShader();

                            prim->draw("trajectorySphere");
                        }
                        m_transformStack.popTransform();
                    }


                }
                  break;
              }
              case PLAYERB:
              {

                (*shader)["PerFragADS"]->use();
                shader->setShaderParam3f("material.Kd",0.8,0.0,0.0);

                if(!(m_importFlag)) // live data
                {
                    // render the vao1 for playerb data
                    for(int i=0;i<m_playerBInterpPtsMappedToRallies.size();i++)
                    {

                        for(int j=0;j<m_playerBInterpPtsMappedToRallies[i].size();j++)
                        {
                            // continue if the depth data is -1
                            // as that represents no impact point
                            // so move to next rally
                            if(m_playerBInterpPtsMappedToRallies[i][j].y < 0.0)
                            {
                                continue;
                            }
                            m_transformStack.pushTransform();
                            {
                                m_transformStack.setPosition(m_playerBInterpPtsMappedToRallies[i][j].x,
                                                             m_playerBInterpPtsMappedToRallies[i][j].y,
                                                             m_playerBInterpPtsMappedToRallies[i][j].z);


                                loadMatricesToADSShader();

                                prim->draw("trajectorySphere");
                            }
                            m_transformStack.popTransform();
                        }

                    }


                }
                else // imported data
                {
                   for(int j=0;j<m_playerBTrajectoryFromFile.size();j = j+3)
                   {
                       // continue if the depth data is -1
                       // as that represents no impact point
                       // so move to next rally
                       if(m_playerBTrajectoryFromFile[j + 1] < 0.0)
                       {
                           continue;
                       }
                       m_transformStack.pushTransform();
                       {
                           m_transformStack.setPosition(m_playerBTrajectoryFromFile[j],
                                                        m_playerBTrajectoryFromFile[j+1],
                                                        m_playerBTrajectoryFromFile[j+2]);


                           loadMatricesToADSShader();

                           prim->draw("trajectorySphere");
                       }
                       m_transformStack.popTransform();
                   }

                }
                break;
              }

              case BOTH:
              {
                // cannot view 3d stats for both players at a time
                break;
              }
            }
              //3d trajectory
              break;
          }
          case VELOCITY_VIEW:
          {
            //velocity graphics view
            m_cam->setEye(ngl::Vec4(0,3.0,5,1));


            if(!(m_impactPtsFilled))
            {
              if(m_playerA->checkDataProcessed() && m_playerB->checkDataProcessed())
              {
                  fillAllImpactPointsBothPlayers();
              }
            }

            if(!(m_speedMapBuilt) && (m_impactPtsFilled == true))
            {
                buildSpeedMap();
            }

              //ngl::ShaderLib *shader=ngl::ShaderLib::instance();
              ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

            if(m_speedMapBuilt)
            {
                drawCrowd("speedPlane");
                drawTable("speedPlane");
            }

            switch(m_whichPlayer)
            {
              case PLAYERA:
              {
                if(m_speedAData.size() != 0)
                {
                    for(int i=0;i<m_playerAImpactPtsAllRalliesIn3D.size();i++)
                    {
                        // put your colour condition as m_speedAData[i] is within a range
                        // set the shader to different colors
                        m_transformStack.pushTransform();
                        {
                            m_transformStack.setPosition(m_playerAImpactPtsAllRalliesIn3D[i].x,
                                                         m_playerAImpactPtsAllRalliesIn3D[i].y,
                                                         m_playerAImpactPtsAllRalliesIn3D[i].z);

                             setColorBasedOnSpeedRange(m_speedAData[i]);
                            //(*shader)["PerFragADS"]->use();
                             loadMatricesToADSShader();

                            prim->draw("sphere");
                        }
                        m_transformStack.popTransform();

                        //std::cout<<"Speed data A:"<<m_speedAData[i]<<"\n";
                    }
                }
                  break;
              }
              case PLAYERB:
              {
                if(m_speedBData.size() != 0)
                {
                    for(int i=0;i<m_playerBImpactPtsAllRalliesIn3D.size();i++)
                    {
                           // put your colour condition as m_speedAData[i] is within a range
                        // set the shader to different colors
                        m_transformStack.pushTransform();
                        {
                            m_transformStack.setPosition(m_playerBImpactPtsAllRalliesIn3D[i].x,
                                                         m_playerBImpactPtsAllRalliesIn3D[i].y,
                                                         m_playerBImpactPtsAllRalliesIn3D[i].z);

                            setColorBasedOnSpeedRange(m_speedBData[i]);
                            loadMatricesToADSShader();
                            //(*shader)["PerFragADS"]->use();

                            prim->draw("sphere");
                        }
                        m_transformStack.popTransform();

                        //std::cout<<"Speed data B:"<<m_speedBData[i]<<"\n";
                    }
                }
                break;
              }
              case BOTH:
              {
                // not allowed, just a placeholder
                break;
              }
            }
              break;
          }
          case RPM_VIEW:
          {
            //rpm graphic
            m_cam->setEye(ngl::Vec4(0,3.0,5,1));


            if(!(m_impactPtsFilled))
            {
              if(m_playerA->checkDataProcessed() && m_playerB->checkDataProcessed())
              {
                  fillAllImpactPointsBothPlayers();
              }
            }

            if(!(m_rpmDataBuilt) && (m_impactPtsFilled == true))
            {
                buildRPMData();
            }

            //ngl::ShaderLib *shader=ngl::ShaderLib::instance();
            ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

            if(m_rpmDataBuilt)
            {
              drawCrowd("rpmPlane");
              drawTable("rpmPlane");
            }

            switch(m_whichPlayer)
            {
              case PLAYERA:
              {
                if(m_playerARPM.size() != 0)
                {
                    for(int i=0;i<m_playerAImpactPtsAllRalliesIn3D.size();i++)
                    {
                        m_transformStack.pushTransform();
                        {
                            m_transformStack.setPosition(m_playerAImpactPtsAllRalliesIn3D[i].x,
                                                         m_playerAImpactPtsAllRalliesIn3D[i].y,
                                                         m_playerAImpactPtsAllRalliesIn3D[i].z);



                            setColorBasedOnRPMRange(m_playerARPM[i]);
                            loadMatricesToADSShader();
                            //(*shader)["PerFragADS"]->use();

                            prim->draw("rpmSphere");
                        }
                        m_transformStack.popTransform();
                    }
                }
                  break;
              }
              case PLAYERB:
              {
                  if(m_playerBRPM.size() != 0)
                  {
                      for(int i=0;i<m_playerBImpactPtsAllRalliesIn3D.size();i++)
                      {
                          m_transformStack.pushTransform();
                          {
                              m_transformStack.setPosition(m_playerBImpactPtsAllRalliesIn3D[i].x,
                                                           m_playerBImpactPtsAllRalliesIn3D[i].y,
                                                           m_playerBImpactPtsAllRalliesIn3D[i].z);


                              setColorBasedOnRPMRange(m_playerBRPM[i]);
                              loadMatricesToADSShader();
                              //(*shader)["PerFragADS"]->use();

                              prim->draw("rpmSphere");
                          }
                          m_transformStack.popTransform();

                       }
                  }
                  break;
              }
              case BOTH:
              {
                  // not allowed, just a place holder
                  break;
              }
            }
              break;
          }
        }
          // or disable start tracking once process stats is clicked--easier way
      }


      if(!(m_3dFlag))
      {

        m_transformStack.pushTransform();
        {
            m_2DScreen->draw(&drawing);
        }
        m_transformStack.popTransform();

        // this ngl text rendering should
        // have been placed along with the multiple switch cases
        // above, but for some reason ngl text was not rendering
        // when placed there, probably because of opencv buffer
        // rendering and shader switch....
        // so we first draw the texture shader buffer and then finally render the percentages
        // in a seperate module renderPercentages()
        if(m_whichStat == IMPACT_PERCENTAGE_DISTRIBUTION_VIEW)
        {
            renderPercentages();
        }
        else
        {
            // other stats do not rely on text rendering,so
            // do nothing
        }
      }
      else
      {



        // 3D drawing, taken care in multiple switch cases in place,so
        // do nothing other than drawing the appropriate net lines
          //loadMatricesToADSShader();


          ngl::VertexArrayObject *vao= ngl::VertexArrayObject::createVOA(GL_LINES);
          ngl::Vec3 points[2];


          m_transformStack.pushTransform();
          {
            // middle net line representaion for ht map
            if(m_whichStat == IMPACT_HEIGHT_MAP_VIEW)
            {

                //draw line
                vao->bind();
                points[0].m_x=0;
                points[0].m_y=0.2;
                points[0].m_z=m_fieldOfPlay.height/20.0;
                points[1].m_x= 0;
                points[1].m_y= 0.2;
                points[1].m_z= -(m_fieldOfPlay.height/20.0);

                vao->setData(2*sizeof(ngl::Vec3),points[0].m_x);
                vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(ngl::Vec3),0);
                vao->setNumIndices(2);
                ngl::ShaderLib *shader = ngl::ShaderLib::instance();
                (*shader)["PerFragADS"]->use();
                shader->setShaderParam3f("material.Kd",0.0,0.0,0.0);
                loadMatricesToADSShader();
                vao->draw();
                vao->removeVOA();

                legendWindow(m_heightMapLegend);

            }
            // middle net line representaion for trajectory
            else if(m_whichStat == TRAJECTORY_VIEW)
            {
                //draw line
                vao->bind();
                points[0].m_x=0;
                points[0].m_y=0.005;
                points[0].m_z=m_finalCourtDepthWorld/2;
                points[1].m_x= 0;
                points[1].m_y= 0.005;
                points[1].m_z= -(m_finalCourtDepthWorld/2);

                vao->setData(2*sizeof(ngl::Vec3),points[0].m_x);
                vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(ngl::Vec3),0);
                vao->setNumIndices(2);
                ngl::ShaderLib *shader = ngl::ShaderLib::instance();
                (*shader)["PerFragADS"]->use();
                shader->setShaderParam3f("material.Kd",0.0,0.0,0.0);
                loadMatricesToADSShader();
                vao->draw();
                vao->removeVOA();

                legendWindow(m_trajectoryLegend);
            }
            // for speed and rpm stats same scale is maintained
            else if(m_whichStat == VELOCITY_VIEW)
            {

                //draw line
                vao->bind();
                points[0].m_x=0;
                points[0].m_y=0;
                points[0].m_z=(m_finalCourtDepthWorld*10)/2;
                points[1].m_x= 0;
                points[1].m_y= 0;
                points[1].m_z= -((m_finalCourtDepthWorld*10)/2);

                vao->setData(2*sizeof(ngl::Vec3),points[0].m_x);
                vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(ngl::Vec3),0);
                vao->setNumIndices(2);
                ngl::ShaderLib *shader = ngl::ShaderLib::instance();
                (*shader)["PerFragADS"]->use();
                shader->setShaderParam3f("material.Kd",0.0,0.0,0.0);
                loadMatricesToADSShader();
                vao->draw();
                vao->removeVOA();


                legendWindow(m_speedLegend);

            }

            else if(m_whichStat == RPM_VIEW)
            {

                //draw line
                vao->bind();
                points[0].m_x=0;
                points[0].m_y=0;
                points[0].m_z=(m_finalCourtDepthWorld*10)/2;
                points[1].m_x= 0;
                points[1].m_y= 0;
                points[1].m_z= -((m_finalCourtDepthWorld*10)/2);

                vao->setData(2*sizeof(ngl::Vec3),points[0].m_x);
                vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(ngl::Vec3),0);
                vao->setNumIndices(2);
                ngl::ShaderLib *shader = ngl::ShaderLib::instance();
                (*shader)["PerFragADS"]->use();
                shader->setShaderParam3f("material.Kd",0.0,0.0,0.0);
                loadMatricesToADSShader();
                vao->draw();
                vao->removeVOA();

                legendWindow(m_rpmLegend);

            }

          }
          m_transformStack.popTransform();
      }
}


void StatsVisualization::timerEvent(
                          QTimerEvent *_event
                         )
{
    if(_event->timerId() == m_updateTimer)
    {
        KinectInterface *kinect=KinectInterface::instance();

        if(m_importFlag == false)
        {


        // when stats window is not created with import flag, it is live window
        // so go ahead with computation, if not fieldOfPlay and width and height
        // would be filled up by the parser

          if(kinect->getBounds())
          {
              m_fieldOfPlay = kinect->getBoxObject();

              m_width = m_fieldOfPlay.width;
              m_height = m_fieldOfPlay.height;
              m_midTopX = m_fieldOfPlay.x + m_width/2;
              m_midTopY = m_fieldOfPlay.y;
              m_midBottomX = m_midTopX;
              m_midBottomY = m_midTopY + m_height;

              m_midLeftX = m_fieldOfPlay.x;
              m_midLeftY = m_midTopY + m_height/2;
              m_midRightX = m_fieldOfPlay.x + m_width;
              m_midRightY = m_midLeftY;

              // we kill the timer once the bounds are calculated finally
              // when rt click is done
              killTimer(m_updateTimer);
          }



        }
        else if(m_importFlag)
        {
            killTimer(m_updateTimer);
        }

        updateGL();
    }


}

void StatsVisualization::processKey(
                                                    QKeyEvent *_event
                                                   )
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the StatsVisualization
 switch (_event->key())
  {

    default : break;
  }
  // finally update the StatsVisualization and re-draw
  updateGL();
}


// not needed----delete this module
void StatsVisualization::fillBallPoints(std::vector<cv::Point3f>& _inputBallPtsList)
{

    std::cout<<"Ball point size is:"<<_inputBallPtsList.size();

    m_inputBallPtsList = _inputBallPtsList;

    m_ballPtsFilled = 1;

    updateGL();
}


void StatsVisualization::updateDisplay()
{
    // this is trigerred when process stats is clicked
    // if process stats is clicked, then tracking has been stopped
    // and its time to check if data is processed - this check is done
    // in paintgl, so we do not have to do anything here apart from calling paintgl
    updateGL();
}

void StatsVisualization::resetPoints()
{
    m_playerAImpactData.clear();
    m_playerBImpactData.clear();
    updateGL();
}

// Once ball pts processed thru ball pts processing, data will be available in the player instances
// CALLS TO 2D AND 3D STATS FUNCTIONS WITH INPUTS BEING PLAYER SPECIFIC POINTS WILL BE SUFFICIENT
void StatsVisualization::fillAllImpactPointsBothPlayers()
{
    if(m_playerA->getPlayerBallPtsAll().size() != 0)
    {
        m_playerAImpactData = m_playerA->getPlayerImpactPtsAllRallies();
        m_impactPtsFilled = true;
    }

    if(m_playerB->getPlayerBallPtsAll().size() != 0)
    {
        m_playerBImpactData = m_playerB->getPlayerImpactPtsAllRallies();
        m_impactPtsFilled = true;
    }



}

void StatsVisualization::buildQuadSpecificData()
{
    m_percentageABL = m_playerA->getPercentageDistributionData(m_midTopX,m_midRightY,BOTTOM_LEFT);
    m_quadSpecABL = m_playerA->getQuadSpecificPoints(BOTTOM_LEFT);

    m_percentageATL = m_playerA->getPercentageDistributionData(m_midTopX,m_midRightY,TOP_LEFT);
    m_quadSpecATL = m_playerA->getQuadSpecificPoints(TOP_LEFT);

    m_percentageATR = m_playerA->getPercentageDistributionData(m_midTopX,m_midRightY,TOP_RIGHT);
    m_quadSpecATR = m_playerA->getQuadSpecificPoints(TOP_RIGHT);

    m_percentageABR = m_playerA->getPercentageDistributionData(m_midTopX,m_midRightY,BOTTOM_RIGHT);
    m_quadSpecABR = m_playerA->getQuadSpecificPoints(BOTTOM_RIGHT);

    m_percentageBBL = m_playerB->getPercentageDistributionData(m_midTopX,m_midRightY,BOTTOM_LEFT);
    m_quadSpecBBL = m_playerB->getQuadSpecificPoints(BOTTOM_LEFT);

    m_percentageBTL = m_playerB->getPercentageDistributionData(m_midTopX,m_midRightY,TOP_LEFT);
    m_quadSpecBTL = m_playerB->getQuadSpecificPoints(TOP_LEFT);

    m_percentageBTR = m_playerB->getPercentageDistributionData(m_midTopX,m_midRightY,TOP_RIGHT);
    m_quadSpecBTR = m_playerB->getQuadSpecificPoints(TOP_RIGHT);

    m_percentageBBR = m_playerB->getPercentageDistributionData(m_midTopX,m_midRightY,BOTTOM_RIGHT);
    m_quadSpecBBR = m_playerB->getQuadSpecificPoints(BOTTOM_RIGHT);

    m_quadMapBuilt = true;
}

void StatsVisualization::setWhichStatToView(int _inputWhichStat)
{
    m_whichStat = (WHICH_STAT)_inputWhichStat;

    if((m_whichStat == IMPACT_HEIGHT_MAP_VIEW) || (m_whichStat == TRAJECTORY_VIEW) ||
            (m_whichStat == VELOCITY_VIEW) || (m_whichStat == RPM_VIEW)) // add more later
    {
        m_3dFlag = true;
    }
    else
    {
        m_3dFlag = false;
    }
    updateGL();
}

void StatsVisualization::setWhoseStatToView(int _inputWhoseStat)
{
    m_whichPlayer = (WHOSE_STATS)_inputWhoseStat;
    updateGL();
}

void StatsVisualization::setWhichQuadToView(int _inputWhichQuad)
{
    m_whichQuad = (WHICH_QUADRANT)_inputWhichQuad;
    updateGL();
}

void StatsVisualization::setKinectToPlayAreaDistance(double _inputDistanceInMeters)
{
    // if the user wishes to change this value from UI
    // it has to be done before starting the tracking
    // this is not a dynamic variable
    m_kinectToPlayAreaInMeters = _inputDistanceInMeters;
}

void StatsVisualization::setDiameterOfBallInMeters(double _inputDiaInMeters)
{
    // if the user wishes to change this value from UI
    // it has to be done before starting the tracking
    // this is not a dynamic variable
    m_diameterOfBAllInMeters = _inputDiaInMeters;
}

void StatsVisualization::buildPointGraphBars()
{
    // grid resol is set to 10 now...
    // make it dynamic later

    std::vector<cv::Point3f> aBarGraphVertices,bBarGraphVertices;

    aBarGraphVertices = m_playerA->getBarGraphVertices(cv::Point(m_fieldOfPlay.x,m_fieldOfPlay.y),m_fieldOfPlay.width,
                                                         m_fieldOfPlay.height,10);

    bBarGraphVertices = m_playerB->getBarGraphVertices(cv::Point(m_fieldOfPlay.x,m_fieldOfPlay.y),m_fieldOfPlay.width,
                                                         m_fieldOfPlay.height,10);


    fillDataVertsWithNormals(aBarGraphVertices,m_dataVertsA);
    fillDataVertsWithNormals(bBarGraphVertices,m_dataVertsB);


    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

    // we scale it down only by 10 and not by 20 as tri plane is drawn from 0,0,0
    // so it will stretch from 10 untis up to 10 units down
    // similarly for width as well from left to right

    prim->createTrianglePlane("heightMapPlane",m_fieldOfPlay.width/10.0,m_fieldOfPlay.height/10.0,1,1,ngl::Vec3(0,1,0));

    // create a vao as a series of GL_TRIANGLES
    m_vao= ngl::VertexArrayObject::createVOA(GL_TRIANGLES);


}


void StatsVisualization::buildTrajectory()
{
    m_playerAInterpPtsMappedToRallies = m_playerA->getPlayerInterpolatedBallPtsAll(m_kinectToPlayAreaInMeters);
    m_playerBInterpPtsMappedToRallies = m_playerB->getPlayerInterpolatedBallPtsAll(m_kinectToPlayAreaInMeters);

    // conversion to world coords just to draw a static field of play
    // plane to be relative to the scale of the trajectories
    // i.e, both the field of play and the trajectories would be in Metres scale

    // note to self: DISPLAY the METRES SCALE on the window

    calculateFieldOfPlayInWorldCoords();

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

    // no scaling... 1 unit = 1 meter
    prim->createTrianglePlane("trajectoryPlane",m_finalCourtWidthWorld,m_finalCourtDepthWorld,1,1,ngl::Vec3(0,1,0));

    prim->createSphere("trajectorySphere",0.004,10.0);

    m_trajectoryBuilt = true;



}

void StatsVisualization::buildSpeedMap()
{
    m_speedAData = m_playerA->getPlayerSpeedData(m_kinectToPlayAreaInMeters);
    m_playerAImpactPtsAllRalliesIn3D = m_playerA->getPlayerImpactPtsIn3DAllRallies();

    m_speedBData = m_playerB->getPlayerSpeedData(m_kinectToPlayAreaInMeters);
    m_playerBImpactPtsAllRalliesIn3D = m_playerB->getPlayerImpactPtsIn3DAllRallies();

    std::cout<<"PlayerA speeddata size is:"<<m_speedAData.size()<<"\n";
    std::cout<<"PlayerA impct point size is:"<<m_playerAImpactPtsAllRalliesIn3D.size()<<"\n";

    std::cout<<"PlayerB speeddata size is:"<<m_speedBData.size()<<"\n";
    std::cout<<"PlayerB impct point size is:"<<m_playerBImpactPtsAllRalliesIn3D.size()<<"\n";

    calculateFieldOfPlayInWorldCoords();

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

    prim->createSphere("sphere",0.08,10.0);

    // scaled by 10 as the speedmap view's scale is
    // 1 unit = 0.1 meter
    prim->createTrianglePlane("speedPlane",m_finalCourtWidthWorld*10.0,m_finalCourtDepthWorld*10.0,1,1,ngl::Vec3(0,1,0));

    m_speedMapBuilt = true;



}

void StatsVisualization::calculateFieldOfPlayInWorldCoords()
{
    // have to set 0.70 to 1.0 and check if scaling issue gets resolved
    // and set it to depth paramter if something other than 1

    float fieldOfPlayXWorld = (m_fieldOfPlay.x/640 - 0.5) * m_kinectToPlayAreaInMeters * 1.111467f;
    float fieldOfPlayYWorld = (0.5 - m_fieldOfPlay.y/480) * m_kinectToPlayAreaInMeters * 0.833599f;

    float fieldOfPlayDownY = m_fieldOfPlay.y + m_fieldOfPlay.height;
    float fieldOfPlayDownYWorld = (0.5 - fieldOfPlayDownY/480) * m_kinectToPlayAreaInMeters * 0.833599f;

    float fieldOfPlayExtremeLeftX = m_fieldOfPlay.x + m_fieldOfPlay.width;
    float fieldOfPlayExtremeLeftXWorld = (fieldOfPlayExtremeLeftX/640 - 0.5) * m_kinectToPlayAreaInMeters * 1.111467f;

    m_finalCourtWidthWorld = fabs(fieldOfPlayExtremeLeftXWorld - fieldOfPlayXWorld);
    m_finalCourtDepthWorld = fabs(fieldOfPlayDownYWorld - fieldOfPlayYWorld);
}

void StatsVisualization::setColorBasedOnSpeedRange(float _inputVelocity)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["PerFragADS"]->use();

    if(_inputVelocity >= 0.0 && _inputVelocity < 0.3)
    {
       shader->setShaderParam3f("material.Kd",0.0,0.8,1.0);
    }
    else if(_inputVelocity >= 0.3 && _inputVelocity < 0.6)
    {
       shader->setShaderParam3f("material.Kd",1.0,1.0,0.0);
    }
    else if(_inputVelocity >= 0.6)
    {
       shader->setShaderParam3f("material.Kd",1.0,0.5,0.0);
    }
}

void StatsVisualization::buildRPMData()
{
    m_playerARPM = m_playerA->getRevolutionsPerMinute(m_diameterOfBAllInMeters,m_kinectToPlayAreaInMeters);
    m_playerAImpactPtsAllRalliesIn3D = m_playerA->getPlayerImpactPtsIn3DAllRallies();

    m_playerBRPM = m_playerB->getRevolutionsPerMinute(m_diameterOfBAllInMeters,m_kinectToPlayAreaInMeters);
    m_playerBImpactPtsAllRalliesIn3D = m_playerB->getPlayerImpactPtsIn3DAllRallies();

    std::cout<<"PlayerA rpmdata size is:"<<m_playerARPM.size()<<"\n";
    std::cout<<"PlayerA impct point size is:"<<m_playerAImpactPtsAllRalliesIn3D.size()<<"\n";

    std::cout<<"PlayerB rpmdata size is:"<<m_playerBRPM.size()<<"\n";
    std::cout<<"PlayerB impct point size is:"<<m_playerBImpactPtsAllRalliesIn3D.size()<<"\n";

    calculateFieldOfPlayInWorldCoords();

    // currently creating balls again which will be rendered color coded
    // based on the rpm value

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

    prim->createSphere("rpmSphere",0.08,10.0);

    // scaled by 10 as the speedmap view's scale is
    // 1 unit = 0.1 meter
    prim->createTrianglePlane("rpmPlane",m_finalCourtWidthWorld*10.0,m_finalCourtDepthWorld*10.0,1,1,ngl::Vec3(0,1,0));

    m_rpmDataBuilt = true;


}

void StatsVisualization::setColorBasedOnRPMRange(float _inputRPM)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["PerFragADS"]->use();

    if(_inputRPM >= 0 && _inputRPM < 100)
    {
       shader->setShaderParam3f("material.Kd",0.0,0.8,1.0);
    }
    else if(_inputRPM >= 100 && _inputRPM < 200)
    {
       shader->setShaderParam3f("material.Kd",1.0,1.0,0.0);
    }
    else if(_inputRPM >= 200)
    {
       shader->setShaderParam3f("material.Kd",1.0,0.5,0.0);
    }
}

void StatsVisualization::fillDataVertsWithNormals(std::vector<cv::Point3f>& _inputVertList,
                                                  std::vector<VertData>& o_normalFilledDataVerts)
{
    // we scale it down only by 10 and not by 20 as tri plane is drawn from 0,0,0
    // so it will stretch from 10 untis up to 10 units down
    // similarly for width as well from left to right
    int cubeIndex = 0;
    VertData d;

    // for every cube
    for(int i=0;i<_inputVertList.size();i=i+8)
    {
        //for every face in the cube - 6 vertices per face (2 triangle per face)
        for(int j=0;j<sizeof(indices)/sizeof(indices[0]);j=j+6)// size of indices = 36
        {
            // GUNDU FACE

            //triangle 1, v1
            d.x = _inputVertList[indices[j]+i].x;// - _gapFactor*normals[j/6].m_x;
            d.y = _inputVertList[indices[j]+i].y;//- _gapFactor*normals[j/6].m_y;
            d.z = _inputVertList[indices[j]+i].z;// - _gapFactor*normals[j/6].m_z;

            d.nx = normals[j/6].m_x;
            d.ny = normals[j/6].m_y;
            d.nz = normals[j/6].m_z;
            o_normalFilledDataVerts.push_back(d);

            //triangle 1, v2
            d.x = _inputVertList[indices[j+1]+i].x;// - _gapFactor*normals[j/6].m_x;
            d.y = _inputVertList[indices[j+1]+i].y;// - _gapFactor*normals[j/6].m_y;
            d.z = _inputVertList[indices[j+1]+i].z;// - _gapFactor*normals[j/6].m_z;

            d.nx = normals[j/6].m_x;
            d.ny = normals[j/6].m_y;
            d.nz = normals[j/6].m_z;
            o_normalFilledDataVerts.push_back(d);

            //triangle 1, v3
            d.x = _inputVertList[indices[j+2]+i].x;// - _gapFactor*normals[j/6].m_x;
            d.y = _inputVertList[indices[j+2]+i].y;// - _gapFactor*normals[j/6].m_y;
            d.z = _inputVertList[indices[j+2]+i].z;// - _gapFactor*normals[j/6].m_z;

            d.nx = normals[j/6].m_x;
            d.ny = normals[j/6].m_y;
            d.nz = normals[j/6].m_z;
            o_normalFilledDataVerts.push_back(d);

            //triangle 2, v1
            d.x = _inputVertList[indices[j+3]+i].x;// - _gapFactor*normals[j/6].m_x;
            d.y = _inputVertList[indices[j+3]+i].y;// - _gapFactor*normals[j/6].m_y;
            d.z = _inputVertList[indices[j+3]+i].z;// - _gapFactor*normals[j/6].m_z;

            d.nx = normals[j/6].m_x;
            d.ny = normals[j/6].m_y;
            d.nz = normals[j/6].m_z;
            o_normalFilledDataVerts.push_back(d);

            //triangle 2, v2
            d.x = _inputVertList[indices[j+4]+i].x;// - _gapFactor*normals[j/6].m_x;
            d.y = _inputVertList[indices[j+4]+i].y;// - _gapFactor*normals[j/6].m_y;
            d.z = _inputVertList[indices[j+4]+i].z;// - _gapFactor*normals[j/6].m_z;

            d.nx = normals[j/6].m_x;
            d.ny = normals[j/6].m_y;
            d.nz = normals[j/6].m_z;
            o_normalFilledDataVerts.push_back(d);

            //triangle 2, v3
            d.x = _inputVertList[indices[j+5]+i].x;// - _gapFactor*normals[j/6].m_x;
            d.y = _inputVertList[indices[j+5]+i].y;// - _gapFactor*normals[j/6].m_y;
            d.z = _inputVertList[indices[j+5]+i].z;// - _gapFactor*normals[j/6].m_z;

            d.nx = normals[j/6].m_x;
            d.ny = normals[j/6].m_y;
            d.nz = normals[j/6].m_z;

            o_normalFilledDataVerts.push_back(d);
        }
        cubeIndex++;
    }
}

void StatsVisualization::readFile(QString _inputFileName)
{
    if(_inputFileName.isEmpty())
    {
        std::cout<<"Filename empty. Returning. Read Aborted\n";
        return;
    }
    //read
    std::cout <<"Reading: "<<"\n";

    cv::FileStorage fs;
    fs.open(_inputFileName.toStdString(), cv::FileStorage::READ);


    fillDataFromFileForBothPlayers(fs,PLAYERA);
    fillDataFromFileForBothPlayers(fs,PLAYERB);

    cv::FileNode n8 = fs["FieldOfPlay"];
    cv::FileNodeIterator it8 = n8.begin();
    m_fieldOfPlay.x = (int)*it8;
    m_fieldOfPlay.y = (int)*(++it8);
    m_fieldOfPlay.width = (int)*(++it8);
    m_fieldOfPlay.height = (int)*(++it8);

    m_finalCourtWidthWorld = (float)fs["CourtWidth"];
    m_finalCourtDepthWorld = (float)fs["CourtDepth"];
    m_width = (int)fs["Width"];
    m_height = (int)fs["Height"];
    m_midTopX = (int)fs["MidTopX"];
    m_midTopY = (int)fs["MidTopY"];
    m_midBottomX = (int)fs["MidBottomX"];
    m_midBottomY = (int)fs["MidBottomY"];
    m_midLeftX = (int)fs["MidLeftX"];
    m_midLeftY = (int)fs["MidLeftY"];
    m_midRightX = (int)fs["MidRightX"];
    m_midRightY = (int)fs["MidRightY"];

    m_quadMapBuilt = true;
    m_speedMapBuilt = true;
    m_rpmDataBuilt = true;
    m_trajectoryBuilt = true;
    m_impactPtsFilled = true;

    //Generating our rendering stuff

    // create a vao as a series of GL_TRIANGLES
    m_vao= ngl::VertexArrayObject::createVOA(GL_TRIANGLES);

    // we will create separate planes and spheres for various statistics instead of
    // trying to scale them each time as we have different units of scaling and
    // those scale factors are calculated at various times
    // and this is much easier than that even though it takes quite a bit of memory

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    prim->createTrianglePlane("heightMapPlane",m_fieldOfPlay.width/10.0,m_fieldOfPlay.height/10.0,1,1,ngl::Vec3(0,1,0));
    prim->createTrianglePlane("trajectoryPlane",m_finalCourtWidthWorld,m_finalCourtDepthWorld,1,1,ngl::Vec3(0,1,0));
    prim->createSphere("trajectorySphere",0.004,10.0);
    prim->createSphere("sphere",0.08,10.0);
    prim->createTrianglePlane("speedPlane",m_finalCourtWidthWorld*10.0,m_finalCourtDepthWorld*10.0,1,1,ngl::Vec3(0,1,0));
    prim->createSphere("rpmSphere",0.08,10.0);
    prim->createTrianglePlane("rpmPlane",m_finalCourtWidthWorld*10.0,m_finalCourtDepthWorld*10.0,1,1,ngl::Vec3(0,1,0));

    updateGL();

}

// recieve file name input from UI
void StatsVisualization::writeFile()
{

    QString fileName = QFileDialog::getSaveFileName(this, "Save File","archive"," Xml File (*.xml)");


    // check if filename is valid, if so call read
    if(fileName.isEmpty())
    {
        std::cout<<"File name empty. Write aborted.\n";
        return;
    }

    cv::FileStorage fs(fileName.toStdString(), cv::FileStorage::WRITE);

    // we build all the data if not already present in the respective buffers
    // this might take quite a while as all data are captured in buffers
    // sequentially
    if(!(m_impactPtsFilled))
    {
        fillAllImpactPointsBothPlayers();
    }

    if(!(m_quadMapBuilt))
    {
        buildQuadSpecificData();
    }

    if(!(m_vao))
    {
        buildPointGraphBars();
    }

    if(!(m_trajectoryBuilt))
    {
        buildTrajectory();
    }

    if(!(m_speedMapBuilt))
    {
        buildSpeedMap();
    }

    if(!(m_rpmDataBuilt))
    {
        buildRPMData();
    }

    // Data corresponding to both the players written out to xml
    // Also, common data such as field of play width etc are written out to xml

    // PLAYERA data

    fs << "PLAYERA";
    fs<< "{";

    fs<< "ImpactData"<<m_playerAImpactData;

    fs<< "QuadSpecBL"<<m_quadSpecABL;

    fs<< "QuadSpecTL"<<m_quadSpecATL;

    fs<< "QuadSpecTR"<<m_quadSpecATR;

    fs<< "QuadSpecBR"<<m_quadSpecABR;

    fs<< "PercentageBL"<< m_percentageABL;

    fs<< "PercentageTL"<< m_percentageATL;

    fs<< "PercentageTR"<< m_percentageATR;

    fs<< "PercentageBR"<< m_percentageABR;

    fs<< "InterpPtsMappedToRallies"<< m_playerAInterpPtsMappedToRallies;

    fs<< "CubeDataVertices"<<"[";
    for(int i=0;i<m_dataVertsA.size();i++)
    {
        fs<< m_dataVertsA[i].nx;
        fs<< m_dataVertsA[i].ny;
        fs<< m_dataVertsA[i].nz;
        fs<< m_dataVertsA[i].x;
        fs<< m_dataVertsA[i].y;
        fs<< m_dataVertsA[i].z;
    }
    fs<< "]";

    fs<< "SpeedData"<<m_speedAData;

    fs<< "PlayerImpactPointsIn3D"<<m_playerAImpactPtsAllRalliesIn3D;

    fs<< "RPMData"<<m_playerARPM;

    fs<< "}";


    //PLAYERB data

    fs<< "PLAYERB";
    fs<< "{";

    fs<< "ImpactData"<<m_playerBImpactData;

    fs<< "QuadSpecBL"<<m_quadSpecBBL;

    fs<< "QuadSpecTL"<<m_quadSpecBTL;

    fs<< "QuadSpecTR"<<m_quadSpecBTR;

    fs<< "QuadSpecBR"<<m_quadSpecBBR;

    fs<< "PercentageBL"<< m_percentageBBL;

    fs<< "PercentageTL"<< m_percentageBTL;

    fs<< "PercentageTR"<< m_percentageBTR;

    fs<< "PercentageBR"<< m_percentageBBR;

    fs<< "InterpPtsMappedToRallies"<< m_playerBInterpPtsMappedToRallies;

    fs<< "CubeDataVertices"<<"[";
    for(int i=0;i<m_dataVertsB.size();i++)
    {
        fs<< m_dataVertsB[i].nx;
        fs<< m_dataVertsB[i].ny;
        fs<< m_dataVertsB[i].nz;
        fs<< m_dataVertsB[i].x;
        fs<< m_dataVertsB[i].y;
        fs<< m_dataVertsB[i].z;
    }
    fs<< "]";

    fs<< "SpeedData"<<m_speedBData;

    fs<< "PlayerImpactPointsIn3D"<<m_playerBImpactPtsAllRalliesIn3D;

    fs<< "RPMData"<<m_playerBRPM;

    fs<< "}";


    fs<<"FieldOfPlay"<<"["<<m_fieldOfPlay.x<<m_fieldOfPlay.y<<m_fieldOfPlay.width<<m_fieldOfPlay.height<<"]";
    fs<<"CourtWidth"<<m_finalCourtWidthWorld;
    fs<<"CourtDepth"<<m_finalCourtDepthWorld;
    fs<<"Width"<<m_width;
    fs<<"Height"<<m_height;
    fs<<"MidTopX"<<m_midTopX;
    fs<<"MidTopY"<<m_midTopY;
    fs<<"MidBottomX"<<m_midBottomX;
    fs<<"MidBottomY"<<m_midBottomY;
    fs<<"MidLeftX"<<m_midLeftX;
    fs<<"MidLeftY"<<m_midLeftY;
    fs<<"MidRightX"<<m_midRightX;
    fs<<"MidRightY"<<m_midRightY;

    fs.release();                                       // explicit close

    std::cout << "Write Done.\n";


}


void StatsVisualization::fillDataFromFileForBothPlayers(cv::FileStorage& _fs,WHOSE_STATS _whichPlayer)
{
    std::string whichPlayerString;

    if(_whichPlayer == PLAYERA)
    {
        whichPlayerString = "PLAYERA";
    }
    else
    {
        whichPlayerString = "PLAYERB";
    }

    cv::FileNode n1 = _fs[whichPlayerString]["ImpactData"];                         // Read string sequence - Get node
    if (n1.type() != cv::FileNode::SEQ)
    {
        std::cout << "Impact pt is not a sequence\n";
    }

    cv::FileNodeIterator it1 = n1.begin(), it1_end = n1.end(); // Go through the node
    for (; it1 != it1_end; ++it1++)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_playerAImpactData.push_back(cv::Point2f((float)*it1,(float)*(++it1)));
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_playerBImpactData.push_back(cv::Point2f((float)*it1,(float)*(++it1)));
        }
    }



    cv::FileNode n2 = _fs[whichPlayerString]["QuadSpecBL"];                         // Read string sequence - Get node
    if (n2.type() != cv::FileNode::SEQ)
    {
        std::cout << "ABL is not a sequence\n";
    }

    cv::FileNodeIterator it2 = n2.begin(), it2_end = n2.end(); // Go through the node
    for (; it2 != it2_end; ++it2++)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_quadSpecABL.push_back(cv::Point2f((float)*it2,(float)*(++it2)));
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_quadSpecBBL.push_back(cv::Point2f((float)*it2,(float)*(++it2)));
        }
    }


    cv::FileNode n3 = _fs[whichPlayerString]["QuadSpecTL"];                         // Read string sequence - Get node
    if (n3.type() != cv::FileNode::SEQ)
    {
        std::cout << "ATL is not a sequence\n";
    }

    cv::FileNodeIterator it3 = n3.begin(), it3_end = n3.end(); // Go through the node
    for (; it3 != it3_end; ++it3++)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_quadSpecATL.push_back(cv::Point2f((float)*it3,(float)*(++it3)));
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_quadSpecBTL.push_back(cv::Point2f((float)*it3,(float)*(++it3)));
        }
    }


    cv::FileNode n4 = _fs[whichPlayerString]["QuadSpecTR"];                         // Read string sequence - Get node
    if (n4.type() != cv::FileNode::SEQ)
    {
        std::cout << "ATR is not a sequence!\n";
    }

    cv::FileNodeIterator it4 = n4.begin(), it4_end = n4.end(); // Go through the node
    for (; it4 != it4_end; ++it4++)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_quadSpecATR.push_back(cv::Point2f((float)*it4,(float)*(++it4)));
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_quadSpecBTR.push_back(cv::Point2f((float)*it4,(float)*(++it4)));
        }
    }

    cv::FileNode n5 = _fs[whichPlayerString]["QuadSpecBR"];                         // Read string sequence - Get node
    if (n5.type() != cv::FileNode::SEQ)
    {
        std::cout << "ABR not a sequence!\n";
    }

    cv::FileNodeIterator it5 = n5.begin(), it5_end = n5.end(); // Go through the node
    for (; it5 != it5_end; ++it5++)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_quadSpecABR.push_back(cv::Point2f((float)*it5,(float)*(++it5)));
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_quadSpecBBR.push_back(cv::Point2f((float)*it5,(float)*(++it5)));
        }
    }


    if(whichPlayerString == "PLAYERA")
    {
        this->m_percentageABL = (float)_fs[whichPlayerString]["PercentageBL"];
        this->m_percentageATL = (float)_fs[whichPlayerString]["PercentageTL"];
        this->m_percentageATR = (float)_fs[whichPlayerString]["PercentageTR"];
        this->m_percentageABR = (float)_fs[whichPlayerString]["PercentageBR"];
    }
    else if(whichPlayerString == "PLAYERB")
    {
        this->m_percentageBBL = (float)_fs[whichPlayerString]["PercentageBL"];
        this->m_percentageBTL = (float)_fs[whichPlayerString]["PercentageTL"];
        this->m_percentageBTR = (float)_fs[whichPlayerString]["PercentageTR"];
        this->m_percentageBBR = (float)_fs[whichPlayerString]["PercentageBR"];
    }



    cv::FileNode trajectoryNode = _fs[whichPlayerString]["InterpPtsMappedToRallies"];  // Read string sequence - Get node
    if (trajectoryNode.type() != cv::FileNode::SEQ)
    {
        std::cout << "Trajectory node not a sequence!\n";
    }

    cv::FileNodeIterator trajectoryNodeit = trajectoryNode.begin(), trajectoryNodeit_end = trajectoryNode.end(); // Go through the node
    for (; trajectoryNodeit != trajectoryNodeit_end; ++trajectoryNodeit)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_playerATrajectoryFromFile.push_back((float)*trajectoryNodeit);
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_playerBTrajectoryFromFile.push_back((float)*trajectoryNodeit);
        }
    }


    cv::FileNode htMapNode = _fs[whichPlayerString]["CubeDataVertices"];  // Read string sequence - Get node
    if (htMapNode.type() != cv::FileNode::SEQ)
    {
        std::cout << "HtMap node not a sequence!\n";
    }

    cv::FileNodeIterator htMapNodeit = htMapNode.begin(), htMapNodeit_end = htMapNode.end(); // Go through the node
    for (; htMapNodeit != htMapNodeit_end; ++htMapNodeit)
    {
        VertData tempData;
        tempData.nx = (float)*(htMapNodeit);
        tempData.ny = (float)*(++htMapNodeit);
        tempData.nz = (float)*(++htMapNodeit);

        tempData.x = (float)*(++htMapNodeit);
        tempData.y = (float)*(++htMapNodeit);
        tempData.z = (float)*(++htMapNodeit);

        if(whichPlayerString == "PLAYERA")
        {
            this->m_dataVertsA.push_back(tempData);
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_dataVertsB.push_back(tempData);
        }
    }


    cv::FileNode n6 = _fs[whichPlayerString]["SpeedData"];                         // Read string sequence - Get node
    if (n6.type() != cv::FileNode::SEQ)
    {
        std::cout << "SpeedData not a sequence!\n";
    }

    cv::FileNodeIterator it6 = n6.begin(), it6_end = n6.end(); // Go through the node
    for (; it6 != it6_end; ++it6)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_speedAData.push_back((float)*it6);
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_speedBData.push_back((float)*it6);
        }
    }


    cv::FileNode impactPt3DNode = _fs[whichPlayerString]["PlayerImpactPointsIn3D"];  // Read string sequence - Get node
    if (impactPt3DNode.type() != cv::FileNode::SEQ)
    {
        std::cout << "PlayerImpactPointsIn3D node not a sequence!\n";
    }

    cv::FileNodeIterator impactPt3DNodeit = impactPt3DNode.begin(), impactPt3DNodeit_end = impactPt3DNode.end(); // Go through the node
    for (; impactPt3DNodeit != impactPt3DNodeit_end; ++impactPt3DNodeit)
    {
        cv::Point3f tempPoint3D((float)*(impactPt3DNodeit), (float)*(++impactPt3DNodeit), (float)*(++impactPt3DNodeit));

        if(whichPlayerString == "PLAYERA")
        {
            this->m_playerAImpactPtsAllRalliesIn3D.push_back(tempPoint3D);
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_playerBImpactPtsAllRalliesIn3D.push_back(tempPoint3D);
        }
    }


    cv::FileNode n7 = _fs[whichPlayerString]["RPMData"];                         // Read string sequence - Get node
    if (n7.type() != cv::FileNode::SEQ)
    {
        std::cout << "RPMData not a sequence!\n";
    }

    cv::FileNodeIterator it7 = n7.begin(), it7_end = n7.end(); // Go through the node
    for (; it7 != it7_end; ++it7)
    {
        if(whichPlayerString == "PLAYERA")
        {
            this->m_playerARPM.push_back((float)*it7);
        }
        else if(whichPlayerString == "PLAYERB")
        {
            this->m_playerBRPM.push_back((float)*it7);
        }

    }
}

void StatsVisualization::drawCrowd(const std::string& _inputPlaneName)
{

    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["TextureShaderStats"]->use();
    // need to bind the active texture before drawing
    glBindTexture(GL_TEXTURE_2D,m_crowdTextureName);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

    ngl::Real positionX,positionZ;
    positionX = positionZ = 0.0;

    if(_inputPlaneName == "heightMapPlane")
    {
        positionX = (m_fieldOfPlay.width/2);// + 300.0;
        positionZ = (m_fieldOfPlay.width/2) - 10; //285.0;
    }
    else if(_inputPlaneName == "trajectoryPlane")
    {
        positionX = m_finalCourtWidthWorld + 3.0;
        positionZ = m_finalCourtWidthWorld + 3.0;
    }
    else if((_inputPlaneName == "speedPlane") || (_inputPlaneName == "rpmPlane"))
    {
        positionX = (m_finalCourtWidthWorld * 10) + 30.0;
        positionZ = (m_finalCourtWidthWorld * 10) + 30.0;
    }
    // rt
    m_transformStack.pushTransform();
    {
        m_transformStack.setScale(10,10,10);
        m_transformStack.setRotation(0,90,90);
        m_transformStack.setPosition(positionX,0,0);

        loadMatricesToTextureShader();
        prim->draw(_inputPlaneName);
    }
    m_transformStack.popTransform();



    // lt
    m_transformStack.pushTransform();
    {
        m_transformStack.setScale(10,10,10);
        m_transformStack.setRotation(0,90,90);
        m_transformStack.setPosition(-(positionX),0,0);


        loadMatricesToTextureShader();
        prim->draw(_inputPlaneName);
    }
    m_transformStack.popTransform();



    // far
    m_transformStack.pushTransform();
    {
        m_transformStack.setScale(10,10,10);
        m_transformStack.setRotation(-90,0,0);
        m_transformStack.setPosition(0,0,positionZ);


        loadMatricesToTextureShader();
        prim->draw(_inputPlaneName);
    }
    m_transformStack.popTransform();



    // back
    m_transformStack.pushTransform();
    {
        m_transformStack.setScale(10,10,10);
        m_transformStack.setRotation(-90,0,0);
        m_transformStack.setPosition(0,0,-(positionZ));


        loadMatricesToTextureShader();
        prim->draw(_inputPlaneName);
    }
    m_transformStack.popTransform();



}

void StatsVisualization::drawTable(const std::string& _inputTableName)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["TextureShaderStats"]->use();
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

    ngl::Real positionY = 0.0;

    if((_inputTableName == "speedPlane") || (_inputTableName == "rpmPlane"))
    {
        positionY = -0.08;
    }
    else
    {
        positionY = 0.0;
    }
    m_transformStack.pushTransform();
    {
        m_transformStack.setPosition(0,positionY,0);
        glBindTexture(GL_TEXTURE_2D,m_tableTextureName);
        loadMatricesToTextureShader();
        prim->draw(_inputTableName);
    }
    m_transformStack.popTransform();

}

void StatsVisualization::drawContainingGridScaleXY()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["TextureShaderStats"]->use();
    // need to bind the active texture before drawing
    glBindTexture(GL_TEXTURE_2D,m_gridXYHtMap);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    ngl::Real positionX,positionZ,positionY;
    positionX = positionZ = positionY = 0;

    // we will create separate planes and spheres for various statistics instead of
    // trying to scale them each time as we have different units of scaling and
    // those scale factors are calculated at various times
    // and this is much easier than that even though it takes quite a bit of memory
    if(m_whichStat == IMPACT_HEIGHT_MAP_VIEW)
    {
        prim->createTrianglePlane("xygridrtplane",m_fieldOfPlay.height/10.0,20,1,1,ngl::Vec3(0,1,0));
        prim->createTrianglePlane("xygridfarplane",m_fieldOfPlay.width/10.0,20,1,1,ngl::Vec3(0,1,0));
        positionX = -(m_fieldOfPlay.width/20.0);
        positionY = 10;
        positionZ = -(m_fieldOfPlay.height/20.0);
    }
    else if(m_whichStat == TRAJECTORY_VIEW)
    {
        prim->createTrianglePlane("xygridrtplane",m_finalCourtDepthWorld,0.5,1,1,ngl::Vec3(0,1,0));
        prim->createTrianglePlane("xygridfarplane",m_finalCourtWidthWorld,0.5,1,1,ngl::Vec3(0,1,0));
        positionX = -(m_finalCourtWidthWorld/2.0);
        positionY = 0.25;
        positionZ = -(m_finalCourtDepthWorld/2.0);
    }


    // rt
    m_transformStack.pushTransform();
    {
        //m_transformStack.setScale(m_fieldOfPlay.width,20,0);
        m_transformStack.setRotation(0,90,-270);
        m_transformStack.setPosition(positionX,positionY,0);
        loadMatricesToTextureShader();
        prim->draw("xygridrtplane");

    }
    m_transformStack.popTransform();



    // far
    m_transformStack.pushTransform();
    {
        //m_transformStack.setScale(m_fieldOfPlay.width,20,0);
        m_transformStack.setRotation(-90,0,0);
        m_transformStack.setPosition(0,positionY,positionZ);
        loadMatricesToTextureShader();
        prim->draw("xygridfarplane");

    }
    m_transformStack.popTransform();


}

void StatsVisualization::renderPercentages()
{

    QString text;

    float bottomLeftPosX = m_midTopX - (m_fieldOfPlay.width/4);
    float bottomLeftPosY = m_midTopY + (m_fieldOfPlay.height/2);

    float topLeftPosX = m_midTopX - (m_fieldOfPlay.width/4);
    float topLeftPosY = m_midTopY + (m_fieldOfPlay.height/4);

    float topRightPosX = m_midTopX + (m_fieldOfPlay.width/8);
    float topRightPosY = m_midTopY + (m_fieldOfPlay.height/4);

    float bottomRightPosX = m_midTopX + (m_fieldOfPlay.width/8);
    float bottomRightPosY = m_midTopY + (m_fieldOfPlay.height/2);

    //m_text->setScreenSize(640,480);

    switch(m_whichStat)
    {
        case IMPACT_PERCENTAGE_DISTRIBUTION_VIEW:
        {
            switch(m_whichPlayer)
            {
                case PLAYERA:
                {
                    m_text->setColour(1,1,0);
                    switch(m_whichQuad)
                    {
                        case BOTTOM_LEFT:
                        {
                            text.sprintf("%3.2f %%",m_percentageABL);
                            m_text->renderText(bottomLeftPosX,bottomLeftPosY,text);//bl
                            break;
                        }
                        case TOP_LEFT:
                        {
                            text.sprintf("%3.2f %%",m_percentageATL);
                            m_text->renderText(topLeftPosX,topLeftPosY,text);//tl
                            break;
                        }
                        case TOP_RIGHT:
                        {
                            text.sprintf("%3.2f %%",m_percentageATR);
                            m_text->renderText(topRightPosX,topRightPosY,text);//tr
                            break;
                        }
                        case BOTTOM_RIGHT:
                        {
                            text.sprintf("%3.2f %%",m_percentageABR);
                            m_text->renderText(bottomRightPosX,bottomRightPosY,text);//br
                            break;
                        }
                        case ALL:
                        {
                            text.sprintf("%3.2f %%",m_percentageABL);
                            m_text->renderText(bottomLeftPosX,bottomLeftPosY,text);//bl

                            text.sprintf("%3.2f %%",m_percentageATL);
                            m_text->renderText(topLeftPosX,topLeftPosY,text);//tl

                            text.sprintf("%3.2f %%",m_percentageATR);
                            m_text->renderText(topRightPosX,topRightPosY,text);//tr

                            text.sprintf("%3.2f %%",m_percentageABR);
                            m_text->renderText(bottomRightPosX,bottomRightPosY,text);//br

                            break;
                        }

                    }
                    break;
                }

            case PLAYERB:
            {
                m_text->setColour(0,0,0);
                switch(m_whichQuad)
                {
                    case BOTTOM_LEFT:
                    {
                        text.sprintf("%3.2f %%",m_percentageBBL);
                        m_text->renderText(bottomLeftPosX,bottomLeftPosY,text);//bl
                        break;
                    }
                    case TOP_LEFT:
                    {
                        text.sprintf("%3.2f %%",m_percentageBTL);
                        m_text->renderText(topLeftPosX,topLeftPosY,text);//tl
                        break;
                    }
                    case TOP_RIGHT:
                    {
                        text.sprintf("%3.2f %%",m_percentageBTR);
                        m_text->renderText(topRightPosX,topRightPosY,text);//tr
                        break;
                    }
                    case BOTTOM_RIGHT:
                    {
                        text.sprintf("%3.2f %%",m_percentageBBR);
                        m_text->renderText(bottomRightPosX,bottomRightPosY,text);//br
                        break;
                    }
                    case ALL:
                    {
                        text.sprintf("%3.2f %%",m_percentageBBL);
                        m_text->renderText(bottomLeftPosX,bottomLeftPosY,text);//bl

                        text.sprintf("%3.2f %%",m_percentageBTL);
                        m_text->renderText(topLeftPosX,topLeftPosY,text);//tl

                        text.sprintf("%3.2f %%",m_percentageBTR);
                        m_text->renderText(topRightPosX,topRightPosY,text);//tr

                        text.sprintf("%3.2f %%",m_percentageBBR);
                        m_text->renderText(bottomRightPosX,bottomRightPosY,text);//br
                        break;
                    }

                }
                break;
            }
            case BOTH:
            {
                switch(m_whichQuad)
                {
                    case BOTTOM_LEFT:
                    {
                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageABL);
                        m_text->renderText(bottomLeftPosX - 80,bottomLeftPosY,text);//bl

                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBBL);
                        m_text->renderText(bottomLeftPosX + 10,bottomLeftPosY,text);//bl
                        break;
                    }
                    case TOP_LEFT:
                    {
                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageATL);
                        m_text->renderText(topLeftPosX - 80,topLeftPosY,text);//tl

                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBTL);
                        m_text->renderText(topLeftPosX + 10,topLeftPosY,text);//tl
                        break;
                    }
                    case TOP_RIGHT:
                    {
                       m_text->setColour(1,1,0);
                       text.sprintf("(%3.0f / ",m_percentageATR);
                       m_text->renderText(topRightPosX - 60,topRightPosY,text);//tr

                       m_text->setColour(0,0,0);
                       text.sprintf("%3.0f)%%",m_percentageBTR);
                       m_text->renderText(topRightPosX + 30,topRightPosY,text);//tr

                       break;
                    }
                    case BOTTOM_RIGHT:
                    {
                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageABR);
                        m_text->renderText(bottomRightPosX - 60,bottomRightPosY,text);//br

                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBBR);
                        m_text->renderText(bottomRightPosX + 30,bottomRightPosY,text);//br
                        break;
                    }
                    case ALL:
                    {
                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageABL);
                        m_text->renderText(bottomLeftPosX - 80,bottomLeftPosY,text);//bl
                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBBL);
                        m_text->renderText(bottomLeftPosX + 10,bottomLeftPosY,text);//bl

                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageATL);
                        m_text->renderText(topLeftPosX - 80,topLeftPosY,text);//tl
                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBTL);
                        m_text->renderText(topLeftPosX + 10,topLeftPosY,text);//tl

                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageATR);
                        m_text->renderText(topRightPosX - 60,topRightPosY,text);//tr
                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBTR);
                        m_text->renderText(topRightPosX + 30,topRightPosY,text);//tr

                        m_text->setColour(1,1,0);
                        text.sprintf("(%3.0f / ",m_percentageABR);
                        m_text->renderText(bottomRightPosX - 60,bottomRightPosY,text);//br
                        m_text->setColour(0,0,0);
                        text.sprintf("%3.0f)%%",m_percentageBBR);
                        m_text->renderText(bottomRightPosX + 30,bottomRightPosY,text);//br

                        break;
                    }

                }
                break;
            }

            }
            break;
        }

        default:
            break;
    }

    // this has to be done at the end to avoid 3d stats rendering in black
    // ngl text has something which makes other shaders work only when ngl
    // text color is set to white before using other shaders
    m_text->setColour(1,1,1);
}

//--------------------------------------------------
/* ALL MOUSE EVENTS SHOULD ONLY WORK IN 3D....so place the parentvar check before processing any mouse events*/
//--------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
void StatsVisualization::mouseMoveEvent (
                               QMouseEvent * _event
                              )
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    updateGL();

  }
  // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    updateGL();

  }

}


//----------------------------------------------------------------------------------------------------------------------
void StatsVisualization::mousePressEvent (
                                QMouseEvent * _event
                               )
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void StatsVisualization::mouseReleaseEvent (
                                  QMouseEvent * _event
                                 )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void StatsVisualization::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if(_event->delta() > 0)
  {
    m_modelPos.m_z+=ZOOM;
  }
  else if(_event->delta() <0 )
  {
    m_modelPos.m_z-=ZOOM;
  }
  updateGL();
}


void StatsVisualization::legendWindow(GLuint _inputLegendName)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    (*shader)["TextureShaderStats"]->use();
    // need to bind the active texture before drawing
    glBindTexture(GL_TEXTURE_2D,_inputLegendName);

    ngl::Transformation trans;

     // get the VBO instance and draw the built in teapot
    ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

    // we will create separate planes and spheres for various statistics instead of
    // trying to scale them each time as we have different units of scaling and
    // those scale factors are calculated at various times
    // and this is much easier than that even though it takes quite a bit of memory
    prim->createTrianglePlane("legendplane",0.8,0.8,1,1,ngl::Vec3(0,1,0));

    // first draw a top  persp // front //side
    ngl::Vec3 from(0,2,0);
    ngl::Vec3 to(0,0,0);
    ngl::Vec3 up(0,0,-1);
    /// a front view (bottom left)
    m_transformStack.pushTransform();
    {
      from.set(0,0,2);
      up.set(0,1,0);
      m_view=ngl::lookAt(from,to,up);
      m_projection=ngl::ortho(-1,1,-1,1, 0.01f, 200.0f);

      glViewport(10,10,m_legendWidth,m_legendHeight);
      //m_2DScreen->draw();

      m_transformStack.setGlobal(trans);
      m_transformStack.setRotation(90,180,180);
      m_transformStack.setPosition(0,0,0);
      //m_transformStack.setScale(m_legendWidth,m_legendHeight,0);
      //loadMatricesToTextureShader();
      loadMatricesToLegendShader();
      prim->draw("legendplane");

    }
    m_transformStack.popTransform();

}

void StatsVisualization::loadMatricesToLegendShader()
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    ngl::Mat4 MVP;
    ngl::Mat4 MV;
    ngl::Mat4 M;
    M=m_transformStack.getCurrAndGlobal().getMatrix();
    MV=  m_transformStack.getCurrAndGlobal().getMatrix()*m_view;
    MVP= M*m_view*m_projection;
    shader->setShaderParamFromMat4("MVP",MVP);
    shader->setUniform("textureFlag",true);

}

