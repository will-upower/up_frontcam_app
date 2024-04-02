__kernel void Conv_YUYV2RGB(__global uchar* yuyv, __global uchar* bgr, int width, int height)
{
    int gid_x = get_global_id(0);
    int gid_y = get_global_id(1);
    
    int index = gid_y * width + gid_x;
    int z = 0;
    
    if (gid_x < width && gid_y < height)
    {
        int r, g, b, y, u, v;
        
        if (!z)
        {
            y = yuyv[index * 2] << 8;
        }
        else
        {
            y = yuyv[index * 2 + 2] << 8;
        }
        
        u = yuyv[index * 2 + 1] - 128;
        v = yuyv[index * 2 + 3] - 128;
        
        r = (y + (359 * v)) >> 8;
        g = ((y - (88 * u)) - (183 * v)) >> 8;
        b = (y + (454 * u)) >> 8;
        
        bgr[index * 3] = convert_uchar_sat(r > 255 ? 255 : (r < 0 ? 0 : r));
        bgr[index * 3 + 1] = convert_uchar_sat(g > 255 ? 255 : (g < 0 ? 0 : g));
        bgr[index * 3 + 2] = convert_uchar_sat(b > 255 ? 255 : (b < 0 ? 0 : b));
        
        if (z++)
        {
            z = 0;
        }
    }
}