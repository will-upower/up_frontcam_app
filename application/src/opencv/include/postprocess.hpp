#include <opencv2/opencv.hpp>
#include <ctime>
#include <iostream>
#include "pose.hpp"
#include "common.h"
#include <fstream>
using namespace std;
using namespace cv;

float heatmaps_4d[POSE_EST_CHANNEL][POSE_EST_HEATMAP_CHANNEL][POSE_EST_INFERENCE_WIDTH][POSE_EST_INFERENCE_HEIGHT];
float pafs_4d[POSE_EST_CHANNEL][POSE_EST_PAF_CHANNEL][POSE_EST_INFERENCE_WIDTH][POSE_EST_INFERENCE_HEIGHT];


namespace Pose_Estimation{
    class Pose_Estimation{
    private:
        int resize_width  = -1;
        int resize_height = -1;
        const vector<vector<int>> human_paf_id = 
        {{12, 13}, {20, 21}, {14, 15}, {16, 17}, {22, 23}, {24, 25}, {0, 1}, {2, 3}, {4, 5},
        {6, 7}, {8, 9}, {10, 11}, {28, 29}, {30, 31}, {34, 35}, {32, 33}, {36, 37}, {18, 19}, {26, 27}};
        const vector<vector<int>> human_keypoint_id = 
        {{1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {1, 8}, {8, 9}, {9, 10}, {1, 11},
                      {11, 12}, {12, 13}, {1, 0}, {0, 14}, {14, 16}, {0, 15}, {15, 17}, {2, 16}, {5, 17}};

        float resize_ratio = 1;
        float resize_ratio_h = 1;
        float resize_ratio_w = 1;
        float resize_left_padding = 0;
        float resize_top_padding = 0;

        const int stride = 8;
        const int upsample_ratio = 4;
    
    public:
        
    
        vector<Pose> run(float* heatmaps_array , float* pafs_array){
            vector<Pose> res = this->postProcess(heatmaps_array, pafs_array);
            
            return res;
        }


        vector<Pose> postProcess(float* heatmaps_array_pe , float* pafs_array_pe){

            this->resize_height = POSE_EST_IMG_HEIGHT;
            this->resize_width = POSE_EST_IMG_WIDTH;
            float input_height = g_frame_height, input_width = g_frame_width;
            this->resize_ratio_h = POSE_EST_IMG_HEIGHT / input_height;
            this->resize_ratio_w = POSE_EST_IMG_WIDTH / input_width;


            int size[] = {POSE_EST_CHANNEL, POSE_EST_HEATMAP_CHANNEL, POSE_EST_INFERENCE_WIDTH, 
                          POSE_EST_INFERENCE_HEIGHT};
            Mat heatmap(4, size, CV_32FC4);

            int x = 0;
            for (int i = 0; i < POSE_EST_CHANNEL; i++)
            {
                for (int j = 0; j < POSE_EST_HEATMAP_CHANNEL; j++)
                {
                    for (int k = 0; k < POSE_EST_INFERENCE_WIDTH; k++)
                    {
                        for (int l = 0; l < POSE_EST_INFERENCE_HEIGHT; l++)
                        {                      
                            heatmaps_4d[i][j][k][l] = heatmaps_array_pe[x];
                            x++;
                        }
                    }
                }
            }

            Mat heatmaps(4, size, CV_32FC1, heatmaps_4d);

            vector<Mat> heatmaps_channels(heatmaps.size[1] - 1);
            for(int i = 0; i<heatmaps.size[1] - 1; i++){
    		    Mat heatMap(heatmaps.size[2], heatmaps.size[3], CV_32F, 
                    reinterpret_cast<float*>(const_cast<uchar*>(heatmaps.ptr(0, i))));
                resize(heatMap, heatMap, Size(0, 0), this->upsample_ratio, 
                    this->upsample_ratio, INTER_CUBIC);
                heatmaps_channels[i] = heatMap;
            }

            int size2[] = {POSE_EST_CHANNEL, POSE_EST_PAF_CHANNEL, POSE_EST_INFERENCE_WIDTH,
                            POSE_EST_INFERENCE_HEIGHT};
            
            int y = 0;
            for (int i = 0; i < POSE_EST_CHANNEL; i++)
            {
                for (int j = 0; j < POSE_EST_PAF_CHANNEL; j++)
                {
                    for (int k = 0; k < POSE_EST_INFERENCE_WIDTH; k++)
                    {
                        for (int l = 0; l < POSE_EST_INFERENCE_HEIGHT; l++)
                        {               
       
                            pafs_4d[i][j][k][l] = pafs_array_pe[y];
                            y++;
                        }
                    }
                }
            }
            
            Mat pafs(4, size2, CV_32FC1, pafs_4d);

            vector<Mat> pafs_channels(pafs.size[1]);
            for(int i = 0; i<pafs.size[1]; i++){
    		    Mat paf(pafs.size[2], pafs.size[3], CV_32F, 
                    reinterpret_cast<float*>(const_cast<uchar*>(pafs.ptr(0,i))));
                resize(paf, paf, Size(0, 0), this->upsample_ratio,
                    this->upsample_ratio, INTER_CUBIC);
                pafs_channels[i] = paf;
            }

            vector<vector<keypoint>> count_kpoint_type; 
            int all_kpoint_count = 0;
            for(int i = 0; i < heatmaps_channels.size(); i++)
            {
                all_kpoint_count += this->get_keypoint(heatmaps_channels[i],
                count_kpoint_type, all_kpoint_count); 
            }

            Mat value_pose;
            vector<keypoint> total_kpoints;
            value_pose = this->keypoints_cluster(count_kpoint_type, pafs_channels, value_pose, total_kpoints);

            for(int keypoint_id = 0;keypoint_id< total_kpoints.size();keypoint_id++){
                total_kpoints[keypoint_id].point.x *= this->stride / this->upsample_ratio;
                total_kpoints[keypoint_id].point.x /= this->resize_ratio_w;
                total_kpoints[keypoint_id].point.y *= this->stride / this->upsample_ratio;                
                total_kpoints[keypoint_id].point.y /= this->resize_ratio_h;
            }

            vector<Pose> pose_data;
            for(int i=0; i<value_pose.rows; i++){
                vector<Point> keypoints_pose(Pose::count_keypoints,Point(-1, -1));
                int valid_num = 0;
                for(int keypoint_id = 0; keypoint_id < Pose::count_keypoints; keypoint_id++){
                    if(value_pose.at<float>(i, keypoint_id) != -1){
                        keypoints_pose[keypoint_id] = total_kpoints[(int)value_pose.at<float>(i, keypoint_id)].point;
                        valid_num++;
                    }
                }
                if(valid_num >= Pose::count_keypoints / 2){
                    Pose p(keypoints_pose, value_pose.at<float>(i, 18));
                    pose_data.push_back(p);
                }
            }
            return pose_data;
        }

        template<typename T> vector<int> argsort(const vector<T>& array){
    	    const int array_len(array.size());
    	    vector<int> array_index(array_len, 0);
	        for (int i = 0; i < array_len; ++i)
            {
		        array_index[i] = i;
            }

        	sort(array_index.begin(), array_index.end(),
	        	[&array](int pos1, int pos2) {return (array[pos1] < array[pos2]);});
    	    return array_index;
        }

/**********************************************************************************************************************
/* Function Name :  keypoints_cluster */
/******************************************************************************************************************//**
 * @brief       grouping keypoints
 * @param[in]   const vector<vector<keypoint>>& count_kpoint_type, const vector<Mat> &pafs_channels, 
                Mat &data_filtered, vector<keypoint> &total_kpoints, int pose_entry_size, float min_paf_score    
 * @param[out]  none
 * @retval      Mat data_filtered
 ***********************************************************************************************************************/

        Mat keypoints_cluster(const vector<vector<keypoint>>& count_kpoint_type, 
                        const vector<Mat> &pafs_channels, 
                        Mat &data_filtered, vector<keypoint> &total_kpoints,
                        int pose_entry_size=20, float min_paf_score=0.2){
            total_kpoints.clear();
            for(int i = 0; i < count_kpoint_type.size(); i++)
            {
                for(int j = 0; j < count_kpoint_type[i].size(); j++)
                {
                    total_kpoints.push_back(count_kpoint_type[i][j]);
                }
            }
            
            int limb_point = 12;
            
            Mat value_pose;
            for(int part_id = 0; part_id < human_paf_id.size(); part_id++)
            {
                vector<Mat> part_pafs{pafs_channels[human_paf_id[part_id][0]], 
                pafs_channels[human_paf_id[part_id][1]]};
                vector<keypoint> keypoint_a = count_kpoint_type[human_keypoint_id[part_id][0]];
                vector<keypoint> keypoint_b = count_kpoint_type[human_keypoint_id[part_id][1]];

                int n = keypoint_a.size();
                int m = keypoint_b.size();
                if(n == 0 || m == 0)
                {
                    continue;
                }
                
                Mat a(m, n, CV_32FC2);
                for(int i = 0; i < m; i++)
                {
                    for(int j = 0; j < n; j++)
                    {
                        a.at<Vec2f>(i,j) = Vec2f(keypoint_a[j].point.x, keypoint_a[j].point.y);
                    }
                }

                Mat b(m, n, CV_32FC2);
                for(int i = 0; i < m; i++)
                {
                    for(int j = 0; j < n; j++)
                    {
                        b.at<Vec2f>(i,j) = Vec2f(keypoint_b[i].point.x, keypoint_b[i].point.y);
                    }
                }

                Mat vector_r = Mat(b - a).reshape(0, m * n);

                Mat steps = 1.0 / (limb_point -1) * vector_r;
                Mat broadcast_steps(steps.rows, limb_point, steps.type());
                for(int j = 0; j < limb_point; j++)
                {
                    steps.copyTo(broadcast_steps.col(j));
                }
                Mat grid(broadcast_steps.rows, limb_point, broadcast_steps.type());
                for(int i = 0; i<grid.rows; i++)
                {
                    for(int j = 0; j<grid.cols; j++)
                    {
                        grid.at<Vec2f>(i,j)=Vec2f(j,j);
                    }
                }

                a = a.reshape(0, m*n);
                Mat a_broadcast(a.rows,limb_point, a.type());
                for(int j = 0; j < a_broadcast.cols; j++)
                {
                    a.copyTo(a_broadcast.col(j));
                }
                Mat points = broadcast_steps.mul(grid) + a_broadcast;
                points.convertTo(points,CV_32S);

                vector<Mat> xy_channels;
                split(points,xy_channels);
                Mat x = xy_channels[0].reshape(0, xy_channels[0].total());
                Mat y = xy_channels[1].reshape(0, xy_channels[1].total());

                Mat field(x.rows, 1, CV_32FC2);
                for(int i = 0; i < x.rows; i++)
                {
                    int x_index = x.at<int>(i,0);
                    int y_index = y.at<int>(i,0);
                    field.at<Vec2f>(i, 0) = 
                        Vec2f(part_pafs[0].at<float>(y_index,x_index), part_pafs[1].at<float>(y_index,x_index));
                }
                field = field.reshape(0, field.rows*field.cols/limb_point);

                Mat vec_norm = vector_r.clone();
                for(int i = 0; i < vec_norm.rows;i ++)
                {
                    for(int j = 0; j < vec_norm.cols; j++)
                    {
                        float norm = sqrt(pow(vec_norm.at<Vec2f>(i, j)[0], 2) + 
                            pow(vec_norm.at<Vec2f>(i, j)[1], 2));
                        vec_norm.at<Vec2f>(i, j) = Vec2f(1 / (norm+1e-6), 1 / (norm+1e-6));
                    }
                }
                    
                Mat vec = vector_r.mul(vec_norm);
                Mat vec_broadcast(vec.rows, limb_point, vec.type());
                for(int i = 0; i < vec_broadcast.cols; i++)
                {
                    vec.copyTo(vec_broadcast.col(i));
                }
                
                Mat score_val_affinity(field.rows, field.cols, CV_32FC1);
                Mat field_dot_vec = field.mul(vec_broadcast);
                for(int i = 0; i < score_val_affinity.rows; i++)
                {
                    for(int j = 0; j < score_val_affinity.cols; j++)
                    {
                        score_val_affinity.at<float>(i, j) = field_dot_vec.at<Vec2f>(i, j)[0]
                         + field_dot_vec.at<Vec2f>(i, j)[1];
                    }
                }
                
                score_val_affinity = score_val_affinity.reshape(0, score_val_affinity.total() / limb_point);
                Mat valid_score_val_affinity = (score_val_affinity > min_paf_score);
                valid_score_val_affinity.convertTo(valid_score_val_affinity, CV_32F, 1.0 / 255);
                Mat valid_num;
    			reduce(valid_score_val_affinity, valid_num, 1, REDUCE_SUM, CV_32F);
                reduce(score_val_affinity.mul(valid_score_val_affinity), score_val_affinity, 1, REDUCE_SUM,CV_32F);                
                score_val_affinity = score_val_affinity.mul(1 / (valid_num + 1e-6));
                Mat success_ratio = valid_num / limb_point;

                Mat limb_val;
                findNonZero((score_val_affinity > 0) & (success_ratio >= 0.9), limb_val);
                vector<Mat> limb_val_channels;

                split(limb_val, limb_val_channels);
                if(limb_val_channels.size() == 0)
                {
                    continue;
                }
                limb_val = limb_val_channels[1];


                vector<int> limb_id_b(limb_val.rows), limb_id_a(limb_val.rows);

                for(int i = 0; i < limb_val.rows; i++)
                {
                    limb_id_b[i] = limb_val.at<int>(i) / n;
                    limb_id_a[i] = limb_val.at<int>(i) % n;
                }

                vector<float> val_limb_affinity_score;
                for(int i = 0; i <limb_val.rows; i++){
                    val_limb_affinity_score.push_back(score_val_affinity.at<float>(limb_val.at<int>(i)));
                }
                nms_function(limb_id_a, limb_id_b, val_limb_affinity_score);

                int connections_len = val_limb_affinity_score.size();
                if(connections_len == 0)
                {
                    continue;
                }

                if(part_id == 0)
                {
                    value_pose = Mat(connections_len, pose_entry_size, CV_32FC1, 1);
                    value_pose *= -1;
                    for(int i = 0; i < connections_len; i++){
                        value_pose.at<float>(i, human_keypoint_id[0][0]) = keypoint_a[limb_id_a[i]].idx;
                        value_pose.at<float>(i, human_keypoint_id[0][1]) = keypoint_b[limb_id_b[i]].idx;
                        value_pose.at<float>(i, value_pose.cols - 1) = 2;
                        value_pose.at<float>(i, value_pose.cols - 2) = total_kpoints[keypoint_a[limb_id_a[i]].idx].conf
                         + total_kpoints[keypoint_b[limb_id_b[i]].idx].conf + val_limb_affinity_score[i];
                    }
                }
                else if (part_id == 17 || part_id == 18)
                {
                    int kpt_a_id = human_keypoint_id[part_id][0];
                    int kpt_b_id = human_keypoint_id[part_id][1];
                    for(int i = 0; i <connections_len; i++)
                    {
                        for(int j = 0; j < value_pose.rows; j++)
                        {
                            if(value_pose.at<float>(j,kpt_a_id) == keypoint_a[limb_id_a[i]].idx && 
                                value_pose.at<float>(j,kpt_b_id) == -1)
                            {    
                                value_pose.at<float>(j,kpt_b_id) =  keypoint_b[limb_id_b[i]].idx;
                            }
                            else if(value_pose.at<float>(j,kpt_b_id) == keypoint_b[limb_id_b[i]].idx && 
                                value_pose.at<float>(j,kpt_a_id) == -1)
                            {
                                value_pose.at<float>(j,kpt_a_id) == keypoint_a[limb_id_a[i]].idx;
                            }
                        }
                    }
                    continue;
                }
                else
                {
                    int kpt_a_id = human_keypoint_id[part_id][0];
                    int kpt_b_id = human_keypoint_id[part_id][1];
                    for(int i = 0; i < connections_len; i++)
                    {
                        int num = 0;
                        for(int j = 0; j<value_pose.rows; j++)
                        {
                            if(value_pose.at<float>(j,kpt_a_id) == keypoint_a[limb_id_a[i]].idx)
                            {
                                value_pose.at<float>(j,kpt_b_id) = keypoint_b[limb_id_b[i]].idx;
                                num++;
                                value_pose.at<float>(j, value_pose.cols - 1) += 1;
                                value_pose.at<float>(j, value_pose.cols - 2) += 
                                total_kpoints[keypoint_b[limb_id_b[i]].idx].conf + val_limb_affinity_score[i];
                            }
                        }
                        if(num == 0)
                        {
                            Mat pose_entry(1, pose_entry_size, CV_32FC1, 1);
                            pose_entry *= -1;
                            pose_entry.at<float>(0, kpt_a_id) = keypoint_a[limb_id_a[i]].idx;
                            pose_entry.at<float>(0, kpt_b_id) = keypoint_b[limb_id_b[i]].idx;
                            pose_entry.at<float>(0, pose_entry.cols - 1) = 2;
                            pose_entry.at<float>(0, pose_entry.cols - 2) = total_kpoints[keypoint_a[limb_id_a[i]].idx].conf
                             + total_kpoints[keypoint_b[limb_id_b[i]].idx].conf + val_limb_affinity_score[i];
                            value_pose.push_back(pose_entry);
                        }
                    }
                }
            }

            data_filtered = Mat(0, pose_entry_size, CV_32FC1);
            for(int i = 0; i < value_pose.rows; i++)
            {
                if(value_pose.at<float>(i, value_pose.cols-1)<3 || 
                    value_pose.at<float>(i, value_pose.cols-2)/value_pose.at<float>(i, value_pose.cols-1)<0.2)
                {
                    continue;
                }
                data_filtered.push_back(value_pose.row(i));
            }
            
            return data_filtered;

        }
/**********************************************************************************************************************
 * End of function keypoints_cluster
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  get_keypoint */
/******************************************************************************************************************//**
 * @brief       filtering keypoint values
 * @param[in]   Mat& heatmap, vector<vector<keypoint>>& total_kpoints, 
                int& all_kpoint_count    
 * @param[out]  none
 * @retval      int keypoint_num
 ***********************************************************************************************************************/
        int get_keypoint(Mat& heatmap, vector<vector<keypoint>>& total_kpoints, 
            int& all_kpoint_count)
        {
            threshold(heatmap, heatmap, 0.1, 0, THRESH_TOZERO);
            Mat heatmap_with_borders;
            copyMakeBorder(heatmap, heatmap_with_borders, 2, 2, 2, 2, BORDER_CONSTANT);
            
            Mat heatmap_center = heatmap_with_borders(Range(1, heatmap_with_borders.rows - 1), 
                Range(1, heatmap_with_borders.cols - 1));
            Mat heatmap_left = heatmap_with_borders(Range(1, heatmap_with_borders.rows - 1), 
                Range(2, heatmap_with_borders.cols));
            Mat heatmap_right = heatmap_with_borders(Range(1, heatmap_with_borders.rows - 1), 
                Range(0, heatmap_with_borders.cols - 2));
            Mat heatmap_up = heatmap_with_borders(Range(2, heatmap_with_borders.rows), 
                Range(1, heatmap_with_borders.cols - 1));
            Mat heatmap_down = heatmap_with_borders(Range(0, heatmap_with_borders.rows - 2), 
                Range(1, heatmap_with_borders.cols - 1));

            Mat heatmap_peaks = (heatmap_center > heatmap_left)
                    & (heatmap_center > heatmap_right)
                    & (heatmap_center > heatmap_up)
                    & (heatmap_center > heatmap_down);
            heatmap_peaks = heatmap_peaks(Range(1, heatmap_center.rows - 1), 
                Range(1, heatmap_center.cols - 1));
            Mat keypoints_mat;
            findNonZero(heatmap_peaks,keypoints_mat);
            vector<Point> keypoints(keypoints_mat.total());
            for(int i=0; i<keypoints_mat.total(); i++)
            {
                keypoints[i] = keypoints_mat.at<Point>(i);
            }
            sort(keypoints.begin(),keypoints.end(),
                [](const Point& a, const Point& b)
                {return a.x<b.x;});
            vector<int> suppressed(keypoints.size());
            vector<keypoint> keypoints_with_score_and_id;
            int keypoint_num = 0;
            for(int i=0; i<keypoints.size(); i++)
            {
                if(suppressed[i])
                {
                    continue;
                }
                for(int j=i+1; j<keypoints.size(); j++)
                {
                    if(sqrt(pow(keypoints[i].x - keypoints[j].x, 2)+pow(keypoints[i].y - keypoints[j].y, 2)) < 6)
                    {
                        suppressed[j] = 1;
                    }
                }
                keypoints_with_score_and_id.push_back({keypoints[i], heatmap.at<float>(keypoints[i].y,
                 keypoints[i].x), all_kpoint_count + keypoint_num});
                keypoint_num++;
            }
            total_kpoints.push_back(keypoints_with_score_and_id);
            
            return keypoint_num;

        }
/**********************************************************************************************************************
 * End of function get_keypoint
 ********************************************************************************************************************/

/**********************************************************************************************************************
/* Function Name :  nms_function */
/******************************************************************************************************************//**
 * @brief       nms algorithm 
 * @param[in]   vector<int>& limb_id_a, vector<int>& limb_id_b, vector<float>& score_val_affinity
 * @param[out]  none
 * @retval      none
 ***********************************************************************************************************************/        
        
        void nms_function(vector<int>& limb_id_a, vector<int>& limb_id_b, vector<float>& score_val_affinity)
        {
            vector<int> order = argsort(score_val_affinity);
            reverse(order.begin(), order.end());

            vector<int> new_limb_id_a,new_limb_id_b;
            vector<float> new_score_val_affinity;
            for(int i=0; i<order.size(); i++)
            {
                new_limb_id_a.push_back(limb_id_a[order[i]]);
                new_limb_id_b.push_back(limb_id_b[order[i]]);
                new_score_val_affinity.push_back(score_val_affinity[order[i]]);
            }

            vector<int> idx;
            set<int> has_kpt_a;
            set<int> has_kpt_b;

            for(int i = 0; i < order.size(); i++)
            {
                if((has_kpt_a.find(limb_id_a[i]) == has_kpt_a.end()) && 
                (has_kpt_b.find(limb_id_b[i]) == has_kpt_b.end()))
                {
                    idx.push_back(i);
                    has_kpt_a.insert(limb_id_a[i]);
                    has_kpt_b.insert(limb_id_b[i]);
                }
            }
            limb_id_a.resize(idx.size());
            limb_id_b.resize(idx.size());
            score_val_affinity.resize(idx.size());
            for(int i=0; i<idx.size(); i++)
            {
                limb_id_a[i] = new_limb_id_a[idx[i]];
                limb_id_b[i] = new_limb_id_b[idx[i]];
                score_val_affinity[i] = new_score_val_affinity[idx[i]];
            }
        }

    };
}
/**********************************************************************************************************************
 * End of function nms_function
 ********************************************************************************************************************/
