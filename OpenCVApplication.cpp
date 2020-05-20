// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height){
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}

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
Mat_<Vec3b> reconstructImage(std::vector<Mat_<Vec3i>> laplacianPyr) {
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
		Mat_<Vec3b> rec = reconstructImage(laplacianPyr);

		imshow("Diff", (rec - src) * 10 + 128);

		imshow("reconstructed", rec);
		imshow("image", src);
		waitKey(0);
	}
}
std::vector<int> RLE(std::vector<int> src)
{
	std::vector<int> dst;
	int cnt = 0;
	for (int i = 0; i < src.size()-1; i++)
	{
		cnt++;
		int current = src[i];
		int next = src[i + 1];
		//daca suntem la  penultima pozitie al vectorului
		if (i == src.size() - 2)
		{
			if (next == current)
			{
				cnt++;
				dst.push_back(cnt);
				dst.push_back(current);
			}
			else
			{
				dst.push_back(cnt);
				dst.push_back(current);
				dst.push_back(1);
				dst.push_back(next);
			}
			break;
		}
		//daca elementrul urmator diferit de curent, initializam cnt cu 0 si incarcam in vectorul rezultat valorile
		if (next != current)
		{

			dst.push_back(cnt);
			dst.push_back(current);
			cnt = 0;

		}

	}
	return dst;
}
void test_RLE()
{
	int vals[] = {0, 0, 0, 0, 25, 36, 25, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 1};
	//int vals2[] = { 1, 2, 4, 0, 25, 36, 25, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 1, 2, 3 };
	int n = sizeof(vals) / sizeof(vals[0]);
	std::vector<int> src;
	for (int i = 0; i < n; i++)
		src.push_back(vals[i]);

	std::vector<int> dst = RLE(src);

	std::cout << "Src array:" << std::endl;
	for (int i = 0; i < n; i++)
	{
		std::cout << src[i] << " ";
	}

	std::cout <<std::endl<< "Dst array:" << std::endl;
	for (int i = 0; i < dst.size(); i++)
	{
		std::cout << dst[i] << " ";
	}
	Sleep(15000);

//	waitKey();
}
Mat_<Vec3i> threshold(Mat_<Vec3i> laplacianPyr, int value) {
	Mat_<Vec3i> dst(laplacianPyr.rows, laplacianPyr.cols, CV_LOAD_IMAGE_UNCHANGED);

	for (int i = 0; i < laplacianPyr.rows; i++) {
		for (int j = 0; j < laplacianPyr.cols; j++) {
			for (int k = 0; k < 3; k++) {
				int pixel = std::abs(laplacianPyr(i, j)[k]);

			//	pixel = pixel - value;
				if (pixel < value) {
					dst(i, j)[k] = 0;
				}
				else
					dst(i, j)[k] = laplacianPyr(i, j)[k];
			}
		}
	}
	return dst;
}
std::vector<Mat_<Vec3i>> laplaceThreshold(Mat src, int layers, int value) {


	std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);
	std::vector<Mat_<Vec3i>> dif;

	for (int i = 1; i < laplacianPyr.size(); i++) 
		dif.push_back(threshold(laplacianPyr[i], value));
	
	return dif;

}
void testLaplacianTreshold(int layers, int value)
{
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);

		std::vector<Mat_<Vec3i>> dif=laplaceThreshold(src,layers,value);

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
				dst(i, j)[k] = findValues(values, img(i, j)[k]);
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

