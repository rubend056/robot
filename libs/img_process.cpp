#include "img_process.h"
#include "nnetwork.h"

using namespace ip;
using namespace cv;
using namespace std;

const int lowerS = 50, upperS = 255;
const int lowerV = 40, upperV = 255;

Mat ip::extractColor (Mat hsvMat, Color col){
    return ip::extractColor(hsvMat, col, lowerS, lowerV);
}

Mat ip::extractColor (Mat hsvMat, Color col, int minV, int minS){
	Mat mask;
	if (hsvMat.empty()){cout << "HSV is empty, can't go on" << endl; return mask;}
	
	int hue = col.hue / 2; 		// To make max 180
	int range = col.range / 2; 	// To make max 180
	
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
		
		Scalar llower (llrange,minS,minV);
		Scalar lupper (lurange,upperS,upperV);
		
		Scalar ulower (ulrange,minS,minV);
		Scalar uupper (uurange,upperS,upperV);
		
		Mat lmask; inRange(hsvMat, llower, lupper, lmask);
		Mat umask; inRange(hsvMat, ulower, uupper, umask);
		
		mask = lmask | umask;
	}else{
		Scalar lower (hue - range,minS,minV);
		Scalar upper (hue + range,upperS,upperV);
		
		inRange(hsvMat, lower, upper, mask); // Extract the color
	}
	return mask;
}



// ******************* CUBES AND BALLS ******************************
static double angle( Point pt1, Point pt2, Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

int thresh = 50, N = 11;
 void ip::find_cubes(const Mat& image, vector<vector<Point>>& squares){
	squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    gray0=timg;

    // try several threshold levels
    for( int l = 0; l < N; l++ )
    {
        // hack: use Canny instead of zero threshold level.
        // Canny helps to catch squares with gradient shading
        if( l == 0 )
        {
            // apply Canny. Take the upper threshold from slider
            // and set the lower to 0 (which forces edges merging)
            Canny(gray0, gray, 0, thresh, 5);
            // dilate canny output to remove potential
            // holes between edge segments
            dilate(gray, gray, Mat(), Point(-1,-1));
        }
        else
        {
            // apply threshold if l!=0:
            //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
            gray = gray0 >= (l+1)*255/N;
        }

        // find contours and store them all as a list
        findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

        vector<Point> approx;

        // test each contour
        for( size_t i = 0; i < contours.size(); i++ )
        {
            // approximate contour with accuracy proportional
            // to the contour perimeter
            approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.02, true);

            // square contours should have 4 vertices after approximation
            // relatively large area (to filter out noisy contours)
            // and be convex.
            // Note: absolute value of an area is used because
            // area may be positive or negative - in accordance with the
            // contour orientation
            if( approx.size() == 4 &&
                fabs(contourArea(approx)) > 1000 &&
                isContourConvex(approx) )
            {
                double maxCosine = 0;

                for( int j = 2; j < 5; j++ )
                {
                    // find the maximum cosine of the angle between joint edges
                    double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                    maxCosine = MAX(maxCosine, cosine);
                }

                // if cosines of all angles are small
                // (all angles are ~90 degree) then write quandrange
                // vertices to resultant sequence
                if( maxCosine < 0.3 )
                    squares.push_back(approx);
            }
        }
    }
    
}
 void ip::draw_cubes(Mat& image, const vector<vector<Point>>& squares, Scalar col){
	for( size_t i = 0; i < squares.size(); i++ )
    {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        polylines(image, &p, &n, 1, true, col, 3, LINE_AA);
    }
}

const int smooth_num = 3;
int smooth_count = 0;
bool smooth_finished = false;
Vec3f ip::circle_v_o;
vector<vector<Vec3f>> circle_v(smooth_num);
 void ip::find_balls(const Mat& image, double min_dist, double param1, double param2, int minRadius, int maxRadius){
	smooth_count++;if(smooth_count==smooth_num)smooth_count=0; // Up the counter
	
	// auto circles = ;
	cv::HoughCircles(image, circle_v[smooth_count], HOUGH_GRADIENT, 2, min_dist, param1, param2, minRadius, maxRadius);
    
    circle_v_o = circle_v[smooth_count][0];
}
 void ip::draw_balls(Mat& image, Scalar col){
	// auto circles = circle_v[smooth_count];
	for( size_t i = 0; i < circle_v[smooth_count].size(); i++ ) // Print out the circles
	{
		Point center(cvRound(circle_v[smooth_count][i][0]), cvRound(circle_v[smooth_count][i][1]));
		int radius = cvRound(circle_v[smooth_count][i][2]);
		// circle center
		circle(image, center, 3, col, -1, 8, 0 );
		// circle outline
		circle(image, center, radius, col, 3, 8, 0 );
	}
    circle_v[smooth_count].clear();
}


