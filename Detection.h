#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <fmt/core.h>
#include <filesystem>

class Detection {
    static bool detectKnock(cv::Mat const& frame, tesseract::TessBaseAPI & tess);
public:
    static void render(int const &offsetMinus, int const &offsetPlus, double const &firstOccurence, std::filesystem::directory_entry const& src, std::string const& outPath);
    static double findKillFrame(std::string const& pathToVideo, tesseract::TessBaseAPI & tess, int const& skip);
    static void getVideos (std::vector<std::filesystem::directory_entry> & out, std::filesystem::directory_entry const& savedir, std::string const& rootPath);
};
