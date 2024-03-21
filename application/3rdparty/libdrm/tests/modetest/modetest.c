/*
 * Copyright 2008 Tungsten Graphics
 *   Jakob Bornecrantz <jakob@tungstengraphics.com>
 * Copyright 2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 * Modified functions : set_mode()
 * 
 * Added functions : vout_deinit(), execute(), vout_setup(), vout_pipes(), vout_planes()
 * 
 * Renamed and Modified functions : parse_resolution_params(), format_support(), bo_fb_create(), set_plane_properties()
 *                                  vout_init(), 
 * 
 * Removed functions : U642I64(), dump_fourcc(), dump_encoders(), dump_mode(), dump_blob(), modifier_to_string(),  
 *                     dump_in_formats(), dump_prop(), dump_connectors(), dump_crtcs(), dump_framebuffers(), dump_planes(),
 *                     page_flip_handler(), add_property(), add_property_optional(), set_gamma(), atomic_set_plane(),
 *                     atomic_set_planes(), atomic_clear_planes(), atomic_clear_FB(), clear_planes(), atomic_set_mode(),
 *                     atomic_clear_mode(), clear_mode(), set_planes(), set_cursors(), clear_cursors(), test_page_flip(),
 *                     parse_connector(), parse_plane(), parse_property(), parse_fill_patterns(), usage(), cursor_supported(),
 * 
 * "soc_vout_param.h" included .
 *
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
#include <ctype.h>
#include <stdbool.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdio.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdlib.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/stdint.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/inttypes.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/unistd.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/strings.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/errno.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/poll.h>
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/sys/time.h>
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include </home/ubuntu/Renesas/rcar-xos/v3.18.0/tools/toolchains/poky/sysroots/aarch64-poky-linux/usr/include/math.h>
#include "soc_vout_param.h"
#include "vout_wrapper.h"
#include "format.h" 

#define PLANE_ID     (34)
#define SCALE        (1.0)


int      crtc_x;
int      crtc_y;
int      crtc_w;
int      crtc_h;
struct   device dev;
struct   plane_arg * plane_args = NULL;
struct   pipe_arg * pipe_args   = NULL;
unsigned plane_count            = 1;
unsigned pipe_count             = 0;
int      pixfmt_bpp;
char     pix_fmt[16];

int     vout_init (void);
int     vout_setup(void);
int     vout_pipes(void);
int     vout_planes(void);
int     execute (void);
int64_t vout_deinit (void);
int     vout_exec (struct device * dev, struct plane_arg * p);
extern void bo_unmap (struct bo * bo);

static float mode_vrefresh(drmModeModeInfo *mode)
{
    return  (mode->clock * 1000.00) / (mode->htotal * mode->vtotal);
}

static drmModeConnector *get_connector_by_id(struct device *dev, uint32_t id)
{
    drmModeConnector * connector;

    int i;
    for (i = 0; i < dev->resources->count_connectors; i++)
    {
        connector = dev->resources->connectors[i].connector;
        if (connector && (connector->connector_id == id))
        {
            return connector;
        }
    }

    return NULL;
}

static struct crtc * get_crtc_by_id(struct device * dev, uint32_t id)
{
    int i;
    for (i = 0; i < dev->resources->count_crtcs; ++i) 
    {
        drmModeCrtc * crtc = dev->resources->crtcs[i].crtc;

        if (crtc && (crtc->crtc_id == id))
        {
            return &dev->resources->crtcs[i];
        }
    }

    return NULL;
}

static uint32_t get_crtc_mask(struct device * dev, struct crtc * crtc)
{
    unsigned int i;

    for (i = 0; i < (unsigned int)dev->resources->count_crtcs; i++) 

    {
        if (crtc->crtc->crtc_id == dev->resources->crtcs[i].crtc->crtc_id)
        {
            return (uint32_t)(1 << i);
        }
    }

    /* Unreachable: crtc->crtc is one of resources->crtcs[] */
    /* Don't return zero or static analysers will complain */
    abort();
    return SUCCESS;
}

