// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


void testGaussianPyr() {
	return;
}
Mat_<Vec3b> difference(Mat_<Vec3b> a, Mat_<Vec3b> b) {
	Mat_<Vec3b> ret(a.rows, a.cols, 0);
	if (a.rows != b.rows || a.cols != b.cols) {
		std::cout << "\nDiferenta intre doua matrici de dimensiuni diferite\n";
		return ret;
	}
	for (int i = 0; i < a.rows; ++i) {
		for (int j = 0; j < a.cols; ++j) {
			ret[i][j][0] = a[i][j][0] - b[i][j][0];
			ret[i][j][1] = a[i][j][1] - b[i][j][1];
			ret[i][j][2] = a[i][j][2] - b[i][j][2];
		}
	}
	return ret;
}
std::vector<Mat_<Vec3b> > generateLaplacianPyr(Mat_<Vec3b> inputImage, int layers) {
	std::vector<Mat_<Vec3b> > ret;
	//std::vector<Mat_<Vec3b> > gaussianPyr = generateGaussianPyr(inputImage, layers);

	for (int i = 0; i < layers; ++i) {
		Mat_<Vec3b> test(255, 255, { 0, 0, 0 });

		ret.push_back(test);
	}

	return ret;
}
void testLaplacianPyr() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src;
		src = imread(fname, CV_LOAD_IMAGE_COLOR);

		int layers;
		std::cout << "\nDati numarul de layere\n";
		std::cin >> layers;


		std::vector<Mat_<Vec3b> > laplacianPyr = generateLaplacianPyr(src, layers);

		for (int i = 0; i < laplacianPyr.size(); ++i) {
			std::string layerName = "laplace pyr #";
			layerName += std::to_string(i);
			imshow(layerName, laplacianPyr[i]);
		}

		imshow("image", src);
		waitKey();
	}
	return;
}
int main() {
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Generate Gaussian Pyramid\n");
		printf(" 2 - Generate Laplacian Pyramid\n");
		printf(" 0 - \n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1:
			testGaussianPyr();
			break;
		case 2:
			testLaplacianPyr();
			break;
		}
	} while (op != 0);
	return 0;
}