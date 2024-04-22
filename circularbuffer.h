#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <iostream>

class CircularBuffer
{
public:
    CircularBuffer(int windowSize);
    ~CircularBuffer();

    int size;
    int numItems;
    double* data;
    double* dataArr;
    void enqueue(double el);
    double dequeue();
    void printBuffer();
    void updateDataArr();
    bool isEmpty();

private:
    int front;
    int rear;
};

#endif // CIRCULARBUFFER_H