static int get_crtc_index(struct device *dev, uint32_t id)
{
    int i;
    for (i = 0; i < dev->resources->count_crtcs; ++i)
    {
        drmModeCrtc * crtc = dev->resources->crtcs[i].crtc;
        if (crtc && (crtc->crtc_id == id))
        {
            return i;
        }
    }

    return FAILED;
}

static drmModeEncoder *get_encoder_by_id(struct device *dev, uint32_t id)
{
    drmModeEncoder * encoder;
    int i;
    for (i = 0; i < dev->resources->count_encoders; i++)
    {
        encoder = dev->resources->encoders[i].encoder;
        if (encoder && (encoder->encoder_id == id))
        {
            return encoder;
        }
    }

    return NULL;
}

static drmModeModeInfo *connector_find_mode(struct device *dev, uint32_t con_id, const char *mode_str,const float vrefresh)
{
    drmModeConnector * connector;
    drmModeModeInfo *  mode;

    int i;

    connector = get_connector_by_id(dev, con_id);
    if ((!connector) || (!connector->count_modes))
    {
        return NULL;
    }
    /* Pick by Index */
    if (mode_str[0] == '#')
    {
        int index = atoi(mode_str + 1);

        if ((index >= connector->count_modes) || (index < 0))
        {
            return NULL;
        }
        return &connector->modes[index];
    }

    /* Pick by Name */
    for (i = 0; i < connector->count_modes; i++)
    {
        mode = &connector->modes[i];
        if (!strcmp(mode->name, mode_str))
        {
            /* If the vertical refresh frequency is not specified
             * then return the first mode that match with the name.
             * Else, return the mode that match the name and
             * the specified vertical refresh frequency.
             */
            if (vrefresh == 0)
            {
                return mode;
            }
            else if (fabs(mode_vrefresh(mode) - vrefresh) < 0.005)
            {
                return mode;
            }
            else
            {
                ; //Do nothing
            }
        }
    }

    return NULL;
}

static struct crtc *pipe_find_crtc(struct device *dev, struct pipe_arg *pipe)
{
    uint32_t possible_crtcs = ~0;
    uint32_t active_crtcs   = 0;
    unsigned int crtc_idx;
    unsigned int i;

    int j;

    for (i = 0; i < pipe->num_cons; ++i)
    {
        uint32_t crtcs_for_connector = 0;

        drmModeConnector * connector;

        drmModeEncoder * encoder;

        int idx;

        connector = get_connector_by_id(dev, pipe->con_ids[i]);
        if (!connector)
        {
            return NULL;
        }

        for (j = 0; j < connector->count_encoders; ++j)
        {
            encoder = get_encoder_by_id(dev, connector->encoders[j]);
            if (!encoder)
            {
                continue;
            }

            crtcs_for_connector |= encoder->possible_crtcs;

            idx = get_crtc_index(dev, encoder->crtc_id);
            if (idx >= 0)
            {
                active_crtcs |= (1 << idx);
            }
        }

        possible_crtcs &= crtcs_for_connector;
    }

    if (!possible_crtcs)
    {
        return NULL;
    }

    /* Return the first possible and active CRTC if one exists, or the first
     * possible CRTC otherwise.
     */
    if (possible_crtcs & active_crtcs)
    {
        crtc_idx = __builtin_ffs(possible_crtcs & active_crtcs);
    }
    else
    {
        crtc_idx = __builtin_ffs(possible_crtcs);
    }

    return &dev->resources->crtcs[crtc_idx - 1];
}

