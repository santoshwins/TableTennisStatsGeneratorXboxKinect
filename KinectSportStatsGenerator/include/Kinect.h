#ifndef __KINECT_H__
#define __KINECT_H__

#include "libfreenect.hpp"
#include <vector>
#include "Mutex.h"
#include <QObject>
/* thanks to Yoda---- from IRC */
class Kinect : public Freenect::FreenectDevice
{
public:
	Kinect(freenect_context *_ctx, int _index);
	~Kinect(){;}
		// Do not call directly even in child
	void VideoCallback(void* _rgb, uint32_t timestamp);

	void DepthCallback(void* _depth, uint32_t timestamp) ;
	bool getRGB(std::vector<uint8_t> &buffer);

	bool getDepth(std::vector<uint8_t> &buffer);
public :
	void setAngle(double _angle);
	void setRedLed();
	void setGreenLed();
	void setYellowLed();
	void setRedLedFlash();
	void setGreenLedFlash();
	void setYellowLedFlash();
	void setVideoMode(int _mode);
	void resetAngle();
private:
	std::vector<uint8_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_video;
	std::vector<uint16_t> m_gamma;
	Mutex m_rgb_mutex;
	Mutex m_depth_mutex;
	bool m_new_rgb_frame;
	bool m_new_depth_frame;

};
#endif // KINECT_H
