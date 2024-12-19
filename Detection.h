#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fmt/core.h>
#include <filesystem>

class Detection {
    static bool detectKnock(int const& x, int const& y, int const& width, int const& height, cv::Mat const& frame, tesseract::TessBaseAPI & tess, tesseract::TessBaseAPI & tessRUS);
public:
    static void render(double const &offsetMinus, double const &offsetPlus, double const &firstOccurence, std::filesystem::directory_entry const& src, std::string const& outPath);
    static double findKillFrame(int const& x, int const& y, int const& width, int const& height, std::string const& pathToVideo, tesseract::TessBaseAPI & tess, tesseract::TessBaseAPI & tessRUS, int const& skip);
    static void getVideos (std::vector<std::filesystem::directory_entry> & out, std::filesystem::directory_entry const& savedir, std::string const& rootPath);
};

struct File {
public:
    bool isKill;
    std::filesystem::directory_entry fs;
};
