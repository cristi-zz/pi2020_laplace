// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


void printGaussianPyr(Mat_<Vec3b> gaussianPyr, std::string text) {
	imshow(text, gaussianPyr);
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
void testGaussianPyr(int noOfLayers){
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3b>> gaussianPyr = generateGaussianPyr(src, noOfLayers);
		for (int i = 0; i < gaussianPyr.size(); i++) {
			std::string x = "gaussian pyr #";
			x += std::to_string(i);
			printGaussianPyr(gaussianPyr[i], x);
		}
		
		waitKey();
	}
}

void printLaplacianImage128(Mat_<Vec3i> laplaceImg, std::string text) {
	Mat_<Vec3b> ret(laplaceImg.rows, laplaceImg.cols);
	for (int i = 0; i < laplaceImg.rows; ++i) {
		for (int j = 0; j < laplaceImg.cols; ++j) {
			for (int k = 0; k < 3; ++k) {
				int val = ((int)laplaceImg[i][j][k] + 128);
				if (val > 255) {
					val = 255;
				}
				else if (val < 0) {
					val = 0;
				}
				ret[i][j][k] = (uchar)val;
			}
		}
	}
	imshow(text, ret);
}
Mat_<Vec3i> ucharToInt(Mat_<Vec3b> mat) {
	Mat_<Vec3i> ret(mat.rows, mat.cols);
	for (int i = 0; i < ret.rows; ++i) {
		for (int j = 0; j < ret.cols; ++j) {
			for (int k = 0; k < 3; ++k) {
				ret[i][j][k] = (int)mat[i][j][k];
			}
		}
	}
	return ret;
}
Mat_<Vec3b> intToUchar(Mat_<Vec3i> mat) {
	Mat_<Vec3b> ret(mat.rows, mat.cols);
	for (int i = 0; i < ret.rows; ++i) {
		for (int j = 0; j < ret.cols; ++j) {
			for (int k = 0; k < 3; ++k) {
				int val = ((int)mat[i][j][k]);
				if (val > 255) {
					val = 255;
				}
				else if (val < 0) {
					val = 0;
				}
				ret[i][j][k] = val;
			}
		}
	}
	return ret;
}
std::vector<Mat_<Vec3i>> generateLaplacianPyr(Mat_<Vec3b> inputImage, int layers) {
	std::vector<Mat_<Vec3i> > ret;
	std::vector<Mat_<Vec3b> > gaussianPyr = generateGaussianPyr(inputImage, layers);
	ret.push_back(ucharToInt(gaussianPyr.back()));

	for (int i = gaussianPyr.size() - 1; i >= 1; --i) {
		Mat_<Vec3b> upLayer;
		pyrUp(gaussianPyr[i], upLayer, Size(gaussianPyr[i - 1].cols, gaussianPyr[i - 1].rows));
		ret.push_back(ucharToInt(gaussianPyr[i - 1]) - ucharToInt(upLayer));
	}
	return ret;
}
void testLaplacianPyr(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src;
		src = imread(fname, CV_LOAD_IMAGE_COLOR);

		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);

		imshow("lapace pyr #0", intToUchar(laplacianPyr[0]));
		for (int i = 1; i < laplacianPyr.size(); ++i) {
			std::string x = "laplacian pyr #";
			x += std::to_string(i);
			printLaplacianImage128(laplacianPyr[i], x);
		}

		imshow("image", src);
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
		std::vector<Mat_<Vec3i> > laplacianPyr = generateLaplacianPyr(src, layers);

		imshow("lapace pyr #0", intToUchar(laplacianPyr[0]));
		for (int i = 1; i < laplacianPyr.size(); ++i) {
			std::string x = "laplacian pyr #";
			x += std::to_string(i);
			printLaplacianImage128(laplacianPyr[i], x);
		}

		imshow("image", src);
		waitKey();
	}
}

Mat_<Vec3b> reconstructImage(Mat_<Vec3b> img, int layers) {
	std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(img, layers);

	Mat_<Vec3b> currentImg = intToUchar(laplacianPyr[0]);
	for (int i = 1; i < laplacianPyr.size(); ++i) {
		Mat_<Vec3b> upLayer;
		pyrUp(currentImg, upLayer, Size(laplacianPyr[i].cols, laplacianPyr[i].rows));
		currentImg = intToUchar(laplacianPyr[i]) + upLayer;
	}
	return currentImg;
}
void testReconstruction(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src;
		src = imread(fname, CV_LOAD_IMAGE_COLOR);

		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);
		Mat_<Vec3b> rec = reconstructImage(src, layers);
		imshow("reconstructed", rec);
		imshow("image", src);
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
		printf(" 4 - Reconstruct\n");
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
			case 4:
				scanf("%d", &n);
				testReconstruction(n);
				break;
		}
	} while (op != 0);
	return 0;
}