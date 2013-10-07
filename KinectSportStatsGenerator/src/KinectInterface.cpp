/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2010 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */

//----------------------------------------------------------------------------------------------------------------------
/// @file KinectInterface.cpp
/// @brief a Qt wrapper for the OpenKinect project a lot of the code has been modified from
/// this project source here https://github.com/OpenKinect/libfreenect
//----------------------------------------------------------------------------------------------------------------------

#include <KinectInterface.h>
#include <QDebug>
#include <cstdlib>
#include <cmath>
#include <cassert>
//#include <boost/lexical_cast.hpp>



//----------------------------------------------------------------------------------------------------------------------
KinectInterface *KinectInterface::s_instance = 0;// initialize pointer
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
void QKinectProcessEvents::run()
{
    // loop while we are active and process the kinect event queue
    while(m_active)
    {
        //qDebug()<<"process thread\n";
        if(freenect_process_events(m_ctx) < 0)
        {
            throw std::runtime_error("Cannot process freenect events");
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Destructor ---------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
KinectInterface::~KinectInterface()
{
        // dtor is not called directly
        if (s_instance)
        {
                delete s_instance;
        }
}
//----------------------------------------------------------------------------------------------------------------------
/// @brief Get instance --------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
KinectInterface* KinectInterface::instance()
{
    // this is the main singleton code first check to see if we exist
    if (s_instance==0 )
    {
        // we do so create an instance (this will validate the pointer so other
        // methods called in the init function will have a valid pointer to use)
        s_instance = new KinectInterface;
        // now setup the actual class (with a valid pointer)
        /// @note this could be made nicer to make it fully thread safe
        if(s_instance->init() ==  false)
        {
            s_instance = 0;
        }
    }
    // otherwise return the existing pointer
    return s_instance;
}


//----------------------------------------------------------------------------------------------------------------------
KinectInterface::KinectInterface() : QObject(0)
{
    // nothing to see here we just need a valid object pointer the init
    // method does all the hard work
    qDebug()<<"ctor called \n";

    for( unsigned int i = 0 ; i < 640*480 ; ++i)
    {
        m_bufferDepthinM[i] = 0;
    }




}
//----------------------------------------------------------------------------------------------------------------------
bool KinectInterface::init()
{
    // first see if we can init the kinect
    if (freenect_init(&m_ctx, NULL) < 0)
    {
        qDebug()<<"freenect_init() failed\n";
        exit(EXIT_FAILURE);
    }
    /// set loggin level make this programmable at some stage
    freenect_set_log_level(m_ctx, FREENECT_LOG_DEBUG);
    /// see how many devices we have
    int nr_devices = freenect_num_devices (m_ctx);
    qDebug()<<"Number of devices found: "<<nr_devices<<"\n";

    if(nr_devices < 1)
    {
        //delete s_instance;
        //s_instance = 0;
        return false;
    }
    /// now allocate the buffers so we can fill them
    m_userDeviceNumber = 0;
    // grab the buffer size and store for later use
    m_resolutionRGBBytes=freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM,FREENECT_VIDEO_RGB).bytes;
    m_bufferDepth=cvCreateMat(480,640,CV_8UC3);

    //m_bufferVideo.resize(m_resolutionRGBBytes);

    m_bufferVideo = cvCreateMat(480,640,CV_8UC3);

//    m_nextBuffer = cvCreateMat(480,640,CV_8UC1);
//    m_prevBuffer = cvCreateMat(480,640,CV_8UC1);
//    m_diffBuffer = cvCreateMat(480,640,CV_8UC1);

    m_resolutionDepthBytes=freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM,FREENECT_DEPTH_11BIT).bytes;
    //m_bufferDepthRaw.resize(m_resolutionDepthBytes);
    m_bufferDepthRaw16=cvCreateMat(480,640,CV_8UC1);

    m_bufferDepthRaw=cvCreateMat(480,640,CV_8UC1);



   // m_originalFrameDepth=NULL;



    m_gamma.resize(2048);
    /// open the device at present hard coded to device 0 as I only
    /// have 1 kinect
    /// \todo make this support multiple devices at some stage
    if (freenect_open_device(m_ctx, &m_dev, m_userDeviceNumber) < 0)
    {
        qDebug()<<"Could not open device\n";
        exit(EXIT_FAILURE);
    }


    /// build the gamma table used for the depth to rgb conversion
    /// taken from the demo programs
//    for (int i=0; i<2048; ++i)
//    {
//        float v = i/2048.0;
//        v = std::pow(v, 3)* 6;
//        m_gamma[i] = v*6*256;
//    }


    // from opencv imaging imformation wiki page http://openkinect.org/wiki/Imaging_Information
    const float k1 = 1.1863;
    const float k2 = 2842.5;
    const float k3 = 0.1236;
    const float offset = 0.037;
    float depth = 0;
    for (size_t i=0; i<2048; i++)
    {
        depth = k3 * tanf(i/k2 + k1) - offset;
        m_gamma[i] = depth;
    }


    /// init our flags
    m_newRgbFrame=false;
    m_newDepthFrame=false;
    m_deviceActive=true;

    m_threshValue = 100;


    // set our video formats to RGB by default
    /// @todo make this more flexible at some stage
    freenect_set_video_mode(m_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
    freenect_set_depth_mode(m_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
    // deprecated
    //freenect_set_video_format(m_dev, FREENECT_VIDEO_RGB);
    //freenect_set_depth_format(m_dev, FREENECT_DEPTH_11BIT);
    /// hook in the callbacks
    freenect_set_depth_callback(m_dev, depthCallback);
    freenect_set_video_callback(m_dev, videoCallback);
    // start the video and depth sub systems
    startVideo();
    startDepth();
    // set the thread to be active and start
    m_process = new QKinectProcessEvents(m_ctx);
    m_process->setActive();
    m_process->start();

    m_depthLower = 0.02;
    m_depthHigher = 1.02; // has to be just above the table (in meteres)

    //m_selectedBoxCoords = NULL;

    m_selectedBoxCoords = cv::Rect(0,0,0,0);

    m_toggleTracking = false;
    m_setBounds = false;

    return true;
}

//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::shutDownKinect()
{
    /// stop the processing thread
    m_process->setInActive();
    /// stop the video and depth callbacks
    freenect_stop_depth(m_dev);
    freenect_stop_video(m_dev);
    // close down our devices
    freenect_close_device(m_dev);
    freenect_shutdown(m_ctx);

}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setLedOff()
{
    freenect_set_led(m_dev,LED_OFF);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setRedLed()
{
    freenect_set_led(m_dev,LED_RED);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setGreenLed()
{
    freenect_set_led(m_dev,LED_GREEN);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setYellowLed()
{
    freenect_set_led(m_dev,LED_YELLOW);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setRedLedFlash()
{
    freenect_set_led(m_dev,LED_BLINK_RED_YELLOW);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setGreenLedFlash()
{
 freenect_set_led(m_dev,LED_BLINK_GREEN);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setYellowLedFlash()
{
    freenect_set_led(m_dev,LED_BLINK_RED_YELLOW);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setVideoMode(
                                                     int _mode
                                                    )
{
    freenect_video_format vm=FREENECT_VIDEO_RGB;
    switch(_mode)
    {
        case 0 : { vm=FREENECT_VIDEO_RGB; break;}
        case 1 : { vm=FREENECT_VIDEO_YUV_RGB; break;}
        case 2 : { vm=FREENECT_VIDEO_IR_8BIT; break;}
        /*
        /// had issues with these modes so sticking to the 3 that work
        case 1 : { vm=FREENECT_VIDEO_BAYER; break;}
        case 2 : { vm=FREENECT_VIDEO_IR_8BIT; break;}
        case 3 : { vm=FREENECT_VIDEO_IR_10BIT; break;}
        case 4 : { vm=FREENECT_VIDEO_IR_10BIT_PACKED; break;}
        case 5 : { vm=FREENECT_VIDEO_YUV_RGB; break;}
        case 6 : { vm=FREENECT_VIDEO_YUV_RAW; break;}
        */
        default : qDebug()<<"index out of bounds for video mode\n";
                            vm=FREENECT_VIDEO_RGB;
        break;
    }
    /// stop the video and set to new mode
    freenect_stop_video(m_dev);
    freenect_set_video_mode(m_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
    //deprecated
//	freenect_set_video_format(m_dev, vm);
    freenect_start_video(m_dev);
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::resetAngle()
{
    freenect_set_tilt_degs(m_dev,0);
}

//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::setAngle(
                                             double _angle
                                            )
{
    // constrain the angle from -30 - +30
    if(_angle > 30)
    {
        _angle = 30;
    }
    else if(_angle <-30)
    {
        _angle=-30;
    }
    freenect_set_tilt_degs(m_dev,_angle);
}


//----------------------------------------------------------------------------------------------------------------------
bool KinectInterface::getRGB(cv::Mat &o_buffer)
{

    //static int _frameCount = 1;
    /// this grabs the rgb data we first need to lock our mutex
    /// this will be unlocked on destruction of the locker
    QMutexLocker locker( &m_mutex );
    // do we have a new frame?
    if(m_newRgbFrame)
    {

        //cv::GaussianBlur(m_bufferVideo,m_bufferVideo,cv::Size(3,3),0.3);

//        cv::cvtColor( m_bufferVideo, m_bufferVideoGray, CV_BGR2GRAY );


//        // if so swap out and fill
//        if(_frameCount == 1)
//        {
//            m_bufferVideoGray.copyTo(m_prevBuffer);
//            //m_bufferVideo.copyTo(m_newBuffer);
//        }

//        m_bufferVideoGray.copyTo(m_nextBuffer);
//        cv::absdiff(m_nextBuffer,m_prevBuffer,m_diffBuffer); // has gpu variant
//        //m_diffBuffer.copyTo(m_nextBuffer - m_prevBuffer); // use cv function
//        m_nextBuffer.copyTo(m_prevBuffer);

//        m_diffBuffer.copyTo(o_buffer);

//        _frameCount++;

        m_bufferVideo.copyTo(o_buffer);
        m_newRgbFrame = false;
        return true;
    }
    else
    {
    return false;
    }
}


//----------------------------------------------------------------------------------------------------------------------
bool KinectInterface::getDepth(
                                                cv::Mat &o_buffer
                                            )
{
    // this fills the depth buffer, first lock our mutex
    QMutexLocker locker( &m_mutex );
    if(m_newDepthFrame)
    {
        // swap data
        m_bufferDepth.copyTo(o_buffer);

        m_newDepthFrame = false;
        return true;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------------------------------------------------
bool KinectInterface::getDepth16Bit(
                                                         cv::Mat &o_buffer
                                                     )
{
    /// fill the 16 Bit data value first lock mutex
    QMutexLocker locker( &m_mutex );
    if(m_newDepthFrame)
    {
        // fill our buffer if avaliable
        m_bufferDepthRaw16.copyTo(o_buffer);
        m_newDepthFrame = false;
        return true;
    }
    else
    {
        return false;
    }
}


//----------------------------------------------------------------------------------------------------------------------
bool KinectInterface::getDepthSliced(
                                                         cv::Mat &o_buffer
                                                     )
{
    /// fill the 16 Bit data value first lock mutex
    QMutexLocker locker( &m_mutex );
    if(m_newDepthFrame)
    {
        // fill our buffer if avaliable
        m_bufferDepthRaw.copyTo(o_buffer);
        m_newDepthFrame = false;
        return true;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
bool KinectInterface::getDepthRaw(cv::Mat &o_buffer, float *o_depthinmBuffer)
{


    QMutexLocker locker( &m_mutex );
    if(m_newDepthFrame)
    {

        m_bufferDepthRaw.copyTo(o_buffer);
        for( unsigned int i = 0 ; i < 640*480 ; ++i)
        {
            o_depthinmBuffer[i] = m_bufferDepthinM[i];
        }
        m_newDepthFrame = false;
        return true;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::grabDepth(
                                                void *_depth,
                                                uint32_t _timestamp
                                             )
{
// this method fills all the different depth buffers at once
// modifed from the sample code glview and cppview.cpp
/// lock our mutex
QMutexLocker locker( &m_mutex );
// cast the void pointer to the unint16_t the data is actually in
uint16_t* depth = static_cast<uint16_t*>(_depth);



// now loop and fill data buffers
for( unsigned int i = 0 ; i < 640*480 ; ++i)
{
    // first our two raw buffers the first will lose precision and may well
    // be removed in the next iterations

    // now get the index into the gamma table
    float pval = m_gamma[depth[i]]; // my notes: so pval is in metres now..using cam caliberation values, we have converted depth values to metres
                                    // using open kinect forum formulae this (i,j,z) value can be transformed to (x,y,z)





    //m_bufferDepthinM.data[i] = m_gamma[depth[i]];

    //m_originalFrameDepth1[_frameCountGrab] = m_gamma[depth[i]];


    //std::cout<<"pval is"<<pval<<"\n";

    // a bounded volume as explained in rally keeper project to detect contours
    if (pval >= m_depthLower && pval <= m_depthHigher)
    {
        m_bufferDepthRaw.data[i]=255;
        m_bufferDepthRaw16.data[i]=255;
        m_bufferDepthinM[i] = depth[i];
    }
    else
    {
        m_bufferDepthRaw.data[i]=0;
        m_bufferDepthRaw16.data[i]=0;
    }
    if (pval <= 0.9)
    {
        m_bufferDepth.data[3*i+0] = 255;
        m_bufferDepth.data[3*i+1] = 0;
        m_bufferDepth.data[3*i+2] = 0;

    }
    else if (pval > 0.9 && pval <= 2)
    {
        m_bufferDepth.data[3*i+0] = 0;
        m_bufferDepth.data[3*i+1] = 255;
        m_bufferDepth.data[3*i+2] = 0;

    }
    else if (pval > 2)
    {
        m_bufferDepth.data[3*i+0] = 0;
        m_bufferDepth.data[3*i+1] = 0;
        m_bufferDepth.data[3*i+2] = 255;

    }
}
// flag we have a new frame
m_newDepthFrame = true;
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::grabVideo(
                                                void *_video,
                                                uint32_t _timestamp
                                             )
{
    // lock our mutex and copy the data from the video buffer
    QMutexLocker locker( &m_mutex );
    uint8_t* rgb = static_cast<uint8_t*>(_video);
    //std::copy(rgb, rgb+m_resolutionRGBBytes, m_bufferVideo.begin());
    memcpy(  m_bufferVideo.data , rgb, m_resolutionRGBBytes);

    m_newRgbFrame = true;
}

//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::startVideo()
{
    if(freenect_start_video(m_dev) < 0)
    {
        throw std::runtime_error("Cannot start RGB callback");
    }
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::stopVideo()
{
    if(freenect_stop_video(m_dev) < 0)
    {
        throw std::runtime_error("Cannot stop RGB callback");
    }
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::startDepth()
{
    if(freenect_start_depth(m_dev) < 0)
    {
        throw std::runtime_error("Cannot start depth callback");
    }
}

//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::stopDepth()
{
    if(freenect_stop_depth(m_dev) < 0)
    {
        throw std::runtime_error("Cannot stop depth callback");
    }
}

//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::toggleVideoState(bool _mode)
{
    if(_mode ==true)
    {
        startVideo();
    }
    else
    {
        stopVideo();
    }
}
//----------------------------------------------------------------------------------------------------------------------
void KinectInterface::toggleDepthState(bool _mode)
{
    if(_mode == true)
    {
        startDepth();
    }
    else
    {
        stopDepth();
    }
}

void KinectInterface::setThreshold(int _value)
{
    m_threshValue = _value;
}

int KinectInterface::getThreshold()
{

   return m_threshValue;
}

float KinectInterface::getZValue(int _depthIndex,int _frameIndex)
{
    //QMutexLocker locker( &m_mutex );
    //return m_originalFrameDepth[_frameIndex].data[_depthIndex];

}

void KinectInterface::setDepthVolumeLower(double _inputLowerDepth)
{
    m_depthLower = _inputLowerDepth;
}

void KinectInterface::setDepthVolumeHigher(double _inputHigherDepth)
{
    m_depthHigher = _inputHigherDepth;
}

void KinectInterface::setBoxObject(int _topX, int _topY, int _width, int _height)
{
    m_setBounds = true;
    m_selectedBoxCoords.x = _topX;
    m_selectedBoxCoords.y = _topY;
    m_selectedBoxCoords.width = _width;
    m_selectedBoxCoords.height = _height;

}

cv::Rect& KinectInterface::getBoxObject()
{
    return m_selectedBoxCoords;
}

void KinectInterface::toggleTracking()
{
    m_toggleTracking = !(m_toggleTracking);
}

bool KinectInterface::getTrackingFlag()
{
    return m_toggleTracking;
}

bool KinectInterface::getBounds()
{
    return m_setBounds;
}

void KinectInterface::setBoundsDefined(bool _isBoundsDefined)
{
    m_setBounds = _isBoundsDefined;
}


//----------------------------------------------------------------------------------------------------------------------
