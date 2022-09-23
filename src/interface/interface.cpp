#include "interface.h"

//Print CLI help text
void show_usage()
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