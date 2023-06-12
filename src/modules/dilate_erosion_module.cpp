#include <opencv2/opencv.hpp>

void dilate_erosion_module ::update() {
    cv::Mat image = cv::imread("C:/Users/benja/Desktop/mark.jpg", cv::IMREAD_COLOR);

    if (image.empty()) {
        std::cout << "Impossible de charger l'image." << std::endl;
        return -1;
    }

    cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat erodeElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    cv::Mat dilatedImage;
    cv::dilate(image, dilatedImage, dilateElement);

    cv::Mat erodedImage;
    cv::erode(image, erodedImage, erodeElement);

    cv::imshow("Image originale", image);
    cv::imshow("Image dilatée", dilatedImage);
    cv::imshow("Image érodée", erodedImage);
    cv::waitKey(0);

    return 0;
}
