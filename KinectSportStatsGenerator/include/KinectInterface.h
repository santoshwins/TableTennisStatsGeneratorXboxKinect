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
/// @file KinectInterface.h
/// @brief a Qt wrapper for the OpenKinect project a lot of the code has been modified from
/// this project source here https://github.com/OpenKinect/libfreenect
//----------------------------------------------------------------------------------------------------------------------

#ifndef QKINECT_H__
#define QKINECT_H__
#include <QObject>
#include <QThread>
#include <stdexcept>
#include <QMutex>
#include <QMutexLocker>
#include <vector>
#include <libfreenect.h>
#include "opencv2/opencv.hpp"

//----------------------------------------------------------------------------------------------------------------------
/// @class QKinectProcessEvents "KinectInterface.h"
/// @author Jonathan Macey
/// @version 1.0
/// @date 20/12/10 Inital commit
/// @brief this is the process event thread class
/// it needs to be a class so we can use the Qt thread system
//----------------------------------------------------------------------------------------------------------------------

class QKinectProcessEvents : public QThread
{
public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor where we pass in the context of the kinect
    /// @param [in] _ctx the context of the current kinect device
    //----------------------------------------------------------------------------------------------------------------------
    inline QKinectProcessEvents(
                                                            freenect_context *_ctx
                                                         )
                                                            {m_ctx=_ctx;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the thread active this will loop the run thread
    /// with a while(m_active) setting this will end the thread loop
    //----------------------------------------------------------------------------------------------------------------------
    inline void setActive(){m_active=true;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief sets the thread active must call QThread::start again to make this
    /// work if the thread has been de-activated
    //----------------------------------------------------------------------------------------------------------------------
    inline void setInActive(){m_active=false;}
protected :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the actual thread main loop, this is not callable and the
    /// QThread::start method of QThread must be called to activate the loop
    //----------------------------------------------------------------------------------------------------------------------
    void run();

private :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief a flag to indicate if the loop is to be active
    /// set true in the ctor
    //----------------------------------------------------------------------------------------------------------------------
    bool m_active;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the context of the kinect device, this must
    /// be set before the thread is run with QThread::start
    //----------------------------------------------------------------------------------------------------------------------
    freenect_context *m_ctx;








};

//----------------------------------------------------------------------------------------------------------------------
/// @class KinectInterface "KinectInterface.h"
/// @author Jonathan Macey
/// @version 1.0
/// @date 20/12/10 Inital commit
/// @brief this class wraps the libfreenect library as a QObject
/// this allows us to use singals and slots to communicate with the class
/// from other Qt GUI elements. This class uses the Singleton pattern so must
/// be accessed via the instance method
//----------------------------------------------------------------------------------------------------------------------

class KinectInterface : public QObject
{
// must include this macro so we inherit all the core QT features such as singnals and slots
Q_OBJECT
public :

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get the instance of the KinectInterface object this will call the
    /// init method of the class if the instance doesn't exist. This is because
    /// this class will construct some threads so we need to create an instance
    /// first then pass that to the other classes in the thread
    /// @returns an instance of the KinectInterface object
    //----------------------------------------------------------------------------------------------------------------------
    static KinectInterface *instance();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief we can set the user device number here by default it will be 1
    /// @param[in] _m the number we wish to set
    //----------------------------------------------------------------------------------------------------------------------
    inline void setUserDeviceNumber(int _m) {m_userDeviceNumber=_m;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to shutdown our device and close
    //----------------------------------------------------------------------------------------------------------------------
    void shutDownKinect();

    int getThreshold();

    // returns the depth value at the given index by looking up
    // the depth table
    float getZValue(int _depthIndex,int _frameIndex);

    // this sets the defined bounds into a cv rect
    void setBoxObject(int _topX,int _topY, int _width, int _height);

    // this sets the final bounds defined flag which is queried
    // by stats window to stop the timer for looking further
    void setBoundsDefined(bool _isBoundsDefined);

    // this returns the defined bounds from the RGB window in a Rect format
    cv::Rect& getBoxObject();

    // this toggles the tracking flag START/STOP
    void toggleTracking();

    // this tells if the user has pressed start tracking
    bool getTrackingFlag();

    // this tells if the user is refinining bounds
    // by right clicking again and again
    bool getBounds();


public slots :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to change the angle of the Kinect
    /// @param[in] _angle the angle to set will be constrained to -/+ 30.0
    //----------------------------------------------------------------------------------------------------------------------
    void setAngle(
                                double _angle
                             );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to turn off the led
    //----------------------------------------------------------------------------------------------------------------------
    void setLedOff();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to enable the red LED
    //----------------------------------------------------------------------------------------------------------------------
    void setRedLed();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to enable the green LED
    //----------------------------------------------------------------------------------------------------------------------
    void setGreenLed();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to enable the yellow LED
    //----------------------------------------------------------------------------------------------------------------------
    void setYellowLed();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to flash the red led
    //----------------------------------------------------------------------------------------------------------------------
    void setRedLedFlash();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to flash the green led
    //----------------------------------------------------------------------------------------------------------------------
    void setGreenLedFlash();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to flash the yellow led
    //----------------------------------------------------------------------------------------------------------------------
    void setYellowLedFlash();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief slot to set the video mode
    /// @brief param _mode the video mode as an int index
    /// FREENECT_VIDEO_RGB = 0,
    /// FREENECT_VIDEO_YUV_RGB = 1,
    /// FREENECT_VIDEO_IR_8BIT = 2,

    //----------------------------------------------------------------------------------------------------------------------
    void setVideoMode(
                                        int _mode
                                     );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief reset the tilt angle
    //----------------------------------------------------------------------------------------------------------------------
    void resetAngle();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief getDepth buffer and convert it to an RGB colour representation
    /// this code is based on the sample implementation glview.c / cppview.cpp
    /// @param[out] o_buffer the buffer to fill
    //----------------------------------------------------------------------------------------------------------------------
    bool getDepth(cv::Mat &o_buffer
                             );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief getDepth buffer as the raw values converted to uint8_t not sure if this will
    /// be much use but have used it in some tracking examples
    /// @param[out] o_buffer the buffer to fill
    //----------------------------------------------------------------------------------------------------------------------
    bool getDepthRaw(
                                     cv::Mat &o_buffer, float *o_depthinmBuffer
                                    );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief getDepth buffer as the raw 16 Bit values
    /// this is useful for generating point cloud renders as we have the full depth range
    /// @param[out] o_buffer the buffer to fill
    //----------------------------------------------------------------------------------------------------------------------
    bool getDepth16Bit(cv::Mat &o_buffer
                                        );


    bool getDepthSliced(cv::Mat &o_buffer
                                        );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get the RGB buffer
    /// @param [out] o_buffer the rgb values
    //----------------------------------------------------------------------------------------------------------------------
    bool getRGB(cv::Mat &o_buffer );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief start the Depth buffer grabbing subsystem
    //----------------------------------------------------------------------------------------------------------------------
    void startDepth();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief stop the Depth buffer grabbing subsystem
    //----------------------------------------------------------------------------------------------------------------------
    void stopDepth();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief start the Video buffer grabbing subsystem
    //----------------------------------------------------------------------------------------------------------------------
    void startVideo();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief stop the Depth buffer grabbing subsystem
    //----------------------------------------------------------------------------------------------------------------------
    void stopVideo();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief convenience method to toggle the video mode  with a bool to indicate
    /// mode, useful for connecting to buttons
    //----------------------------------------------------------------------------------------------------------------------
    void toggleVideoState(
                                                bool _mode
                                             );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief convenience method to toggle the depth mode  with a bool to indicate
    /// mode, useful for connecting to buttons
    //----------------------------------------------------------------------------------------------------------------------
    void toggleDepthState(
                                                bool _mode
                                             );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief return a pointer to the active device context
    /// @returns the current active contects
    //----------------------------------------------------------------------------------------------------------------------
    inline freenect_context *getContext(){return m_ctx;}

    void setThreshold(int _value);

    void setDepthVolumeLower(double _inputLowerDepth);


    void setDepthVolumeHigher(double _inputHigherDepth);




private :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief private ctor as we are a singleton class
    //----------------------------------------------------------------------------------------------------------------------
    KinectInterface();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief private dtor as we are a singleton class
    //----------------------------------------------------------------------------------------------------------------------
    ~KinectInterface();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief private copy ctor as we are a singleton class
    //----------------------------------------------------------------------------------------------------------------------
    Q_DISABLE_COPY(KinectInterface)
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief private init method, this makes the instance method thread safe
    /// as all the initialisations will be done here
    //----------------------------------------------------------------------------------------------------------------------
    bool init();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our instance of the Kinect device
    //----------------------------------------------------------------------------------------------------------------------
    static KinectInterface *s_instance;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the contex for the device
    //----------------------------------------------------------------------------------------------------------------------
    freenect_context *m_ctx;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our device pointer
    //----------------------------------------------------------------------------------------------------------------------
    freenect_device *m_dev;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief user device number
    //----------------------------------------------------------------------------------------------------------------------
    int m_userDeviceNumber;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our depth buffer for the current depth frames will be filled
    /// from the depth callback, this data is converted into an RGB representation
    /// with Red being closest and blue far away, this is done via a gamma table
    /// and converting the depth from 16 bit to 8 bit RGB useful for OpenCV like
    /// motion tracking
    //----------------------------------------------------------------------------------------------------------------------
    cv::Mat m_bufferDepth;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our video buffer for the current rgb video frames will be filled
    /// from the rgb callback
    //----------------------------------------------------------------------------------------------------------------------
    cv::Mat m_bufferVideo,m_nextBuffer,m_prevBuffer, m_diffBuffer, m_bufferVideoGray;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our depth buffer for the current depth frames will be filled
    /// from the depth callback this is the unaltered data converted to 8 bit
    /// not really useful so may get rid of at a later date just legacy from
    /// testing the other examples
    //----------------------------------------------------------------------------------------------------------------------
    cv::Mat m_bufferDepthRaw;

    float  m_bufferDepthinM[640 * 480];

    //std::map<int, float[640*480]> m_originalFrameDepth1;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the 16Bit raw depth values, this is good for voxel rendering of the
    /// data
    //----------------------------------------------------------------------------------------------------------------------
    cv::Mat m_bufferDepthRaw16;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our gamma table this is filled in the ctor and used in the depth
    /// callbacks, this is taken from the sample code glview.c cppview.cpp
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<float> m_gamma;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if there is a new rgb frame
    //----------------------------------------------------------------------------------------------------------------------
    bool m_newRgbFrame;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if there is a new depth frame
    //----------------------------------------------------------------------------------------------------------------------
    bool m_newDepthFrame;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if we need to stop the device
    //----------------------------------------------------------------------------------------------------------------------
    bool m_stopDevice;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if the device is active
    //----------------------------------------------------------------------------------------------------------------------
    bool m_deviceActive;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief size of the RGB buffer for the current mode
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_resolutionRGBBytes;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief size of the depth buffer for the current mode
  //----------------------------------------------------------------------------------------------------------------------
  unsigned int m_resolutionDepthBytes;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief pointer to our thread process used to process the events
    //----------------------------------------------------------------------------------------------------------------------
    QKinectProcessEvents *m_process;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief our mutex used for the threaded processes, for ease we use a
    /// QMutexLocker to automagically control our mutex state
    //----------------------------------------------------------------------------------------------------------------------
    QMutex m_mutex;


    int m_threshValue;

    double m_depthLower, m_depthHigher;

    cv::Rect m_selectedBoxCoords;

    bool m_toggleTracking,m_setBounds;


    //----------------------------------------------------------------------------------------------------------------------
    /// @brief function for depth callback this hooks into the libfreenect callback system
    /// @param[in] _dev the device we are querying
    /// @param[out] the actual depth data returned from the device
    /// @param _timestamp the time stamp for the grab (not used at present)
    //----------------------------------------------------------------------------------------------------------------------
    void depthFunc(
                                    freenect_device *_dev,
                                    void *o_depth,
                                    uint32_t _timestamp
                                );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief function for Video callback this hooks into the libfreenect callback system
    /// @param[in] _dev the device we are querying
    /// @param[out] o_rgb the actual video data returned from the device
    /// @param _timestamp the time stamp for the grab (not used at present)
    //----------------------------------------------------------------------------------------------------------------------
    void rgbFunc(
                                freenect_device *_dev,
                                void *o_rgb,
                                uint32_t _timestamp
                            );


    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to grab the depth passed to the depthCallback function
    /// as we are hooking into a C lib we need to do it this way as the callback
    /// functions must be static (see below)
    /// @param[out] _depth the depth data
    /// @param[in] _timestamp the timestamp of the operation (not used)
    //----------------------------------------------------------------------------------------------------------------------
    void grabDepth(
                                    void *_depth,
                                    uint32_t timestamp
                                );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief method to grab the Video passed to the depthCallback function
    /// as we are hooking into a C lib we need to do it this way as the callback
    /// functions must be static (see below)
    /// @param[out] _video the depth data
    /// @param[in] _timestamp the timestamp of the operation (not used)
    //----------------------------------------------------------------------------------------------------------------------
    void grabVideo(
                                    void *_video,
                                    uint32_t timestamp
                                );
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this hooks into the feenet callback system an is the main way of
    /// getting the data from the kinect this is not called directly
    /// @param[in] _dev this is not used as we use the class m_dev
    /// @param[out] _video the depth data
    /// @param[in] _timestamp the timestamp of the operation (not used)
    //----------------------------------------------------------------------------------------------------------------------
    static inline void depthCallback(
                                                                     freenect_device *_dev,
                                                                     void *_depth,
                                                                     uint32_t _timestamp=0
                                                                    )
    {
        Q_UNUSED(_dev);

        /// get an instance of our device
        KinectInterface *kinect=KinectInterface::instance();
        /// then call the grab method to fill the depth buffer and return it
        kinect->grabDepth(_depth,_timestamp);
    }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this hooks into the feenet callback system an is the main way of
    /// getting the data from the kinect this is not called directly
    /// @param[in] _dev this is not used as we use the class m_dev
    /// @param[out] _video the depth data
    /// @param[in] _timestamp the timestamp of the operation (not used)
    //----------------------------------------------------------------------------------------------------------------------

    static inline void videoCallback(
                                                                     freenect_device *_dev,
                                                                     void *_video,
                                                                     uint32_t _timestamp=0
                                                                    )
    {
        Q_UNUSED(_dev);

        /// get an instance of our device
        KinectInterface *kinect=KinectInterface::instance();
        /// then fill the video buffer
        kinect->grabVideo(_video, _timestamp);
    }




};



#endif
