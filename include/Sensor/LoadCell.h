//
// Created by jaehoon on 22. 6. 1.
//
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>

class LoadCell {
public:
    bool isConnected = true;
    bool isTtySet = true;
    bool isReaded = true;

    LoadCell() {
        mSerialPort = open("/dev/ttyUSB0", O_RDWR);
        if (tcgetattr(mSerialPort, &mTty) != 0) {
            printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
            isConnected = false;
        }
        std::cout << "USB serial communication is successfully connected." << std::endl;
        sleep(1);
        mTty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
        mTty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
        mTty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
        mTty.c_cflag |= CS8; // 8 bits per byte (most common)
        mTty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
        mTty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

        mTty.c_lflag &= ~ICANON;
        mTty.c_lflag &= ~ECHO; // Disable echo
        mTty.c_lflag &= ~ECHOE; // Disable erasure
        mTty.c_lflag &= ~ECHONL; // Disable new-line echo
        mTty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
        mTty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
        mTty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                          ICRNL); // Disable any special handling of received bytes

        mTty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
        mTty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
        // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
        // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

        mTty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
        mTty.c_cc[VMIN] = 0;

        cfsetispeed(&mTty, B115200);
        cfsetospeed(&mTty, B115200);
        if (tcsetattr(mSerialPort, TCSANOW, &mTty) != 0) {
            printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
            isTtySet = false;
        }
        std::cout << "tty setting is successfully set." << std::endl;
        sleep(1);
        memset(&mReadBuf, '\0', sizeof(mReadBuf));
        mNumBytes = read(mSerialPort, &mReadBuf, sizeof(mReadBuf));

        // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
        if (mNumBytes < 0) {
            printf("Error reading: %s", strerror(errno));
            isReaded = false;
        }
        std::cout << "Success to initialize USB serial COM communication for LoadCell." << std::endl;

        flushData(10);
        nulling();
    }

    void readData();
    void nulling();
    void flushData(int num);

    int getRawData() { return mReadedData; }

    double getSensoredWeight() { return mSensoredWeight; }

    double getSensoredForce() { return mSensoredForce; }


private:
    int mSerialPort;
    int mNumBytes;
    int mReaded[10];
    int mLineFeedCode = 10;
    int mSplitCode = 46;
    int mNegativeValueCode = 45;
    int mReadedData = 0;
    int mIdx = 0;

    char mReadBuf[1];

    bool mIsDataStore = true;
    bool mIsNegativeValue = false;

    double mInclineWeight = 0.01081112031;
    double mBiasWeight = -288.7621365;
    double mInclineForce = 0.0001060570902;
    double mBiasForce = -2.832756559;
    double mSensoredWeight = 0;
    double mSensoredForce = 0;

    struct termios mTty;
};