/*
 * Copyright 2008 Tungsten Graphics
 *   Jakob Bornecrantz <jakob@tungstengraphics.com>
 * Copyright 2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 * Modified Functions : bo_create_dumb(), bo_map(), bo_create(), bo_destroy()
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vout_wrapper.h"  /* Modified */

#include <sys/ioctl.h>

#include "drm.h"
#include "drm_fourcc.h"

#include "libdrm_macros.h"
#include "xf86drm.h"

#include "buffers.h"
#include "format.h"


#define VAL_ZERO            (0)
#define VAL_ONE             (1)

#define MAKE_YUV_601_Y(r, g, b) \
   ((( 66 * (r) + 129 * (g) +  25 * (b) + 128) >> 8) + 16)
#define MAKE_YUV_601_U(r, g, b) \
   (((-38 * (r) -  74 * (g) + 112 * (b) + 128) >> 8) + 128)
#define MAKE_YUV_601_V(r, g, b) \
   (((112 * (r) -  94 * (g) -  18 * (b) + 128) >> 8) + 128)

#define MAKE_YUV_601(r, g, b) \
   { .y = MAKE_YUV_601_Y(r, g, b), \
     .u = MAKE_YUV_601_U(r, g, b), \
     .v = MAKE_YUV_601_V(r, g, b) }

struct color_yuv 
{
   unsigned char y;
   unsigned char u;
   unsigned char v;
};

void fill_tiles_yuv_packed(const struct util_format_info *info,
                 void *mem, unsigned int width,
                 unsigned int height, unsigned int stride)
{
   const struct util_yuv_info *yuv = &info->yuv;
   unsigned char *y_mem = (yuv->order & YUV_YC) ? mem : mem + 1;
   unsigned char *c_mem = (yuv->order & YUV_CY) ? mem : mem + 1;
   unsigned int u = (yuv->order & YUV_YCrCb) ? 2 : 0;
   unsigned int v = (yuv->order & YUV_YCbCr) ? 2 : 0;
   unsigned int x;
   unsigned int y;

    struct color_yuv color = MAKE_YUV_601(0, 0, 0); /* Black color */

   for (y = 0; y < height; ++y) {
       for (x = 0; x < width; x += 2) {
           y_mem[2*x] = color.y;
           c_mem[2*x+u] = color.u;
           y_mem[2*x+2] = color.y;
           c_mem[2*x+v] = color.v;
       }

       y_mem += stride;
       c_mem += stride;
   }
}

void fill_tiles(const struct util_format_info *info, void *planes[3],
              unsigned int width, unsigned int height, 
              unsigned int stride)
{
   unsigned char *u, *v;

   switch (info->format) {
   case DRM_FORMAT_UYVY:
   case DRM_FORMAT_VYUY:
   case DRM_FORMAT_YUYV:
   case DRM_FORMAT_YVYU:
       return fill_tiles_yuv_packed(info, planes[0],
                        width, height, stride);
   }
}

void util_fill_pattern(uint32_t format, \
              void *planes[3], unsigned int width, \
              unsigned int height, unsigned int stride)
{
   const struct util_format_info *info;

   info = util_format_info_find(format);
   if (info == NULL)
       return;

   return fill_tiles(info, planes, width, height, stride);
}
 
struct bo * bo_create_dumb(int fd, unsigned int width, unsigned int height, unsigned int bpp)
{
    struct bo * bo = {VAL_ZERO};
    struct drm_mode_create_dumb arg = {VAL_ZERO};
    int ret = INVALID;
    bo = calloc(1, sizeof(*bo));
    if (bo == NULL) 
    {
        PRINT_ERROR("failed to allocate buffer object\n");
        return NULL;
    }
    memset(&arg, VAL_ZERO, sizeof(arg));
    arg.bpp    = bpp;
    arg.width  = width;
    arg.height = height;
    ret = wr_vout_control(fd, DRM_IOCTL_MODE_CREATE_DUMB, &arg);
    if (ret) 
    {
        PRINT_ERROR("failed to create dumb buffer: %s\n", strerror(errno));
        free(bo);
        return NULL;
    }
    bo->fd     = fd;
    bo->handle = arg.handle;
    bo->size   = arg.size;
    bo->pitch  = arg.pitch;
    return bo;
}