static int pipe_find_crtc_and_mode(struct device *dev, struct pipe_arg *pipe)
{
    drmModeModeInfo * mode = NULL;
    int i;
    pipe->mode = NULL;
    for (i = 0; i < (int)pipe->num_cons; i++)
    {
        mode = connector_find_mode(dev, pipe->con_ids[i],
                pipe->mode_str, pipe->vrefresh);
        if (mode == NULL)
        {
            if (pipe->vrefresh)
            {
                fprintf(stderr,
                "pipe_find_crtc_and_mode:failed to find mode "
                "\"%s-%.2fHz\" for connector %s\n",
                pipe->mode_str, pipe->vrefresh, pipe->cons[i]);
            }
            else
            {
                fprintf(stderr,
                "pipe_find_crtc_and_mode : failed to find mode \"%s\" for connector %s\n",
                pipe->mode_str, pipe->cons[i]);
            }
            return -EINVAL;
        }
    }

    /* If the CRTC ID was specified, get the corresponding CRTC. Otherwise
     * locate a CRTC that can be attached to all the connectors.
     */
    if (pipe->crtc_id != ((uint32_t)-1))
    {
        for (i = 0; i < dev->resources->count_crtcs; i++)
        {
            struct crtc * crtc = &dev->resources->crtcs[i];

            if (pipe->crtc_id == crtc->crtc->crtc_id)
            {
                pipe->crtc = crtc;
                break;
            }
        }
    } 
    else
    {
        pipe->crtc = pipe_find_crtc(dev, pipe);
    }

    if (!pipe->crtc)
    {
        fprintf(stderr, "failed to find CRTC for pipe\n");
        return -EINVAL;
    }

    pipe->mode = mode;
    pipe->crtc->mode = mode;

    return SUCCESS;
}

static int set_mode(struct device *dev, struct pipe_arg *pipes, unsigned int count)
{
    uint32_t handles[4] = {0};
    uint32_t pitches[4] = {0};
    uint32_t offsets[4] = {0};
    unsigned int fb_id;
    struct bo * bo =NULL;
    unsigned int i;
    unsigned int j;
    int ret = 1;
    int x;
    dev->mode.width = 0;
    dev->mode.height = 0;
    dev->mode.fb_id = 0;

    for (i = 0; i < count; i++)
    {
        struct pipe_arg * pipe = &pipes[i];
        ret = pipe_find_crtc_and_mode(dev, pipe);
        if (ret < 0)
        {
            continue;
        }
        dev->mode.width += pipe->mode->hdisplay;
        if (dev->mode.height < pipe->mode->vdisplay)
        {
            dev->mode.height = pipe->mode->vdisplay;
        }
    }
    bo = bo_create(dev->fd, pipes[0].fourcc, dev->mode.width,
               dev->mode.height, handles, pitches, offsets);
    if (bo == NULL)
    {
        return FAILED;
    }
    dev->mode.bo = bo;
    ret = wr_vout_create_frameBuffer(dev->fd, dev->mode.width, dev->mode.height,
                pipes[0].fourcc, handles, pitches, offsets, &fb_id, 0);
    if (ret)
    {
        fprintf(stderr, "failed to add fb (%ux%u): %s\n",
            dev->mode.width, dev->mode.height, strerror(errno));
        return FAILED;
    }

    dev->mode.fb_id = fb_id;
    x = 0;
    for (i = 0; i < count; i++)
    {
        struct pipe_arg * pipe = &pipes[i];

        if (pipe->mode == NULL)
        {
            continue;
        }
        printf("setting mode %s-%.2fHz@%s on connectors ",
               pipe->mode->name, mode_vrefresh(pipe->mode),
               pipe->format_str);
        for (j = 0; j < pipe->num_cons; ++j)
        {
            printf("%s, ", pipe->cons[j]);
        }
        printf("crtc %d\n", pipe->crtc->crtc->crtc_id);
        ret = wr_vout_drmModeSetCrtc(dev->fd, pipe->crtc->crtc->crtc_id, fb_id,
                     x, 0, pipe->con_ids, pipe->num_cons,
                     pipe);

        /* XXX: Actually check if this is needed */
        wr_vout_drmModeDirtyFB(dev->fd, fb_id, NULL, 0);
        x += pipe->mode->hdisplay;

        if (ret)
        {
            fprintf(stderr, "failed to set mode: %s\n", strerror(errno));
            return FAILED;
        }
    }
    return ret;
}

