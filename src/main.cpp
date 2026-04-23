#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    // Try ttyAMA0 first (Hardware UART on Pi 5 GPIO 14/15)
    // If this fails, try "/dev/serial0"
    const char* portName = "/dev/ttyAMA0";
    int serial_port = open(portName, O_RDWR | O_NOCTTY);

    if (serial_port < 0) {
        std::cerr << "Error " << errno << " opening " << portName << ": " << strerror(errno) << std::endl;
        std::cerr << "Try: sudo ./lora" << std::endl;
        return 1;
    }

    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0) {
        std::cerr << "Error from tcgetattr" << std::endl;
        return 1;
    }

    // Standard Baud Rate for Waveshare E22 is 9600
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;        // No parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;            // 8 bits
    tty.c_cflag |= (CLOCAL | CREAD); 

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); 
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

    tty.c_oflag &= ~OPOST;

    // Non-blocking read timeout (100ms)
    tty.c_cc[VTIME] = 1;
    tty.c_cc[VMIN] = 0;

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        std::cerr << "Error from tcsetattr" << std::endl;
        return 1;
    }

    std::cout << "--- Pi 5 LoRa UART Started on " << portName << " ---" << std::endl;
    std::cout << "Make sure jumpers are on 'B' and M0/M1 are GND." << std::endl;

    int count = 0;
    char read_buf[256];

    while (true) {
        // 1. TRANSMIT
        std::string msg = "Ping #" + std::to_string(count++) + "\r\n";
        int bytes_written = write(serial_port, msg.c_str(), msg.length());
        
        if (bytes_written > 0) {
            std::cout << "[TX]: " << msg;
        } else {
            std::cerr << "Write Error!" << std::endl;
        }

        // 2. RECEIVE (Check if anything came back)
        // We check several times during the sleep interval
        for(int i = 0; i < 10; i++) {
            memset(&read_buf, '\0', sizeof(read_buf));
            int n = read(serial_port, &read_buf, sizeof(read_buf));
            if (n > 0) {
                std::cout << "[RX]: " << read_buf << std::endl;
            }
            usleep(200000); // Check every 200ms
        }
    }

    close(serial_port);
    return 0;
}