#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <fstream>
#include <iostream>


class FileReader {
private:
    std::ifstream file;     // Input file stream
    unsigned char buffer;   // Buffer to hold the current byte
    int bitPosition;        // Current bit position within the buffer (0-7)
    bool isBufferValid;     // Flag to check if the buffer has valid data

    // Loads the next byte into the buffer
    bool loadNextByte() {
        if (file.get(reinterpret_cast<char&>(buffer))) {
            bitPosition = 7; // Reset bit position to the most significant bit
            isBufferValid = true;
            return true;
        }
        isBufferValid = false;
        return false;
    }

public:
    // Constructor: opens the file in binary mode
    FileReader(const std::string& filename) : buffer(0), bitPosition(-1), isBufferValid(false) {
        file.open(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file: " << filename << std::endl;
        }
    }

    // Destructor: closes the file
    ~FileReader() {
        if (file.is_open()) {
            file.close();
        }
    }

    // Reads the next bit from the file
    bool readBit(bool& bit) {
        if (!isBufferValid || bitPosition < 0) {
            if (!loadNextByte()) {
                return false; // End of file reached
            }
        }

        // Extract the current bit from the buffer
        bit = (buffer >> bitPosition) & 1;
        bitPosition--; // Move to the next bit position
        return true;
    }

    // Checks if the file is successfully opened
    bool isOpen() const {
        return file.is_open();
    }
};

#endif