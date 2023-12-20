/*
 * DRM based mode setting test program
 * Copyright 2008 Tungsten Graphics
 *   Jakob Bornecrantz <jakob@tungstengraphics.com>
 * Copyright 2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <poll.h>
#include <sys/time.h>

#include "vout_display_linux.h"

/******************************************************************************************************************//**
/* Function Name : R_Vout_open */
/******************************************************************************************************************//**
 * @brief       drm device open
 * @param[in]   device
 * @param[in]   module
 * @param[out]  none                 
 * @retval      fd
 *********************************************************************************************************************/
int R_Vout_open(const char * device, const char * module)
{
    int fd = INVALID;

    if (module)
    {
        fd = drmOpen(module, device);

        if (fd < 0) 
        {
            PRINT_ERROR("Failed to open device '%s': %s\n", module, strerror(errno));
            return -errno;
        }

    }
    else 
    {
        PRINT_ERROR("No device found \n");
        return -ENODEV;
    }

    return fd;
    
}
/**********************************************************************************************************************
 End of function R_Vout_open
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_Close */
/******************************************************************************************************************//**
 * @brief       drm device close
 * @param[in]   fd
 * @param[out]  none                 
 * @retval      none
 *********************************************************************************************************************/
void R_Vout_Close(int fd)
{
    drmClose(fd);
}
/**********************************************************************************************************************
 End of function R_Vout_Close
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_GetResources */
/******************************************************************************************************************//**
 * @brief       drm get resources
 * @param[in]   dev
 * @param[out]  none                 
 * @retval      res
 *********************************************************************************************************************/
struct resources * R_Vout_GetResources(struct device * dev)
{
    drmModeRes * _res;

    drmModePlaneRes * plane_res;

    struct resources * res;

    int i;

    res = calloc(1, sizeof(* res));

    if (res == 0)
    {
        return NULL;
    }

    drmSetClientCap(dev->fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);

    _res = drmModeGetResources(dev->fd);

    if (!_res)
    {
        PRINT_ERROR("drmModeGetResources failed: %s\n", strerror(errno));
        free(res);
        return NULL;
    }
    res->count_crtcs      = _res->count_crtcs;
    res->count_encoders   = _res->count_encoders;
    res->count_connectors = _res->count_connectors;
    res->count_fbs        = _res->count_fbs;

    res->crtcs      = calloc((size_t)res->count_crtcs, sizeof(* res->crtcs));
    res->encoders   = calloc((size_t)res->count_encoders, sizeof(* res->encoders));
    res->connectors = calloc((size_t)_res->count_connectors, sizeof(* res->connectors));
    res->fbs        = calloc((size_t)res->count_fbs, sizeof(* res->fbs));

