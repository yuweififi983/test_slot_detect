#ifndef _SLOT_DETECT_PUBLIC_H_
#define _SLOT_DETECT_PUBLIC_H_

#include <cstdint>
#include <list>
#include <string>
#include <vector>
#include "type_def.h"
#include "slot_type.h"
using namespace std;

namespace zros {

namespace slot_detect {
class SlotDetection 
{
public:
	struct InputParam{
		std::uint32_t img_height;
		std::uint32_t img_width;
		u32_rect blind_area;
		string model_path;
	};
	enum SlotDetectFlag {
		LeftEnable = 1,
		RightEnable = 2,
		TwoSideEnable = 3
	};
	SlotDetection(InputParam param);
	~SlotDetection();
	bool Process(ImgInfor& yuv, SlotDetectFlag detect_flag, std::list<SlotInfo>& slot_list);
	void slot_sod_status(ImgInfor& yuv, std::list<SlotInfo>& slot_list, std::vector<uint32_t> &sod_status);
private:
	std::list<SlotInfo> slotInfo;
	std::uint64_t algo_handle;
	std::string model_path;
};

}
}

#endif
