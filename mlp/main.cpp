#include "mdvmreader.hpp"

#include <math.h>

#include <iostream>
#include <vector>
#include <iomanip>

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


const int MAX_WEIGHT = 32;
const int MAX_DIV = RAND_MAX / MAX_WEIGHT;

inline float randf() {
    return ((float)rand() / (float)MAX_DIV) - (MAX_WEIGHT / 2);
} 


int layerLayout[] = { 784, 14, 5, 16, 10 };
int layerCount = -666;
int maxLayerLength = -666;
int iterations = 0;
float **weights = nullptr;
float **gradient = nullptr;
float **activations = nullptr;
float *dataBP = nullptr;
float *dataBPN = nullptr;

void initializeNetwork() {
    int len = sizeof(layerLayout) / sizeof(int);
    layerCount = len;

    weights = new float*[len - 1];
    gradient = new float*[len - 1];
    activations = new float*[len - 1];

    for (int i = 0; i < len - 1; i++) {
        if (layerLayout[i + 1] > maxLayerLength) {
            maxLayerLength = layerLayout[i + 1];
        }
        int nodeSize = layerLayout[i] + 1;
        int layerSize = layerLayout[i + 1];
        weights[i] = new float[nodeSize * layerSize];
        gradient[i] = new float[nodeSize * layerSize];
        activations[i] = new float[layerSize * 2];
    }

    dataBP = new float[maxLayerLength];
    dataBPN = new float[maxLayerLength];

    for (int i = 0; i < len - 1; i++) {
        int layerSize = (layerLayout[i] + 1) * layerLayout[i + 1];
        for (int j = 0; j < layerSize; j++) {
            weights[i][j] = randf();
        }
    }
}

void destroyNetwork() {
    for (int i = 0; i < layerCount - 1; i++) {
        delete[] weights[i];
        delete[] gradient[i];
    }

    delete[] weights;
    delete[] gradient;
    delete[] activations;
    delete[] dataBP;
}


inline float LReLU(float x) {
    if (x > 0) return x;
    else return x * 0.18f;
}

inline float d_LReLU(float x) {
    if (x > 0) return 1.f;
    else return 0.18f;
}

inline float d_tanh(float x) {
    float tnh = tanhf(x);
    return 1 - tnh * tnh;
}

void calculate(uint8_t *data, int pattern) {
    int nodeSize = layerLayout[0];
    for (int i = 0; i < layerLayout[1]; i++) {
        int nodeOffset = (nodeSize + 1) * i;
        float activation = weights[0][nodeOffset];
        for (int j = 0; j < nodeSize; j++) {
            activation += weights[0][nodeOffset + 1 + j] * (data[nodeSize * pattern + j] / 255.f);
        }
        activations[0][i * 2] = tanhf(activation);
        activations[0][i * 2 + 1] = activation;
    }

    for (int i = 1; i < layerCount - 1; i++) {
        int nodeSize = layerLayout[i];
        for (int j = 0; j < layerLayout[i + 1]; j++) {
            int nodeOffset = (nodeSize + 1) * j;
            float activation = weights[i][nodeOffset];
            for (int k = 0; k < nodeSize; k++) {
                activation += weights[i][nodeOffset + 1 + k] * activations[i - 1][k * 2];
            }
            activations[i][j * 2] = tanhf(activation);
            activations[i][j * 2 + 1] = activation;
        }
    }
}

void train(uint8_t *input, int pattern, float *output) {

    for (int i = 0; i < layerLayout[layerCount - 1]; i++) {
        float errToOut = (activations[layerCount - 2][i * 2] - output[i]);
        float outToAct = d_tanh(activations[layerCount - 2][i * 2 + 1]);
        dataBP[i] = errToOut * outToAct;
    }

    int nodeSize = layerLayout[layerCount - 2] + 1;

    for (int i = 0; i < layerLayout[layerCount - 1]; i++) {
        int nodeOffset = i * nodeSize;
        float dataBPi = dataBP[i];
        gradient[layerCount - 2][nodeOffset] += dataBPi;
        for (int j = 1; j < nodeSize; j++) {
            gradient[layerCount - 2][nodeOffset + j] += dataBPi * activations[layerCount - 3][j * 2 - 2];
        }
    }

    for (int i = layerCount - 3; i > 0; i--) {

        nodeSize = layerLayout[i] + 1;

        for (int j = 0; j < layerLayout[i + 1]; j++) {
            float errToIn = 0;
            for (int k = 0; k < layerLayout[i + 2]; k++) {
                errToIn += dataBP[k] * weights[i + 1][nodeSize * k + j + 1];
            }
            dataBPN[j] = errToIn * d_tanh(activations[i][j * 2 + 1]);
        }

        float *temp = dataBP;
        dataBP = dataBPN;
        dataBPN = temp;

        for (int j = 0; j < layerLayout[i + 1]; j++) {
            int nodeOffset = j * nodeSize;
            float dataBPj = dataBP[j];
            gradient[i][nodeOffset] += dataBPj;
            for (int k = 1; k < nodeSize; k++) {
                gradient[i][nodeOffset + k] += dataBPj * activations[i - 1][k * 2 - 2];
            }
        }
    }

    nodeSize = layerLayout[0] + 1;

    for (int i = 0; i < layerLayout[1]; i++) {
        float errToIn = 0;
        for (int j = 0; j < layerLayout[2]; j++) {
            errToIn += dataBP[j] * weights[1][nodeSize * j + i + 1];
        }
        dataBPN[i] = errToIn;
    }

    for (int i = 0; i < layerLayout[1]; i++) {
        int nodeOffset = i * nodeSize;
        float dataBPNi = dataBPN[i];
        gradient[0][nodeOffset] += dataBPNi;
        for (int j = 1; j < nodeSize; j++) {
            gradient[0][nodeOffset + j] += dataBPNi * (((float)(input[(pattern * nodeSize) + (j * 2 - 2)])) / 255.f);
        }
    }

    iterations++;
}

void descent(float learningRate) {
    
}

int main(int argc, char *argv[]) {
    loadMNISTDigitSet();
    srand(time(0));
    initializeNetwork();

    calculate(testImages, 3);
    float stuffs[] = { 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
    train(testImages, 3, stuffs);

    destroyNetwork();
    freeMNISTDigitSet();
    return 0;
}
