#include "opencv/cv.hpp"
#include "opencv2/highgui.hpp"

#include <Windows.h>
#include <iostream>
#include <time.h>

#define WIN_NAME "ħ���� Ž��"
#define DIFF_THRESHOLD 0.1
#define FACE_CLASSIFIER_PATH "C:\\Users\\CIEL_PC_SIMUL1\\Downloads\\opencv\\build\\etc\\haarcascades\\haarcascade_frontalface_default.xml"
#define FACE_SEARCH_SCALE 1.1
#define MERGE_DETECTED_GROUP_CNTS 3
#define FACE_FRAME_WIDTH 50
#define FACE_FRAME_HEIGHT 50
#define FACE_FRAME_THICKNESS 1
#define INTRUDER_IMAGES_SAVE_PATH "C:\\Users\\CIEL_PC_SIMUL1\\Desktop\\intruder\\"

/**
*   ���� �ð������� ���ڿ��� �������� �޼ҵ�
*/
const std::string getCurrentTS2Str() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", &tstruct);

	return buf;
}

/**
*   ���� �޼ҵ�
*/
int main(int argc, char *argv[]) {
	// �� ķ ����
	cv::VideoCapture capture(0);


	// �� ķ�� �������� ���� ��� ���� ��� �� ����
	if (!capture.isOpened()) {
		std::cerr << "�� ķ ����̽��� ã�� �� �����ϴ�." << std::endl;
		return 0;
	}

	// ������ ����
	cv::namedWindow(WIN_NAME, 1);

	// ���ν� ���ø� ����
	cv::CascadeClassifier face_classifier;
	face_classifier.load(FACE_CLASSIFIER_PATH);

	std::cout << "'esc' Ű�� �̿��Ͽ� ���α׷� ���� " << std::endl;

	// ���� �׷��� ������ �������� �����ϴ� ����
	cv::Mat frameBeforeGrayScale;

	while (true) {
		bool isFrameValid = true;
		cv::Mat frameOriginalMat;
		cv::Mat frame;


		try {
			// �� ķ �������� ���� ũ�� ����
			capture >> frameOriginalMat;
			cv::resize(frameOriginalMat, frame, cv::Size(frameOriginalMat.cols / 2, frameOriginalMat.rows / 2), 0, 0, CV_INTER_NN);
		}
		catch (cv::Exception& e) {
			// ���� ���
			std::cerr << "������ ��ҿ� �����߱⿡, �ش� �������� �����մϴ�." << e.err << std::endl;
			isFrameValid = false;
		}

		// ���� �������� �׷��� �����Ϸ� ����
		cv::Mat frameCurrentGrayScale;
		cv::cvtColor(frame, frameCurrentGrayScale, CV_BGR2GRAY);

		if (frameCurrentGrayScale.size == frameBeforeGrayScale.size) {
			// ����ȭ�� ���� ���� �����Ӱ� ���� �������� ������ ������ ����

			double errorL2 = cv::norm(frameCurrentGrayScale, frameBeforeGrayScale, CV_L2);
			double diff = errorL2 / (double)(frameCurrentGrayScale.rows * frameBeforeGrayScale.rows);

			// �Ӱ�ġ ������ �������� ���̰� Ŭ ���
			if (diff >= DIFF_THRESHOLD) {
				// �󱼿����� ���� �� ���� ����
				std::vector<cv::Rect> faces;

				// ���ν� ���ø��� �̿��Ͽ� ���ν�
				face_classifier.detectMultiScale(
					frameCurrentGrayScale, faces,
					FACE_SEARCH_SCALE,
					MERGE_DETECTED_GROUP_CNTS,
					CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE,
					cv::Size(FACE_FRAME_WIDTH, FACE_FRAME_HEIGHT)
				);

				int facesSize = faces.size();

				// ħ������ ���� ������ ���
				if (facesSize != 0) {
					for (int i = 0; i < facesSize; i++) {
						// ħ������ �� �������� ��ü ������ ���� ����� �ʾ��� ��쿡��
						if (

							0 <= faces[i].x
							&& 0 <= faces[i].width
							&& faces[i].x + faces[i].width <= frame.cols
							&& 0 <= faces[i].y
							&& 0 <= faces[i].height
							&& faces[i].y + faces[i].height <= frame.rows
							) {
							// ħ������ �� �κи� �����ӿ��� �߶�
							cv::Mat faceFrame = frame(cv::Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));

							// ħ������ ���� ���� �� ���丮�� ���� ��� ����
							CreateDirectory(INTRUDER_IMAGES_SAVE_PATH, NULL);

							// ħ������ �� �������� �̹����� ����
							cv::imwrite(INTRUDER_IMAGES_SAVE_PATH + getCurrentTS2Str() + std::string("_") + std::to_string(i) + std::string(".jpg"), faceFrame);

							// ħ���� Ž�� �޼����� ȭ�鿡 ���

							cv::putText(
								frame,
								"Intruder Detected!",
								cv::Point(50, 230),
								0,
								1.0,
								cv::Scalar(0, 0, 255)
							);
						}
					}
				}

				// �������� �־����� ħ���� ���� �������� ���� ���
				else {
					// ������ ���� �޼����� ȭ�鿡 ���
					printf("wtf");
					cv::putText(
						frame,
						"Moving Object Detected!",
						cv::Point(20, 230),
						0,
						1.0,
						cv::Scalar(255, 0, 0)
					);
				}
			}
		}

		// �����쿡 ��� ���
		cv::imshow(WIN_NAME, frame);

		// ���� �������� ���� ������ ���� ������ �ű�
		if (frameBeforeGrayScale.empty())
			frameBeforeGrayScale = frameCurrentGrayScale;

		int keyCode = cv::waitKey(30);

		// esc Ű�� ������ ������ ĸ�� ����
		if (keyCode == 27) {
			break;
		}
	}

	return 0;
}
