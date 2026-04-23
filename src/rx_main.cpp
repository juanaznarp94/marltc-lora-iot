#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int main() {
    const char* portName = "/dev/ttyAMA0";
    int serial_port = open(portName, O_RDWR);

    if (serial_port < 0) {
        std::cerr << "Error opening " << portName << ". Use sudo." << std::endl;
        return 1;
    }

    struct termios tty;
    tcgetattr(serial_port, &tty);
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD | CS8);
    tty.c_lflag &= ~(ICANON | ECHO | ISIG);
    tcsetattr(serial_port, TCSANOW, &tty);

    std::cout << "--- SX1262 Receiver Listening ---" << std::endl;

    char read_buf[1024];
    while (true) {
        memset(&read_buf, '\0', sizeof(read_buf));
        int n = read(serial_port, &read_buf, sizeof(read_buf));
        
        if (n > 0) {
            std::cout << "[DATA RECEIVED]: " << read_buf << std::endl;
        }
        // Small sleep to prevent CPU spiking
        usleep(100000); 
    }

    close(serial_port);
    return 0;
}