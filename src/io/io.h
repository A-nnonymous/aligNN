#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#define COMPLETE true
#define FAILED false

template<typename T>
bool write_csv(T *data, int row, int column, std::string filepath)
{
    std::ofstream output;
    output.open(filepath + ".csv", std::ios::out);
    std::cout << "Output file stream opening success. " << std::endl;
    for(int j = 0; j < row; j++)
    {
        for (int i = 0; i < column; i++)
        {
            output << data[j * row + i] << ",";
        }
        output << "\n";
    }
    output.close();
    return true;
    
}

template<typename dtype>
bool write_binary(dtype *data, int length, std::string filepath)
{
    std::cout.precision(2);
    std::stringstream sstream;
    std::ofstream output;
    std::string suffix;
    sstream << typeid(dtype).name(); sstream >> suffix; suffix = "." + suffix;
    std::string fullpath = filepath + suffix;
    
    std::cout << "=======================" << "\n";
    output.open(fullpath, std::ios::out |std::ios::binary);
    std::cout << "Output file stream for " << fullpath << " is successfully opened. " << "\n";
    std::cout << "Estimated storage usage: " << std::fixed << length * sizeof(dtype) / (double)(1024 * 1024) << "MB" << "\n";

    std::cout << "Start to output......" << "\n";
    output.write((char*)data, length * sizeof(dtype));
    output.close();
    std::cout << "Output for " << fullpath << " is completed. " << "\n";
    std::cout << "\n";
    return true;
}

/*
    "length" represent the amount of data instances in the size of known dtype,
    if set to zero, then allocate full length of memory space and load it all to memory.
*/
template<typename dtype>
void read_binary(std::string filepath, dtype* outside_array_pointer, size_t length)
{
    if(length == 0)
    {
        std::ifstream fastseek(filepath, std::ios::binary | std::ios::ate);
        length = fastseek.tellg();
        fastseek.close();
        std::cout << "Chosed to load whole file, costs " << length << " bytes of memory.\n";
    }
    std::ifstream bin;
    bin.open(filepath, std::ios::in | std::ios::binary);
    
    bin.read((char *)outside_array_pointer, length * sizeof(dtype));

}

template <typename idxtype, typename dtype>
void buffered_output(dtype item, idxtype &buffer_idx, idxtype buffer_size,
                     dtype *buffer, std::ostream &output) {
  if (buffer_idx < buffer_size) {
    buffer[buffer_idx++] = item;
  } else {
    output.write((char *)buffer, buffer_size * sizeof(dtype));
    buffer_idx = 0;
    buffer[buffer_idx++] = item;
  }
}

template <typename dtype>
bool DNA2SIG(std::string raw_inputpath, std::string raw_outputpath, int buffer_size, bool cleave_zero) 
{ // (A)denine represented by    -1
  // (T)hymine represented by     1
  // (C)ytosine represented by   -2
  // (G)uanine represented by     2
  std::ifstream input(raw_inputpath);
  std::ofstream output(raw_outputpath);
  int buffer_idx = 0;
  if(buffer_size == 0)
  {
      std::ifstream fastseek(raw_inputpath, std::ios::binary | std::ios::ate);
      buffer_size = fastseek.tellg();
      fastseek.close();
      std::cout << "Chosed to load whole file, costs " << buffer_size << " bytes of memory.\n";
  }
  dtype *buffer = (dtype *)malloc(buffer_size * sizeof(dtype));
  if (buffer != NULL) {
    std::cout << "Output buffer successfully allocated in size of :"
              << buffer_size * sizeof(dtype) / (double)(1024 * 1024) << "MB"
              << "\n";
  } else {
    std::cerr << "Buffer allocation failed, please chose another buffer size!"
              << "\n";
  }

  char c;
  while (input.get(c)) {
    // Load the buffer
    switch (c) {
    case 'A':
      buffered_output<int, dtype>(-1, buffer_idx, buffer_size, buffer, output);
      break;
    case 'a':
      buffered_output<int, dtype>(-1, buffer_idx, buffer_size, buffer, output);
      break;
    case 'T':
      buffered_output<int, dtype>(1, buffer_idx, buffer_size, buffer, output);
      break;
    case 't':
      buffered_output<int, dtype>(1, buffer_idx, buffer_size, buffer, output);
      break;
    case 'C':
      buffered_output<int, dtype>(-2, buffer_idx, buffer_size, buffer, output);
      break;
    case 'c':
      buffered_output<int, dtype>(-2, buffer_idx, buffer_size, buffer, output);
      break;
    case 'G':
      buffered_output<int, dtype>(2, buffer_idx, buffer_size, buffer, output);
      break;
    case 'g':
      buffered_output<int, dtype>(2, buffer_idx, buffer_size, buffer, output);
      break;
    default: // Intend to cut the hard mask because it provide no information.
      if (!cleave_zero)
        buffered_output<int, dtype>(0, buffer_idx, buffer_size, buffer, output);
      break;
    }
  }
  if (buffer_idx != 0) // Clean the remaining buffer if not empty.
  {
    output.write((char *)buffer, (buffer_idx) * sizeof(dtype));
  }
  free(buffer);
  return COMPLETE;
}