#include "Image.hpp"

using namespace std;


int main()
{
    // Create image object
    Image img;

    // Keep trying to open the serial port
    while (!img.start_serial()) {};

    img.find_first_pixel();
    bool input_result = false;
    while (true) {
        img.read_image();
        input_result = img.print_image_and_check_input(img.frame);
        if (input_result) {
            break;
        }
    }
}

