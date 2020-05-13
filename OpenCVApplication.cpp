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
void testGaussianPyr(int noOfLayers) {
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

/*
	Primim o piramida laplaciana ca si parametru (si nu imaginea sursa)
*/
Mat_<Vec3b> reconstructImage(std::vector<Mat_<Vec3i>> laplacianPyr, int layers) {
	//std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(img, layers);

	Mat_<Vec3b> currentImg = intToUchar(laplacianPyr[0]);
	for (int i = 1; i < laplacianPyr.size(); ++i) {
		Mat_<Vec3b> upLayer;
		pyrUp(currentImg, upLayer, Size(laplacianPyr[i].cols, laplacianPyr[i].rows));
		currentImg = intToUchar(laplacianPyr[i] + ucharToInt(upLayer));
	}
	return currentImg;
}
void testReconstruction(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src;
		src = imread(fname, CV_LOAD_IMAGE_COLOR);

		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);
		Mat_<Vec3b> rec = reconstructImage(laplacianPyr, layers);

		imshow("Diff", (rec - src) * 10 + 128);

		imshow("reconstructed", rec);
		imshow("image", src);
		waitKey(0);
	}
}

Mat_<Vec3i> threshold(Mat_<Vec3i> laplacianPyr, int value) {
	Mat_<Vec3i> dst(laplacianPyr.rows, laplacianPyr.cols, CV_LOAD_IMAGE_UNCHANGED);

	for (int i = 0; i < laplacianPyr.rows; i++) {
		for (int j = 0; j < laplacianPyr.cols; j++) {
			for (int k = 0; k < 3; k++) {
				int pyr = std::abs(laplacianPyr(i, j)[k]);

				pyr = pyr - value;
				if (pyr < 0) {
					dst(i, j)[k] = 0;
				}
				else
					dst(i, j)[k] = laplacianPyr(i, j)[k];
			}
		}
	}
	return dst;
}

void laplaceThreshold(int layers, int value) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);

		std::vector<Mat_<Vec3i>> dif;
		for (int i = 1; i < laplacianPyr.size(); i++) {
			dif.push_back(threshold(laplacianPyr[i], value));
		}

		for (int i = 1; i < laplacianPyr.size(); ++i) {
			std::string x = "laplacian pyr #";
			x += std::to_string(i);
			printLaplacianImage128(laplacianPyr[i], x);
		}

		for (int i = 0; i < dif.size(); ++i) {
			std::string x = "threshold pyr #";
			x += std::to_string(i + 1);
			printLaplacianImage128(dif[i], x);
		}

		waitKey();
	}
}

int* histogram(Mat_<Vec3i> src) {
	int *hst = (int *)calloc(511, sizeof(int));
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			for (int k = 0; k < 3; k++) {
				hst[src(i, j)[k] + 255]++;
			}
		}
	}
	return hst;
}

float* fdp_hs(Mat_<Vec3i> src) {
	float *fdp = (float *)calloc(511, sizeof(float));
	int *hst = histogram(src);
	int M = src.cols * src.rows;
	for (int i = 0; i < 511; i++) {
		fdp[i] = 1.0f*hst[i] / M;
	}
	return fdp;
}

std::vector<int> getValues(Mat_<Vec3i> src) {

	float *fdp = fdp_hs(src);

	int wh = 5;
	float th = 0.0003;

	std::vector<int> threshold;

	for (int k = 0 + wh; k < 511 - wh; k++) {

		float suma = 0;
		bool max_local = true;

		for (int i = k - wh; i < k + wh; i++) {
			suma += fdp[i];
			if (fdp[k] < fdp[i])
				max_local = false;
		}

		float value = (suma / (2 * wh + 1)) + th;

		if ((fdp[k] > value) && (max_local == true)) {
			threshold.push_back(k - 255);
			//printf(" th = %d\n", threshold[threshold.size() - 1]);
		}
	}
	return threshold;
}

int findValues(std::vector<int> threshold, int val) {
	for (int i = 0; i < threshold.size() - 1; i++) {
		if (threshold[i] <= val && threshold[i + 1] >= val) {
			int min_val = val - threshold[i];
			int max_val = threshold[i + 1] - val;
			if (min_val <= max_val)
				return threshold[i];
			else return threshold[i + 1];
		}
	}

}

Mat_<Vec3i> quantImg(Mat_<Vec3i> img) {
	Mat_<Vec3i> dst(img.rows, img.cols, CV_LOAD_IMAGE_UNCHANGED);
	std::vector<int> values = getValues(img);

	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			for (int k = 0; k < 3; k++) {
				//printf("%d - ", img(i, j)[k]);
				dst(i, j)[k] = findValues(values, img(i, j)[k]);
				//printf(" %d\n", dst(i, j)[k]);
			}
		}
	}
	return dst;
}

void quantization(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);

		std::vector<Mat_<Vec3i>> imgQuant;

		for (int i = 1; i < laplacianPyr.size(); i++) {
			imgQuant.push_back(quantImg(laplacianPyr[i]));
			//printLaplacianImage128(imgQuant[i], "x");
		}

		for (int i = 0; i < imgQuant.size(); ++i) {
			std::string x = "quant pyr #";
			x += std::to_string(i + 1);
			printLaplacianImage128(imgQuant[i], x);
		}

		for (int i = 1; i < laplacianPyr.size(); ++i) {
			std::string x = "laplacian pyr #";
			x += std::to_string(i);
			printLaplacianImage128(laplacianPyr[i], x);
		}

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
		printf(" 4 - Reconstruct\n");
		printf(" 5 - Laplace threshhold\n");
		printf(" 6 - Quantization\n");
		printf(" 0 - \n\n");
		printf("Option: ");
		scanf("%d", &op);
		int n;
		int t;
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
		case 5:
			printf(" layers = ");
			scanf("%d", &n);
			printf(" threshold = ");
			scanf("%d", &t);
			laplaceThreshold(n, t);
			break;
		case 6:
			printf(" layers = ");
			scanf("%d", &n);
			quantization(n);
			break;
		}
	} while (op != 0);
	return 0;
}