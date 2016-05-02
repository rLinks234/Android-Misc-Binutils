#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"

#include "extdump.h"

////////////////////////////////

void _cl_asciiPrintDevice(CLDeviceInfo* pDevice, uint32_t pIndex, FILE* pOutput) {

	fprintf(pOutput, "\n\tDevice[%u]\n\t", pIndex);
	fprintf(pOutput, "Name: %s\n\t", pDevice->name);
	fprintf(pOutput, "Vendor: %s\n\t", pDevice->vendor);
	fprintf(pOutput, "Vendor ID: %u\n\t", pDevice->vendorID);
	fprintf(pOutput, "Version: %s\n\t", pDevice->version);
	fprintf(pOutput, "Profile: %s\n\t", pDevice->profile);
	fprintf(pOutput, "C Compile Version: %s\n\t", pDevice->cVersion);
	fprintf(pOutput, "Driver Version: %s\n\t", pDevice->driverVersion);
	fprintf(pOutput, "Extensions: %s\n\t", pDevice->extensions);
	fprintf(pOutput, "Image Support: %u\n\t", pDevice->imageSupport);
	fprintf(pOutput, "Max Compute Units: %u\n\t", pDevice->maxComputeUnits);
	fprintf(pOutput, "Max Constant Args: %u\n\t", pDevice->maxConstantArgs);
	fprintf(pOutput, "Max Work Item Dimensions: %u\n\t", pDevice->maxWorkItemDimensions);
	fprintf(pOutput, "Max Work Group Size: %u\n\t", pDevice->maxWorkGroupSize);
	//fprintf(pOutput, "\n\t", pDevice->maxWorkItemSizes); // How shouild we print this?
	fprintf(pOutput, "Local Memory Size: %llu\n\t", pDevice->localMemSize);
	fprintf(pOutput, "Global Memory Size: %llu\n\t", pDevice->globalMemSize);
	fprintf(pOutput, "Max Memory Allocation Size: %llu\n", pDevice->maxMemAllocSize);

}

void _cl_asciiPrintPlatform(CLPlatformInfo* pPlatform, uint32_t pIndex, FILE* pOutput) {

	fprintf(pOutput, "\nPlatform[%u] (%u devices)\n", pIndex, pPlatform->deviceCount);
	fprintf(pOutput, "Profile: %s\n", pPlatform->profile);
	fprintf(pOutput, "Version: %s\n", pPlatform->version);
	fprintf(pOutput, "Name: %s\n", pPlatform->name);
	fprintf(pOutput, "Vendor: %s\n", pPlatform->vendor);
	fprintf(pOutput, "Extensions: %s\n", pPlatform->extensions);

	for (uint32_t i = 0; i < pPlatform->deviceCount; i++) {
		_cl_asciiPrintDevice( &(pPlatform->devices[i]), i, pOutput );
	}

}

void _cl_asciiPrintInfo(CLPlatformInfo* pPlatforms, uint32_t pPlatformCount, FILE* pOutput) {

	fprintf(pOutput, "\n%u OpenCL Platform(s) found.\n", pPlatformCount);

	for (uint32_t i = 0; i < pPlatformCount; i++) {
		_cl_asciiPrintPlatform(&(pPlatforms[i]), i, pOutput);
	}

}

////////////////////////////////

