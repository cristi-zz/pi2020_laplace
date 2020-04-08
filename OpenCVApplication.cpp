// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

std::vector<Mat_<Vec3b>> generateGaussianPyr(Mat_<Vec3b> img, int noOfLayers);
void testGaussianPyr(int noOfLayers);

std::vector<Mat_<Vec3b> > generateLaplacianPyr(Mat_<Vec3b> inputImage, int layers) {
	std::vector<Mat_<Vec3b> > ret;
	std::vector<Mat_<Vec3b> > gaussianPyr = generateGaussianPyr(inputImage, layers);

	for (int i = gaussianPyr.size() - 1; i >= 1; --i) {
		Mat_<Vec3b> laplacianLayer;
		pyrUp(gaussianPyr[i], laplacianLayer, Size(gaussianPyr[i - 1].cols, gaussianPyr[i - 1].rows)); 
		ret.push_back(gaussianPyr[i - 1] - laplacianLayer);
	}

	return ret;
}
void testLaplacianPyr(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src;
		src = imread(fname, CV_LOAD_IMAGE_COLOR);

		std::vector<Mat_<Vec3b> > laplacianPyr = generateLaplacianPyr(src, layers);

		for (int i = 0; i < laplacianPyr.size(); ++i) {
			std::string layerName = "laplace pyr #";
			layerName += std::to_string(i);
			imshow(layerName, laplacianPyr[i]);
		}

		imshow("image", src);
	}
}
std::vector<Mat_<Vec3b>> generateGaussianPyr(Mat_<Vec3b> img, int noOfLayers) {
	std::vector<Mat_<Vec3b>> gaussianPyr;
	Mat_<Vec3b> layer;
	gaussianPyr.push_back(img);
	for (int i = 0; i < noOfLayers; i++) {
		pyrDown(gaussianPyr[i], layer);
		gaussianPyr.push_back(layer);
	}
	return gaussianPyr;
}
void testGaussianPyr(int noOfLayers)
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3b>> gaussianPyr = generateGaussianPyr(src, noOfLayers);
		for (int i = 0; i < gaussianPyr.size(); i++) {
			std::string x = "gaussian pyr #";
			x += std::to_string(i);
			imshow(x, gaussianPyr[i]);
		}
		
		waitKey();
	}
}
void testBoth(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3b>> gaussianPyr = generateGaussianPyr(src, layers);
		for (int i = 0; i < gaussianPyr.size(); i++) {
			std::string x = "gaussian pyr #";
			x += std::to_string(i);
			imshow(x, gaussianPyr[i]);
		}
		std::vector<Mat_<Vec3b> > laplacianPyr = generateLaplacianPyr(src, layers);

		for (int i = 0; i < laplacianPyr.size(); ++i) {
			std::string layerName = "laplace pyr #";
			layerName += std::to_string(i);
			imshow(layerName, laplacianPyr[i]);
		}

		imshow("image", src);
		waitKey();
	}
}
int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Generate Gaussian Pyramid\n");
		printf(" 2 - Generate Laplacian Pyramid\n");
		printf(" 3 - Generate Both\n");
		printf(" 0 - \n\n");
		printf("Option: ");
		scanf("%d", &op);
		int n;
		switch (op)
		{
		case 1:
			
			scanf("%d", &n);
			testGaussianPyr(n);
			break;
		case 2:
			scanf("%d", &n);
			testLaplacianPyr(n);
			break;
		case 3:
			scanf("%d", &n);
			testBoth(n);
			break;
		}
	} while (op != 0);
	return 0;
}