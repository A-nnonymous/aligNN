#include <iostream>
#include <filesystem>
#include <fstream>
#include <bitset>
#include <stdlib.h>
#define COMPLETE true
#define FAILED false
using namespace std;

template<typename idxtype, typename dtype>
void buffered_output(dtype item, idxtype &buffer_idx, idxtype buffer_size, dtype *buffer, ostream &output)
{
    if(buffer_idx < buffer_size)
    {
        buffer[buffer_idx++] = item; 
    }
    else
    {
        output.write((char*)buffer, buffer_size * sizeof(dtype));
        buffer_idx = 0;
        buffer[buffer_idx++] = item; 
    }
}
template<typename dtype>
bool formatting(istream &input, ostream &output, int buffer_size, bool cleave_zero)
{ // (A)denine represented by   -1
  // (T)hymine represented by   1
  // (C)ytosine represented by  -2
  // (G)uanine represented by   2
    int buffer_idx = 0;
    dtype *buffer = (dtype *)malloc(buffer_size * sizeof(dtype));
    if(buffer != NULL)
    {
        std::cout << "Output buffer successfully allocated in size of :" << std::fixed << buffer_size * sizeof(dtype) / (double)(1024 * 1024) << "MB" << "\n";
    }
    else
    {
        std::cerr << "Buffer allocation failed, please chose another buffer size!" << "\n";
    }    
    char c;
    while(input.get(c))
    {
        // Load the buffer
        switch (c)
        {
        case 'A': 
            buffered_output<int,dtype>(-1, buffer_idx, buffer_size, buffer, output);
            break;
        case 'a': 
            buffered_output<int,dtype>(-1, buffer_idx, buffer_size, buffer, output);
            break;
        case 'T': 
            buffered_output<int,dtype>(1, buffer_idx, buffer_size, buffer, output);
            break;
        case 't': 
            buffered_output<int,dtype>(1, buffer_idx, buffer_size, buffer, output);
            break;
        case 'C': 
            buffered_output<int,dtype>(-2, buffer_idx, buffer_size, buffer, output);
            break;
        case 'c': 
            buffered_output<int,dtype>(-2, buffer_idx, buffer_size, buffer, output);
            break;
        case 'G': 
            buffered_output<int,dtype>(2, buffer_idx, buffer_size, buffer, output);
            break;
        case 'g': 
            buffered_output<int,dtype>(2, buffer_idx, buffer_size, buffer, output);
            break;
        default: // Intend to cut the hard mask because it provide no information.
            if(!cleave_zero)buffered_output<int,dtype>(0, buffer_idx, buffer_size, buffer, output);
            break;
        }
    }
    if(buffer_idx != 0) // Clean the remaining buffer if not empty.
    {
        output.write((char*)buffer, (buffer_idx) * sizeof(dtype));
    }
    free(buffer);
    return COMPLETE;
}

int main()
{
    bool complete_flag = false;
    string tag = "processed_";
    ifstream raw_record;
    const filesystem::path genome_folder = "../data/references/hs38DH/split";
    ofstream processed_record;
    string output_folder = "../output/bitwise_processed/";
    for(auto const& chr_entry: filesystem::directory_iterator{genome_folder})
    {
        string raw_filename;
        string raw_filepath = chr_entry.path().string();
        const size_t last_slash_idx = raw_filepath.rfind("/");
        if(string::npos != last_slash_idx)
        {
            raw_filename = tag + raw_filepath.substr(last_slash_idx + 1, string::npos);
        }
        string output_filepath = output_folder + raw_filename + ".bin";
        raw_record.open(raw_filepath);
        processed_record.open(output_filepath, ios::out);
        
        if(formatting<float>(raw_record, processed_record, 300 * 1024 * 1024, true) == COMPLETE)
        {
            cout<< "Formatting completed on file: "<< raw_filepath <<endl;
            cout<< "Output result to : "<< output_filepath <<endl;
        }
        else
        {
            cout<< "Formatting failed on file: "<< raw_filepath <<endl;
        }
        raw_record.close();
        processed_record.close();

    }
    return 0;
}