int bo_map(struct bo *bo, void ** out)
{
    struct drm_mode_map_dumb arg = {VAL_ZERO};
    void * map = NULL;
    int ret    = INVALID;
    memset(&arg, VAL_ZERO, sizeof(arg));
    arg.handle = bo->handle;
    ret = wr_vout_control(bo->fd, DRM_IOCTL_MODE_MAP_DUMB, &arg);
    if (ret)
    {
        PRINT_ERROR("failed drmIoctl %s\n", strerror(errno));
        return ret;
    }
    map = drm_mmap(VAL_ZERO, bo->size, PROT_READ | PROT_WRITE, MAP_SHARED, bo->fd, (__off_t)arg.offset);
    if (map == MAP_FAILED)
    {   
        PRINT_ERROR("failed drm_mmap %s\n", strerror(errno));
        return -EINVAL;
    }
    bo->ptr = map;
    if (bo->ptr) 
    {
        *out = map;
    }
    return SUCCESS;
}

void bo_unmap(struct bo *bo)
{
    if (!bo->ptr)
        return;

    drm_munmap(bo->ptr, bo->size);
    bo->ptr = NULL;
}

struct bo * bo_create(int fd, unsigned int format, unsigned int width, unsigned int height, unsigned int handles[4], unsigned int pitches[4], unsigned int offsets[4])
{
    unsigned int virtual_height;
    struct bo *bo = NULL;
    unsigned int bpp;
    void *planes[3] = { 0, };
    void *virtual;
    int ret;

    switch (format)
    {
        case DRM_FORMAT_C8:
        case DRM_FORMAT_NV12:
        case DRM_FORMAT_NV21:
        case DRM_FORMAT_NV16:
        case DRM_FORMAT_NV61:
        case DRM_FORMAT_YUV420:
        case DRM_FORMAT_YVU420:
            bpp = 8;
            break;

        case DRM_FORMAT_ARGB4444:
        case DRM_FORMAT_XRGB4444:
        case DRM_FORMAT_ABGR4444:
        case DRM_FORMAT_XBGR4444:
        case DRM_FORMAT_RGBA4444:
        case DRM_FORMAT_RGBX4444:
        case DRM_FORMAT_BGRA4444:
        case DRM_FORMAT_BGRX4444:
        case DRM_FORMAT_ARGB1555:
        case DRM_FORMAT_XRGB1555:
        case DRM_FORMAT_ABGR1555:
        case DRM_FORMAT_XBGR1555:
        case DRM_FORMAT_RGBA5551:
        case DRM_FORMAT_RGBX5551:
        case DRM_FORMAT_BGRA5551:
        case DRM_FORMAT_BGRX5551:
        case DRM_FORMAT_RGB565:
        case DRM_FORMAT_BGR565:    
        case DRM_FORMAT_UYVY:
        case DRM_FORMAT_VYUY:
        case DRM_FORMAT_YUYV:
        case DRM_FORMAT_YVYU:
            bpp = 16;
            break;

        case DRM_FORMAT_BGR888:
        case DRM_FORMAT_RGB888:
            bpp = 24;
            break;

        case DRM_FORMAT_ARGB8888:
        case DRM_FORMAT_XRGB8888:
        case DRM_FORMAT_ABGR8888:
        case DRM_FORMAT_XBGR8888:
        case DRM_FORMAT_RGBA8888:
        case DRM_FORMAT_RGBX8888:
        case DRM_FORMAT_BGRA8888:
        case DRM_FORMAT_BGRX8888:
        case DRM_FORMAT_ARGB2101010:
        case DRM_FORMAT_XRGB2101010:
        case DRM_FORMAT_ABGR2101010:
        case DRM_FORMAT_XBGR2101010:
        case DRM_FORMAT_RGBA1010102:
        case DRM_FORMAT_RGBX1010102:
        case DRM_FORMAT_BGRA1010102:
        case DRM_FORMAT_BGRX1010102:
            bpp = 32;
            break;

        case DRM_FORMAT_XRGB16161616F:
        case DRM_FORMAT_XBGR16161616F:
        case DRM_FORMAT_ARGB16161616F:
        case DRM_FORMAT_ABGR16161616F:
            bpp = 64;
            break;

        default:
            PRINT_ERROR("unsupported format 0x%08x\n", format);
            return NULL;
    }

    switch (format) {
    case DRM_FORMAT_NV12:
    case DRM_FORMAT_NV21:
    case DRM_FORMAT_YUV420:
    case DRM_FORMAT_YVU420:
        virtual_height = height * 3 / 2;
        break;

    case DRM_FORMAT_NV16:
    case DRM_FORMAT_NV61:
        virtual_height = height * 2;
        break;

    default:
        virtual_height = height;
        break;
    }

