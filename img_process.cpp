#include "img_process.h"
#include "nnetwork.h"

using namespace ip;
using namespace cv;

struct Color{
	public:
		int hue;	// From 0-360
		int range;	// From 0-360
		Color(int h, int r){
			hue = h;
			range = r;
		}
};

//Define all the colors
vector<Color> colors = {/*BLUE 178-260*/ Color(219, 41), /*GREEN 90-150*/ Color(120, 30), /*RED 0-20 340-360*/ Color(0, 20), /*YELLOW 30-90*/ Color(60, 30)};

Mat extractColor (Mat hsvMat, Color col){
	Mat mask; 
	int hue = col.hue / 2;
	int range = col.range / 2;
	
	if (hue < range || 180 - hue < range){ // This part basically wraps any value, in case it goes over or lower than 0-180
		int lrange = hue - range, urange = hue + range;
		int llrange = 0, lurange, ulrange, uurange = 180;
		if(lrange < 0){
			lurange = urange;
			ulrange = 180 + lrange; //Since lrange is negative, it works
		}else{
			lurange = 0 + (urange - 180);
			ulrange = lrange;
		}
		
		Scalar llower (llrange,50,100);
		Scalar lupper (lurange,255,255);
		
		Scalar ulower (ulrange,50,90);
		Scalar uupper (uurange,255,255);
		
		Mat lmask; inRange(hsvMat, llower, lupper, lmask);
		Mat umask; inRange(hsvMat, ulower, uupper, umask);
		
		mask = lmask | umask;
	}else{
		Scalar lower (hue - range,50,90);
		Scalar upper (hue + range,255,255);
		
		inRange(hsvMat, lower, upper, mask); // Extract the color
	}
	return mask;
}

vector<Object> ip::process(){ //Use the trained NN to clashify objects
	Mat hsv = getHSV();
	Mat rgb = getRGB();
	//All the calculations done here
	
	vector<Mat> colorMasks(colors.size());
	for(int i = 0; i<colors.size(); i++){
		colorMasks[i] = extractColor(hsv, colors[i]);
	}
	
	Mat masked;
	rgb.copyTo(masked, colorMasks[2]);
	setFinal(masked);
	
	
	//This is a placeholder
	vector<Object> toReturn(1);
	Object test;
	toReturn[0] = test;
	return toReturn;
}

Mat ip::outputProcessed(Mat mat, vector<Object> objects){
	
	
	
}


