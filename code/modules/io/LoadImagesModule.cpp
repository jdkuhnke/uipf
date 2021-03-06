#include <iostream>
#include <opencv2/highgui/highgui.hpp>
// boost filesystem for iterating directories
#include <boost/filesystem.hpp>

#include "logging.hpp"
#include "exceptions.hpp"
#include "data/std.hpp"
#include "data/opencv.hpp"
#include "data/list.hpp"


#define UIPF_MODULE_ID "uipf.opencv.load_images"
#define UIPF_MODULE_NAME "openCV Load Images from folder"
#define UIPF_MODULE_CATEGORY "opencv"
#define UIPF_MODULE_CLASS LoadImages

#define UIPF_MODULE_OUTPUTS \
		{"imageset", uipf::DataDescription(uipf::data::List::id(/*TODO implement typed lists*/), "the loaded images.")}

#define UIPF_MODULE_PARAMS \
		{"path", uipf::ParamDescription("name of the directory to load from.") }, \
		{"mode", uipf::ParamDescription("can be either 'color' or 'grayscale' for whether to load the image in color or grayscale mode. Defaults to 'color'.", true) }, \
		{"exif", uipf::ParamDescription("whether to load EXIF data. Defaults to 'yes'.", true) }

#include "Module.hpp"

using namespace cv;
using namespace uipf;
using namespace uipf::data;

cv::Mat loadimage(const std::string& strFilename, const std::string& mode) {
	Mat image;

	// check whether to load the image in grayscale mode, defaults to color
	if (mode.compare("grayscale") == 0) {
		image = imread (strFilename.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
	} else {
		image = imread (strFilename.c_str(), CV_LOAD_IMAGE_COLOR);
	}

	if (!image.data) { // Check for invalid input
		UIPF_LOG_WARNING("unable to read image ", strFilename);
		//throw ErrorException(string("Could not open or find the image: ") + strFilename);
	}
	return image;
}

void LoadImages::run() {

	using namespace boost::filesystem;
	using namespace std;

	List::ptr list(new List());

	std::string sPath = getParam<std::string>("path","./images");
	std::string mode = getParam<std::string>("mode", "color");

	UIPF_LOG_TRACE("Looking for images in ", sPath);

	path p(sPath);
//	if (p.is_relative()) {
//		p = system_complete(p);
//	}

	try {
		if (!exists(p)) {
			throw ErrorException(std::string("Image search path does not exist: ") + p.string());
		} else if (is_directory(p)) {

			vector<string> fileNames;

			directory_iterator end_itr;
			for (directory_iterator itr( p ); itr != end_itr; ++itr) {
				// try to load library if it is a file ending with system specific name, .so/.dll
				if (is_regular_file(itr->path())) {
					fileNames.push_back(itr->path().string());
				}
			}

			// two loops to allow progess report
			updateProgress(0, (int) fileNames.size());
			int i = 0;
			for(string file: fileNames) {
				Mat mat = loadimage(file, mode);
				if (mat.data) {
					OpenCVMat::ptr image(new OpenCVMat(mat));
					image->filename = file;
					if (getParam<bool>("exif", true)) {
						image->exif = load_image_exif_data(image->filename);
					}
					list->getContent().push_back(image);
				}
				updateProgress(++i, (int) fileNames.size());
			}

		} else if (is_regular_file(p)) {
			Mat mat = loadimage(p.string(), mode);
			if (mat.data) {
				OpenCVMat::ptr image(new OpenCVMat(mat));
				image->filename = p.string();
				if (getParam<bool>("exif", true)) {
					image->exif = load_image_exif_data(image->filename);
				}
				list->getContent().push_back(image);
			}
		} else {
			throw ErrorException(std::string("Image search path is not a regular file or directory: ") + p.string());
		}
	}
	catch (const filesystem_error& ex) {
		throw ErrorException(std::string("Failed to read image search path: ") + p.string() + std::string(" : ") + ex.what());
	}

	UIPF_LOG_INFO("loaded ", list->getContent().size(), " images from ", p.string());

	setOutputData<List>("imageset", list);
}