// vector<Vec3f> circle_v;
//  void ip::find_balls(const Mat& image, double min_dist, double param1, double param2, int minRadius, int maxRadius){
// 	// smooth_count++;if(smooth_count==smooth_num)smooth_count=0; // Up the counter
	
// 	// auto circles = ;
// 	cv::HoughCircles(image, circle_v, HOUGH_GRADIENT, 2, min_dist, param1, param2, minRadius, maxRadius);
//     // cout << circle_v[smooth_count].size() << endl;
// }
//  void ip::draw_balls(Mat& image, Scalar col){
// 	// auto circles = circle_v[smooth_count];
// 	for( size_t i = 0; i < circle_v.size(); i++ ) // Print out the circles
// 	{
// 		Point center(cvRound(circle_v[i][0]), cvRound(circle_v[i][1]));
// 		int radius = cvRound(circle_v[i][2]);
// 		// circle center
// 		circle(image, center, 3, col, -1, 8, 0 );
// 		// circle outline
// 		circle(image, center, radius, col, 3, 8, 0 );
// 	}
//     circle_v.clear();
// }


// *************************************************************************

// float hueFunction(float hue, Color color){
	
// }
// Mat relativeColor(Mat hsvMat, Color col){
// 	std::vector<cv::Mat> hsv_split;cv::split(hsvMat, hsv_split); // Split the hsv image
// 	Mat outMat(Size(hsvMat.cols, hsvMat.rows), CV_8UC1, Scalar(0,0,0)); // Create the output image
	
// 	for(int i=0; i<hsvMat.rows; i++){
// 	    for(int j=0; j<hsvMat.cols; j++){
// 	        auto hue_val = ((float)hsv_split[0].at<uchar>(i,j)) / 255; // Normalized hue value
// 	        auto val_val = ((float)hsv_split[2].at<uchar>(i,j)) / 255; // Normalized value
	        
// 	    }
// 	}
	
// }

Mat ip::processMat(Mat hsv, Color col, int minV, int minS){
	
	Mat defmask = extractColor(hsv, col, minV, minS);
	
	Mat valueMat;
	extractChannel(hsv, valueMat, 2);
	Mat outputMat;
	valueMat.copyTo(outputMat, defmask);
	return outputMat;
}

// Mat ip::getTrainMat(Mat mat){
// 	return processMat(mat, colors[2]);
// }


//This function will return a mat to help users visualize the training, a simple graph.
//This will call drawing functions on opencv

// vector<float> mse_list;
// float mse_highest = 0;
// Mat ip::showTraining(Mat mat, float mse){
// 	if (mse > mse_highest)mse_highest = mse; //Update highest
// 	mse_list.push_back(mse); // Add to MSE list
	
// 	auto it = mse_list.begin();
// 	int mat_w = mat.cols;
// 	int mat_h = mat.rows;
// 	float mse_size = 1.f / (mse_list.size()-1);
	
	
// 	auto p_a = cv::Point2i(0, mat_h - (*it / mse_highest * mat_h));
// 	it++;
// 	if (mse_list.size() <= 2){
// 		Point2i p_b;
// 		if (mse_list.size() == 1)
// 			p_b = cv::Point2i(mat_w, p_a.y);
// 		else{
// 			p_b = cv::Point2i(mat_w, mat_h - (*it / mse_highest * mat_h));
// 		}
// 		cv::line(mat, p_a, p_b, cv::Scalar(255,255,255));
// 		return mat;
// 	}
	
// 	int count = 1;
// 	auto p_b = cv::Point2i(mse_size * count * mat_w, mat_h - (*it / mse_highest * mat_h));
// 	cv::line(mat, p_a, p_b, cv::Scalar(255,255,255));
// 	it++;
// 	for (; it!= mse_list.end(); it++){
// 		count ++;
// 		p_a = cv::Point2i(p_b);
// 		p_b = cv::Point2i(mse_size * count * mat_w, mat_h - (*it / mse_highest * mat_h));
// 		cv::line(mat, p_a, p_b, cv::Scalar(255,255,255));
// 	}
	
// 	return mat;
// }




// vector<Object> ip::process(){ //Use the trained NN to clashify objects
// 	Mat hsv = getHSV();
// 	Mat rgb = getRGB();
// 	//All the calculations done here
	
// 	vector<Mat> colorMasks(colors.size());
// 	for(int i = 0; i<colors.size(); i++){
// 		colorMasks[i] = extractColor(hsv, colors[i]);
// 	}
	
// 	Mat masked;
// 	rgb.copyTo(masked, colorMasks[2]);
// 	setFinal(masked);
	
// 	//This is a placeholder
// 	vector<Object> toReturn(1);
// 	Object test;
// 	toReturn[0] = test;
// 	return toReturn;
// }



