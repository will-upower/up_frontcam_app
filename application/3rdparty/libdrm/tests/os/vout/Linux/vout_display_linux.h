/*
 * Copyright 2008 Tungsten Graphics
 *   Jakob Bornecrantz <jakob@tungstengraphics.com>
 * Copyright 2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 * structures are reused from modetest.c
 * Renamed, Modified and Reused from modetest.c:
 *      get_resources(struct device *dev) -> R_Vout_GetResources(struct device * dev) 
 *      set_property(struct device *dev, struct property_arg *p) -> R_Vout_Set_Property(struct device * dev, struct property_arg * p)
 *      clear_planes(struct device *dev, struct plane_arg *p, unsigned int count) -> R_Vout_Clear_Planes(struct device * dev, struct plane_arg * p)
 *      free_resources(struct resources *res) -> R_Vout_Free_Resources(struct resources * res)
 *      set_plane(struct device *dev, struct plane_arg *p) -> R_Vout_SetPlane(struct device * dev, struct plane_arg * p)
 * New functions : R_Vout_open(const char * device, const char * module)
 *                 R_Vout_Close(int fd)
 *                 R_Vout_control(int fd, unsigned long request, void * arg)
 *                 R_Vout_Create_FrameBuffer(int fd, const uint32_t w, const uint32_t h, unsigned int fourcc, uint32_t handles[4], uint32_t pitches[4], uint32_t offsets[4], unsigned int *fb_id, unsigned int flags)
 *                 R_Vout_drmGetCap(int fd, uint64_t capability, uint64_t *value)
 *                 R_vout_drmModeSetCrtc(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t * connectors, int count, struct pipe_arg *pipe)
 *                 R_vout_drmModeDirtyFB(int fd, uint32_t bufferid, void * clips, uint32_t num_clips)
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

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef _VOUTDISPLAY_H_
#define _VOUTDISPLAY_H_

#define VAL_ZERO            (0)
#define VAL_ONE             (1)

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "xf86drm.h"
#include "xf86drmMode.h"
#include "drm_fourcc.h"
#include "libdrm_macros.h"
#include "common.h"
#include "buffers.h"

/*structure definitions */
struct plane_arg
{
    uint32_t     plane_id;                  /* the id of plane to use */
    uint32_t     crtc_id;                   /* the id of CRTC to bind to */
    bool         has_position;
    int32_t      x;
    int32_t      y;
    uint32_t     w;
    uint32_t     h;
    double       scale;
    unsigned int fb_id;
    unsigned int old_fb_id;
    struct bo *  bo;
    struct bo *  old_bo;
    char         format_str[5]; /* need to leave room for terminating \0 */
    unsigned int fourcc;
};

struct crtc 
{
    drmModeCrtc *             crtc;
    drmModeObjectProperties * props;
    drmModePropertyRes **     props_info;
    drmModeModeInfo *         mode;
};

struct encoder 
{
    drmModeEncoder * encoder;
};

struct connector
{
    drmModeConnector *        connector;
    drmModeObjectProperties * props;
    drmModePropertyRes **     props_info;
    char *                    name;
};

struct fb 
{
    drmModeFB * fb;
};

struct plane 
{
    drmModePlane *            plane;
    drmModeObjectProperties * props;
    drmModePropertyRes **     props_info;
};

struct resources 
{
    struct crtc *          crtcs;
    int                    count_crtcs;

    struct encoder *       encoders;
    int                    count_encoders;
    struct connector *     connectors;
    int                    count_connectors;
    struct fb *            fbs;
    int                    count_fbs;
    struct plane *         planes;
    uint32_t               count_planes;
};
struct device 
{
    int                    fd;
    struct resources *     resources;

    struct 
    {
        unsigned int   width;
        unsigned int   height;
        unsigned int   fb_id;
        struct bo *    bo;
        struct bo *    cursor_bo;
    } mode;

    int use_atomic;
    drmModeAtomicReq * req;
};


struct property_arg 
{
    uint32_t obj_id;
    uint32_t obj_type;
    char     name[DRM_PROP_NAME_LEN+1];
    uint32_t prop_id;
    uint64_t value;
    bool     optional;
};

struct pipe_arg 
{
    const char **    cons;
    uint32_t *       con_ids;
    unsigned int     num_cons;
    uint32_t         crtc_id;
    char             mode_str[DATA_LEN_64];
    char             format_str[5];
    float            vrefresh;
    unsigned int     fourcc;
    drmModeModeInfo * mode;
    struct crtc *     crtc;
    unsigned int      fb_id[2];
    unsigned int      current_fb_id;
    struct timeval    start;
    int               swap_count;
    
};

/**********************************************************************************************************************
 External variable decleration 
 *********************************************************************************************************************/
extern int pixfmt_bpp;
extern int crtc_x;
extern int crtc_y;
extern int crtc_w;
extern int crtc_h;

/**********************************************************************************************************************
 Function Declarations
*********************************************************************************************************************/
int R_Vout_open (const char * device, const char * module);
struct resources * R_Vout_GetResources (struct device * dev);
void R_Vout_Close (int fd);
bool R_Vout_Set_Property (struct device * dev, struct property_arg * p);
void R_Vout_Clear_Planes (struct device * dev, struct plane_arg * p);
void R_Vout_Free_Resources (struct resources * res);

int R_Vout_SetPlane (struct device * dev, struct plane_arg * p);
int R_Vout_control (int fd, unsigned long request, void * arg);
int R_Vout_Create_FrameBuffer (int fd, unsigned int fourcc, const uint32_t w, const uint32_t h, uint32_t handles[4], uint32_t pitches[4], uint32_t offsets[4], unsigned int * fb_id, unsigned int flags);
int R_Vout_drmGetCap (int fd, uint64_t capability, uint64_t * value);
int R_vout_drmModeSetCrtc (int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t * connectors, int count, struct pipe_arg * pipe);
int R_vout_drmModeDirtyFB (int fd, uint32_t bufferid, void * clips, uint32_t num_clips);

#endif