Mat_<Vec3b> toImage128(Mat_<Vec3i> laplaceImg) {
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
	return ret;
}
Mat_<Vec3b> quantSingle(Mat_<Vec3b> img, int show) {
	Mat_<Vec3b> ret(img.rows, img.cols);

	Mat_<Vec3b> img_prag(img.rows, img.cols);
	for (int culoare = 0; culoare < 3; ++culoare) {
		int DIM_HIST = 256;
		std::vector<int> hist(DIM_HIST);
		std::vector<float> FDP(DIM_HIST);

		int M = 0;
		for (int i = 0; i < img.rows; ++i) {
			for (int j = 0; j < img.cols; ++j) {
				hist[img(i, j)[culoare]] ++;
				M++;
			}
		}
		for (int i = 0; i < DIM_HIST; ++i) {
			FDP[i] = 1.0 * hist[i] / M;
		}
		int WH = 2;
		float TH = 0.00000003;
		std::vector<int> praguri;
		praguri.push_back(0);
		for (int k = 0; k < DIM_HIST; ++k) {
			/* Aici optim ar fii sa precalculam sumele partiale part[k] = suma
			tuturor valorilor hist[i], 0 >= i <= k (cu ajutorul recurentei
			part[k] = part[k - 1] + hist[k]), astfel incat sa calculam suma
			dintre left si right (din fereastra) in O(1). */
			int lo = max(0, k - WH);
			int hi = min(DIM_HIST - 1, k + WH);
			float sum = 0.0;
			float maxx_val = -1.0;
			for (int tt = lo; tt <= hi; ++tt) {
				sum += FDP[tt];
				maxx_val = max(maxx_val, FDP[tt]);
			}
			float medie = sum / (hi - lo + 1);
			if (FDP[k] > medie + TH && FDP[k] >= maxx_val) {
				praguri.push_back(k);
			}
		}
		praguri.push_back(255);
		for (int i = 0; i < img.rows; ++i) {
			for (int j = 0; j < img.cols; ++j) {
				int mn = 1000;
				/*In loc de cautare liniara putem folosi cautare binara pentru
				a reduce timpul de cautare al pragului, reducand complexitatea
				la O(logn)*/
				for (int k = 0; k < praguri.size(); ++k) {
					if (abs(praguri[k] - img(i, j)[culoare]) < mn) {
						mn = abs(praguri[k] - img(i, j)[culoare]);
						img_prag(i, j) = praguri[k];
						ret[i][j][culoare] = praguri[k];
					}
				}
			}
		}
		
		std::vector<int> hist2(256, 0);
		for (int i = 0; i < img.rows; ++i) {
			for (int j = 0; j < img.cols; ++j) {
				hist2[ret(i, j)[culoare]] ++;
			}
		}
		int* ptr_hist = &hist2[0];
		
		std::vector<int> histt(256);
		for (int i = 0; i < img.rows; ++i) {
			for (int j = 0; j < img.cols; ++j) {
				histt[img(i, j)[culoare]] ++;
			}
		}
		int* org_hist = &histt[0];
		if (show == 1) {
			std::string x = "hist quant ";
			x += std::to_string(culoare + 1);
			std::string y = "hist original ";
			y += std::to_string(culoare + 1);
			showHistogram(x, ptr_hist, img.cols, 300);
			showHistogram(y, org_hist, img.cols, 300);
		}
	}
	return ret;
}
void quantManualTreshold(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);
		std::vector<Mat_<Vec3b>> imgQuant;
		for (int i = 1; i < laplacianPyr.size(); i++) {
			imgQuant.push_back(quantSingle(toImage128(laplacianPyr[i]), (laplacianPyr.size() - 1 == i)));
		}
		for (int i = 0; i < imgQuant.size(); ++i) {
			std::string x = "quant pyr #";
			x += std::to_string(i + 1);
			imshow(x, imgQuant[i]);
			//printLaplacianImage128(imgQuant[i], x);
		}
		for (int i = 1; i < laplacianPyr.size(); ++i) {
			std::string x = "laplacian pyr #";
			x += std::to_string(i);
			printLaplacianImage128(laplacianPyr[i], x);
		}
		waitKey();
	}
}

void accumulatedHistogram(Mat_<Vec3b> laplacianLayer, int *hist) {
	for (int i = 0; i < laplacianLayer.rows; i++) {
		for (int j = 0; j < laplacianLayer.cols; j++) {
			for (int k = 0; k < 3; k++) {
				hist[laplacianLayer(i, j)[k]]++;
			}
		}
	}
}

int * quantHisto(int *hist, int rows, int cols) {
	Mat_<Vec3b> ret(rows, cols);
	int DIM_HIST = 256;
	int *histo_q = (int *)calloc(DIM_HIST, sizeof(int));
	for (int culoare = 0; culoare < 3; ++culoare) {
		std::vector<float> FDP(DIM_HIST);
		int M = rows * cols;
		for (int i = 0; i < DIM_HIST; ++i) {
			FDP[i] = 1.0 * hist[i] / M;
		}

		int WH = 2;
		float TH = 0.00000003;
		std::vector<int> praguri;
		for (int k = 0; k < DIM_HIST; ++k) {
			int lo = max(0, k - WH);
			int hi = min(DIM_HIST - 1, k + WH);
			float sum = 0.0;
			float maxx_val = -1.0;
			for (int tt = lo; tt <= hi; ++tt) {
				sum += FDP[tt];
				maxx_val = max(maxx_val, FDP[tt]);
			}
			float medie = sum / (hi - lo + 1);
			if (FDP[k] > medie + TH && FDP[k] >= maxx_val) {
				praguri.push_back(k);
			}
		}

		/*Nu lucram cu o imagine anume => am incercat sa ma folosesc de histograma ???*/
		for (int i = 0; i < DIM_HIST; i++) { 
			int value = findValues(praguri, i);
			histo_q[value] += hist[i];
		}
	}
	return histo_q;
}

