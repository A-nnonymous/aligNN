#include "test.h"

int test_with_sine(int frequency, int duration_as_seconds, int sample_rate_as_hertz,
                    int depth_factor, int precision_factor, 
                    int thread_number, std::string output_filename)
{
    double twopi = 2.0*PI;
    std::system("mkdir -p output");
    std:: string output_filepath = "./output" + output_filename;

    int signal_length = duration_as_seconds * sample_rate_as_hertz;
    int specturm_size = signal_length * depth_factor * precision_factor * 2;
    std::cout << "Working thread number: " << thread_number << std::endl;
    std::cout << "Output file: " << output_filename<< std::endl;
    std::cout << "Depth factor " << depth_factor << std::endl;
    std::cout << "Precision factor " << precision_factor << std::endl;
    std::cout << "This time uses " << signal_length << " signal points." << std::endl;


    float* signal_buffer = (float *)malloc(signal_length * sizeof(float));
    float* realtime_buffer = (float *)malloc(signal_length * sizeof(float));
    float* spectrum_buffer = (float *)malloc(specturm_size * sizeof(float));
    std::cout << "Memory allocation completed, start generating signal..."<<"\n";
     
    for (auto N = 0; N <= signal_length; N++)
    {
        signal_buffer[N] = sin(twopi * (
                                            N * frequency / (float)sample_rate_as_hertz
                                          ));
        realtime_buffer[N] = N * duration_as_seconds / (float)sample_rate_as_hertz;
    }
    std::cout << "Signal generation completed, start computing spectrum..."<<"\n";

    //show_usage();
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
    fcwt::cwt(&signal_buffer[0], signal_length, &spectrum_buffer[0],
        1, depth_factor, precision_factor, twopi,
        thread_number, false);

    std::cout << "Spectrum computation completed, start computing spectrum..."<<"\n";

    write_binary<float>(signal_buffer, signal_length, output_filepath + "_signal");
    free(signal_buffer);
    write_binary<float>(realtime_buffer, signal_length, output_filepath + "_realtime");
    free(realtime_buffer);
    write_binary<float>(spectrum_buffer, specturm_size, output_filepath + "_spectrum");
    free(spectrum_buffer);
    
    return COMPLETE;
}

void test_with_DNA()
{
    double twopi = 2.0*PI;
    int length  = 1000; 
    const int noctaves = 7;
    const int nsuboctaves = 40;
    int spectrum_size = length * noctaves * nsuboctaves;
    float *testdna = (float *)malloc(length * sizeof(float));
    float *result = (float*)malloc(length * noctaves * nsuboctaves * 2 * sizeof(float));
    read_binary<float>("/home/output/bitwise_processed/processed_chr1.fa.bin", testdna, length);
    fcwt::cwt(&testdna[0], length, &result[0], 1, noctaves, nsuboctaves, twopi, 8, false);    
    write_binary<float>(result, spectrum_size, "DNAspectrum");
    write_binary<float>(testdna, length, "DNAfloat");
}

void parse_whole_directory()
{
    bool complete_flag = false;
    const std::filesystem::path genome_folder = "../data/references/hs38DH/split";
    std::string output_folder = "../output/bitwise_processed/";
    std::string mkdir = "mkdir -p " + output_folder;
    std::system(mkdir.c_str());
    std::string tag = "processed_";

    for (auto const &chr_entry : std::filesystem::directory_iterator{genome_folder}) 
    {
        std::string raw_filename;
        std::string raw_filepath = chr_entry.path().string();
        const size_t last_slash_idx = raw_filepath.rfind("/");
        if (std::string::npos != last_slash_idx) 
        {
            raw_filename =
                tag + raw_filepath.substr(last_slash_idx + 1, std::string::npos);
        }
        std::string output_filepath = output_folder + raw_filename + ".bin";

        if (DNA2SIG<float>(raw_filepath, output_filepath, 0, true) == COMPLETE)
        {
        std::cout << "Formatting completed on file: " << raw_filepath
                    << std::endl;
        std::cout << "Output result to : " << output_filepath << std::endl;
        } else {
        std::cout << "Formatting failed on file: " << raw_filepath << std::endl;
        }
  }
}