    if ((!res->crtcs) || (!res->encoders) || (!res->connectors) || (!res->fbs))
    {
        drmModeFreeResources(_res);
        goto error;
    }

#define get_resource(_res, __res, type, Type)                   \
    do {                                    \
        for (i = 0; i < (int)(_res)->count_##type##s; ++i) {    \
            uint32_t type##id = (__res)->type##s[i];            \
            (_res)->type##s[i].type =                           \
                drmModeGet##Type(dev->fd, type##id);            \
            if (!(_res)->type##s[i].type)                       \
            PRINT_ERROR("could not get %s %i: %s\n",#type, type##id,strerror(errno));       \
        }                               \
    } while (0)

    get_resource(res, _res, crtc, Crtc);
    get_resource(res, _res, encoder, Encoder);
    get_resource(res, _res, connector, Connector);
    get_resource(res, _res, fb, FB);

    drmModeFreeResources(_res);

#define get_properties(_res, type, Type)                    \
    do {                                    \
        for (i = 0; i < (int)(_res)->count_##type##s; ++i) {    \
            struct type *obj = &res->type##s[i];            \
            unsigned int j;                     \
            obj->props =                        \
                drmModeObjectGetProperties(dev->fd, obj->type->type##_id, \
                               DRM_MODE_OBJECT_##Type); \
            if (!obj->props) {                  \
                PRINT_ERROR("could not get properties\n"); \
                continue;                   \
            }                           \
            obj->props_info = calloc(obj->props->count_props,   \
                         sizeof(*obj->props_info)); \
            if (!obj->props_info)                   \
                continue;                   \
            for (j = 0; j < obj->props->count_props; ++j)       \
                obj->props_info[j] =                \
                    drmModeGetProperty(dev->fd, obj->props->props[j]); \
        }                               \
    } while (0)

    get_properties(res, crtc, CRTC);
    get_properties(res, connector, CONNECTOR);

    for (i = 0; i < res->count_crtcs; ++i)
    {
        res->crtcs[i].mode = &res->crtcs[i].crtc->mode;
    }

    plane_res = drmModeGetPlaneResources(dev->fd);

    if (!plane_res)
    {
        PRINT_ERROR("drmModeGetPlaneResources failed: %s\n", strerror(errno));
        return res;
    }

    res->count_planes = plane_res->count_planes;

    res->planes = calloc(res->count_planes, sizeof(* res->planes));

    if (!res->planes)
    {
        drmModeFreePlaneResources(plane_res);
        goto error;
    }

    get_resource(res, plane_res, plane, Plane);
    drmModeFreePlaneResources(plane_res);
    get_properties(res, plane, PLANE);

    return res;

error:
    R_Vout_Free_Resources(res);
    return NULL;
}
/**********************************************************************************************************************
 End of function R_Vout_GetResources
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_Set_Property */
/******************************************************************************************************************//**
 * @brief       drm set property
 * @param[in]   dev
 * @param[in]   p
 * @param[out]  none                 
 * @retval      true
 * @retval      false
 *********************************************************************************************************************/
bool R_Vout_Set_Property(struct device * dev, struct property_arg * p)
{
    drmModeObjectProperties * props  = NULL;

    drmModePropertyRes ** props_info = NULL;

    const char * obj_type;

    int ret;

    int i;

    p->obj_type = 0;
    p->prop_id  = 0;

#define find_object(_res, type, Type)                   \
    do {                                    \
        for (i = 0; i < (int)(_res)->count_##type##s; ++i) {    \
            struct type *obj = &(_res)->type##s[i];         \
            if (obj->type->type##_id != p->obj_id)          \
                continue;                   \
            p->obj_type = DRM_MODE_OBJECT_##Type;           \
            obj_type = #Type;                   \
            props = obj->props;                 \
            props_info = obj->props_info;               \
        }                               \
    } while(0)                              \

    find_object(dev->resources, crtc, CRTC);

    if (p->obj_type == 0)
    {
        find_object(dev->resources, connector, CONNECTOR);
    }

    if (p->obj_type == 0)
    {
        find_object(dev->resources, plane, PLANE);
    }

    if (p->obj_type == 0)
    {
        PRINT_ERROR("Object %i not found, can't set property\n",p->obj_id);
        return false;
    }

    if (!props)
    {
        PRINT_ERROR("%s %i has no properties\n",obj_type, p->obj_id);
        return false;
    }

    for (i = 0; i < (int)props->count_props; ++i)
    {
        if (!props_info[i])
        {
            continue;
        }

        if (strcmp(props_info[i]->name, p->name) == 0)
        {
            break;
        }
    }

    if (i == (int)props->count_props)
    {
        if (!p->optional)
        {
            PRINT_ERROR("%s %i has no %s property\n", obj_type, p->obj_id, p->name);
        }
        return false;
    }

    p->prop_id = props->props[i];

    if (!dev->use_atomic)
    {
        ret = drmModeObjectSetProperty(dev->fd, p->obj_id, p->obj_type, p->prop_id, p->value);
    }
    else
    {
        ret = drmModeAtomicAddProperty(dev->req, p->obj_id, p->prop_id, p->value);
    }

    if (ret < 0)
    {
        PRINT_ERROR("failed to set %s %i property %s to %" PRIu64 ": %s\n", obj_type, p->obj_id, p->name, p->value, strerror(errno));
    }

    return true;
}
/**********************************************************************************************************************
 End of function R_Vout_Set_Property
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_Clear_Planes */
/******************************************************************************************************************//**
 * @brief       drm clear planes
 * @param[in]   dev
 * @param[in]   p         pointer to plane argument structure
 * @param[out]  none
 * @retval      none
 *********************************************************************************************************************/
void R_Vout_Clear_Planes(struct device * dev, struct plane_arg * p)
{
    struct drm_mode_destroy_dumb arg = {VAL_ZERO};

    int ret = INVALID;

    memset(&arg, VAL_ZERO, sizeof(arg));
    arg.handle = p->bo->handle;
    
    if (p->fb_id)
    {
        drmModeRmFB(dev->fd, p->fb_id);
    }

    if (p->bo)
    {
        ret = drmIoctl(p->bo->fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);

        if (ret)
        {
            PRINT_ERROR("Failed to destroy dumb buffer: %s\n", strerror(errno));
        }

        free(p->bo);
    }
}
/**********************************************************************************************************************
 End of function R_Vout_Clear_Planes
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_Free_Resources */
/******************************************************************************************************************//**
 * @brief       drm free resources
 * @param[in]   res
 * @param[out]  none
 * @retval      none
 *********************************************************************************************************************/
void R_Vout_Free_Resources(struct resources * res)
{
    int i;

    if (!res)
    {
        PRINT_ERROR("free resources  failed\n");
        return ;
    }
#define free_resource(_res, type, Type)                 \
    do {                                    \
        if (!(_res)->type##s)                       \
            break;                          \
        for (i = 0; i < (int)(_res)->count_##type##s; ++i) {    \
            if (!(_res)->type##s[i].type)               \
                break;                      \
            drmModeFree##Type((_res)->type##s[i].type);     \
        }                               \
        free((_res)->type##s);                      \
    } while (0)

#define free_properties(_res, type)                 \
    do {                                    \
        for (i = 0; i < (int)(_res)->count_##type##s; ++i) {    \
            unsigned int j;                                     \
            for (j = 0; j < res->type##s[i].props->count_props; ++j)\
                drmModeFreeProperty(res->type##s[i].props_info[j]);\
            free(res->type##s[i].props_info);           \
            drmModeFreeObjectProperties(res->type##s[i].props); \
        }                               \
    } while (0)

    free_properties(res, plane);
    free_resource(res, plane, Plane);

    free_properties(res, connector);
    free_properties(res, crtc);

    for (i = 0; i < res->count_connectors; i++)
    {
        free(res->connectors[i].name);
    }

    free_resource(res, fb, FB);
    free_resource(res, connector, Connector);
    free_resource(res, encoder, Encoder);
    free_resource(res, crtc, Crtc);

    free(res);
}
/**********************************************************************************************************************
 End of function R_Vout_Free_Resources
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_SetPlane */
/******************************************************************************************************************//**
 * @brief       drm set plane
 * @param[in]   none
 * @param[out]  dev
 * @param[out]  p               pointer to plane argument structure
 * @retval      true            success
 * @retval      false           failed
 *********************************************************************************************************************/
int R_Vout_SetPlane(struct device * dev, struct plane_arg * p)
{
    if (drmModeSetPlane(dev->fd, p->plane_id, p->crtc_id, p->fb_id, 0, p->x, p->y, (uint32_t)p->w, (uint32_t)p->h, 0, 0, p->w << 16, p->h << 16))
    {
        PRINT_ERROR("failed to enable plane: %s\n", strerror(errno));
        return FAILED;
    }
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function R_Vout_SetPlane
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_control */
/******************************************************************************************************************//**
 * @brief       drm control
 * @param[in]   fd
 * @param[in]   request
 * @param[in]   arg
 * @param[out]  none
 * @retval      ret
 *********************************************************************************************************************/
int R_Vout_control(int fd, unsigned long request, void * arg)
{
    int ret = INVALID;

    switch (request)
    {
        case DRM_IOCTL_MODE_CREATE_DUMB:
            ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, arg);
            break;
        case DRM_IOCTL_MODE_MAP_DUMB:
            ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, arg);
            break;
        case DRM_IOCTL_MODE_DESTROY_DUMB:
            ret = drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, arg);
            break;
        default:
            break;
    }
    return (ret);
}
/**********************************************************************************************************************
 End of function R_Vout_control
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_Create_FrameBuffer */
/******************************************************************************************************************//**
 * @brief       frame buffer creation for Vout
 * @param[in]   fd
 * @param[in]   w           width
 * @param[in]   h           height
 * @param[in]   fourcc      frame buffer pixel format
 * @param[in]   handles     buffer handles
 * @param[in]   pitches     pitch of each plane
 * @param[in]   offsets     offset of each plane
 * @param[in]   fb_id       Id of buffer frame
 * @param[in]   flags
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int R_Vout_Create_FrameBuffer(int fd, const uint32_t w, const uint32_t h, unsigned int fourcc, uint32_t handles[4], uint32_t pitches[4], uint32_t offsets[4], unsigned int *fb_id, unsigned int flags)
{
    int ret = INVALID;

    ret = drmModeAddFB2(fd, w, h, fourcc, handles, pitches, offsets, fb_id, flags);

    return (ret);
}
/**********************************************************************************************************************
 End of function R_Vout_Create_FrameBuffer
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_Vout_drmGetCap */
/******************************************************************************************************************//**
 * @brief       capability of drm
 * @param[in]   fd
 * @param[in]   capability
 * @param[in]   value
 * @param[out]  none                 
 * @retval      ret
 *********************************************************************************************************************/
int R_Vout_drmGetCap(int fd, uint64_t capability, uint64_t *value)
{

    int ret = INVALID;

    ret = drmGetCap(fd, capability, value);

    return (ret);
} 
/**********************************************************************************************************************
 End of function R_Vout_drmGetCap
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_vout_drmModeSetCrtc */
/******************************************************************************************************************//**
 * @brief       setting crtc mode of vout
 * @param[in]   fd              
 * @param[in]   crtc_id         Id of crtc
 * @param[in]   fb_id           Id of frame buffer
 * @param[in]   x               Position of frame buffer
 * @param[in]   y               Position of frame buffer
 * @param[in]   connectors
 * @param[in]   count
 * @param[in]   pipe
 * @param[out]  none
 * @retval      ret
 *********************************************************************************************************************/
int R_vout_drmModeSetCrtc(int fd, uint32_t crtc_id, uint32_t fb_id, uint32_t x, uint32_t y, uint32_t * connectors, int count, struct pipe_arg *pipe)
{

    int ret = INVALID;

    ret = drmModeSetCrtc(fd, crtc_id, fb_id, x, y, connectors, count, pipe->mode);

    return (ret);
}
/**********************************************************************************************************************
 End of function R_vout_drmModeSetCrtc
 *********************************************************************************************************************/

/******************************************************************************************************************//**
/* Function Name : R_vout_drmModeDirtyFB */
/******************************************************************************************************************//**
 * @brief       setting drm mode as Dirty frame buffer
 * @param[in]   fd
 * @param[in]   bufferid
 * @param[in]   clips
 * @param[in]   num_clips
 * @param[out]  none
 * @retval      ret
 *********************************************************************************************************************/
int R_vout_drmModeDirtyFB(int fd, uint32_t bufferid, void * clips, uint32_t num_clips)
{
    int ret = INVALID;
    drmModeDirtyFB(fd, bufferid, clips, num_clips);

    return (ret);
}
/**********************************************************************************************************************
 End of function R_vout_drmModeDirtyFB
 *********************************************************************************************************************/
