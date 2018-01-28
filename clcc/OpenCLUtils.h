/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OpenCLUtils.h
 * Author: dcr
 *
 * Created on January 27, 2018, 10:35 AM
 */

#ifndef OPENCLUTILS_H
#define OPENCLUTILS_H


#include <iostream>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <CL/cl.h>

using namespace std;


// Report about an OpenCL problem.
// Macro is used instead of a function here
// to report source file name and line number.
//#define SAMPLE_CHECK_ERRORS(ERR)                        \
//    if(ERR != CL_SUCCESS)                               \
//    {                                                   \
//        throw Error(                                    \
//            string("OpenCL error ") +                           \
//            string(opencl_error_to_str(ERR)) +                  \
//            string(" happened in file ") +		\
//			string((const char*)(__FILE__)) +   \
//            string(" at line ") + \
//			string((const char*)(__LINE__)) +\
//			string(".")        \
//        );                                              \
//    }

#define SAMPLE_CHECK_ERRORS(ERR)                        \
    if(ERR != CL_SUCCESS)                               \
    {                                                   \
        throw Error(                                    \
            string("OpenCL error ") +                           \
            string(opencl_error_to_str(ERR))                   \
			+ string(" happened in file ") \
			+ string((const char*)(__FILE__))    \
			+ string(" at line ") + to_string(__LINE__)  \
        );                                              \
    }

string opencl_error_to_str(cl_int error);
cl_platform_id selectPlatform(cl_uint selected_platform_index);
cl_device_id selectDevice(cl_platform_id platform, cl_uint selected_device_index);
string getDeviceName(cl_device_id device_id);
string getPlatformName(cl_platform_id platform_id);
cl_context createContext(cl_platform_id platform, cl_device_id device/*const cl_context_properties* additional_context_props*/);
cl_command_queue createQueue(cl_device_id device, cl_context context, cl_command_queue_properties queue_properties);


class OpenCLUtils
{
public:
	OpenCLUtils();
	~OpenCLUtils();

public:
	
};

// Base class for all exception in samples
class Error : public std::runtime_error
{
public:
	Error(const string& msg) :
		std::runtime_error(msg)
	{
	}
};

#endif /* OPENCLUTILS_H */

