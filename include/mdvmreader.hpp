#pragma once

#include <stdint.h>
#include <iostream>

namespace MDVM {
    
    inline uint32_t swapEndian(uint8_t *bytes);

    int loadMDVMFile(std::string path, int dimc, uint8_t **dest); 

}
