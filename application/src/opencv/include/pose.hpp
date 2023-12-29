#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
namespace Pose_Estimation
{

    struct keypoint{
        Point point;
        float conf;
        int idx;
        keypoint(Point point, float conf, int idx):
            point(point), conf(conf), idx(idx){}
    };

    class Pose{
    public:
        vector<Point> keypoints;
        float conf_value;
        Rect b_box;
        int draw_track_id;
        const static int count_keypoints = 18;
        Pose(vector<Point> keypoints, float conf_value=1):
            keypoints(keypoints), conf_value(conf_value){
                this->b_box = this->bound_box();                
                this->draw_track_id = -1;
        }

        Pose& operator=(const Pose& object)
        {
            this->keypoints = object.keypoints;
            this->conf_value = object.conf_value;
            this->b_box = object.b_box;
            this->draw_track_id = object.draw_track_id;
            return *this;
        }
/**********************************************************************************************************************
/* Function Name :  pose_draw */
/******************************************************************************************************************//**
 * @brief       drawing pose over image
 * @param[in]   Mat img,bool draw_draw_track_id    
 * @param[out]  none
 * @retval      none
 ***********************************************************************************************************************/

        void pose_draw(Mat &img, bool draw_draw_track_id = false)
        {
            for(int part_id = 0; part_id < this->human_keypoint_id.size() - 2; part_id++){
                int keypoint_a = this->human_keypoint_id[part_id][0];
                int gl_keypoint_a = this->keypoints[keypoint_a].x;
                if(gl_keypoint_a != -1)
                {
                    Point p_a = this->keypoints[keypoint_a];
                    circle(img, p_a, 3, Scalar(0, 255, 255), -1);
                }
                int keypoint_b = this->human_keypoint_id[part_id][1];
                int gl_keypoint_b = this->keypoints[keypoint_b].x;
                if(gl_keypoint_b != -1)
                {
                    Point p_b = this->keypoints[keypoint_b];
                    circle(img, p_b, 3, Scalar(0, 255, 255), -1);
                }
                if(gl_keypoint_a != -1 && gl_keypoint_b != -1)
                {
                    Point p_a = this->keypoints[keypoint_a];
                    Point p_b = this->keypoints[keypoint_b];
                    line(img, p_a, p_b, Scalar(0, 255, 255), 2);
                }
            }
            if(draw_draw_track_id && this->draw_track_id != -1)
            {
                putText(img, to_string(this->draw_track_id), Point(this->b_box.x - 20, this->b_box.y + 30), 
                    FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 255), 3, 8);
            }

        }
/**********************************************************************************************************************
 * End of function pose_draw
 ********************************************************************************************************************/        

    private:
        const vector<string> kpt_names{"nose", "neck",
                 "r_sho", "r_elb", "r_wri", "l_sho", "l_elb", "l_wri",
                 "r_hip", "r_knee", "r_ank", "l_hip", "l_knee", "l_ank",
                 "r_eye", "l_eye",
                 "r_ear", "l_ear"};

        const vector<vector<int>> human_keypoint_id = 
        {{1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {1, 8}, {8, 9}, {9, 10}, {1, 11},
                      {11, 12}, {12, 13}, {1, 0}, {0, 14}, {14, 16}, {0, 15}, {15, 17}, {2, 16}, {5, 17}};

/**********************************************************************************************************************
/* Function Name :  bound_box */
/******************************************************************************************************************//**
 * @brief       Getting bounding box
 * @param[in]   none    
 * @param[out]  none
 * @retval      Rect boundingRect
 ***********************************************************************************************************************/

        Rect bound_box()
        {
            vector<Point> found_keypoints(this->keypoints.size());
            auto it = copy_if(this->keypoints.begin(), this->keypoints.end(), found_keypoints.begin(),
                [](const Point& p){ return p.x != -1;});
            found_keypoints.resize(distance(found_keypoints.begin(), it));
            return boundingRect(found_keypoints);
        }
/**********************************************************************************************************************
 * End of function bound_box
 ********************************************************************************************************************/
    };

    class posedraw_tracker
    {        
    public:

/**********************************************************************************************************************
/* Function Name :  draw_track */
/******************************************************************************************************************//**
 * @brief       track pose value
 * @param[in]   vector<Pose> &pose_data, int threshold     
 * @param[out]  none
 * @retval      none
 ***********************************************************************************************************************/
        void draw_track(vector<Pose> &pose_data, int threshold = 3)
        {
            sort(pose_data.begin(), pose_data.end(),
                [](const Pose& a, const Pose& b){
                    return a.conf_value>b.conf_value;  
                });
            vector<int> mask(prev_pose_data.size(), 1);
            for(int i = 0; i < pose_data.size(); i++){
                int matched_id = -1, matched_pose_id = -1, matched_iou = -1;
                for(int j = 0; j < this->prev_pose_data.size(); j++)
                {
                    if(!mask[j])
                    {
                        continue;
                    }
                    int iou = common_pose(pose_data[i], this->prev_pose_data[j]);
                    if(iou > matched_iou)
                    {
                        matched_id = j;
                        matched_pose_id = this->prev_pose_data[j].draw_track_id;
                        matched_iou = iou;
                    }
                }
                if(matched_iou >= threshold)
                {
                    mask[matched_id] = 0;
                }
                else
                {
                    matched_pose_id = ++this->max_pose_id;
                }
               pose_data[i].draw_track_id = matched_pose_id;
            }
            this->prev_pose_data = pose_data;
        }
/**********************************************************************************************************************
 * End of function draw_track
 ********************************************************************************************************************/
    private:
        int max_pose_id = -1;
        vector<Pose> prev_pose_data;

        const vector<float> data_val{0.002704, 0.024964, 0.024964, 0.020736, 0.015376,
        0.024964, 0.020736, 0.015376, 0.04579601, 0.030276,
        0.031684, 0.04579601, 0.030276, 0.031684, 0.0025,
        0.0025, 0.0049, 0.0049};

/**********************************************************************************************************************
/* Function Name :  common_pose */
/******************************************************************************************************************//**
 * @brief       getting similalarity keypoints
 * @param[in]   const Pose& a, const Pose& b, float threshold    
 * @param[out]  none
 * @retval      int count_common_keypoint
 ***********************************************************************************************************************/
        int common_pose(const Pose& a, const Pose& b, float threshold = 0.5)
        {
            int count_common_keypoint = 0;
            for(int keypoint_id = 0; keypoint_id < Pose::count_keypoints; keypoint_id++)
            {
                if(a.keypoints[keypoint_id].x != -1 && b.keypoints[keypoint_id].x != -1)
                {
                    double distance = norm(a.keypoints[keypoint_id] - b.keypoints[keypoint_id]);
                    double area = max(a.b_box.area(), b.b_box.area());
                    double similarity = exp( -distance / (2 * area) * posedraw_tracker::data_val[keypoint_id]);
                    if(similarity > threshold)
                    {
                        count_common_keypoint++;
                    }
                }
            }
            return count_common_keypoint;
        }
    };
}
/**********************************************************************************************************************
 * End of function common_pose
 ********************************************************************************************************************/