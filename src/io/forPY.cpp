#include <iostream>
#include <vector>
#include <math.h>
#include <fcwt.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>


//Print CLI help text
static void show_usage(std::string name)
{
    std::cerr 
    << "Usage: fcwt <Number_of_threads> <Input_file> <Output_file(csv format)>  <Depth_factor>  <Precision_factor>\n"
    << "        Number_of_threads: Working threads on this job.\n"
    << "        Input_file: Text file of genome with NO HEADER.\n"
    << "        Output_file: Wavelet transform of input, formatted in CSV(Seperated in comma and \\n) \n"
    << "        Depth_factor: Argument for CWT, define the absolute depth of spectrum. \n"
    << "        Precision_factor: Argument for CWT, define the resolution of specturm. \n"
              << std::endl;
}
double rdtsc()
{
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;    
}

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

template<typename Buffer>
bool debug_dna_parsing(std::string target, uint64_t length, Buffer *outside_buffer)
{
    char c;
    std::ifstream raw_bpfile;
    raw_bpfile.open(target, std::ios::in);
    Buffer this_element;
    for(auto count = 0; count < length; count++)
    {
        raw_bpfile.get(c);
        switch (c)
        {
        case 'A':
            this_element = -1;
            break;
        case 'T':
            this_element = -0.5;
            break;
        case 'G':
            this_element = 0.5;
            break;
        case 'C':
            this_element = 1;
            break;
        
        default:
            this_element = 0;
            break;
        }
        outside_buffer[count] = this_element;
    }
    return true;
}

int main(int argc, const char * argv[]) {
    
    float fs = 1000; //sampling frequency
    float twopi = 2.0*3.1415;
    //Initialize variables with default values
    int size = 1000;
    //If user requests help
    if ((argv[1] == "-h") || (argv[1] == "--help") || argc < 3) {
        show_usage(argv[0]);
        return 0;
    }
    

    
    //Check algorithm, number of samples and number of threads
    int thread_number = std::stoi(argv[1]);
    std::string input_filepath = argv[2];
    std::string output_filepath = argv[3];
    int depth_factor = std::stoi(argv[4]);
    int precision_factor = std::stoi(argv[5]);

    uint64_t debug_temp_length = std::stoi(argv[6]);
    uint64_t specturm_size = debug_temp_length * depth_factor * precision_factor * 2;
    // Argument check.
    std::cout << "Working thread number: " << thread_number << std::endl;
    std::cout << "Input file: " << input_filepath << std::endl;
    std::cout << "Output file: " << output_filepath << std::endl;
    std::cout << "Depth factor " << depth_factor << std::endl;
    std::cout << "Precision factor " << precision_factor << std::endl;
    std::cout << "This time uses " << debug_temp_length << " signal points." << std::endl;

    


    //3000 scales spread over 5 octaves
    //const int noctaves = 5;
    //const int nsuboctaves = 600;

    //input: n real numbers
    //std::vector<float> sig(n);
    float* signal_buffer = (float*)malloc(debug_temp_length * sizeof(float));
    
    //output: n x scales complex numbers
    //std::vector<float> tfm(n*noctaves*nsuboctaves*2);
    float* specturm_buffer = (float*)malloc(specturm_size * sizeof(float));
    //initialize with 1 Hz cosine wave
     
    //#pragma GCC unroll 1000
    for (auto i = 0; i <= debug_temp_length; i++)
    {
        signal_buffer[i] = sin(twopi*((float)(i/fs)));
    }
    /*
    if(debug_dna_parsing<float>(input_filepath, debug_temp_length, signal_buffer) == true)
    {
        std::cout << "DNA parsing completed." << std::endl;
    }
    else
    {

        std::cout << "DNA parsing failed." << std::endl;
    }*/
    
    
    //Arguments:
    //input     - floating pointer to input array
    //length    - integer signal length
    //output    - floating pointer to output array
    //startoct  - scale range begin (2^startoct)
    //endoct    - scale range end (2^endoct)
    //suboct    - exponential subdivisions of each octave
    //sigma     - parameter to control time-frequency precision
    //nthreads  - number of threads to use
    //optplans  - use FFTW optimization plans if true
    double pstart = rdtsc();
    sleep(1);
    double pclocks = rdtsc()-pstart;
    double freq = pclocks/1000000000;
    std::cout<< "Current cpu frequency = "<< freq <<"Ghz"<<std::endl;
    double tick = rdtsc();
    fcwt::cwt(&signal_buffer[0], debug_temp_length, &specturm_buffer[0], 1, depth_factor, precision_factor, twopi, thread_number, false);

    double realtime = (rdtsc() - tick)/pclocks;
    std::cout << "output file size estimated to be:" << specturm_size * sizeof(float) /1000000 << "MB"<< std::endl;
    write_csv<float>(specturm_buffer, depth_factor * precision_factor * 2, debug_temp_length, output_filepath);
    write_csv<float>(signal_buffer, 1, debug_temp_length, "original_signal");


    std::cout<< "This work spend: " <<realtime<<"seconds"<<std::endl;

    std::cout << "Average " << debug_temp_length / realtime << " signal points processed per second." << std::endl;
    std::cout << "If spliced in " << debug_temp_length << " base pair per instance, " << (3000000000/debug_temp_length) * realtime /60 << "minutes is needed to traverse." << std::endl;
    
    return 0;
}