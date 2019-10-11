#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <math.h>
#include<unistd.h>
#include<dirent.h>   //format of directory entries
#include<sys/stat.h>
#include<sys/types.h>
#include<stdlib.h>
#include <algorithm>
#include <vector>
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"
#include "slot_type.h"
#include "type_def.h"
#include "slot_detect_public.h"

using namespace cv;
using namespace std;
using namespace zros;
using namespace slot_detect;

class slotDetect{
	public:
		slotDetect(string pic_dir, string model_path);
		~slotDetect();	
		void slotdetectProcess();	
	private:
		vector<string> picVec;
		SlotDetection *pslotDet;
		void drawSlot(IplImage *src, std::list<SlotInfo> slot_list, 
		std::vector<uint32_t> sod_status, string file_name);
		void getpicPath(string &dir_name);
};

void slotDetect::getpicPath(string &dir_name)
{
	auto dir = opendir(dir_name.data());
	struct dirent *ptr;
	if (dir) {
		while ((ptr = readdir(dir)) != NULL) {
			if ( 0 == strcmp (ptr->d_name, "..") || 0 == strcmp (ptr->d_name, ".")) {
				continue;
			} else if (ptr->d_type == 8) {//如果是文件，则调用过滤器filter
				string imgFile = dir_name + "/" + ptr->d_name;
				int pos = imgFile.find_last_of(".");
				string pic_type = imgFile.substr(pos + 1, imgFile.length() - pos);
				if (pic_type.compare("bmp") || pic_type.compare("jpg")) {
					cout << "pic path" << imgFile << "\n";
					picVec.emplace_back(imgFile);
				}
			}
		}
		closedir(dir);
	}	
}

slotDetect::slotDetect(string pic_dir, string model_path)
{
	SlotDetection::InputParam inparam;
	//get pic path
	getpicPath(pic_dir);
	inparam.img_width = 240;//algorithm need with is 240
	inparam.img_height = 300;//algorithm need with is 300
	inparam.blind_area.start.x = 94;
	inparam.blind_area.start.y = 99;
	inparam.blind_area.size.w = 50;
	inparam.blind_area.size.h = 113;	
	//car blind_area
	inparam.model_path = model_path;
	pslotDet = new SlotDetection(inparam);
	cout << "init succeed\n" << "\n";
}
	
void slotDetect::slotdetectProcess()
{
	int width = 240;
	int height = 300;
	ImgInfor input_img;
	std::vector<uint32_t> sod_status;
	IplImage *gray, * resize_gray;
	int pic_num = 0;
	if (picVec.size() == 0)
		cout << "on pic need detect" << "\n";
	for (int i = 0; i < picVec.size(); i++) {
		IplImage *src = cvLoadImage(picVec[i].c_str());
		//cvSaveImage("rgb1.bmp", src);
		gray = cvCreateImage(cvSize(src->width, src->height), 8, 1);
		resize_gray = cvCreateImage(cvSize(width, height), 8, 1);
		cvCvtColor(src, gray, CV_RGB2GRAY);
		cvResize(gray, resize_gray);
		//cvSaveImage("gray3.bmp", resize_gray);
		input_img.width = resize_gray->width;
		input_img.height = resize_gray->height;
		input_img.width_step = resize_gray->width;
		input_img.image_fmt = GRAY;
		input_img.data_buf = (std::uint8_t*)resize_gray->imageData;

		SlotDetection::SlotDetectFlag detect_flag = SlotDetection::TwoSideEnable;
		std::list<SlotInfo> slot_list;
		pslotDet->Process(input_img, detect_flag, slot_list);
		pslotDet->slot_sod_status(input_img, slot_list, sod_status);

	#if 1
		//cout << "process one" << "\n";
		string pic_path = picVec[i];
		int pic_pos = pic_path.find_last_of("/");
		string file_name = pic_path.substr(pic_pos + 1, pic_path.length() - pic_pos);
		drawSlot(src, slot_list, sod_status, file_name);
	#endif
		cvReleaseImage(&src);
		cvReleaseImage(&gray);
		cvReleaseImage(&resize_gray);
		pic_num++;
	}
	cout << "process total image num is " << pic_num << "\n";
	
}
void slotDetect::drawSlot(IplImage *src, std::list<SlotInfo> slot_list, std::vector<uint32_t> sod_status, string file_name)
{
	std::list<SlotInfo>::iterator it;
	int i = 0;
	
	for (it = slot_list.begin(); it != slot_list.end(); it++) {
		SlotInfo slot_infor = *it;
		
		CvPoint p0, p1, p2, p3;
		p0.x = slot_infor.img_pos[0].x * 2;
		p0.y = slot_infor.img_pos[0].y * 2;
		
		p1.x = slot_infor.img_pos[1].x * 2;
		p1.y = slot_infor.img_pos[1].y * 2;
		
		p2.x = slot_infor.img_pos[2].x * 2;
		p2.y = slot_infor.img_pos[2].y * 2;
		
		p3.x = slot_infor.img_pos[3].x * 2;
		p3.y = slot_infor.img_pos[3].y * 2;
		
		
		if (slot_infor.slot_shape == Vertical) {
			cvLine(src, p0, p3, Scalar(0, 0, 255), 1);
			cvLine(src, p0, p1, Scalar(0, 0, 255), 1);
			cvLine(src, p3, p2, Scalar(0, 0, 255), 1);
			
		} else if (slot_infor.slot_shape == Horizontal) {
			cvLine(src, p0, p3, Scalar(255, 0, 0), 1);
			cvLine(src, p0, p1, Scalar(255, 0, 0), 1);
			cvLine(src, p3, p2, Scalar(255, 0, 0), 1);
		} else {
			cvLine(src, p0, p3, Scalar(0, 255, 0), 1);
			cvLine(src, p0, p1, Scalar(0, 255, 0), 1);
			cvLine(src, p3, p2, Scalar(0, 255, 0), 1);
		}
		CvPoint txt_pt;
		txt_pt.x = (p0.x + p1.x) / 2;
		txt_pt.y = p0.y + 20;
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_TRIPLEX,
			0.8, 0.8, 0, 1, CV_AA);
		if (sod_status[i] == 0)
			cvPutText(src, "0", txt_pt, &font, cv::Scalar(255, 255, 0));
		else if (sod_status[i] == 1)
			cvPutText(src, "1", txt_pt, &font, cv::Scalar(255, 255, 0));
		i++;	
	}
	//cout << "show slot" << "\n";
	//cvShowImage("res", src);
	//waitKey(0);
	string save_path = "res/" + file_name;
	cvSaveImage(save_path.c_str(), src);
}

slotDetect::~slotDetect()
{
	if (pslotDet)
		delete pslotDet;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		cout << "should at least give 2 parameter, e.g. srcPath detectedPath" << endl;
		exit(1);
	}
	slotDetect *pslotDetect = new slotDetect(argv[1], argv[2]);
	pslotDetect->slotdetectProcess();
	if (pslotDetect)
		delete pslotDetect;
	
	printf("detect end\n");
	return 0;
}
