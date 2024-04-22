#include "circularbuffer.h"

CircularBuffer::CircularBuffer(int windowSize)
{
    data = new double[windowSize];
    dataArr = new double[windowSize];

    this->size = windowSize;
    this->front = 0;
    this->rear = -1;
    this->numItems = 0;
}

CircularBuffer::~CircularBuffer()
{
    delete[] data;
    delete[] dataArr;
}

void CircularBuffer::enqueue(double el)
{
    this->rear = (this->rear + 1) % this->size;
    this->data[this->rear] = el;
    if (this->numItems < this->size)
      ++this->numItems;
    else
      this->front = (this->front + 1) % this->size;

    updateDataArr();
}

double CircularBuffer::dequeue()
{
  if (!this->numItems)
  {
    return 0;
  }

  double el = this->data[this->front];
  this->data[this->front] = 0;
  this->front = (this->front + 1) % this->size;
  --this->numItems;

  updateDataArr();

  return el;
}


void CircularBuffer::printBuffer()
{
  if (!this->numItems)
  {
    std::cout << "Buffer is Empty!\n";
    return;
  }

  int count = 0;
  int index;
  while (count < this->numItems)
  {
    index = (this->front + count) % this->size;
    std::cout << this->data[index] << " ";
    ++count;
  }
  std::cout << std::endl;
}

void CircularBuffer::updateDataArr()
{
  int index, count;
  for (count = 0; count < this->size; ++count)
  {
    index = (this->front + count) % this->size;
    dataArr[count] = this->data[index];
  }
}

bool CircularBuffer::isEmpty()
{
    return this->numItems == 0;
}
