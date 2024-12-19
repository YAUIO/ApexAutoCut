#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fmt/core.h>
#include <iostream>
#include <filesystem>

#include "Detection.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fmt::println("Usage: ApexAutoCut <rootPath>");
        return -1;
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
        Detection::render(4,2,killTime,vidf,savedir.path().generic_string());
    }

    tess.End();
    cv::destroyAllWindows();
    return 0;
}
