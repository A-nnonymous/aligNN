#include <iostream>
#include <vector>
#include <math.h>
#include <fcwt.h>
#include <fstream>
#include <stdlib.h>
#include <sstream>

double rdtsc()
{
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;    
}

template<typename dtype>
bool write_binary(dtype *data, int row, int column, std::string filename, int buffer_strategy)
{
    std::cout.precision(2);
    std::stringstream sstream;
    std::ofstream output;
    std::string suffix;
    sstream << typeid(dtype).name(); sstream >> suffix; suffix = "." + suffix;
    std::string fullpath = filename + suffix;
    
    std::cout << "=======================" << "\n";
    output.open(fullpath, std::ios::out |std::ios::binary);
    std::cout << "Output file stream for " << fullpath << " is successfully opened. " << "\n";
    std::cout << "Estimated storage usage: " << std::fixed << row * column * sizeof(dtype) / (double)(1024 * 1024) << "MB" << "\n";

    // Some useless hard-work using buffer.
    /*
    int buffer_idx = 0;
    int buffer_size = (buffer_strategy? buffer_strategy : (row * column));  // If chose strategy 0, output with full buffer.
    dtype *buffer = (dtype*)malloc(buffer_size * sizeof(dtype));
    if(buffer != NULL)
    {
        std::cout << "Output buffer successfully allocated in size of :" << std::fixed << buffer_size * sizeof(dtype) / (double)(1024 * 1024) << "MB" << "\n";
    }
    else
    {
        std::cerr << "Buffer allocation failed, please chose another buffer size!" << "\n";
    }
    */
    double pstart = rdtsc();
    sleep(1);
    double pclocks = rdtsc() - pstart;
    double freq = pclocks/1000000000;
    std::cout << "Current cpu frequency = " << freq << "Ghz" << "\n";
    std::cout << "Start to output......" << "\n";
    double tick = rdtsc();
    // Some useless hard-work using buffer.
    /*for(int j = 0; j < row; j++)
    {
        for (int i = 0; i < column; i++)
        {
            if(buffer_idx < buffer_size)
            {
                buffer[buffer_idx++] = data[j * column +i]; 
            }
            else
            {
                output.write((char*)buffer, buffer_size * sizeof(dtype));
                buffer_idx = 0;
                buffer[buffer_idx++] = data[j * column +i]; 
            }
        }
    }
    if(buffer_idx != 0) // Clean the remaining buffer if not empty.
    {
        output.write((char*)buffer, (buffer_idx) * sizeof(dtype));
    }
    free(buffer);
    */
    output.write((char*)data, row * column * sizeof(dtype));
    output.close();
    double realtime = (rdtsc() - tick)/pclocks;
    std::cout << "Output for " << fullpath << " is completed. " << "\n";
    std::cout << "Time spend: " << realtime << "seconds" << "\n";
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
int main(int argc, const char * argv[]) {
    /*
    int n = 248956422; //signal length
    double fs = 10000; //sampling frequency
    double twopi = 2.0*PI;
    
    //3000 scales spread over 5 octaves
    const int noctaves = 8;
    const int nsuboctaves = 20;

    //input: n real numbers
    float *sig = (float*)malloc(n * sizeof(float));
    
    //output: n x scales complex numbers
    float *tfm = (float*)malloc(n*noctaves*nsuboctaves*2 * sizeof(float));
    std::cout << "Memory allocation completed, start generating signal..."<<"\n";
    //initialize with 20 Hz cosine wave
    for(auto index = 0; index < n; index++) {
        sig[index] = sin((double)(2500 *twopi * index / fs));
    }

    std::cout << "Signal generation completed, start computing spectrum..."<<"\n";
    
    fcwt::cwt(&sig[0], n, &tfm[0], 1, noctaves, nsuboctaves, twopi, 8, false);
    std::cout << "Spectrum computation completed, start computing spectrum..."<<"\n";
    write_binary<float>(tfm, noctaves*nsuboctaves*2, n, "official", 0);
    write_binary<float>(sig, 1, n, "official_original",0);
    */
    double twopi = 2.0*PI;
    int length  = 1000; 
    const int noctaves = 7;
    const int nsuboctaves = 40;
    float *testdna = (float *)malloc(length * sizeof(float));
    float *result = (float*)malloc(length * noctaves * nsuboctaves * 2 * sizeof(float));
    read_binary<float>("/home/output/bitwise_processed/processed_chr1.fa.bin", testdna, length);
    fcwt::cwt(&testdna[0], length, &result[0], 1, noctaves, nsuboctaves, twopi, 8, false);    
    write_binary<float>(result, noctaves * nsuboctaves * 2, length, "DNAspectrum", 0);

    write_binary<float>(testdna, 1, length, "DNAfloat", 0);
    return 0;
}