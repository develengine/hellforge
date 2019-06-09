#include "mdvmreader.hpp"
#include "knnc.hpp"

#include <iostream>

uint8_t *trainingImages = nullptr;
uint8_t *trainingLabels = nullptr;
uint8_t *testImages = nullptr;
uint8_t *testLabels = nullptr;

const int TRAIN_COUNT  = 60000;
const int TEST_COUNT = 10000;

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

void loadMNISTDigitSet() {
    MDVM::loadMDVMFile("res/train-labels-idx1-ubyte", 1, &trainingLabels);
    MDVM::loadMDVMFile("res/train-images-idx3-ubyte", 3, &trainingImages);
    MDVM::loadMDVMFile("res/t10k-labels-idx1-ubyte",  1, &testLabels);
    MDVM::loadMDVMFile("res/t10k-images-idx3-ubyte",  3, &testImages);
}

void freeMNISTDigitSet() { 
    delete[] trainingImages;
    delete[] trainingLabels;
    delete[] testImages;
    delete[] testLabels;
}

int main(int argc, char *argv[]) {
    loadMNISTDigitSet();

    KNN::Compressor<784, 10>  comp;
    comp.train(trainingImages, trainingLabels, TRAIN_COUNT);
    comp.test(testImages, testLabels, TEST_COUNT);

    freeMNISTDigitSet();

    return 0;
}
