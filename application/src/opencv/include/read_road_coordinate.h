#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include "draw_detection.h"

extern road_map g_road_map;

int readRoadCordinate(char *file_name) {
    std::string line;                       /* string for reading each line */
    std::vector<std::vector <float>> arr;     /* vector<vector<int>> (2D arr) */
    std::vector<float> notarr;                /* vector to hold single values */

    memset((char*)&g_road_map, 0 , sizeof(g_road_map));

    std::ifstream fin(file_name);    /* open filename given by 1st argument */
    if (!fin.good()) {  /* validate file open for reading */
        std::cerr << "error: file open '"<< file_name <<"' failed.\n";
        return -1;
    }

    if (getline (fin, line)) {  /* read each remaining data line */
        float i;
        std::vector<float> tmp;           /* vector<int> for each line */
        std::stringstream ss (line);    /* stringstream to read from */
        while (ss >> i) {                /* read int from stringstream */
            tmp.push_back(i);           /* add to tmp vector */
        }
        //if (tmp.size() == MAX_ROAD_CORD_SIZE)            /* if single value in line */
            arr.push_back(tmp);         /* add tmp vector to arr */
    }

    while(getline (fin, line)) {  /* read each remaining data line */
        float i;
        std::vector<float> tmp;           /* vector<int> for each line */
        std::stringstream ss (line);    /* stringstream to read from */
        while (ss >> i) {                /* read int from stringstream */
            tmp.push_back(i);           /* add to tmp vector */
        }
        if (tmp.size() > 0)            /* if single value in line */
            arr.push_back(tmp);         /* add tmp vector to arr */
    }

    int arr_size = arr.size();

    if (arr_size < 1 || arr_size > 6) {
        cout << "Invalid cordinates, size: " << arr_size << endl;
    }
    else
    {
        cout << "VALID  cordinates, size: " << arr[0].size() << endl;
        cout << "VALID  cordinates, size: " << arr[1].size() << endl;
        cout << "VALID  cordinates, size: " << arr[2].size() << endl;
        cout << "VALID  cordinates, size: " << arr[3].size() << endl;
    }
    if (arr_size != 0) {
        
            copy(arr[0].begin(), arr[0].end(), g_road_map.road);
    
       
            g_road_map.total_det = arr[2].size();
            int index = 0;
            for (unsigned int i = 0; i < arr[2].size(); i++) {
                g_road_map.bb_data[i].start.x = arr[1][index++];
                g_road_map.bb_data[i].start.y = arr[1][index++];
                g_road_map.bb_data[i].end.x = arr[1][index++];
                g_road_map.bb_data[i].end.y = arr[1][index++];
                g_road_map.bb_data[i].score = arr[2][i];
            }
            for (unsigned int j = 0; j < 3; j++) 
            {
                index = 0;
                for (unsigned int k = 0; k < 17; k++) 
                {
                    g_road_map.pos[j].cor[k].x = arr[3+j][index++];
                    g_road_map.pos[j].cor[k].y = arr[3+j][index++];
                }
            }
    }

    return arr[2].size();
}