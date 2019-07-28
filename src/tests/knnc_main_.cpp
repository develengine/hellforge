#include "mdvmreader.hpp"
#include "knnc.hpp"

#include <iostream>

uint8_t *trainingImages = nullptr;
uint8_t *trainingLabels = nullptr;
uint8_t *testImages = nullptr;
uint8_t *testLabels = nullptr;

const int TRAIN_COUNT  = 60000;
const int TEST_COUNT = 10000;

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

    KNN::Compressor<784, 10, float>  comp;
    comp.train(trainingImages, trainingLabels, TRAIN_COUNT);
    comp.test(testImages, testLabels, TEST_COUNT);

    freeMNISTDigitSet();

    return 0;
}
