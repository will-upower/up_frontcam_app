#include <fcntl.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include "frontcam_mmap.h"

// #include <semaphore.h>

// static const char *sem_name = "/global_semaphore";
// static sem_t *semaphore;

int mmap_image_init()
{
    //gp_vin_out_buffer
    //g_customize, g_frame_width, g_frame_height, BPP_*, get_buffer_size() are all defined in the common.h or mmap.h file
    size_t size = get_buffer_size();
    mode_t old_umask = umask(0);
    mmap_file = open("image_buffer_out.dat", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0666);
    umask(old_umask);

    if (mmap_file == -1) {
        PRINT_ERROR("Failed to open or create mmap file 'image_buffer_out.dat'\n");
        return FAILED; // 1
    }

    if (ftruncate(mmap_file, size) == -1) {
        PRINT_ERROR("Failed to set the size of mmap file 'image_buffer_out.dat'\n");
        close(mmap_file);
        mmap_file = -1;
        return FAILED; // 1
    }
    
    mapped_buffer_out = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mmap_file, 0);
    
    if (mapped_buffer_out == MAP_FAILED) {
        PRINT_ERROR("Failed to map mmap buffer to file 'image_buffer_out.dat'\n");
        close(mmap_file);
        mmap_file = -1;
        return FAILED; // 1
    }

    return SUCCESS; // 0
}

int mmap_deinit() {
    size_t size = get_buffer_size();
    
    if (munmap(mapped_buffer_out, size) != 0) {
        PRINT_ERROR("Error unmapping file 'image_buffer_out.dat'\n");
        close(mmap_file);
        return FAILED; // 1
    }
    
    if (mmap_file != -1) {
        if (close(mmap_file) != 0) {
            PRINT_ERROR("Failed to close mmap file 'image_buffer_out.dat'\n");
            return FAILED; // 1
        }
        mmap_file = -1;
    }

    return SUCCESS; // 0
}

int mmap_copy() {
    size_t size = get_buffer_size();
    if (gp_vin_out_buffer == NULL || mapped_buffer_out == NULL) {
        PRINT_ERROR("gp_vin_out_buffer or mapped_buffer_out is NULL\n");
        return FAILED; // 1
    }

    // semaphore = sem_open(sem_name, O_CREAT, 0644, 1);
    // if (semaphore == SEM_FAILED) {
    //     PRINT_ERROR("sem_open failed\n");
    //     return FAILED;
    // }

    memcpy((void*)mapped_buffer_out, gp_vin_out_buffer, size);

    // // Release the semaphore
    // if (sem_post(semaphore) < 0) {
    //     PRINT_ERROR("sem_post failed\n");
    //     return FAILED;
    // }

    return SUCCESS; // 0
}

int in_mmap_init(const char* filename) 
{
    size_t size = get_buffer_size();

    // Open the file
    mmap_file_in = open(filename, O_RDONLY);
    if (mmap_file_in == -1) {
        PRINT_ERROR("Failed to open file %s\n", filename);
        return FAILED;
    }
    // Map the file into memory
    mapped_buffer_in = mmap(NULL, size, PROT_READ, MAP_SHARED, mmap_file_in, 0);
    if (mapped_buffer_in == MAP_FAILED) {
        PRINT_ERROR("Memory allocation failed for %s\n", filename);
        close(mmap_file_in);
        return FAILED;
    }

    // semaphore = sem_open(sem_name, O_CREAT, 0644, 1);
    // if (semaphore == SEM_FAILED) {
    //     PRINT_ERROR("sem_open failed\n");
    //     return FAILED;
    // }

    // if (sem_wait(semaphore) < 0) {
    //     PRINT_ERROR("sem_wait failed\n");
    //     return FAILED;
    // }

    // printf("Semaphore created and locked\n");

    // // Release the semaphore
    // if (sem_post(semaphore) < 0) {
    //     PRINT_ERROR("sem_post failed\n");
    //     return FAILED;
    // }

    // printf("Semaphore unlocked\n");

    return SUCCESS;
}


int in_mmap_deinit() {
    size_t size = get_buffer_size();
    if (munmap(mapped_buffer_in, size) == -1) {
        PRINT_ERROR("Memory de-allocation failed!\n");
        return FAILED;
    }
    close(mmap_file_in);

    // // Close semaphore
    // if (sem_close(semaphore) < 0) {
    //     PRINT_ERROR("sem_close failed\n");
    //     return FAILED;
    // }

    return SUCCESS;
}