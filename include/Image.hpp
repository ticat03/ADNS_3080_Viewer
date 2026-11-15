#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <libserialport.h>
#include <opencv2/opencv.hpp>
#include <string>

#define SCALE 20
#define WIN_SIZE 30

class Image {
private:
    struct sp_port *port;
    bool frame_ready;
    std::string window_name;
public:
    cv::Mat frame;
    Image();
    bool start_serial();
    void find_first_pixel();
    void read_image();
    bool print_image_and_check_input(cv::Mat img);
};

#endif