static int parse_resolution_params(struct pipe_arg *pipe)
{
    char resolution[DATA_LEN_64];
    int len = (sizeof((char*)resolution))+1;
    pipe->vrefresh = 0;
    pipe->crtc_id = CRTC_ID;
    strcpy(pipe->format_str, pix_fmt);
    pipe->num_cons = 1;
    pipe->con_ids = calloc(pipe->num_cons, sizeof(*pipe->con_ids));
    pipe->cons = calloc(pipe->num_cons, sizeof(*pipe->cons));
    pipe->cons[0] = CONNECTOR_ID;
    sprintf(resolution, "%dx%d", g_customize.VOUT_Display_Width,g_customize.VOUT_Display_Height);
    strncpy(pipe->mode_str, (char*)resolution, len);
    pipe->mode_str[len] = '\0';
    pipe->fourcc = util_format_fourcc(pipe->format_str);
    if (pipe->fourcc == 0)
    {
        fprintf(stderr, "unknown format %s\n", pipe->format_str);
        return FAILED;
    }

    return SUCCESS;
}

static bool format_support(const drmModePlanePtr ovr, uint32_t fmt)
{
    unsigned int i;

    for (i = 0; i < ovr->count_formats; ++i)
    {
        if (ovr->formats[i] == fmt)
        {
            return true;
        }
    }

    return false;
}

static int bo_fb_create(int fd, unsigned int fourcc, const uint32_t w, const uint32_t h, struct bo ** out_bo, unsigned int * out_fb_id)
{
    uint32_t handles[4] = {0};
    uint32_t pitches[4] = {0};
    uint32_t offsets[4] = {0};
    struct bo * bo;
    unsigned int fb_id;
    bo = bo_create(fd, fourcc, w, h, handles, pitches, offsets);

    if (bo == NULL)
    {
        PRINT_ERROR("bo creation failed\n");
        return FAILED;
    }

    if (wr_vout_create_frameBuffer(fd, w, h, fourcc, handles, pitches, offsets, &fb_id, 0)) 
    {
        PRINT_ERROR("failed to add fb (%ux%u): %s\n", w, h, strerror(errno));
        bo_destroy(bo);
        return FAILED;
    }

    *out_bo = bo;
    *out_fb_id = fb_id;

    return SUCCESS;
}

static int set_plane(struct device * dev, struct plane_arg * p)
{
    drmModePlane * ovr;
    uint32_t plane_id;
    struct crtc * crtc = NULL;
    unsigned int i;
    unsigned int crtc_mask;
    /* Find an unused plane which can be connected to our CRTC. Find the
     * CRTC index first, then iterate over available planes.
     */
    crtc = get_crtc_by_id(dev, p->crtc_id);

    if (!crtc)
    {
        PRINT_ERROR("CRTC %u not found\n", p->crtc_id);
        return FAILED;
    }

    crtc_mask = get_crtc_mask(dev, crtc);
    plane_id  = p->plane_id;

    for (i = 0; i < dev->resources->count_planes; i++) 
    {
        ovr = dev->resources->planes[i].plane;

        if (!ovr)
        {
            continue;
        }

        if (plane_id && (plane_id != ovr->plane_id))
        {
            continue;
        }

        if (!format_support(ovr, p->fourcc))
        {
            continue;
        }

        if ((ovr->possible_crtcs & crtc_mask) &&
            ((ovr->crtc_id == 0) || (ovr->crtc_id == p->crtc_id)))
        {
            plane_id = ovr->plane_id;
            break;
        }
    }
    if (i == dev->resources->count_planes) 
    {
        PRINT_ERROR("no unused plane available for CRTC %u\n", p->crtc_id);
        return FAILED;
    }

    DEBUG_PRINT("Display %dx%d@%s on plane %u\n",
        p->w, p->h, p->format_str, plane_id);
    /* just use single plane format for now.. */
    if (bo_fb_create(dev->fd, p->fourcc, p->w, p->h, &p->bo, &p->fb_id))
    {
        PRINT_ERROR("bo_fb creation failed\n");
        return FAILED;
    }

    crtc_w = (int)(p->w * p->scale);
    crtc_h = (int)(p->h * p->scale);

    if (!p->has_position) 
    {
        /* Default to the middle of the screen */
        crtc_x = (crtc->mode->hdisplay - crtc_w) / 2;
        crtc_y = (crtc->mode->vdisplay - crtc_h) / 2;
    } else 
    {
        crtc_x = p->x;
        crtc_y = p->y;
    }

    ovr->crtc_id = p->crtc_id;

    return SUCCESS;
}

