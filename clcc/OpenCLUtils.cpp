#include "OpenCLUtils.h"

#include <string>

OpenCLUtils::OpenCLUtils()
{
}


OpenCLUtils::~OpenCLUtils()
{
}


cl_platform_id selectPlatform(cl_uint selected_platform_index)
{
	using namespace std;

	cl_uint num_of_platforms = 0;
	// get total number of available platforms:
	cl_int err = clGetPlatformIDs(0, 0, &num_of_platforms);
	SAMPLE_CHECK_ERRORS(err);

	// use vector for automatic memory management
	vector<cl_platform_id> platforms(num_of_platforms);
	// get IDs for all platforms:
	err = clGetPlatformIDs(num_of_platforms, &platforms[0], 0);
	SAMPLE_CHECK_ERRORS(err);

	
	bool by_index = true;

	
	cout << "Platforms (" << num_of_platforms << "):\n";

	// TODO In case of empty platform name select the default platform or 0th platform?

	for (cl_uint i = 0; i < num_of_platforms; ++i)
	{
		// Get the length for the i-th platform name
		size_t platform_name_length = 0;
		err = clGetPlatformInfo(
			platforms[i],
			CL_PLATFORM_NAME,
			0,
			0,
			&platform_name_length
		);
		SAMPLE_CHECK_ERRORS(err);

		// Get the name itself for the i-th platform
		// use vector for automatic memory management
		vector<char> platform_name(platform_name_length);
		err = clGetPlatformInfo(
			platforms[i],
			CL_PLATFORM_NAME,
			platform_name_length,
			&platform_name[0],
			0
		);
		SAMPLE_CHECK_ERRORS(err);

		cout << "    [" << i << "] " << &platform_name[0];

		// decide if this i-th platform is what we are looking for
		// we select the first one matched skipping the next one if any
		//
		if (
			selected_platform_index == i || // we already selected the platform by index
			//string(&platform_name[0]).find(required_platform_subname) != string::npos &&
			selected_platform_index == num_of_platforms // haven't selected yet
			)
		{
			cout << " [Selected]";
			selected_platform_index = i;
			// do not stop here, just want to see all available platforms
		}

		// TODO Something when more than one platform matches a given subname

		cout << endl;
	}

	if (by_index && selected_platform_index >= num_of_platforms)
	{
		throw Error(
			"Given index of platform (" + to_string(selected_platform_index) + ") "
			"is out of range of available platforms"
		);
	}

	/*if (!by_index && selected_platform_index >= num_of_platforms)
	{
		throw Error(
			"There is no found platform with name containing \"" +
			required_platform_subname + "\" as a substring\n"
		);
	}*/

	return platforms[selected_platform_index];
}

