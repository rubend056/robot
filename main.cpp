#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv)
{
	// if ( argc != 2 )
	// {
	// 	printf("usage: DisplayImage.out <Image_Path>\n");
    //     return -1;
    // }

    Mat image;
    image = imread("logo.png", 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
	
    namedWindow("Image", WINDOW_AUTOSIZE);
	Mat resized;
	resize(image, resized, Size(400,400));
    imshow("Image", resized);
	
	// resizeWindow("Image", Size(500,500));
	
    waitKey();
	
	return 0;
}
