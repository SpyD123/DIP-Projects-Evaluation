#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;


Mat getMedianDarkChannel(Mat src, int patch)
{
        //CV_8UC1 means MAT having unsigned 8 bit integers with one channel
        Mat rgbmin = Mat::zeros(src.rows, src.cols, CV_8UC1);
        //Final MAT
	Mat MDCP;
        //Vec3b is used to store the color for a pixel
        Vec3b intensity;

	for(int m=0; m<src.rows; m++)
	{
		for(int n=0; n<src.cols; n++)
		{
			//Pixel intensity of current pixel at (m,n) index
			intensity = src.at<Vec3b>(m,n);
			//Creating rgbmin using minimum of all three values (R,G,B) of a specific pixel
			rgbmin.at<uchar>(m,n) = min(min(intensity.val[0], intensity.val[1]), intensity.val[2]);
		}
	}
	//Median Filtering using a Window Size= patch X patch
	medianBlur(rgbmin, MDCP, patch);
        namedWindow("Dark Channel Matrix", CV_WINDOW_AUTOSIZE);
        imshow("Dark Channel Matrix", MDCP);
	return MDCP;
}



int estimateA(Mat DC)
{
	double minDC, maxDC;
	//Using minMaxLoc to find the min and max DC values
	minMaxLoc(DC, &minDC, &maxDC);
	cout<<"Estimated Airlight is:"<<maxDC<<endl;
	return maxDC;
}


Mat estimateTransmission(Mat DCP, int ac)
{
	double w = 0.75;
	//transmission MAT of same size as DCP MAT with type CV_8UC1
	Mat transmission = Mat::zeros(DCP.rows, DCP.cols, CV_8UC1);
	Scalar intensity;

	for (int m=0; m<DCP.rows; m++)
	{
		for (int n=0; n<DCP.cols; n++)
		{
			//intensity of DCP at point (m,n) is the value of DCP MAT at that point 
			intensity = DCP.at<uchar>(m,n);
			//calculating transmission MAT based on intensity value with weight = 0.75
			//transmission MAT is weighted negative of DCP MAT
			transmission.at<uchar>(m,n) = (1 - w * intensity.val[0] / ac) * 255;
		}
	}
        namedWindow("Transmission Matrix", CV_WINDOW_AUTOSIZE);
        imshow("Transmission Matrix", transmission);
	return transmission;
}


Mat getDehazed(Mat source, Mat t, int al)
{
	double tmin = 0.1;
	double tmax;
	
	Scalar inttran;
	Vec3b intsrc;
	//dehazed is a MAT of size same as source image but with 8 bit unsigned vales having 3 channels
	Mat dehazed = Mat::zeros(source.rows, source.cols, CV_8UC3);
        Mat thresholded_transmission = Mat::zeros(source.rows, source.cols, CV_8UC1);
	for(int i=0; i<source.rows; i++)
	{
		for(int j=0; j<source.cols; j++)
		{
			//taking value of transmission MAT at (i,j)
			inttran = t.at<uchar>(i,j);
			//taking value of input image at point (i,j)
			intsrc = source.at<Vec3b>(i,j);
			//finding tmax using high pass filtering with value tmin = 0.1
			tmax = (inttran.val[0]/255) < tmin ? tmin : (inttran.val[0]/255);
                        thresholded_transmission.at<uchar>(i,j) = tmax*255;
			for(int k=0; k<3; k++)
			{
				//changing the intensities of input image to make dehaze MAT
				dehazed.at<Vec3b>(i,j)[k] = abs((intsrc.val[k] - al) / tmax + al) > 255 ? 255 : abs((intsrc.val[k] - al) / tmax + al);
			}
		}
	}
        namedWindow("Thresholded Transmission Matrix", CV_WINDOW_AUTOSIZE);
        imshow("Thresholded Transmission Matrix", thresholded_transmission);
	return dehazed;
}




int main(int argc, char** argv)
{
	cout<<"Enter File Name : ";
	char name[10000];
	cin>>name;
	Mat fog = imread(name);
	Mat darkChannel;
	Mat T;
	Mat fogfree;
	Mat beforeafter = Mat::zeros(fog.rows, 2 * fog.cols, CV_8UC3);
	//Deviding the beforeafter into two parts roil(Left Part) and roir(Right Part) 
	Rect roil (0, 0, fog.cols, fog.rows);	
	Rect roir (fog.cols, 0, fog.cols, fog.rows);
	namedWindow("before and after", CV_WINDOW_AUTOSIZE);
	int Airlight;
	//Calculating the DC MAT
	darkChannel = getMedianDarkChannel(fog, 5);
	//Calculating Airlight
	Airlight = estimateA(darkChannel);
	//Getting approximate distribution of haze in the input image
	T = estimateTransmission(darkChannel, Airlight);
	//finding Dehazed MAT
	fogfree = getDehazed(fog, T, Airlight);

	fog.copyTo(beforeafter(roil));
	fogfree.copyTo(beforeafter(roir));
	imwrite("./dehazed.jpg", fogfree);
	imshow("before and after", beforeafter);
	waitKey();

	return 0;
}