Mat_<Vec3b> applyHisto(Mat_<Vec3b> laplacianLayer, int *histo_q) { //???
	Mat_<Vec3b> newLaplacianLayer(laplacianLayer.rows, laplacianLayer.cols);
	for (int i = 0; i < laplacianLayer.rows; i++) {
		for (int j = 0; j < laplacianLayer.cols; j++) {
			for (int k = 0; k < 3; k++) {
				if (histo_q[laplacianLayer(i, j)[k]] != 0) {
					newLaplacianLayer(i, j)[k] = laplacianLayer(i, j)[k];
				}
			}
		}
	}
	return newLaplacianLayer;
}

void testQuantAllLayers(int layers) {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname, CV_LOAD_IMAGE_COLOR);
		std::vector<Mat_<Vec3i>> laplacianPyr = generateLaplacianPyr(src, layers);

		int *hist = (int *)calloc(256, sizeof(int));
		for (int i = 0; i < laplacianPyr.size(); i++) {
			accumulatedHistogram(toImage128(laplacianPyr[i]), hist);
		}

		int *histo_q = quantHisto(hist, src.rows, src.cols); 

		std::vector<Mat_<Vec3b>> L_quant;
		std::vector<Mat_<Vec3i>> L_quant_int;
		for (int i = 0; i < laplacianPyr.size(); i++) {
			L_quant.push_back(applyHisto(toImage128(laplacianPyr[i]), histo_q));
		}
		for (int i = 0; i < laplacianPyr.size(); i++) {
			L_quant_int.push_back(ucharToInt(L_quant[i])); //prea alba
		}
		/*for (int i = 0; i < L_quant.size(); ++i) {
			std::string x = "quant pyr #";
			x += std::to_string(i + 1);
			imshow(x, L_quant[i]);
		}
		for (int i = 0; i < L_quant_int.size(); ++i) {
			std::string x = "quant pyr int #";
			x += std::to_string(i + 1);
			printLaplacianImage128(L_quant_int[i], x);
		}*/
		
		Mat_<Vec3b> rec = reconstructImage(L_quant_int);

		imshow("Diff", (rec - src) * 10 + 128);
		imshow("reconstructed", rec);
		imshow("image", src);
		showHistogram("hist", hist, src.cols, 300);
		showHistogram("histo_q", histo_q, src.cols, 300);

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
		printf(" 6 - Quantization without manual threshold\n");
		printf(" 7 - Quantization with manual threshold\n");
		printf(" 8 - RLE Demo\n");
		printf(" 9 - Quantization - all layers\n");
		printf(" 0 - \n\n");
		printf("Option: ");
		scanf("%d", &op);
		int n;
		int t;
		switch (op)
		{
		case 1:
			printf(" layers = ");
			scanf("%d", &n);
			testGaussianPyr(n);
			break;
		case 2:
			printf(" layers = ");
			scanf("%d", &n);
			testLaplacianPyr(n);
			break;
		case 3:
			printf(" layers = ");
			scanf("%d", &n);
			testBoth(n);
			break;
		case 4:
			printf(" layers = ");
			scanf("%d", &n);
			testReconstruction(n);
			break;
		case 5:
			printf(" layers = ");
			scanf("%d", &n);
			printf(" threshold = ");
			scanf("%d", &t);
			testLaplacianTreshold(n, t);
			break;
		case 6:
			printf(" layers = ");
			scanf("%d", &n);
			quantization(n);
			break;
		case 7:
			printf(" layers = ");
			scanf("%d", &n);
			quantManualTreshold(n);
			break;
		case 8:
			test_RLE();
			break;
		case 9:
			printf(" layers = ");
			scanf("%d", &n);
			testQuantAllLayers(n);
			break;
		}

	} while (op != 0);
	return 0;
}