#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void getContours(Mat fDil, Mat frame);

int main() {

	VideoCapture cap("Resources/test.mp4");
	Mat frame, fGray, fBlur, fCanny, fDil;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));

	while (true) {

		cap >> frame;
		cvtColor(frame, fGray, COLOR_BGR2GRAY);
		GaussianBlur(fGray, fBlur, Size(3, 3), 3, 0);
		Canny(fBlur, fCanny, 25, 75);
		dilate(fCanny, fDil, kernel);

		if (frame.empty() || fGray.empty()) {

			system("cls");
			cerr << "Empty matrix" << endl;
			break;
		}

		getContours(fDil, frame);

		imshow("Frame", frame);
		//imshow("Frame Gray", fGray);
		//imshow("Frame Blur", fBlur);
		//imshow("Frame Canny", fCanny);
		//imshow("Frame Dilate", fDil);

		if (waitKey(1) == 'q') {

			system("cls");
			cout << "Goodbye" << endl;
			break;
		}
	}

	return 0;
}

void getContours(Mat fDil, Mat frame) {

	vector<vector<Point>> contours;
	vector<Vec4i> hairarchy;
	findContours(fDil, contours, hairarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(frame, contours, -1, Scalar(255, 0, 255), 3);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	string objectType;

	// Bounding box
	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);
		//system("cls");
		cout << area << endl;

		if (area > 100000) {

			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			//cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			rectangle(frame, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 3);
		}
	}

	// Adding name to our shapes
	for (int i = 0; i < contours.size(); i++) {

		int area = contourArea(contours[i]);
		//system("cls");
		//cout << area << endl;

		if (area > 100000) {

			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			boundRect[i] = boundingRect(conPoly[i]);

			int objCorn = (int)conPoly[i].size();

			if (objCorn == 3) { objectType = "Tri"; }
			if (objCorn == 4) {
				
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				//cout << aspRatio << endl;
				if (aspRatio > 0.95 && aspRatio < 1.05) { objectType = "Square"; }
				else { objectType = "Rect"; }
			}
			if (objCorn > 4) { objectType = "Circle"; }

			putText(frame, objectType, { boundRect[i].x, boundRect[i].y - 5 },
				FONT_HERSHEY_PLAIN, 1.4, Scalar(0, 69, 255), 2);
		}
	}
}