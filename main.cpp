#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fmt/core.h>
#include <iostream>
#include <thread>

#include "Detection.h"

int main() {
    cv::VideoCapture capture("/home/yudek/Documents/CLionProjects/ApexAutoCut/data/1.mp4");

    if (!capture.isOpened()) {
        std::cerr << "Error: Cannot open video file!" << std::endl;
        return -1;
    }

    tesseract::TessBaseAPI tess;

    if (tess.Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cerr << "Error: Could not initialize tesseract." << std::endl;
        return -1;
    }

    cv::Mat frame;
    int frameCount = 0;
    int skip = 32;

    double firstOccurence = 0;

    while (true) {
        for (int s = 0; s < skip; s++) {
            capture >> frame;
            frameCount++;
        }

        if (frame.empty()) {
            break;
        }

        if (Detection::detectKnock(frame, tess)) {
            firstOccurence = capture.get(cv::CAP_PROP_POS_MSEC);
            break;
        }
    }

    firstOccurence = firstOccurence/1000;

    fmt::println("Detected kill at frame {}, time {}s",frameCount,firstOccurence);

    std::string command = "ffmpeg -i /home/yudek/Documents/CLionProjects/ApexAutoCut/data/1.mp4 -ss "
                            + std::to_string(firstOccurence-4)
                            +  " -to " + std::to_string(firstOccurence+2)
                            +  " -c:v libx264 -c:a aac " +
                          "/home/yudek/Documents/CLionProjects/ApexAutoCut/data/output_" + std::to_string(frameCount) + ".mp4";

    fmt::println("{}", command);

    // Execute FFmpeg command
    system(command.c_str());

    capture.release();
    tess.End();
    cv::destroyAllWindows();
    return 0;
}
