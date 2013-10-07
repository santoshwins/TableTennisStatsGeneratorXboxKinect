#include "ScreenQuad.h"
#include <ngl/ShaderLib.h>


ScreenQuad::ScreenQuad(int _width, int _height,const std::string &_shader)
{


//static ngl::Vec3 verts[]=
//                        {
//                          ngl::Vec3(-1,-1,0),
//                          ngl::Vec3(1,-1,0),
//                          ngl::Vec3(1,1,0),
//                          ngl::Vec3(1,1,0),
//                          ngl::Vec3(-1,-1,0),
//                          ngl::Vec3(-1,1,0)
//                        };


    static ngl::Vec3 verts[]=
                            {
                              ngl::Vec3(-1,-1,0),
                              ngl::Vec3(-1,1,0),
                              ngl::Vec3(1,1,0),
                              ngl::Vec3(1,1,0),
                              ngl::Vec3(-1,-1,0),
                              ngl::Vec3(1,-1,0)
                            };


//static ngl::Vec2 uv[]=
//                      {
//                        ngl::Vec2(1,1),
//                        ngl::Vec2(0,1),
//                        ngl::Vec2(0,0),
//                        ngl::Vec2(0,0),
//                        ngl::Vec2(1,1),
//                        ngl::Vec2(1,0)
//                      };


static ngl::Vec2 uv[]=
                      {
                        ngl::Vec2(0,1),
                        ngl::Vec2(0,0),
                        ngl::Vec2(1,0),
                        ngl::Vec2(1,0),
                        ngl::Vec2(0,1),
                        ngl::Vec2(1,1)
                      };


  m_shader=_shader;
  m_textureWidth=_width;
  m_textureHeight=_height;

  glGenVertexArrays(1,&m_vao);
  glBindVertexArray(m_vao);

  GLuint vbo[2];
  glGenBuffers(2,vbo);
  glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
  glBufferData(GL_ARRAY_BUFFER,6*sizeof(ngl::Vec3),&verts[0].m_x,GL_STATIC_DRAW);

  glVertexAttribPointer(0, // atrib 0
                        3, // with x,y,z
                        GL_FLOAT, // what type
                        GL_FALSE, // normalize?
                        0, // stride
                        0 // start ptr
                        );
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
  glBufferData(GL_ARRAY_BUFFER,6*sizeof(ngl::Vec2),&uv[0].m_x,GL_STATIC_DRAW);

  glVertexAttribPointer(1, // atrib 1
                        2, // with u,v
                        GL_FLOAT, // what type
                        GL_FALSE, // normalize?
                        0, // stride
                        0 // start ptr
                        );
  glEnableVertexAttribArray(1);



  glBindVertexArray(0);

  glGenTextures(1, &m_texture);
  glActiveTextureARB(GL_TEXTURE0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  m_textureMode=GL_RGB;

}

ScreenQuad::~ScreenQuad()
{
  glDeleteVertexArrays(1,&m_vao);
  glDeleteTextures(1,&m_texture);
}

void ScreenQuad::draw(cv::Mat *_data)
{
  ngl::ShaderLib *shader = ngl::ShaderLib::instance();
  shader->use(m_shader);
  shader->setUniform("textureFlag",false);

  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glBindVertexArray(m_vao);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, m_textureMode, m_textureWidth, m_textureHeight, 0, m_textureMode, GL_UNSIGNED_BYTE, &_data->data[0]);
  glGenerateMipmap(GL_TEXTURE_2D);
  glDrawArrays(GL_TRIANGLES,0,6);
  glBindVertexArray(0);
}


