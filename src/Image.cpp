#include "Image.hpp"
#include <thread>
#include <chrono>


/* Helper function for error handling. */
int check(enum sp_return result)
{
	/* For this example we'll just exit on any error by calling abort(). */
	char *error_message;

	switch (result) {
	case SP_ERR_ARG:
		printf("Error: Invalid argument.\n");
		abort();
	case SP_ERR_FAIL:
		error_message = sp_last_error_message();
		printf("Error: Failed: %s\n", error_message);
		sp_free_error_message(error_message);
		abort();
	case SP_ERR_SUPP:
		printf("Error: Not supported.\n");
		abort();
	case SP_ERR_MEM:
		printf("Error: Couldn't allocate memory.\n");
		abort();
	case SP_OK:
	default:
		return result;
	}
}

// Constructor
Image::Image()
    : frame(WIN_SIZE, WIN_SIZE, CV_8UC1, cv::Scalar(0, 0, 0)),
    frame_ready(false),
    window_name("ADNS-3080 Viewer")
{
    // Start OpenCV Window
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
}

// Function to attempt to open the serial port
bool Image::start_serial() {
    struct sp_port **port_list;
    enum sp_return result = sp_list_ports(&port_list);

    if (result == SP_OK) // If the ports list is not empty
    {
        // Go through each serial port in the list
        int vid = 0, pid = 0;
        for (int i = 0; port_list[i] != NULL; i++)
        {
            // Obtain the port's vid and pid
            enum sp_return result_vid_pin = sp_get_port_usb_vid_pid(port_list[i], &vid, &pid);

            // Raspberry Pi Pico -> VID 0x2E8A; Arduino-Pico Framework -> PID 0xF00A
            if (vid == 0x2E8A && pid == 0xF00A) { // if the device was found
                // Get the serial port's name
                char* port_name = sp_get_port_name(port_list[i]);
                printf("Found a Raspberry Pi Pico at port %s\n", port_name);
                
                // Wait for a bit to let linux setup permissions for the usb device
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

                // Start the serial port
                port = port_list[i];
                check(sp_open(port, SP_MODE_READ));

                // Serial port configuration
                check(sp_set_baudrate(port, 115200));
                check(sp_set_bits(port, 8));
                check(sp_set_parity(port, SP_PARITY_NONE));
                check(sp_set_stopbits(port, 1));
                check(sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE));
                check(sp_set_dtr(port, SP_DTR_ON));
                check(sp_set_rts(port, SP_RTS_ON));

                printf("Serial port successfully opened and configured");

                return true;
            }
        }
    }

    // Show standby image
    cv::Mat standby_img = cv::imread(std::string(ASSET_DIR) + "/standby.jpeg", cv::IMREAD_GRAYSCALE);
    print_image_and_check_input(standby_img);

    return false;
}

// Function to find the first pixel
void Image::find_first_pixel() {
    uint8_t first_pixel = 0;

    while (!frame_ready) {
        // read pixels until we obtain the first pixel
        sp_blocking_read(port, &first_pixel, 1, 200);
        if (first_pixel && 0b01000000) {
            frame.data[0] = (first_pixel && 0b00111111);
            break;
        }
    }
    frame_ready = true;
}

// Function to read the frame buffer in the ADNS-3080
void Image::read_image() {
    if (frame_ready) { // If we just read the first pixel
        // Read the remaining 899 pixels
        sp_blocking_read(port, frame.data + 1, WIN_SIZE * WIN_SIZE - 1, 1000);
        // Bring max brightness up to 255
        frame.convertTo(frame, -1, 255.0 / 63.0);
        frame_ready = false;
    } else {
        // Read the 900 pixels
        sp_blocking_read(port, frame.data, WIN_SIZE * WIN_SIZE, 1000);
        // Bring max brightness up to 255
        frame.convertTo(frame, -1, 255.0 / 63.0);
    }
}

// Function to print an image in the OpenCV Window, and receive keyboard input
bool Image::print_image_and_check_input(cv::Mat img) {
    // Scale up the image
    cv::Mat big;
    cv::resize(img, big, cv::Size(), SCALE, SCALE, cv::INTER_NEAREST);
    // Show the scaled image
    cv::imshow(window_name, big);
    // Obtain keyboard input
    if (cv::pollKey() == 27) {
        return true;
    } else {
        return false;
    }
}