static int set_plane_properties(struct plane_arg * plane, uint32_t plane_id)
{
    plane->plane_id     = plane_id;
    plane->crtc_id      = CRTC_ID;
    plane->has_position = true;
    plane->scale        = SCALE;

    switch(plane_id)
    {
        case 34:
        #if(CDNN)
            if(((true == g_customize.CPU_Load_Enable)  || (true == g_customize.CDNN_Load_Enable)) 
                && (g_frame_width >= 1280) && (g_frame_height >= 720))
        #else
            if((true == g_customize.CPU_Load_Enable) &&
            ((g_frame_width >= 1280) && (g_frame_height >= 720)))
        #endif   
            {    
                plane->w = 1280;
                plane->h = 720;
            }
            else
            {
                plane->w = g_frame_width;
                plane->h = g_frame_height;
            }
            plane->x = g_customize.VOUT_Pos_X;
            plane->y = g_customize.VOUT_Pos_Y;
            strcpy(plane->format_str, pix_fmt);
            break;
        case 36:
            plane->w = 1280;
            plane->h = 360;
            plane->x = 0;
            plane->y = 720;
            strcpy(plane->format_str, "BG24");
            break;
        case 38:
            plane->w = 640;
            plane->h = 1080;
            plane->x = 1280;
            plane->y = 0;
            strcpy(plane->format_str, "BG24");
            break;
        default:
            printf("Plane ID (%d) Error \n", plane_id);
            break;
    };
    plane->fourcc = util_format_fourcc(plane->format_str);

    if (plane->fourcc == 0)
    {
        PRINT_ERROR("unknown format %s\n", plane->format_str);
        return -EINVAL;
    }

    return SUCCESS;
}

static drmModeConnector *get_connector_by_name(struct device *dev, const char *name)
{
    struct connector * connector;

    int i;

    for (i = 0; i < dev->resources->count_connectors; i++)
    {
        connector = &dev->resources->connectors[i];

        if (strcmp(connector->name, name) == 0)        
        {
            return connector->connector;
        }
    }

    return NULL;
}

static int pipe_resolve_connectors(struct device *dev, struct pipe_arg *pipe)
{
    drmModeConnector * connector;
    unsigned int i;
    uint32_t id;
    char * endp;
    for (i = 0; i < pipe->num_cons; i++)
    {
        id = strtoul(pipe->cons[i], &endp, 10);
        if (endp == pipe->cons[i])
        {
            connector = get_connector_by_name(dev, pipe->cons[i]);
            if (!connector)
            {
                fprintf(stderr, "no connector named '%s'\n",
                    pipe->cons[i]);
                return -ENODEV;
            }

            id = connector->connector_id;
        }

        pipe->con_ids[i] = id;
    }

    return SUCCESS;
}

