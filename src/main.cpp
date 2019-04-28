#include "mdvmreader.hpp"

#include <iostream>
#include <string>

#include <stdio.h>

uint8_t *trainingImages = nullptr;
uint8_t *trainingLabels = nullptr;

uint8_t *testImages = nullptr;
uint8_t *testLabels = nullptr;

template <typename T>
void free(T *p) {
    if (p != nullptr) {
        delete[] p;
        p = nullptr;
    }
}

void loadMNISTDigitSet(
    std::string trLabels,
    std::string trImages,
    std::string teLabels,
    std::string teImages
) {
    MDVM::loadMDVMFile("res/" + trImages, 3, trainingImages);    
    MDVM::loadMDVMFile("res/" + trLabels, 1, trainingLabels);
    MDVM::loadMDVMFile("res/" + teImages, 3, testImages);
    MDVM::loadMDVMFile("res/" + teLabels, 1, testLabels);
}

void freeMNISTDigitSet() { 
    free(trainingImages);
    free(trainingLabels);
    free(testImages);
    free(testLabels);
}

int main(int argc, char *argv[]) {
    loadMNISTDigitSet(
        "train-labels-idx1-ubyte",
        "train-images-idx3-ubyte",
        "t10k-labels-idx1-ubyte",
        "t10k-images-idx3-ubyte"
    );
    
    freeMNISTDigitSet();

    return 0;
}

