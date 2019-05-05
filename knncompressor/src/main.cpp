#include "mdvmreader.hpp"

#include <iostream>
#include <string>

#include <stdio.h>
#include <math.h>
#include <string.h>

uint8_t *trainingImages = nullptr;
uint8_t *trainingLabels = nullptr;
uint8_t *testImages = nullptr;
uint8_t *testLabels = nullptr;

const int PATTERN_SIZE = 784;
const int TRAIN_IMAGE_COUNT  = 60000;
const int TEST_IMAGE_COUNT = 10000;
const int LABEL_COUNT  = 10;

struct Guess {
    int label;
    int index;
};

struct PatternBranch {
    int patternCount   = 0;
    int *mergeCounters = nullptr;
    float **data       = nullptr;

    void append(uint8_t *input) {
        float **newData  = new float*[patternCount + 1];
        int *newCounters = new int[patternCount + 1];

        if (data != nullptr) {
            memcpy(newData, data, patternCount * sizeof(float));
            memcpy(newCounters, mergeCounters, patternCount * sizeof(float));
            delete[] data;
            delete[] mergeCounters;
        }

        mergeCounters[patternCount] = 1;
        data[patternCount] = new float[PATTERN_SIZE];
        for (int i = 0; i < PATTERN_SIZE; i++)
            data[patternCount][i] = (float)input[i] / 256.f;
        patternCount++;
    } 

    ~PatternBranch() {
        for (int i = 0; i < patternCount; i++)
            delete[] data[i];
        delete[] data;
    }
};

PatternBranch patterns[LABEL_COUNT];


template <typename T>
void free(T *p) {
    if (p != nullptr) {
        delete[] p;
        p = nullptr;
    }
}

void loadMNISTDigitSet() {
    MDVM::loadMDVMFile("res/train-labels-idx1-ubyte", 3, trainingImages);
    MDVM::loadMDVMFile("res/train-images-idx3-ubyte", 1, trainingLabels);
    MDVM::loadMDVMFile("res/t10k-labels-idx1-ubyte",  3, testImages);
    MDVM::loadMDVMFile("res/t10k-images-idx3-ubyte",  1, testLabels);
}

void freeMNISTDigitSet() { 
    free(trainingImages);
    free(trainingLabels);
    free(testImages);
    free(testLabels);
}

float comparePatterns(uint8_t *a, float *b) {
    double differenceSum = 0;
    for (int i = 0; i < PATTERN_SIZE; i++) {
        differenceSum += abs(b[i] - ((float)a[i] / 256.f));
    }
    return 1 - (differenceSum / PATTERN_SIZE);
}

void mergePatterns(uint8_t *a, float *b, int m) {
    int n = m + 1;
    for (int i = 0; i < PATTERN_SIZE; i++)
        b[i] = ((b[i] * m) + ((float)a[i] / 256.f)) / n;
}

Guess guess(uint8_t *input) {
    float highestScore = 0.f;
    int bestIndex = -1;
    int bestLabel = -1;
    for (int i = 0; i < LABEL_COUNT; i++) {
        int branchLength = patterns[i].patternCount;
        for (int j = 0; j < branchLength; j++) {
            float score = comparePatterns(input, patterns[i].data[j]);
            if (score > highestScore) {
                bestIndex = j;
                bestLabel = i;
            }
        }
    }
    return { bestLabel, bestIndex };
}

void compressData() {
    for (int i = 0; i < TRAIN_IMAGE_COUNT; i++) {
        uint8_t *currentData = &(trainingImages[i * PATTERN_SIZE]); 
        int currentLabel = trainingLabels[i];
        Guess guessed = guess(currentData);
        PatternBranch &dest = patterns[currentLabel];

        if (guessed.label == -1 || guessed.label != currentLabel) {
            dest.append(currentData);
        } else {
            mergePatterns(
                currentData,
                dest.data[guessed.index],
                dest.mergeCounters[guessed.index]
            );
            dest.mergeCounters[guessed.index]++;
        }
    }
}

void testData() {
    int wrongCount = 0;    
    for (int i = 0; i < TEST_IMAGE_COUNT; i++) {
        uint8_t *currentData = &(testImages[i * PATTERN_SIZE]);
        int currentLabel = testLabels[i];
        Guess guessed = guess(currentData);
        if (guessed.label != currentLabel)
            wrongCount++;
    }
    float score = (float)wrongCount / (float)TEST_IMAGE_COUNT;
    std::cout << "Score: " << score << '\n';
}

int main(int argc, char *argv[]) {
    loadMNISTDigitSet();
    
    compressData();
    testData();

    freeMNISTDigitSet();

    return 0;
}

