#include "Detection.h"

bool Detection::detectKnock(cv::Mat const& frame, tesseract::TessBaseAPI & tess) {
    if (frame.empty()) {
        std::cerr << "Error: Cannot read frame image!" << std::endl;
        return false;
    }

    int x = 660;
    int y = 760;
    int width = 600;
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

void Detection::render(double const &offsetMinus, double const &offsetPlus, double const &firstOccurence, std::filesystem::directory_entry const& src, std::string const& outPath) {
    std::string command = "ffmpeg -i \"" + src.path().generic_string() + "\" -ss "
                            + std::to_string(firstOccurence-offsetMinus)
                            +  " -to " + std::to_string(firstOccurence+offsetPlus)
                            +  " -c:v libx264 -c:a aac \"" +
                          outPath + "output_" + src.path().filename().generic_string() + ".mp4\"";

    fmt::println("{}", command);

    system(command.c_str());
}

double Detection::findKillFrame(std::string const &pathToVideo, tesseract::TessBaseAPI & tess, int const& skip) {
    cv::VideoCapture capture(pathToVideo);

    if (!capture.isOpened()) {
        std::cerr << "Error: Cannot open video file!" << std::endl;
        return {};
    }

    cv::Mat frame;
    int frameCount = 0;

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
            firstOccurence = capture.get(cv::CAP_PROP_POS_MSEC)/1000;
            break;
        }
    }

    capture.release();

    return firstOccurence;
}

void Detection::getVideos(std::vector<std::filesystem::directory_entry> & out, std::filesystem::directory_entry const &savedir, std::string const &rootPath) {
    for (auto const& e : std::filesystem::directory_iterator(rootPath)) {
        if (e.is_directory()) {
            if (e.path() == savedir.path()) {
                continue;
            }

            for (auto const& ie : std::filesystem::directory_iterator(e)) {
                if (!ie.is_directory() && ie.path().extension() == ".mp4") {
                    out.push_back(ie);
                }
            }
        } else if (e.path().extension() == ".mp4"){
            out.push_back(e);
        }
    }
}


