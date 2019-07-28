#pragma once

#include <vector>

#include <stdio.h>
#include <math.h>

namespace KNN {

struct Guess {
    int label;
    int index;
};

template<typename T = float>
void merge(uint8_t *a, T *b, int length, int merges) {
    for (int i = 0; i < length; i++) {
        float merged = (b[i] * merges + (a[i] / (T)256)) / (merges + 1);
        b[i] = merged;
    }
}

template<typename T = float>
float compare(uint8_t *a, T *b, int length) {
    T difference = 0.f;
    for (int i = 0; i < length; i++) {
        T af = (T)a[i] / (T)256;
        difference += abs(af - b[i]);
    }
    return 1 - (difference / length);
}

template<unsigned int PATTERN_SIZE, unsigned int LABEL_COUNT, typename T = float>
class Compressor {

    std::vector<int> counters[LABEL_COUNT];
    std::vector<T *> banks[LABEL_COUNT];

    void append(uint8_t *data, int label) {
        T *arr = new T[PATTERN_SIZE];
        for (int i = 0; i < PATTERN_SIZE; i++)
            arr[i] = (T)data[i] / (T)256;
        banks[label].push_back(arr);
    }

    Guess guess(uint8_t *data) {
        Guess bestGuess = { -1, -1 };
        T bestScore = -1;
        for (int i = 0; i < LABEL_COUNT; i++) {
            for (int j = 0; j < banks[i].size(); j++) {
                T score = compare<T>(data, banks[i][j], PATTERN_SIZE);
                if (score > bestScore) {
                    bestScore = score;
                    bestGuess = { i, j };
                }
            }
        }
        return bestGuess;
    }

public:
 
    void train(uint8_t *data, uint8_t *labels, unsigned int count) {
        for (int i = 0; i < count; i++) {
            uint8_t *pattern = &(data[PATTERN_SIZE * i]);
            uint8_t label = labels[i];
    
            Guess guessed = guess(pattern);
            if (guessed.label != label) {
                append(pattern, label);
                counters[label].push_back(1);
            } else {
                merge<T>(
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
    
    void test(uint8_t *data, uint8_t *labels, unsigned int count) {
        int succesCount = 0;
    
        for (int i = 0; i < count; i++) {
            uint8_t *pattern = &(data[PATTERN_SIZE * i]);
            uint8_t label = labels[i];
    
            Guess guessed = guess(pattern);
            if (guessed.label == label)
                succesCount++;
    
            std::cout << "Testing: " << i << '\n';
        }
    
        std::cout << "Succes: " << ((float)succesCount / (float)count) * 100 << "%\n";
    }

    ~Compressor() {
        for (int i = 0; i < LABEL_COUNT; i++)
            for (int j = 0; j < banks[i].size(); j++)
                delete[] banks[i][j];
    }

};

}