/**********************************************************************************************************************
/* Function Name : vout_pipes */
/******************************************************************************************************************//**
 * @brief       Vout pipes initialization
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
***********************************************************************************************************************/
int vout_pipes(void)
{
    pipe_args = realloc(pipe_args,
                (pipe_count + 1) * (sizeof *pipe_args));
    if (pipe_args == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        return FAILED;
    }

    if (parse_resolution_params(&pipe_args[pipe_count]) < 0)
    {
        printf("Error : parse_resolution_params() Unable to parse resolution parameters \n");
    }
    pipe_count++;
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function vout_pipes
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : vout_setup */
/******************************************************************************************************************//**
 * @brief       Vout initial setup based on customization parameters
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
***********************************************************************************************************************/
int vout_setup(void)
{
#if (CDNN)
    if((true == g_customize.CDNN_Load_Enable) || (true == g_customize.CPU_Load_Enable))
#else
    if(true == g_customize.CPU_Load_Enable)
#endif
    {
        plane_count         = 3;
    }
    
    switch (g_customize.VOUT_Display_Format)
    {
        case 0:
            pixfmt_bpp = 2;
            strcpy(pix_fmt,"YUYV");
            break;

        case 1:
            pixfmt_bpp = 2;
            strcpy(pix_fmt,"UYVY");
            break;

        case 2:
            pixfmt_bpp = 3;
            strcpy(pix_fmt,"BG24");
            break;

        default:
            pixfmt_bpp = 2;
            strcpy(pix_fmt,"YUYV");
            break;
    }
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function vout_setup
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : vout_planes */
/******************************************************************************************************************//**
 * @brief       Vout planes initialization
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
***********************************************************************************************************************/
int vout_planes(void)
{
    int plane_id            = PLANE_ID; 
    int set = 0;
    plane_args = realloc(plane_args, (plane_count + 1) * (sizeof *plane_args));
    
    if (plane_args == NULL) 
    {
        PRINT_ERROR("memory allocation failed. \n");
        return FAILED;
    }

    for (int i = 0; i < plane_count; i++)
    {
        memset(&plane_args[i], 0, sizeof(* plane_args));
        set = set_plane_properties(&plane_args[i],plane_id);
        plane_id = plane_id+2;
    }
    if (set != SUCCESS)
    {
        PRINT_ERROR("setting plane properties failed \n");
        return FAILED;
    }
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function vout_planes
 *********************************************************************************************************************/

int vout_init(void)
{
    int set_preferred       = 0;
    char * device           = NULL;
    unsigned int i          = 0;
    unsigned int count      = 0;
    unsigned int prop_count = 0;
    char module[16];
    strcpy(module, g_customize.DRM_Module);
    struct property_arg * prop_args = NULL;
    int ret = 0;

    ret = vout_setup();
    if (ret) 
    {
        PRINT_ERROR("VOUT setup failed\n");
        return FAILED;
    }

    ret = vout_pipes();
    if (ret) 
    {
        PRINT_ERROR("VOUT pipes setup failed\n");
        return FAILED;
    }

    ret = vout_planes();
    if (ret) 
    {
        PRINT_ERROR("VOUT planes setup failed\n");
        return FAILED;
    }

    memset(&dev, 0, sizeof dev);
    dev.fd = wr_vout_open(device, module);

    if (dev.fd < 0)
    {
        PRINT_ERROR("No device found \n");
        return FAILED;
    }

    dev.resources = wr_vout_getresources(&dev);
    if (!dev.resources)
    {
        wr_vout_close(dev.fd);
        PRINT_ERROR("failed to get resources\n");
        return FAILED ;
    }

    for (i = 0; i < pipe_count; i++)
    {
        if (pipe_resolve_connectors(&dev, &pipe_args[i]) < 0)
        {
            wr_vout_free_resources(dev.resources);
            wr_vout_close(dev.fd);
            return FAILED;
        }
    }
    
    for (i = 0; i < prop_count; ++i)
    {
        wr_vout_set_property(&dev, &prop_args[i]);
    }

    if (set_preferred || count || plane_count) 
    {
        uint64_t cap = 0;

        ret = wr_vout_drmGetCap(dev.fd, DRM_CAP_DUMB_BUFFER, &cap);

        if (ret || (cap == 0))
        {
            PRINT_ERROR("driver doesn't support the dumb buffer API\n");
            return FAILED;
        }

    }

    if (set_mode(&dev, pipe_args, pipe_count))
    {
        PRINT_ERROR("Vout mode not set\n");
        return FAILED;
    }

    for (i = 0; i < plane_count; i++)
    {
        if (set_plane(&dev, &plane_args[i]))
        {
            PRINT_ERROR("Vout plane not set\n");
            return FAILED;
        }
        
        if (wr_vout_setPlane(&dev, &plane_args[i]))
        {
            PRINT_ERROR("Video output Execution failed \n");
            return FAILED;
        }
    }

    return SUCCESS;
}

/**********************************************************************************************************************
/* Function Name : vout_deinit */
/******************************************************************************************************************//**
 * @brief       Vout deinitialization
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
***********************************************************************************************************************/
int64_t vout_deinit(void)
{
    int i = 0;
    for (i = 0; i < plane_count; i++)
    {
        wr_vout_clear_planes(&dev, &plane_args[i]);
    }
    wr_vout_free_resources(dev.resources);
    wr_vout_close(dev.fd);
    return SUCCESS;
}
/**********************************************************************************************************************
 End of function vout_deinit
 *********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name : execute */
/******************************************************************************************************************//**
 * @brief       Vout execution
 * @param[in]   none
 * @param[out]  none
 * @retval      true            success
 * @retval      false           fail
***********************************************************************************************************************/
int execute(void)
{
    int i = 0;
    for (i = 0; i < plane_count; i++)
    {
        switch(plane_args[i].plane_id)
        {
            case 34:
                if(true == g_customize.CPU_Load_Enable) 
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_vout_in, (size_t) (1280 * 720 * pixfmt_bpp));
                    
                }
#if (CDNN)
                else if(true == g_customize.CDNN_Load_Enable) 
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_vout_in, (size_t) (1280 * 720 * pixfmt_bpp));
                    
                }
#endif
                else if(plane_count == 4)
                {
                    memcpy(plane_args[i].bo->ptr, (char *)mul_plane_buffer_1, (size_t) (960 * 540 * BPP_RGB));
                }
                else
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_vout_in, (size_t) (g_frame_width * g_frame_height * pixfmt_bpp));
                }
                break;

            case 36:
                if(true == g_customize.CPU_Load_Enable)
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_graph_buffer, (size_t) (1280 * 360 * BPP_RGB));
                }
