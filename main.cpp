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
    bool _delete = false;

    if (argc < 2) {
        fmt::println(
                "Usage: ApexAutoCut <rootPath> <delete(bool)> / ApexAutoCut <rootPath> <delete(bool)> <bpm> / ApexAutoCut <rootPath> <delete(bool)> <before(s)> <after(s)>");
        return -1;
    } else if (argc == 3) {
        if (argv[2] == std::string("true")) {
            _delete = true;
        }
    } else if (argc == 4) {
        if (argv[2] == std::string("true")) {
            _delete = true;
        }

        bpm = std::stod(argv[3]);
        offsetBefore = (60 / bpm) * 4;
        offsetAfter = (60 / bpm) * 2;

        while (offsetBefore < 1) {
            offsetBefore = offsetBefore * 2;
            offsetAfter = offsetBefore * 2;
        }

    } else if (argc == 5) {
        if (argv[2] == std::string("true")) {
            _delete = true;
        }

        offsetBefore = std::stod(argv[3]);
        offsetAfter = std::stod(argv[4]);
    }

    fmt::println("Config: bpm:{} offsetPre:{} offsetAfter:{} delete:{}", bpm, offsetBefore, offsetAfter, _delete);

    std::string rootPath = argv[1];

    tesseract::TessBaseAPI tess;

    tesseract::TessBaseAPI tessRUS;

    if (tess.Init(nullptr, "eng", tesseract::OEM_LSTM_ONLY)) {
        std::cerr << "Error: Could not initialize tesseract." << std::endl;
        return {};
    }

    if (tessRUS.Init(nullptr, "rus", tesseract::OEM_LSTM_ONLY)) {
        std::cerr << "Error: Could not initialize tesseractRUS." << std::endl;
        return {};
    }

    auto vec = std::vector<std::filesystem::directory_entry>();

    auto savedir = std::filesystem::directory_entry(rootPath + "out/");
    if (!savedir.exists()) {
        create_directory(savedir);
    }

    auto wDir = std::filesystem::directory_entry(savedir.path().generic_string() + "/wins");

    if (!wDir.exists()) {
        create_directory(wDir);
    }

    auto saveVec = std::vector<File>();

    Detection::getVideos(vec, savedir, rootPath);

    auto counter = 0;

    for (auto const &vidf: vec) {
        fmt::println("Processing {}, {}/{}", vidf.path().filename().generic_string(), counter, vec.size());
        counter++;

        auto killTime = Detection::findKillFrame(660, 760, 600, 40, vidf.path().generic_string(), tess, tessRUS, 32); //knocked

        if (killTime == 0) {
            saveVec.push_back(File(false, vidf));
            continue;
        }

        saveVec.push_back(File(true, vidf));

        fmt::println("Detected kill at {}s", killTime);

        if (offsetBefore != 0) {
            Detection::render(offsetBefore, offsetAfter, killTime, vidf, savedir.path().generic_string());
        } else {
            Detection::render(4, 2, killTime, vidf, savedir.path().generic_string());
        }
    }

    counter = 0;

    for (auto &f: saveVec) {
        if (!f.isKill) {
            fmt::println("Reprocessing {}, {}/{}", f.fs.path().filename().generic_string(), counter, saveVec.size());
            counter++;

            auto killTime = Detection::findKillFrame(660, 820, 600, 60, f.fs.path().generic_string(), tess, tessRUS,
                                                     32); //respawn + team eliminated + knocked (2 cases)

            if (killTime == 0) {
                fmt::println("Reprocessing for 2nd time {}, {}/{}", f.fs.path().filename().generic_string(), counter-1, saveVec.size());
                killTime = Detection::findKillFrame(660, 894, 600, 40, f.fs.path().generic_string(), tess, tessRUS,
                                                    32); //respawn + team eliminated + knocked (1 case)
                if (killTime == 0) {
                    continue;
                }
            }

            f.isKill = true;

            fmt::println("Detected kill at {}s", killTime);

            if (offsetBefore != 0) {
                Detection::render(offsetBefore, offsetAfter, killTime, f.fs, savedir.path().generic_string());
            } else {
                Detection::render(4, 2, killTime, f.fs, savedir.path().generic_string());
            }
        }
    }

    counter = 0;

    for (auto const &f: saveVec) {
        if (f.isKill) {
            if (_delete) {
                std::filesystem::remove(f.fs.path().generic_string());
            }
        } else {
            fmt::println("Moving file from:{} to:{}, {}/{}", f.fs.path().generic_string(),
                         wDir.path().generic_string() + "/" + f.fs.path().filename().generic_string(), counter,
                         saveVec.size());
            std::filesystem::copy(f.fs.path().generic_string(),
                                  wDir.path().generic_string() + "/" + f.fs.path().filename().generic_string());
            if (_delete) {
                std::filesystem::remove(f.fs.path().generic_string());
            }
        }
        counter++;
    }


    tess.End();
    tessRUS.End();
    cv::destroyAllWindows();
    return 0;
}