string opencl_error_to_str(cl_int error)
{
#define CASE_CL_CONSTANT(NAME) case NAME: return #NAME;

	// Suppose that no combinations are possible.
	// TODO: Test whether all error codes are listed here
	switch (error)
	{
		CASE_CL_CONSTANT(CL_SUCCESS)
			CASE_CL_CONSTANT(CL_DEVICE_NOT_FOUND)
			CASE_CL_CONSTANT(CL_DEVICE_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_COMPILER_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_MEM_OBJECT_ALLOCATION_FAILURE)
			CASE_CL_CONSTANT(CL_OUT_OF_RESOURCES)
			CASE_CL_CONSTANT(CL_OUT_OF_HOST_MEMORY)
			CASE_CL_CONSTANT(CL_PROFILING_INFO_NOT_AVAILABLE)
			CASE_CL_CONSTANT(CL_MEM_COPY_OVERLAP)
			CASE_CL_CONSTANT(CL_IMAGE_FORMAT_MISMATCH)
			CASE_CL_CONSTANT(CL_IMAGE_FORMAT_NOT_SUPPORTED)
			CASE_CL_CONSTANT(CL_BUILD_PROGRAM_FAILURE)
			CASE_CL_CONSTANT(CL_MAP_FAILURE)
			CASE_CL_CONSTANT(CL_MISALIGNED_SUB_BUFFER_OFFSET)
			CASE_CL_CONSTANT(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST)
			CASE_CL_CONSTANT(CL_INVALID_VALUE)
			CASE_CL_CONSTANT(CL_INVALID_DEVICE_TYPE)
			CASE_CL_CONSTANT(CL_INVALID_PLATFORM)
			CASE_CL_CONSTANT(CL_INVALID_DEVICE)
			CASE_CL_CONSTANT(CL_INVALID_CONTEXT)
			CASE_CL_CONSTANT(CL_INVALID_QUEUE_PROPERTIES)
			CASE_CL_CONSTANT(CL_INVALID_COMMAND_QUEUE)
			CASE_CL_CONSTANT(CL_INVALID_HOST_PTR)
			CASE_CL_CONSTANT(CL_INVALID_MEM_OBJECT)
			CASE_CL_CONSTANT(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
			CASE_CL_CONSTANT(CL_INVALID_IMAGE_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_SAMPLER)
			CASE_CL_CONSTANT(CL_INVALID_BINARY)
			CASE_CL_CONSTANT(CL_INVALID_BUILD_OPTIONS)
			CASE_CL_CONSTANT(CL_INVALID_PROGRAM)
			CASE_CL_CONSTANT(CL_INVALID_PROGRAM_EXECUTABLE)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_NAME)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_DEFINITION)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL)
			CASE_CL_CONSTANT(CL_INVALID_ARG_INDEX)
			CASE_CL_CONSTANT(CL_INVALID_ARG_VALUE)
			CASE_CL_CONSTANT(CL_INVALID_ARG_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_KERNEL_ARGS)
			CASE_CL_CONSTANT(CL_INVALID_WORK_DIMENSION)
			CASE_CL_CONSTANT(CL_INVALID_WORK_GROUP_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_WORK_ITEM_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_GLOBAL_OFFSET)
			CASE_CL_CONSTANT(CL_INVALID_EVENT_WAIT_LIST)
			CASE_CL_CONSTANT(CL_INVALID_EVENT)
			CASE_CL_CONSTANT(CL_INVALID_OPERATION)
			CASE_CL_CONSTANT(CL_INVALID_GL_OBJECT)
			CASE_CL_CONSTANT(CL_INVALID_BUFFER_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_MIP_LEVEL)
			CASE_CL_CONSTANT(CL_INVALID_GLOBAL_WORK_SIZE)
			CASE_CL_CONSTANT(CL_INVALID_PROPERTY)
//			CASE_CL_CONSTANT(CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR)
	default:
		return "UNKNOWN ERROR CODE " + error;
	}

#undef CASE_CL_CONSTANT
}



cl_device_id selectDevice(cl_platform_id platform, cl_uint selected_device_index)
{
	using namespace std;

	// List devices of a given type only
	cl_device_type device_type = CL_DEVICE_TYPE_ALL; //  = parseDeviceType(device_type_name);

	cl_uint num_of_devices = 0;
	cl_int err = clGetDeviceIDs(
		platform,
		device_type,
		0,
		0,
		&num_of_devices
	);

	SAMPLE_CHECK_ERRORS(err);

	vector<cl_device_id> devices(num_of_devices);

	cl_device_id selectedDeviceId = 0;

	err = clGetDeviceIDs(
		platform,
		device_type,
		num_of_devices,
		&devices[0],
		0
	);
	SAMPLE_CHECK_ERRORS(err);

	cout << "Devices (" << num_of_devices;
	
	cout << "):\n";

	for (cl_uint i = 0; i < num_of_devices; ++i)
	{
		// Get the length for the i-th device name
		size_t device_name_length = 0;
		err = clGetDeviceInfo(
			devices[i],
			CL_DEVICE_NAME,
			0,
			0,
			&device_name_length
		);
		SAMPLE_CHECK_ERRORS(err);

		// Get the name itself for the i-th device
		// use vector for automatic memory management
		vector<char> device_name(device_name_length);
		err = clGetDeviceInfo(
			devices[i],
			CL_DEVICE_NAME,
			device_name_length,
			&device_name[0],
			0
		);
		SAMPLE_CHECK_ERRORS(err);

		cout << "    [" << i << "] " << &device_name[0];
		
		if (i == selected_device_index)
		{
			cout << " [Selected]";
			selectedDeviceId = devices[i];
		}

		cout << '\n';
	}

	return selectedDeviceId;
}

