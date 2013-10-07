#ifndef SCREEN_QUAD_H__
#define SCREEN_QUAD_H__
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>
#include "opencv2/opencv.hpp"


/// @file ScreenQuad.h
/// @brief a basic Qt GL window class for ngl demos
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/10/10
/// Revision History :
/// Initial Version 10/10/10 (Binary day ;-0 )
/// @class ScreenQuad
/// @brief this is taken from Jon's demo

class ScreenQuad
{
  public :
  /// @brief ctor, note this will scale to any screen size so
  /// the width and height are for the texture image passed in
    ScreenQuad(
               int _textureWidth,
               int _textureHeight,
               const std::string &_shader
                );
    ~ScreenQuad();
    void draw(cv::Mat *_data);

    inline void setTextureMode(GLenum _mode){m_textureMode=_mode;}

  private :
    GLuint m_vao;
    GLuint m_texture;
    std::string m_shader;
    int m_textureWidth;
    int m_textureHeight;
    GLenum m_textureMode;

};




#endif