    bo = bo_create_dumb(fd, width, height, bpp);
    if (!bo)
        return NULL;

    ret = bo_map(bo, &virtual);

    if (ret)
    {
        PRINT_ERROR("Failed to map buffer: %s\n", strerror(-errno));
        bo_destroy(bo);
        return NULL;
    }

    switch (format)
    {
        case DRM_FORMAT_UYVY:
        case DRM_FORMAT_VYUY:
        case DRM_FORMAT_YUYV:
        case DRM_FORMAT_YVYU:
            offsets[0] = VAL_ZERO;
            handles[0] = bo->handle;
            pitches[0] = (unsigned int)bo->pitch;

            planes[0] = virtual;
            break;
        case DRM_FORMAT_BGR888:
        case DRM_FORMAT_RGB888:
            offsets[0] = VAL_ZERO;
            handles[0] = bo->handle;
            pitches[0] = (unsigned int)bo->pitch;
            break;

        case DRM_FORMAT_NV12:
        case DRM_FORMAT_NV21:
        case DRM_FORMAT_NV16:
        case DRM_FORMAT_NV61:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;
            pitches[1] = pitches[0];
            offsets[1] = pitches[0] * height;
            handles[1] = bo->handle;

            planes[0] = virtual;
            planes[1] = virtual + offsets[1];
            break;

        case DRM_FORMAT_YUV420:
        case DRM_FORMAT_YVU420:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;
            pitches[1] = pitches[0] / 2;
            offsets[1] = pitches[0] * height;
            handles[1] = bo->handle;
            pitches[2] = pitches[1];
            offsets[2] = offsets[1] + pitches[1] * height / 2;
            handles[2] = bo->handle;

            planes[0] = virtual;
            planes[1] = virtual + offsets[1];
            planes[2] = virtual + offsets[2];
            break;

        case DRM_FORMAT_C8:
        case DRM_FORMAT_ARGB4444:
        case DRM_FORMAT_XRGB4444:
        case DRM_FORMAT_ABGR4444:
        case DRM_FORMAT_XBGR4444:
        case DRM_FORMAT_RGBA4444:
        case DRM_FORMAT_RGBX4444:
        case DRM_FORMAT_BGRA4444:
        case DRM_FORMAT_BGRX4444:
        case DRM_FORMAT_ARGB1555:
        case DRM_FORMAT_XRGB1555:
        case DRM_FORMAT_ABGR1555:
        case DRM_FORMAT_XBGR1555:
        case DRM_FORMAT_RGBA5551:
        case DRM_FORMAT_RGBX5551:
        case DRM_FORMAT_BGRA5551:
        case DRM_FORMAT_BGRX5551:
        case DRM_FORMAT_RGB565:
        case DRM_FORMAT_BGR565:
        case DRM_FORMAT_ARGB8888:
        case DRM_FORMAT_XRGB8888:
        case DRM_FORMAT_ABGR8888:
        case DRM_FORMAT_XBGR8888:
        case DRM_FORMAT_RGBA8888:
        case DRM_FORMAT_RGBX8888:
        case DRM_FORMAT_BGRA8888:
        case DRM_FORMAT_BGRX8888:
        case DRM_FORMAT_ARGB2101010:
        case DRM_FORMAT_XRGB2101010:
        case DRM_FORMAT_ABGR2101010:
        case DRM_FORMAT_XBGR2101010:
        case DRM_FORMAT_RGBA1010102:
        case DRM_FORMAT_RGBX1010102:
        case DRM_FORMAT_BGRA1010102:
        case DRM_FORMAT_BGRX1010102:
        case DRM_FORMAT_XRGB16161616F:
        case DRM_FORMAT_XBGR16161616F:
        case DRM_FORMAT_ARGB16161616F:
        case DRM_FORMAT_ABGR16161616F:
            offsets[0] = 0;
            handles[0] = bo->handle;
            pitches[0] = bo->pitch;

            planes[0] = virtual;
            break;
        default :
            ;
    }

    /* Fill initializing data of YUV packaged buffer */
    util_fill_pattern(format, planes, width, height, pitches[0]);
    return bo;
}

void bo_destroy(struct bo *bo)
{
    struct drm_mode_destroy_dumb arg = {VAL_ZERO};
    int ret = INVALID;
    memset(&arg, VAL_ZERO, sizeof(arg));
    arg.handle = bo->handle;
    ret = wr_vout_control(bo->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);
    if (ret)
    {
        PRINT_ERROR("Failed to destroy dumb buffer: %s\n", strerror(errno));
    }

    free(bo);
}
