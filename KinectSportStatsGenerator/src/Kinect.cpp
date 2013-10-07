#include "Kinect.h"
#include "Mutex.h"
#include <cmath>
#include <iostream>
/* thanks to Yoda---- from IRC */
Kinect::Kinect(freenect_context *_ctx, int _index) :FreenectDevice(_ctx, _index)
{

	m_buffer_depth.resize(FREENECT_VIDEO_RGB_SIZE);
	m_buffer_video.resize(FREENECT_VIDEO_RGB_SIZE);
	m_gamma.resize(2048);
	m_new_rgb_frame=false;
	m_new_depth_frame=false;

	for( unsigned int i = 0 ; i < 2048 ; i++)
	{
		float v = i/2048.0;
		v = std::pow(v, 3)* 6;
		m_gamma[i] = v*6*256;
	}

	//m_device = &freenect.createDevice(0);
	//m_device->startVideo();
	//m_device->startDepth();

}

// Do not call directly even in child
void Kinect::VideoCallback(void* _rgb, uint32_t timestamp)
{
//	std::cout << "RGB callback" << std::endl;
	m_rgb_mutex.lock();
	uint8_t* rgb = static_cast<uint8_t*>(_rgb);
	std::copy(rgb, rgb+FREENECT_VIDEO_RGB_SIZE, m_buffer_video.begin());
	m_new_rgb_frame = true;
	m_rgb_mutex.unlock();
};
// Do not call directly even in child
void Kinect::DepthCallback(void* _depth, uint32_t timestamp)
{
//	std::cout << "Depth callback" << std::endl;
	m_depth_mutex.lock();
	uint16_t* depth = static_cast<uint16_t*>(_depth);
	for( unsigned int i = 0 ; i < FREENECT_FRAME_PIX ; i++) {
		int pval = m_gamma[depth[i]];
		int lb = pval & 0xff;
		switch (pval>>8) {
		case 0:
			m_buffer_depth[3*i+0] = 255;
			m_buffer_depth[3*i+1] = 255-lb;
			m_buffer_depth[3*i+2] = 255-lb;
			break;
		case 1:
			m_buffer_depth[3*i+0] = 255;
			m_buffer_depth[3*i+1] = lb;
			m_buffer_depth[3*i+2] = 0;
			break;
		case 2:
			m_buffer_depth[3*i+0] = 255-lb;
			m_buffer_depth[3*i+1] = 255;
			m_buffer_depth[3*i+2] = 0;
			break;
		case 3:
			m_buffer_depth[3*i+0] = 0;
			m_buffer_depth[3*i+1] = 255;
			m_buffer_depth[3*i+2] = lb;
			break;
		case 4:
			m_buffer_depth[3*i+0] = 0;
			m_buffer_depth[3*i+1] = 255-lb;
			m_buffer_depth[3*i+2] = 255;
			break;
		case 5:
			m_buffer_depth[3*i+0] = 0;
			m_buffer_depth[3*i+1] = 0;
			m_buffer_depth[3*i+2] = 255-lb;
			break;
		default:
			m_buffer_depth[3*i+0] = 0;
			m_buffer_depth[3*i+1] = 0;
			m_buffer_depth[3*i+2] = 0;
			break;
		}
	}
	m_new_depth_frame = true;
	m_depth_mutex.unlock();
}

bool Kinect::getRGB(std::vector<uint8_t> &buffer)
{
	m_rgb_mutex.lock();
	if(m_new_rgb_frame) {
		buffer.swap(m_buffer_video);
		m_new_rgb_frame = false;
		m_rgb_mutex.unlock();
		return true;
	} else {
		m_rgb_mutex.unlock();
		return false;
	}
}

bool Kinect::getDepth(std::vector<uint8_t> &buffer)
{
	m_depth_mutex.lock();
	if(m_new_depth_frame) {
		buffer.swap(m_buffer_depth);
		m_new_depth_frame = false;
		m_depth_mutex.unlock();
		return true;
	} else {
		m_depth_mutex.unlock();
		return false;
	}
}

void Kinect::setAngle(double _angle)
{
	if(_angle > 30)
	{
		_angle = 30;
	}
	else if(_angle <-30)
	{
		_angle=-30;
	}
	this->setTiltDegrees(_angle);

}
void Kinect::setRedLed()
{
	this->setLed(LED_RED);
}
void Kinect::setGreenLed()
{
	this->setLed(LED_GREEN);
}
void Kinect::setYellowLed()
{
	this->setLed(LED_YELLOW);
}
void Kinect::setRedLedFlash()
{
	this->setLed(LED_BLINK_RED_YELLOW);
}
void Kinect::setGreenLedFlash()
{
this->setLed(LED_BLINK_GREEN);

}
void Kinect::setYellowLedFlash()
{
	this->setLed(LED_BLINK_YELLOW);

}
void Kinect::setVideoMode(int _mode)
{

}
void Kinect::resetAngle()
{
	this->setTiltDegrees(0);
}
