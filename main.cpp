#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fmt/core.h>
#include <iostream>
#include <filesystem>

#include "Detection.h"

int main(int argc, char *argv[]) {

    double bpm = 0;
    double offsetBefore = 0;
    double offsetAfter = 0;

    if (argc < 2) {
        fmt::println("Usage: ApexAutoCut <rootPath> / ApexAutoCut <rootPath>  <bpm> / ApexAutoCut <rootPath> <before(s)> <after(s)>");
        return -1;
    } else if (argc == 3) {
        bpm = std::stod(argv[2]);
        offsetBefore = (60/bpm)*4;
        offsetAfter = (60/bpm)*2;

        while (offsetBefore < 1) {
            offsetBefore = offsetBefore * 2;
            offsetAfter = offsetBefore * 2;
        }

    } else if (argc == 4) {
        offsetBefore = std::stod(argv[2]);
        offsetAfter = std::stod(argv[3]);
    }

    std::string rootPath = argv[1];

    tesseract::TessBaseAPI tess;

    if (tess.Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cerr << "Error: Could not initialize tesseract." << std::endl;
        return {};
    }

    auto vec = std::vector<std::filesystem::directory_entry>();

    auto savedir = std::filesystem::directory_entry("/home/yudek/Documents/CLionProjects/ApexAutoCut/data/out/");
    if (!savedir.exists()) {
        create_directory(savedir);
    }

    Detection::getVideos(vec,savedir,rootPath);

    for (auto const& vidf : vec) {
        fmt::println("Processing {}",vidf.path().filename().generic_string());

        auto killTime = Detection::findKillFrame(vidf.path().generic_string(),tess,16);

        if (killTime == 0) {
            continue;
        }

        fmt::println("Detected kill at {}s",killTime);

        if (offsetBefore != 0) {
            Detection::render(offsetBefore,offsetAfter,killTime,vidf,savedir.path().generic_string());
        } else {
            Detection::render(4,2,killTime,vidf,savedir.path().generic_string());
        }

    }

    tess.End();
    cv::destroyAllWindows();
    return 0;
}
