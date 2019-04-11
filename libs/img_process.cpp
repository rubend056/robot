#include "img_process.h"
#include "nnetwork.h"
#include "options.h"

using namespace ip;
using namespace cv;
using namespace std;

const int lowerS = 50, upperS = 255;
const int lowerV = 40, upperV = 255;

// Mat ip::extractColor (Mat hsvMat, Color col){
//     return ip::extractColor(hsvMat, col, lowerS, lowerV);
// }

Mat ip::extractColor (Mat hsvMat, Color col){
    int minV = col.minV;
    int minS = col.minS;

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

int thresh = 50, N = 1;
 void ip::find_cubes(const Mat& image, vector<vector<Point>>& squares){
	squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    
    // down-scale and upscale the image to filter out the noise
    // pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    // pyrUp(pyr, timg, image.size());
    timg = image.clone();
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
            Canny(gray0, gray, 0, thresh, 7);
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
            approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.04, true);

            // square contours should have 4 vertices after approximation
            // relatively large area (to filter out noisy contours)
            // and be convex.
            // Note: absolute value of an area is used because
            // area may be positive or negative - in accordance with the
            // contour orientation
            if( 
                approx.size() == 4 &&
                fabs(contourArea(approx)) > 200 &&
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
                if( maxCosine < 0.45 )
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

template <typename T>
class MovingAverage{
public:
    T *v;
    int size;
    MovingAverage(int _size):size(_size){v = static_cast<T*>(malloc(sizeof(T)*size));}
    // ~MovingAverage(){memset(v,0,)}
    void add(T d){
        memcpy( v, v+1, sizeof(T)*(size-1) ); // Move all values -1 index in the array, deleting the last one
        memcpy( v+size-1, &d, sizeof(T) ); // Copy new value to last element
    }
};


// const int smooth_num = 2;
// int smooth_count = 0;
// bool smooth_finished = false;
Vec3f* ip::circle_v_o;
int ip::color_num = 0;
#define SMOOTH_NUM 2
vector<Vec3f> ip::circle_s_oa[MAX_COLORS];
vector<Vec3f> circle_va[MAX_COLORS][SMOOTH_NUM];

void ip::find_balls(const Mat& image, double min_dist, double param1, double param2, int minRadius, int maxRadius){
    auto circle_v = circle_va[color_num];
    auto circle_s_o = circle_s_oa[color_num];
	// smooth_count++;if(smooth_count==smooth_num)smooth_count=0; // Up the counter
    for(int i=0;i<SMOOTH_NUM-1;i++)circle_v[i] = circle_v[i+1]; // Shifting the values

    circle_v[SMOOTH_NUM-1].clear();
	cv::HoughCircles(image, circle_v[SMOOTH_NUM-1], HOUGH_GRADIENT, 2, min_dist, param1, param2, minRadius, maxRadius);
    if (circle_v[SMOOTH_NUM-1].size() == 1 && circle_v[SMOOTH_NUM-1][0][2] == 0)circle_v[SMOOTH_NUM-1].clear(); // Clearing circle if zero size
    
    // Now we'll filter everything and output to circle_s_o
    static float x_filter = 5, y_filter = 5, r_filter = 5;
    circle_s_oa[color_num].clear();
    auto ca_b = circle_v[SMOOTH_NUM-2];
    auto ca_a = circle_v[SMOOTH_NUM-1];
    for(int i=0;i<ca_b.size();i++){
        auto c = ca_b[i];
        for(int e=0;e<ca_a.size();e++){
            auto d = ca_a[e];
            if (
                d[0] < c[0] + x_filter && d[0] > c[0] - x_filter &&
                d[1] < c[1] + y_filter && d[1] > c[1] - y_filter &&
                d[2] < c[2] + r_filter && d[2] > c[2] - r_filter
            ){
                Vec3f f;
                for(int j=0;j<3;j++)f[j] = (c[j] + d[j]) * .5;
                circle_s_oa[color_num].push_back(f);
            }
        }
    }
    
    if (circle_s_oa[color_num].size() > 0){
        circle_v_o = &(circle_s_oa[color_num][0]);
    }
    else circle_v_o = NULL;
}


void ip::draw_balls(Mat& image, Scalar col){
	auto circles = circle_va[color_num][SMOOTH_NUM-1];
	for( size_t i = 0; i < circles.size(); i++ ) // Print out the circles
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(image, center, 3, col, -1, 8, 0);
		// circle outline
		circle(image, center, radius, col, 3, 8, 0);
	}
    // circles.clear();
}



void ip::draw_all_balls(Mat& image){
    for( int e = 0; e < MAX_COLORS; e++ ){
        auto circles = circle_s_oa[e];
        if (circles.size() == 0)continue;

        auto v3 = circles[0]; //cout << v3[0] << " " << v3[1] << " " << v3[2] << endl;

        for( size_t i = 0; i < circles.size(); i++ ) // Print out the circles
        {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle(image, center, 3, colors_bgr[e], -1, 8, 0 );
            // circle outline
            circle(image, center, radius, colors_bgr[e], 3, 8, 0 );
        }
    }
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
// 	for( size_t i = 0; i < SMOOTH_NUM; i++ ) // Print out the circles
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

Mat ip::processMat(Mat hsv, Color col){
	
	Mat defmask = extractColor(hsv, col);
	
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



