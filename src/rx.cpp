#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    // Standard Pi 5 Hardware UART path
    const char* portName = "/dev/ttyAMA0";
    int serial_port = open(portName, O_RDWR | O_NOCTTY);

    if (serial_port < 0) {
        std::cerr << "Error " << errno << " opening " << portName << ": " << strerror(errno) << std::endl;
        return 1;
    }

    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Error from tcgetattr" << std::endl;
        return 1;
    }

    // Must match the transmitter's baud rate (9600)
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= (CLOCAL | CREAD);

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    tty.c_oflag &= ~OPOST;

    // Blocking read: it will wait up to 0.1s for data
    tty.c_cc[VTIME] = 1; 
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr" << std::endl;
        return 1;
    }

    std::cout << "--- SX1262 Receiver Started (Pi 5) ---" << std::endl;
    std::cout << "Listening on " << portName << "..." << std::endl;

    char read_buf[1024]; // Larger buffer for incoming packets

    while (true) {
        memset(&read_buf, '\0', sizeof(read_buf));
        
        // Read is non-blocking (or timed-out) based on VTIME
        int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

        if (num_bytes > 0) {
            std::cout << "[RX DATA]: " << read_buf << std::flush;
        } else if (num_bytes < 0) {
            std::cerr << "Read error: " << strerror(errno) << std::endl;
        }

        // Small sleep to keep CPU usage low
        usleep(10000); 
    }

    close(serial_port);
    return 0;
}