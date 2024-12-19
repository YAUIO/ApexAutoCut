#include "Detection.h"

bool Detection::detectKnock(cv::Mat const& frame, tesseract::TessBaseAPI & tess) {
    if (frame.empty()) {
        std::cerr << "Error: Cannot read frame image!" << std::endl;
        return false;
    }

    int x = 770;
    int y = 760;
    int width = 400;
    int height = 60;

    cv::Rect roi(x, y, width, height);
    cv::Mat cropped = frame(roi);

    cv::Mat gray;
    cv::cvtColor(cropped, gray, cv::COLOR_BGR2GRAY);

    tess.SetImage(gray.data, gray.cols, gray.rows, 1, gray.step);
    std::string text = tess.GetUTF8Text();

    if (text.find("KNOCKED DOWN") != std::string::npos) {
        return true;
    }

    return false;
}