void pfn_notify(const char *errinfo, const void *private_info, size_t cb, void *user_data)
{
	fprintf(stderr, "[pfn_notify] OpenCL Error : %s\n", errinfo);
        fflush(stderr);
}


string getPlatformName(cl_platform_id platform_id)
{
    // Get the length for the i-th platform name
    size_t platform_name_length = 0;
    cl_int      err = clGetPlatformInfo(
            platform_id,
            CL_PLATFORM_NAME,
            0,
            0,
            &platform_name_length
    );
    SAMPLE_CHECK_ERRORS(err);

    // Get the name itself for the i-th platform
    // use vector for automatic memory management
    vector<char> platform_name(platform_name_length);
    err = clGetPlatformInfo(
            platform_id,
            CL_PLATFORM_NAME,
            platform_name_length,
            &platform_name[0],
            0
    );
    SAMPLE_CHECK_ERRORS(err);

    string str =  &platform_name[0];
                
    return str;
}

string getDeviceName(cl_device_id device_id)
{
		size_t device_name_length = 0;
		cl_int err = clGetDeviceInfo(
			device_id,
			CL_DEVICE_NAME,
			0,
			0,
			&device_name_length
		);
		SAMPLE_CHECK_ERRORS(err);

		// Get the name itself for the i-th device
		// use vector for automatic memory management
		vector<char> device_name(device_name_length);
		err = clGetDeviceInfo(
			device_id,
			CL_DEVICE_NAME,
			device_name_length,
			&device_name[0],
			0
		);
		SAMPLE_CHECK_ERRORS(err);

		string str =  &device_name[0];
                
                return str;
    
}





cl_context createContext(cl_platform_id platform, cl_device_id device/*const cl_context_properties* additional_context_props*/)
{
	using namespace std;

	if (!platform)
	{
		throw Error("Platform is not selected");
	}

	if (!device)
	{
		throw Error("Device is not selected");
	}

	size_t number_of_additional_props = 0;
	/*if (additional_context_props)
	{
		// count all additional props including terminating 0
		while (additional_context_props[number_of_additional_props++]);
		number_of_additional_props--;   // now exclude terminating 0
	}*/

	// allocate enough space for platform and all additional props if any
	std::vector<cl_context_properties> context_props(
		2 + // for CL_CONTEXT_PLATFORM and platform itself
		number_of_additional_props +
		1   // for terminating zero
	);

	context_props[0] = CL_CONTEXT_PLATFORM;
	context_props[1] = cl_context_properties(platform);
	/*
	std::copy(
		additional_context_props,
		additional_context_props + number_of_additional_props,
		context_props.begin() + 2   // +2 -- skipping already initialized platform entries
	);*/

	context_props.back() = 0;

	cl_int err = 0;
	cl_context context = clCreateContext(&context_props[0], 1, &device, pfn_notify, 0, &err);
	SAMPLE_CHECK_ERRORS(err);

	return context;
}

cl_command_queue createQueue(cl_device_id device, cl_context context, cl_command_queue_properties queue_properties)
{
	using namespace std;

	if (!device)
	{
		throw Error("Device is not selected");
	}

	cl_int err = 0;
	cl_command_queue queue = clCreateCommandQueue(context, device, queue_properties, &err);
	SAMPLE_CHECK_ERRORS(err);

	return queue;
}