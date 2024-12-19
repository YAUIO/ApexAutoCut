#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

class Detection {
public:
    static bool detectKnock(cv::Mat const& frame, tesseract::TessBaseAPI & tess);
};
