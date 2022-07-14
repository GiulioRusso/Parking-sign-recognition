/*
 
 EIID / AIA 2020 - 2021 UNICAS Traffic Sign Detection [parking]
 
 Prof. Bria Alessandro
 
 Students:
 - Camasso Giulio
 - Marzilli Alessandro
 - Russo Giulio
 
*/

#pragma once

#include "ucasConfig.h"
#include "aiaConfig.h"
#include <opencv2/core/core.hpp>


namespace parking
{

    typedef std::vector <cv::Point> object;
    typedef std::vector < object > objects;


    // ### PARSING ###

    struct data {
        float x;
        float y;
        float height;
        float width;
    };

    void copy_file(const std::string& src, const std::string& dst);

    std::string parseAttribute(std::string& line);


    // ### PREPROCESSING ###

    void mean_shift_preprocessing(cv::Mat& img, int index, const std::string& dest_path);

    cv::Mat clahe_preprocessing(cv::Mat img, int num);

    int percentile(std::vector<int> & histogram, int percVal);
    
    template <typename T>
    void create_histogram(std::vector<T>  dataset_vector,std::vector<T>& intensity_x, std::vector<T>& occurrence_y);
    
    template <typename T>
    void print_values_histogram(std::vector<T>& intensity_x,std::vector<T>& occurence_y,const std::string& name);

    // OLD
    void statistics_values_extraction(std::vector <std::string> images,std::vector <std::string> jsons);
    

    // ### ELABORATION ###

    objects & filterObjects(objects & objs, bool (*pred)(object & obj));

    // N. 1 Cartago Criterion
    bool cartagoCriterion(object& obj);

    // N. 2 Color Contrast Criterion
    objects& colorContrastCriterion(objects& candidate_objects, cv::Mat img_hsv, cv::Mat img_binary);

    // N. 3 Top-Bottom Criterion
    objects& topBottomAreaCriterion(objects& candidate_objects, cv::Mat img);

    // N. 4 Convexity Criterion
    bool convexityCriterion(object & obj);

    // N. 5 Rectangularity Criterion
    cv::RotatedRect correctedMinAreaRect(const object& obj);
    bool rectangularityCriterion(object& obj);
    
    // N. 6 Aspect Ratio Criterion
    bool aspectRatioCriterion(object& obj);

    // N. 7 Circularity Criterion
    bool circularityCriterion(object& obj);

    // N. 8 Triangularity Criterion
    bool triangularityCriterion(object& obj);

    // N. 9 Match-Shape Criterion
    objects template_countours(cv::Mat& image_template);
    bool matchShapeCriterion(object& obj);

 
    // ### PERFORMANCE ###

    std::vector<std::vector<cv::Rect>> label_signs(std::vector <std::string> jsons, int num_image);

    float score_calculation(object obj, double maxDist);

    // To print all values
    void print_launch_configuration();
}