void _clPopulate(cl_device_id pDevice, CLDeviceInfo* pDeviceInfo, void* pInfoBuffer, uint32_t pInfoBufferCapacity) {

	uint32_t infoInt = 0;
	uint64_t infoLong = 0L;
	char* infoStr;
	size_t infoLength = 0;

	clGetDeviceInfo(pDevice, CL_DEVICE_NAME, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->name = infoStr;

	clGetDeviceInfo(pDevice, CL_DEVICE_VENDOR, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->vendor = infoStr;

	clGetDeviceInfo(pDevice, CL_DEVICE_VENDOR_ID, 4, &infoInt, &infoLength);
	pDeviceInfo->vendorID = infoInt;

	clGetDeviceInfo(pDevice, CL_DEVICE_VERSION, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->version = infoStr;

	clGetDeviceInfo(pDevice, CL_DEVICE_PROFILE, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->profile = infoStr;

	clGetDeviceInfo(pDevice, CL_DEVICE_OPENCL_C_VERSION, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->cVersion = infoStr;

	clGetDeviceInfo(pDevice, CL_DRIVER_VERSION, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->driverVersion = infoStr;

	clGetDeviceInfo(pDevice, CL_DEVICE_EXTENSIONS, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	infoStr = new char[infoLength + 1];
	infoStr[infoLength] = '\0';
	memcpy(infoStr, pInfoBuffer, infoLength);
	pDeviceInfo->extensions = infoStr;

	clGetDeviceInfo(pDevice, CL_DEVICE_IMAGE_SUPPORT, 4, &infoInt, &infoLength);
	pDeviceInfo->imageSupport = infoInt;

	clGetDeviceInfo(pDevice, CL_DEVICE_MAX_COMPUTE_UNITS, 4, &infoInt, &infoLength);
	pDeviceInfo->maxComputeUnits = infoInt;

	clGetDeviceInfo(pDevice, CL_DEVICE_MAX_CONSTANT_ARGS, 4, &infoInt, &infoLength);
	pDeviceInfo->maxConstantArgs = infoInt;

	clGetDeviceInfo(pDevice, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 4, &infoInt, &infoLength);
	pDeviceInfo->maxWorkItemDimensions = infoInt;

	clGetDeviceInfo(pDevice, CL_DEVICE_MAX_WORK_GROUP_SIZE, 4, &infoInt, &infoLength);
	pDeviceInfo->maxWorkGroupSize = infoInt;

	clGetDeviceInfo(pDevice, CL_DEVICE_MAX_WORK_ITEM_SIZES, pInfoBufferCapacity, pInfoBuffer, &infoLength);
	uint32_t* sz = new uint32_t[infoLength / 4];
	memcpy(sz, pInfoBuffer, infoLength);
	pDeviceInfo->maxWorkItemSizes = sz;

	clGetDeviceInfo(pDevice, CL_DEVICE_LOCAL_MEM_SIZE, 8, &infoLong, &infoLength);
	pDeviceInfo->localMemSize = infoLong;

	clGetDeviceInfo(pDevice, CL_DEVICE_GLOBAL_MEM_SIZE, 8, &infoLong, &infoLength);
	pDeviceInfo->globalMemSize = infoLong;

	clGetDeviceInfo(pDevice, CL_DEVICE_MAX_MEM_ALLOC_SIZE, 8, &infoLong, &infoLength);
	pDeviceInfo->maxMemAllocSize = infoLong;

}

void _process_CL(ExtDumpInstance* pInstance, void* pInfoBuffer, uint32_t pInfoBufferCapacity) {

	if (!pInstance->clHandle) {
		// No OpenCL found on this device
		return;

	} else {

		clGetPlatformIDs = reinterpret_cast<_CL_GetPlatformIDs_t>(dlsym(pInstance->clHandle, "clGetPlatformIDs"));
		clGetDeviceIDs = reinterpret_cast<_CL_GetDeviceIDs_t>(dlsym(pInstance->clHandle, "clGetDeviceIDs"));
		clGetPlatformInfo = reinterpret_cast<_CL_GetPlatformInfo_t>(dlsym(pInstance->clHandle, "clGetPlatformInfo"));
		clGetDeviceInfo = reinterpret_cast<_CL_GetDeviceInfo_t>(dlsym(pInstance->clHandle, "clGetDeviceInfo"));

	}

	// Used below to get size of output
	size_t infoLength = 0;

	uint32_t numberOfPlatforms = 0;
	clGetPlatformIDs(0, nullptr, &numberOfPlatforms);
	cl_platform_id* platforms = new cl_platform_id[numberOfPlatforms];
	clGetPlatformIDs(numberOfPlatforms, platforms, nullptr);

	CLPlatformInfo* platformsInfo = new CLPlatformInfo[numberOfPlatforms];

	for (uint32_t i = 0; i < numberOfPlatforms; i++) {

		cl_platform_id currentPlatform = platforms[i];
		CLPlatformInfo* currentPlatformInfo = &(platformsInfo[i]);

		uint32_t numberOfDevices = 0;
		clGetDeviceIDs(currentPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numberOfDevices);
		cl_device_id* devices = new cl_device_id[numberOfDevices];
		clGetDeviceIDs(currentPlatform, CL_DEVICE_TYPE_ALL, numberOfDevices, devices, nullptr);

		CLDeviceInfo* devicesInfo = new CLDeviceInfo[numberOfDevices];

		currentPlatformInfo->deviceCount = numberOfDevices;

		////////////////
		// Query Platform

		char* infoStr;

		clGetPlatformInfo(currentPlatform, CL_PLATFORM_PROFILE, pInfoBufferCapacity, pInfoBuffer, &infoLength);
		infoStr = new char[infoLength + 1];
		infoStr[infoLength] = '\0';
		memcpy(infoStr, pInfoBuffer, infoLength);
		currentPlatformInfo->profile = infoStr;

		clGetPlatformInfo(currentPlatform, CL_PLATFORM_VERSION, pInfoBufferCapacity, pInfoBuffer, &infoLength);
		infoStr = new char[infoLength + 1];
		infoStr[infoLength] = '\0';
		memcpy(infoStr, pInfoBuffer, infoLength);
		currentPlatformInfo->version = infoStr;

		clGetPlatformInfo(currentPlatform, CL_PLATFORM_NAME, pInfoBufferCapacity, pInfoBuffer, &infoLength);
		infoStr = new char[infoLength + 1];
		infoStr[infoLength] = '\0';
		memcpy(infoStr, pInfoBuffer, infoLength);
		currentPlatformInfo->name = infoStr;

		clGetPlatformInfo(currentPlatform, CL_PLATFORM_VENDOR, pInfoBufferCapacity, pInfoBuffer, &infoLength);
		infoStr = new char[infoLength + 1];
		infoStr[infoLength] = '\0';
		memcpy(infoStr, pInfoBuffer, infoLength);
		currentPlatformInfo->vendor = infoStr;

		// Get Platform 
		clGetPlatformInfo(currentPlatform, CL_PLATFORM_EXTENSIONS, pInfoBufferCapacity, pInfoBuffer, &infoLength);
		infoStr = new char[infoLength + 1];
		infoStr[infoLength] = '\0';
		memcpy(infoStr, pInfoBuffer, infoLength);
		currentPlatformInfo->extensions = infoStr;

		////////////////
		// Query Devices on this Platform

		for (uint32_t j = 0; j < numberOfDevices; j++) {
			_clPopulate(devices[j], &(devicesInfo[j]), pInfoBuffer, pInfoBufferCapacity);
		}

		currentPlatformInfo->devices = devicesInfo;

		delete[] devices;

	}

	pInstance->clPlatformCount = numberOfPlatforms;
	pInstance->clPlatforms = platformsInfo;

	delete[] platforms;

}

#pragma clang diagnostic pop