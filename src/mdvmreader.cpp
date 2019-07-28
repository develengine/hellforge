#include "mdvmreader.hpp"

#include <stdio.h>

namespace MDVM {

inline uint32_t swapEndian(uint8_t *bytes) {
    return (uint32_t)((bytes[0] << 24) | 
                      (bytes[1] << 16) | 
                      (bytes[2] << 8) | 
                       bytes[3]);
}

int loadMDVMFile(std::string path, int dimc, uint8_t **dest) { 
    auto f = fopen(path.c_str(), "r");
    uint8_t bytes[4 + dimc * 4];
    
    uint32_t magic = 0;
    uint32_t *dimensions = new uint32_t[dimc];    
    int dataLength = 1;

    int read = fread(bytes, sizeof(bytes), 1, f);
    if (read != 1)
        return read;

    magic = swapEndian(bytes);
    for (int i = 0; i < dimc; i++) { 
        dimensions[i] = swapEndian(&(bytes[4 + i * 4]));
        dataLength *= dimensions[i];
    }

#ifdef ENG_DEBUG
    std::cout << magic << '\n';
    for (int i = 0; i < dimc; i++)
        std::cout << dimensions[i] << '\n';
#endif

    *dest = new uint8_t[dataLength];
    read = fread(*dest, dataLength, 1, f);
    if (read != 1)
        return read;
    
    fclose(f);
    return 1;
}

void printDigit(uint8_t *data) {
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            float value = data[i*28+j];
            char output = ' ';
            if (value > 192) {
                output = '#';
            } else if (value > 128) {
                output = ':';
            } else if (value > 64) {
                output = '`';
            }
            std::cout << output;
        }
        std::cout << '\n';
    }
}

void printDigit(float *data) {
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            float value = data[i*28+j];
            char output = ' ';
            if (value > 0.75) {
                output = '#';
            } else if (value > 0.5) {
                output = ':';
            } else if (value > 0.25) {
                output = '`';
            }
            std::cout << output;
        }
        std::cout << '\n';
    }
}

}