#if (CDNN)
                else if(true == g_customize.CDNN_Load_Enable) 
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_graph_buffer, (size_t) (1280 * 360 * BPP_RGB));
                    
                }
#endif
                else if(plane_count == 4)
                {
                    //memset(mul_plane_buffer_2,255,960*540*3);
                    memcpy(plane_args[i].bo->ptr, (char *)mul_plane_buffer_2, (size_t) (960 * 540 * BPP_RGB));
                } 
                memcpy(plane_args[i].bo->ptr, (char *)gp_graph_buffer, (size_t) (1280 * 360 * BPP_RGB));
                break;

            case 38:
                if(true == g_customize.CPU_Load_Enable)
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_log_buffer, (size_t) (640 * 1080 * BPP_RGB));
                }
#if (CDNN)
                else if(true == g_customize.CDNN_Load_Enable) 
                {
                    memcpy(plane_args[i].bo->ptr, (char *)gp_log_buffer, (size_t) (640 * 1080 * BPP_RGB));
                }
#endif
                else if(plane_count == 4)
                {   //memset(mul_plane_buffer_3,255,960*540*3);
                    memcpy(plane_args[i].bo->ptr, (char *)mul_plane_buffer_3, (size_t) (960 * 540 * BPP_RGB));
                }
                break;
                
                case 40:
                    memcpy(plane_args[i].bo->ptr, (char *)mul_plane_buffer_4, (size_t) (960 * 540 * BPP_RGB));                
                break;

            default:
                printf("Plane ID is ERR\n");
                break;
        };
    }

    return SUCCESS;
}
/**********************************************************************************************************************
 End of function execute
 *********************************************************************************************************************/