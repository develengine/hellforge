#include "mdvmreader.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <math.h>
#include <string.h>

uint8_t *trainingImages = nullptr;
uint8_t *trainingLabels = nullptr;
uint8_t *testImages = nullptr;
uint8_t *testLabels = nullptr;

const int PATTERN_SIZE = 784;
const int TRAIN_COUNT  = 60000;
const int TEST_COUNT = 10000;
const int LABEL_COUNT  = 10;

struct Guess {
    int label;
    int index;
};

std::vector<int> counters[LABEL_COUNT];
std::vector<float *> banks[LABEL_COUNT];

void merge(uint8_t *a, float *b, int length, int merges) {
    for (int i = 0; i < length; i++) {
        float merged = (b[i] * merges + (a[i] / 256.f)) / (merges + 1);
        b[i] = merged;
    }
}

void append(uint8_t *data, std::vector<float *> &vec, int length) {
    float *arr = new float[length];
    for (int i = 0; i < length; i++)
        arr[i] = (float)data[i] / 256.f;
    vec.push_back(arr);
}

float compare(uint8_t *a, float *b, int length) {
    float difference = 0.f;
    for (int i = 0; i < length; i++) {
        float af = (float)a[i] / 256.f;
        difference += abs(af - b[i]);
    }
    return 1 - (difference / length);
}

Guess guess(uint8_t *data) {
    Guess bestGuess = { -1, -1 };
    float bestScore = -1.f;
    for (int i = 0; i < LABEL_COUNT; i++) {
        for (int j = 0; j < banks[i].size(); j++) {
            float score = compare(data, banks[i][j], PATTERN_SIZE);
            if (score > bestScore) {
                bestScore = score;
                bestGuess = { i, j };
            }
        }
    }
    return bestGuess;
}

void train() {
    for (int i = 0; i < TRAIN_COUNT; i++) {
        uint8_t *pattern = &(trainingImages[PATTERN_SIZE * i]);
        uint8_t label = trainingLabels[i];

        Guess guessed = guess(pattern);
        if (guessed.label != label) {
            append(pattern, banks[label], PATTERN_SIZE);
            counters[label].push_back(1);
        } else {
            merge(
                pattern, 
                banks[label][guessed.index],
                PATTERN_SIZE,
                counters[label][guessed.index]
            );
            counters[label][guessed.index]++;
        }

        std::cout << "Training: " << i << '\n';
    }
}

void test() {
    int succesCount = 0;

    for (int i = 0; i < TEST_COUNT; i++) {
        uint8_t *pattern = &(testImages[PATTERN_SIZE * i]);
        uint8_t label = testLabels[i];

        Guess guessed = guess(pattern);
        if (guessed.label == label)
            succesCount++;

        std::cout << "Testing: " << i << '\n';
    }

    std::cout << "Succes: " << ((float)succesCount / (float)TEST_COUNT) * 100 << "%\n";
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

void freeProcessData() {
    for (int i = 0; i < LABEL_COUNT; i++)
        for (int j = 0; j < banks[i].size(); j++)
            delete[] banks[i][j];
}

int main(int argc, char *argv[]) {
    loadMNISTDigitSet();

//     printDigit(&(testImages[PATTERN_SIZE * 17]));
//     append(testImages, banks[7], PATTERN_SIZE);
//     append(&(testImages[PATTERN_SIZE * 3]), banks[0], PATTERN_SIZE);
//     float val = compare(&(testImages[PATTERN_SIZE * 8]), banks[7][0], PATTERN_SIZE);
//     merge(&(testImages[PATTERN_SIZE * 17]), banks[7][0], PATTERN_SIZE, 1);
//     printDigit(banks[7][0]);
//     std::cout << "Lol: " << val << '\n';
//     
//     Guess ges = guess(&(testImages[PATTERN_SIZE * 17]));
//     std::cout << "Guess: " << ges.pattern << ", " << ges.index << '\n';

    train();
    test();

    freeMNISTDigitSet();
    freeProcessData();

    return 0;
}
