#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

void Conv_YUYV2RGB_OpenCL(unsigned char *yuyv, unsigned char *bgr, int width, int height)
{
    cl_int err;
    printf("+=+=+=+=+=+=+=+=+=+=+=+= OpenCL Conv_YUYV2RGB +=+=+=+=+=+=+=+=+=+=+=+=\n");
    // Load OpenCL program source code
    FILE *file = fopen("Conv_YUYV2RGB.cl", "r");
    fseek(file, 0, SEEK_END);
    size_t source_size = ftell(file);
    rewind(file);
    char *source_code = (char *)malloc(source_size + 1);
    fread(source_code, 1, source_size, file);
    source_code[source_size] = '\0';
    fclose(file);

    // Initialize OpenCL
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, NULL);
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);

    // Create OpenCL program
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_code, &source_size, &err);
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Create OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "Conv_YUYV2RGB", &err);

    // Create OpenCL buffers
    cl_mem yuyv_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, width * height * 2, yuyv, &err);
    cl_mem bgr_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height * 3, NULL, &err);

    // Set OpenCL kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &yuyv_buffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bgr_buffer);
    err |= clSetKernelArg(kernel, 2, sizeof(int), &width);
    err |= clSetKernelArg(kernel, 3, sizeof(int), &height);

    // Execute OpenCL kernel
    size_t global_work_size[2] = {width, height};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, NULL, 0, NULL, NULL);
    err = clFinish(queue);

    // Read results back from OpenCL device
    err = clEnqueueReadBuffer(queue, bgr_buffer, CL_TRUE, 0, width * height * 3, bgr, 0, NULL, NULL);

    // Cleanup OpenCL resources
    clReleaseMemObject(yuyv_buffer);
    clReleaseMemObject(bgr_buffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    free(source_code);
}
