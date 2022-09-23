#pragma once
#include <stdlib.h>
#include <iostream>
#include "../main.h"
#include <filesystem>

int test_with_sine(int frequency, int duration_as_seconds, int sample_rate_as_hertz,
                    int depth_factor, int precision_factor, 
                    int thread_number, std::string output_filename);
void test_with_DNA();
void parse_whole_directory();