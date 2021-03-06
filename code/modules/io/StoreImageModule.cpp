#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

#include "logging.hpp"
#include "exceptions.hpp"
#include "data/std.hpp"
#include "data/opencv.hpp"
#include "util.hpp"


#define UIPF_MODULE_ID "uipf.opencv.store_image"
#define UIPF_MODULE_NAME "openCV Store Image"
#define UIPF_MODULE_CATEGORY "opencv"
#define UIPF_MODULE_CLASS StoreImage

#define UIPF_MODULE_INPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the image.")}

#define UIPF_MODULE_OUTPUTS \
		{"image", uipf::DataDescription(uipf::data::OpenCVMat::id(), "the stored image, optional, can be used for further processing.")}

#define UIPF_MODULE_PARAMS \
		{"filename", uipf::ParamDescription("file name of the file to save to. imageformat is derived by fileending automatically. Defaults to 'ORIGINAL_NAME_<step name>.png'.", true) }, \
		{"path", uipf::ParamDescription("optional, if given, will replace the path where the image is stored, also no postfix is added to the filename.", true) }, \
		{"quality", uipf::ParamDescription("compression quality (optional)", true) }

// TODO docs about quality param, refer to http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#imwrite

#include "Module.hpp"


void StoreImage::run() {

	using namespace cv;
	using namespace uipf;
	using namespace uipf::data;
	namespace fs = boost::filesystem;

	std::string filename = getParam<std::string>("filename", "");
	std::string basePath = getParam<std::string>("path", "");
	OpenCVMat::ptr image = getInputData<OpenCVMat>("image");

	// set image name based on processing step if not given
	if (filename.empty()) {
		UIPF_LOG_DEBUG("filename is empty, generating new name for ", image->filename);
		if (image->filename.empty()) {
			filename = getProcessingStepName() + string(".png");
		} else if (basePath.empty()) {
			fs::path f(image->filename);
			string ext = f.extension().string();
			if (ext.empty()) {
				ext = ".png";
			}
			string name = f.stem().string();
			fs::path path = f.parent_path();
			filename = (path / fs::path(name + string("_") + getProcessingStepName() + uipf_str_to_lower(ext))).string();
		} else {
			filename = image->filename;
		}
		UIPF_LOG_DEBUG("new image filename: ", filename);
	}
	if (!basePath.empty()) {
		fs::path f(filename);
		fs::path bp(basePath);
		filename = (bp / fs::path(f.stem().string() + uipf_str_to_lower(f.extension().string())) ).string();
		if (!fs::is_directory(bp)) {
			// TODO error check and reporting
			fs::create_directories(bp);
		}
		UIPF_LOG_DEBUG("adjusted base path to: ", filename);
	}

	std::vector<int> params;

	// choose image quality
	// http://docs.opencv.org/2.4/modules/highgui/doc/reading_and_writing_images_and_video.html#imwrite
	int quality = getParam<int>("quality",-1);
	if (quality != -1) {
		std::string fname_lower = uipf_str_to_lower(filename);
		if (uipf_str_ends_with(fname_lower, ".jpeg") || uipf_str_ends_with(fname_lower, ".jpg")) {
			params.push_back(CV_IMWRITE_JPEG_QUALITY);
		} else if (uipf_str_ends_with(fname_lower, "png")) {
			params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		} else if (uipf_str_ends_with(fname_lower, "bmp") || uipf_str_ends_with(fname_lower, "ppm") ||
				   uipf_str_ends_with(fname_lower, "pgm")) {
			params.push_back(CV_IMWRITE_PXM_BINARY);
		}

		params.push_back(quality);
	}

	image->store(filename, params);
	UIPF_LOG_INFO("stored image ", image->filename);

	setOutputData<OpenCVMat>("image", image);
}
