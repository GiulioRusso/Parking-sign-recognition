/*
 
 EIID / AIA 2020 - 2021 UNICAS Traffic Sign Detection [parking]
 
 Prof. Bria Alessandro
 
 Students:
 - Camasso Giulio
 - Marzilli Alessandro
 - Russo Giulio
 
*/

#include <iostream>
#include "ucasConfig.h"
#include "config.h"
#include "functions.h"

using namespace parking;


// ### PARSING ###

void parking::copy_file(const std::string& src, const std::string& dst) {
    system(ucas::strprintf("cp %s %s", src.c_str(), dst.c_str()).c_str());
};

// Reads values from josn files
std::string parking::parseAttribute(std::string& line) {
    //printf("parsing line \"%s\"\n", line.c_str());
    ucas::clsi(line);
    line.erase(std::remove(line.begin(), line.end(), '"'), line.end());
    line.erase(std::remove(line.begin(), line.end(), ','), line.end());
    std::vector <std::string> tokens;
    ucas::split(line, ":", tokens);
    if (tokens.size() != 2)
        throw ucas::Error(ucas::strprintf("Expected 2 tokens, found %d in \"%s\"", tokens.size(), line.c_str()));
    return tokens[1];
};


// ### PREPROCESSING ###

// Mean Shift preprocessing and saving the result into a specified destination path
void parking::mean_shift_preprocessing(cv::Mat& img, int index, const std::string& dest_path) {
        cv::pyrMeanShiftFiltering(img, img, 5, 5);
        cv::imwrite(dest_path + "/" + std::to_string(index) + ".jpg", img);
};

// Clahe funcion to apply on a certain image in input
cv::Mat parking::clahe_preprocessing(cv::Mat img, int clipLimit){
    cv::Mat img_Lab;
    cv::cvtColor(img, img_Lab, cv::COLOR_BGR2Lab);
                   
    std::vector< cv::Mat > channels;
    cv::split(img_Lab, channels);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, cv::Size(8,8));
    clahe->apply(channels[0], channels[0]);
    cv::Mat img_eq_CLAHE;
    cv::merge(channels, img_eq_CLAHE);
    cv::cvtColor(img_eq_CLAHE, img_eq_CLAHE, cv::COLOR_Lab2BGR);

    //cv::namedWindow("IMG", cv::WINDOW_NORMAL)
    //cv::imshow("IMG", img_eq_CLAHE);
    //cv::waitKey(0);
    
    return img_eq_CLAHE;
}

// Percentile function used on a std::vector<int> histogram - OLD
int parking::percentile(std::vector<int> & histogram, int percVal)
{
    int sum = 0;
    for(auto & h : histogram)
        sum += h;

    int perc = 0;
    for(int acc = 0; ; perc++)
    {
        acc += histogram[perc];
        if(acc >= (percVal/100.0f)*sum)
            break;
    }

    return perc;
}

// Create the x and y axis of the histogram into two std::vector
template<typename T>
void parking::create_histogram(std::vector<T> dataset_vector,std::vector<T>& intensity_x, std::vector<T>& occurrence_y)
{
    sort(dataset_vector.begin(), dataset_vector.end());
    intensity_x = dataset_vector;
    intensity_x.erase(unique(intensity_x.begin(), intensity_x.end()), intensity_x.end());
    for (auto& it : intensity_x)
        occurrence_y.push_back(std::count(dataset_vector.begin(), dataset_vector.end(), it));

}

// Stamp of the x and y axis of the histogram to be used for external plot (eg. MatLab)
template <typename T>
void parking::print_values_histogram(std::vector<T>& intensity_x,std::vector<T>& occurrence_y,const std::string& name)
{
    std::cout << name << " = [ ";
    for (int i=0; i<intensity_x.size(); i++)
    {
        if (i == intensity_x.size() -1)
            std::cout << intensity_x[i] << " ";
        else
            std::cout << intensity_x[i] << ", ";
    }
    std::cout << "];" << std::endl;

    std::cout << "OCC_"<< name <<" = [ ";
    for (int i=0; i<occurrence_y.size(); i++)
    {
        if (i == occurrence_y.size() -1)
            std::cout << occurrence_y[i] << " ";
        else
            std::cout << occurrence_y[i] << ", ";
    }
    std::cout << "];" << std::endl;
}

// Extraction of the statistics values on the dataset - OLD

void parking::statistics_values_extraction(std::vector <std::string> images,std::vector <std::string> jsons) {
    // Histograms vectors for the statistics parameters
    std::vector<int> hue_dataset_vector;
    std::vector<int> saturation_dataset_vector;
    std::vector<int> value_dataset_vector;
    std::vector<int> area_dataset_vector;
    std::vector<float> rectangularity_dataset_vector;

    for (int subset_index = 0; subset_index < images.size(); subset_index++)
    {
        cv::Mat img = cv::imread(images[subset_index], cv::IMREAD_UNCHANGED);
        cv::Mat img_hsv = img.clone();
        cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);
        cv::Mat img_debug = img.clone();
        
        int numb_signs = 0;
        
        // ### PARSING ###
        
        std::vector<cv::Rect> rect_labels;
        rect_labels.clear();

        // NB. without the occluded ones if SKIP_OCCLUDED_AND_OOF is defined
        rect_labels = label_signs(jsons, subset_index)[0];
  
        if(!rect_labels.empty()){
            for( auto & rect_label : rect_labels){
                
                numb_signs++;
                
                // drawing debug rectangle
                cv::rectangle(img_debug, cv::Point(rect_label.x, rect_label.y), cv::Point(rect_label.x + rect_label.width, rect_label.y + rect_label.height), CV_RGB(255, 255, 0), 5);
//                 cv::imshow("PARSING DEBUG", img_debug);
//                 cv::waitKey(0);

                // sum values for each img
                int hue_sum_img = 0;
                int saturation_sum_img = 0;
                int value_sum_img = 0;
                int area_img = rect_label.height * rect_label.width;
                float rectangularity_img = rect_label.height / rect_label.width;
                for (int row = rect_label.y; row < rect_label.y + rect_label.height; row++)
                {
                    cv::Vec3b* ythRow = img_hsv.template ptr<cv::Vec3b>(row);
                    for (int col = rect_label.x; col < rect_label.x + rect_label.width; col++)
                    {
                        hue_sum_img = hue_sum_img + ythRow[col][0]; // Hue
                        saturation_sum_img = saturation_sum_img + ythRow[col][1]; // Saturation
                        value_sum_img = value_sum_img + ythRow[col][2]; // Value
                    } // for col
                } // for row

                // average for the current img
                int hue_avg_img = hue_sum_img / area_img;
                int saturation_avg_img = saturation_sum_img / area_img;
                int value_avg_img = value_sum_img / area_img;

                // saving into the dataset vector
                hue_dataset_vector.push_back(hue_avg_img);
                saturation_dataset_vector.push_back(saturation_avg_img);
                value_dataset_vector.push_back(value_avg_img);
                area_dataset_vector.push_back(area_img);
                rectangularity_dataset_vector.push_back(rectangularity_img);
            }
    
        }
        
    } // for images

    // hue histogram
    std::vector<int> hue_intensity_x,hue_occurrence_y;
    create_histogram(hue_dataset_vector, hue_intensity_x,hue_occurrence_y);
    
    // plot w/ MatLab
    print_values_histogram(hue_intensity_x, hue_occurrence_y,"HUE");
    
    // Percentile values
    int hue_perc1 = 5;
    int hue_perc2 = 95;
    int hue_perc_min = hue_intensity_x[percentile(hue_intensity_x, hue_perc1)];
    int hue_perc_max = hue_intensity_x[percentile(hue_intensity_x, hue_perc2)];
//    std::cout<<" Hue Perc Min: " << hue_perc_min << std::endl;
//    std::cout<<" Hue Perc Max: " << hue_perc_max << std::endl;

    // saturation histogram
    std::vector<int> saturation_intensity_x,saturation_occurrence_y;
    create_histogram(saturation_dataset_vector, saturation_intensity_x,saturation_occurrence_y);

    // plot w/ MatLab
    print_values_histogram(saturation_intensity_x, saturation_occurrence_y,"SAT");
//    std::cout << "Sat Min: " << saturation_intensity_x[0] << std::endl;
//    std::cout << "Sat Max: " << saturation_intensity_x[saturation_intensity_x.size() - 1] << std::endl;

    // value histogram
    std::vector<int> value_intensity_x,value_occurrence_y;
    create_histogram(value_dataset_vector, value_intensity_x,value_occurrence_y);

    // plot w/ MatLab
    print_values_histogram(value_intensity_x, value_occurrence_y,"VAL");
//    std::cout << "Val Min: " << value_intensity_x[0] << std::endl;
//    std::cout << "Val Max: " << value_intensity_x[value_intensity_x.size() - 1] << std::endl;

    // area histogram
    std::vector<int> area_intensity_x,area_occurrence_y;
    create_histogram(area_dataset_vector, area_intensity_x,area_occurrence_y);

    // plot w/ MatLab
    print_values_histogram(area_intensity_x, area_occurrence_y,"AREA");
    
    // rectangularity histogram
    std::vector<float> rectangularity_intensity_x,rectangularity_occurrence_y;
    create_histogram(rectangularity_dataset_vector, rectangularity_intensity_x,rectangularity_occurrence_y);

    // plot w/ MatLab
    print_values_histogram(rectangularity_intensity_x, rectangularity_occurrence_y,"REC");
};


// ### CRITERIA ###

// Used to apply almost all the critera
objects & parking::filterObjects(objects & objs, bool (*pred)(object & obj))
{
    objs.erase(std::remove_if(objs.begin(), objs.end(), pred), objs.end());
    return objs;
}


// 01 - Filter objects on area, perimeter and their ratio
bool parking::cartagoCriterion(object& obj)
{
    double perimeter = cv::arcLength(obj, true);
    double area = cv::contourArea(obj);
    double cartagoValue = perimeter / area;

    return (area         < MIN_AREA      || area         > MAX_AREA      ||
            perimeter    < MIN_PERIMETER || perimeter    > MAX_PERIMETER ||
            cartagoValue < MIN_CARTAGO   || cartagoValue > MAX_CARTAGO);
}


// 02 - Filter objects counting black pixels on binarized image and checking their HSV value
objects& parking::colorContrastCriterion(objects& candidate_objects, cv::Mat img_hsv, cv::Mat img_binary)
{
    int white_area = 0, black_area = 0;       // Counter for black and white pixels (of the binarized image)

    // HSV sums
    int hue_sum_img = 0;
    int saturation_sum_img = 0;
    int value_sum_img = 0;

    // HSV averages
    int hue_avg_img = -1;
    int saturation_avg_img = -1;
    int value_avg_img = -1;

    /*
    Draws a contour to be filled.
    We count the black pixels (contrast elements in the contour)
    and check for those pixels the hsv values (looking for blue color)
    (e.g. checking the blue hole in 'P' letters, or blue 'P' letters for the inverted signs)
    */
    for (int obj = 0; obj < candidate_objects.size(); obj++)
    {
        cv::Mat canvas = cv::Mat::zeros(img_binary.size(), CV_8UC3);
        cv::fillPoly(canvas, candidate_objects[obj], CV_RGB(0, 255, 255));
        cv::Rect object_rectangle = cv::boundingRect(candidate_objects[obj]);

        white_area = 0;
        black_area = 0;
        hue_sum_img = 0;
        saturation_sum_img = 0;
        value_sum_img = 0;

        for (int row = object_rectangle.y; row < (object_rectangle.y + object_rectangle.height); row++)
        {
            cv::Vec3b* ythRow_canvas = canvas.template ptr<cv::Vec3b>(row);

            unsigned char* ythRow_bin = img_binary.template ptr<unsigned char>(row);

            cv::Vec3b* ythRow_hsv = img_hsv.template ptr<cv::Vec3b>(row);

            /*
            if the pixel of the canvas is (BGR 255-255-0), we check if it is white or black in the binarized image:
            - black : we take the hsv value in the original hsv image
            - white : we only increment the counter for white pixels
            */
            for (int col = object_rectangle.x; col < (object_rectangle.x + object_rectangle.width); col++)
                if (ythRow_canvas[col][0] == 255 && ythRow_canvas[col][1] == 255 && ythRow_canvas[col][2] == 0) // BGR
                {
                    // std::cout << int(ythRow_img[col]) << std::endl;      //DEBUG
                    if (int(ythRow_bin[col]) == 255)
                        white_area++;
                    if (int(ythRow_bin[col]) == 0)
                    {
                        hue_sum_img = hue_sum_img + ythRow_hsv[col][0];                 // Hue
                        saturation_sum_img = saturation_sum_img + ythRow_hsv[col][1];   // Saturation
                        value_sum_img = value_sum_img + ythRow_hsv[col][2];             // Value
                        black_area++;
                    }
                }
        }

        // resetting to -1
        hue_avg_img = -1;
        saturation_avg_img = -1;
        value_avg_img = -1;

        // computing avgs
        if (black_area != 0) {
            hue_avg_img = hue_sum_img / black_area;
            saturation_avg_img = saturation_sum_img / black_area;
            value_avg_img = value_sum_img / black_area;
        }

        bool deleted = false;   // flag to avoid double erasing

        // B/W sub-criterion
        // black area, white area and their ratio should be in a defined range
        if (black_area < MIN_BLACK || black_area > MAX_BLACK ||
            white_area < MIN_WHITE || white_area > MAX_WHITE ||
            black_area  / float(white_area) > MAX_BW_RATIO   ||
            (black_area / float(white_area) > MEDIUM1_BW_RATIO && float(black_area) / float(white_area) < MEDIUM2_BW_RATIO)
            )
        {
            candidate_objects.erase(candidate_objects.begin() + obj--);
            deleted = true;
        }
        // HSV sub-criterion
        if (!deleted)
            if (hue_avg_img        < MIN_HUE || hue_avg_img        > MAX_HUE ||
                saturation_avg_img < MIN_SAT || saturation_avg_img > MAX_SAT ||
                value_avg_img      < MIN_VAL || value_avg_img      > MAX_VAL)
                candidate_objects.erase(candidate_objects.begin() + obj--);
    }

    return candidate_objects;
}


// 03 - Filter the objects w/ an area distribution different from the P shape
/*
Draws a bounding rect fo each contour, and separates it into 4 parts: Top-right, top-left, bottom-right, bottom-left.
Check the area for each quadrant and analyze their ratio:

1 - Top Area        (1+2) / Bottom Area     (3+4)
2 - Left Area       (1+3) / Right Area      (2+4)
3 - Top-Right Area    (2) / Bottom-Right Area (4)
4 - Top-Left Area     (1) / Bottom-Left Area  (3)
5 - Bottom-Right Area (4) / Bottom-Left Area  (3)


               (.x,.y)      (.x +.width/2,.y)        (.x+.width, .y)
                            |   1   |   2   |
  (.x, .y + .height/2)      |_______|_______|        (.x+.width, .y + .height/2)
                            |       |       |
                            |   3   |   4   |
    (.x, .y + .height) (.x +.width/2, .y + .height)  (.x + .width, .y + .height)

*/
objects& parking::topBottomAreaCriterion(objects& candidate_objects, cv::Mat img)
{
    float p_area_topleft = 0;
    float p_area_topright = 0;
    float p_area_bottomleft = 0;
    float p_area_bottomright = 0;

    for (int obj = 0; obj < candidate_objects.size(); obj++)
    {
        cv::Mat canvas = cv::Mat::zeros(img.size(), CV_8UC3);
        cv::fillPoly(canvas, candidate_objects[obj], CV_RGB(0, 255, 255));

        // cv::namedWindow("CONTOURS DEBUG", cv::WINDOW_NORMAL);
        // cv::imshow("CONTOURS DEBUG", debug_contours);
        // cv::waitKey(0);

        cv::Rect object_rectangle = cv::boundingRect(candidate_objects[obj]);

        p_area_topleft = 0;
        for (int row = object_rectangle.y; row < (object_rectangle.y + object_rectangle.height / 2); row++)
        {
            cv::Vec3b* ythRow = canvas.template ptr<cv::Vec3b>(row);
            for (int col = object_rectangle.x; col < (object_rectangle.x + object_rectangle.width / 2); col++)
                // if the pixel is inside the contour & the BGR color is the debugging one
                if (ythRow[col][0] == 255 && ythRow[col][1] == 255 && ythRow[col][2] == 0) { // B-G-R           
                    p_area_topleft++;    // increment the area
                    // debug drawing
                    // ythRow[col][0] = 0;
                    // ythRow[col][1] = 0;
                    // ythRow[col][2] = 255;
                }
        }

        p_area_topright = 0;
        for (int row = object_rectangle.y; row < (object_rectangle.y + object_rectangle.height / 2); row++)
        {
            cv::Vec3b* ythRow = canvas.template ptr<cv::Vec3b>(row);
            for (int col = (object_rectangle.x + object_rectangle.width / 2); col < (object_rectangle.x + object_rectangle.width); col++)
                // if the pixel is inside the contour & the BGR color is the debugging one
                if (ythRow[col][0] == 255 && ythRow[col][1] == 255 && ythRow[col][2] == 0) { // B-G-R 
                    p_area_topright++;  // increment the area
                    // debug drawing
                    // ythRow[col][0] = 0;
                    // ythRow[col][1] = 0;
                    // ythRow[col][2] = 255;
                }
        }

        p_area_bottomleft = 0;
        for (int row = (object_rectangle.y + object_rectangle.height / 2); row < (object_rectangle.y + object_rectangle.height); row++)
        {
            cv::Vec3b* ythRow = canvas.template ptr<cv::Vec3b>(row);
            for (int col = object_rectangle.x; col < (object_rectangle.x + object_rectangle.width / 2); col++)
                // if the pixel is inside the contour & the BGR color is the debugging one
                if (ythRow[col][0] == 255 && ythRow[col][1] == 255 && ythRow[col][2] == 0) { // B-G-R   
                    p_area_bottomleft++;     // increment the area
                    // debug drawing
                    // ythRow[col][0] = 0;
                    // ythRow[col][1] = 0;
                    // ythRow[col][2] = 255;
                }
        }

        p_area_bottomright = 0;
        for (int row = (object_rectangle.y + object_rectangle.height / 2); row < (object_rectangle.y + object_rectangle.height); row++)
        {
            cv::Vec3b* ythRow = canvas.template ptr<cv::Vec3b>(row);
            for (int col = (object_rectangle.x + object_rectangle.width / 2); col < (object_rectangle.x + object_rectangle.width); col++)
                // if the pixel is inside the contour & the BGR color is the debugging one
                if (ythRow[col][0] == 255 && ythRow[col][1] == 255 && ythRow[col][2] == 0) { // B-G-R
                    p_area_bottomright++;    // increment the area
                    // debug drawing
                    // ythRow[col][0] = 0;
                    // ythRow[col][1] = 0;
                    // ythRow[col][2] = 255;
                }
        }

        if (
            (p_area_topleft + p_area_topright)   / (p_area_bottomleft + p_area_bottomright) < MIN_TOP_BOTTOM_RATIO      || // 01 TOP / BOT
            (p_area_topleft + p_area_topright)   / (p_area_bottomleft + p_area_bottomright) > MAX_TOP_BOTTOM_RATIO      ||
            (p_area_topleft + p_area_bottomleft) / (p_area_topright + p_area_bottomright)   < MIN_LEFT_RIGHT_RATIO      || // 02 LEFT / RIGHT
            (p_area_topleft + p_area_bottomleft) / (p_area_topright + p_area_bottomright)   > MAX_LEFT_RIGHT_RATIO      ||
            (p_area_topright)                    / (p_area_bottomright)                     < MIN_TOPBOT_RIGHT_RATIO    || // 03 TOP-RIGHT / BOTTOM-RIGHT
            (p_area_topright)                    / (p_area_bottomright)                     > MAX_TOPBOT_RIGHT_RATIO    ||
            (p_area_topleft)                     / (p_area_bottomleft)                      < MIN_TOPBOT_LEFT_RATIO     || // 04 TOP-LEFT / BOTTOM-LEFT
            (p_area_topleft)                     / (p_area_bottomleft)                      > MAX_TOPBOT_LEFT_RATIO     ||
            (p_area_bottomright)                 / (p_area_bottomleft)                      < MIN_BOT_RIGHT_LEFT_RATIO  || // 05 BOTTOM-RIGHT / BOTTOM-LEFT
            (p_area_bottomright)                 / (p_area_bottomleft)                      > MAX_BOT_RIGHT_LEFT_RATIO)
        {
            candidate_objects.erase(candidate_objects.begin() + obj--);
        }
    }

    return candidate_objects;
}


// 04 - Filter objects on their area-convexHull ratio
bool parking::convexityCriterion(object & obj)
{
    object hull;
    cv::convexHull(obj, hull);
    return ( cv::contourArea(obj)/cv::contourArea(hull) < MIN_CONVEXITY || cv::contourArea(obj)/cv::contourArea(hull) > MAX_CONVEXITY );
}

// Required for Criterion N. 04
// Correct the OpenCV minAreaRect having height > width
cv::RotatedRect parking::correctedMinAreaRect(const object& obj){
    cv::RotatedRect rect = cv::minAreaRect(obj);
    if (rect.size.height < rect.size.width){
        std::swap(rect.size.width, rect.size.height);
        rect.angle += 90.f;
    }
    return rect;
}

// 05 Filter ojects on minarearect aspectRatio and area/minarearect area
bool parking::rectangularityCriterion(object& obj)
{
    cv::RotatedRect minarearect = parking::correctedMinAreaRect(obj);
    cv::Point2f vertices[4];
    minarearect.points(vertices);

    return (minarearect.size.aspectRatio()                 < MIN_MAR_ASPECT_RATIO ||
            minarearect.size.aspectRatio()                 > MAX_MAR_ASPECT_RATIO ||
            cv::contourArea(obj) / minarearect.size.area() < MIN_RECTANGULARITY   ||
            cv::contourArea(obj) / minarearect.size.area() > MAX_RECTANGULARITY    );
}


// 06 - Filter objects on boundingRect aspect ratio
bool parking::aspectRatioCriterion(object& obj)
{
    cv::Rect object_rectangle = cv::boundingRect(obj);
    float aspectratio_bounding = object_rectangle.width / (float)object_rectangle.height;

    return (aspectratio_bounding < MIN_BR_ASPECT_RATIO || aspectratio_bounding > MAX_BR_ASPECT_RATIO);
}


// 07 - Filter the objects on minEnclosingCircle ratio
bool parking::circularityCriterion(object& obj)
{
    cv::Point2f center;
    float radius = 0;
    cv::minEnclosingCircle(obj, center, radius);
    double minenclcircle_ratio = cv::contourArea(obj) / double(aia::PI * radius * radius);

    return (minenclcircle_ratio < MIN_CIRCULARITY || minenclcircle_ratio > MAX_CIRCULARITY);
}


// 08 - Filter objects on minenclosingtriangle
bool parking::triangularityCriterion(object& obj)
{
    std::vector<cv::Point> triangle;
    cv::minEnclosingTriangle(obj, triangle);
    double minencltriangle_ratio = cv::contourArea(obj) / cv::contourArea(triangle);

    return (minencltriangle_ratio < MIN_TRIANGULARITY || minencltriangle_ratio > MAX_TRIANGULARITY);
}


// Required for Criterion N. 09
// Extract the contorus from a given template image
objects parking::template_countours(cv::Mat& img_template){
    
    cv::Mat img_template_bin = cv::Mat::zeros(img_template.size(), CV_8U);
    cv::threshold(img_template, img_template_bin, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    objects template_contours;
    cv::findContours(img_template_bin.clone(), template_contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // cv::namedWindow("BINARIZED TEMPLATE", cv::WINDOW_NORMAL);
    // cv::imshow("BINARIZED TEMPLATE", img_template_bin);
    // cv::waitKey(0);
    
    return template_contours;
}

// 09 - Filter the objects that does not match with a P template given in input
bool parking::matchShapeCriterion(object & obj){
    // Front
    cv::Mat img_template_front = cv::imread(TEMPLATE_FOLDER + "P-front.png", cv::IMREAD_GRAYSCALE);
    objects template_front_contours = template_countours(img_template_front);

    // Distortion to the right
    cv::Mat img_template_right = cv::imread(TEMPLATE_FOLDER + "P-right.png", cv::IMREAD_GRAYSCALE);
    objects template_right_contours = template_countours(img_template_right);

    // Distortion to the left
    cv::Mat img_template_left = cv::imread(TEMPLATE_FOLDER + "P-left.png", cv::IMREAD_GRAYSCALE);
    objects template_left_contours = template_countours(img_template_left);

    // Stretch distortion
    cv::Mat img_template_stretch = cv::imread(TEMPLATE_FOLDER + "P-stretch.png", cv::IMREAD_GRAYSCALE);
    objects template_stretch_contours = template_countours(img_template_stretch);
    
    // Stretch 45
    cv::Mat img_template_stretch_45 = cv::imread(TEMPLATE_FOLDER + "P-45.png", cv::IMREAD_GRAYSCALE);
    objects template_stretch_45_contours = template_countours(img_template_stretch_45);
    
    // Stretch 7
    cv::Mat img_template_stretch_7 = cv::imread(TEMPLATE_FOLDER + "P-7.png", cv::IMREAD_GRAYSCALE);
    objects template_stretch_7_contours = template_countours(img_template_stretch_7);
  
    // debug template contours
    /*
    cv::Mat debug_img = cv::Mat::zeros(img_template_stretch_45.size(), CV_8UC3);
    for (int i=0; i<template_stretch_7_contours.size(); i++){
        cv::drawContours(debug_img, template_stretch_7_contours, i, CV_RGB(0,255,255));
        cv::namedWindow("debug", cv::WINDOW_NORMAL);
        cv::imshow("debug", debug_img);
        cv::waitKey(0);
    }
    */

    float shape_distance_Pext_front      = cv::matchShapes(obj, template_front_contours[2],      cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_right      = cv::matchShapes(obj, template_right_contours[2],      cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_left       = cv::matchShapes(obj, template_left_contours[2],       cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_stretch    = cv::matchShapes(obj, template_stretch_contours[2],    cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_stretch_45 = cv::matchShapes(obj, template_stretch_45_contours[2], cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_stretch_7  = cv::matchShapes(obj, template_stretch_7_contours[2],  cv::CONTOURS_MATCH_I3, 0);

    
    double minDist = std::min( {shape_distance_Pext_front, shape_distance_Pext_right, shape_distance_Pext_left, shape_distance_Pext_stretch, shape_distance_Pext_stretch_45, shape_distance_Pext_stretch_7} );

    return minDist > MAX_SHAPE_I3;
}


// ### PERFORMANCE ###

/*
creates a vector of vector of cv::rect, containing groundtruths read from .json:
- output[0]: all groundtruths, or only NOT occluded and OOF signs if SKIP_OCCLUDED_AND_OOF is defined.
- output[1]: only occluded and OOF signs if SKIP_OCCLUDED_AND_OOF is defined. (Otherwise it's empty)
*/
std::vector<std::vector<cv::Rect>> parking::label_signs(std::vector <std::string> jsons, int num_image)
{
    std::vector<std::vector<cv::Rect>> output;
    std::vector<cv::Rect> label_rects;
    std::vector<cv::Rect> label_occluded_rects;
    std::ifstream f(jsons[num_image]);
    std::string line;

    bool last_ambiguous = false;
    bool last_occluded = false;
    bool last_out_of_frame = false;

    data last_bbox;
    int count = 0;

    
    while (std::getline(f, line))
    {
        if (line.find("ambiguous") != std::string::npos)
            last_ambiguous = parseAttribute(line) == "true";

        else if (line.find("occluded") != std::string::npos)
            last_occluded = parseAttribute(line) == "true";

        else if (line.find("out-of-frame") != std::string::npos)
            last_out_of_frame = parseAttribute(line) == "true";

        else if (line.find("\"bbox\":") != std::string::npos) {
            std::getline(f, line);
            last_bbox.x = ucas::str2num<float>(parseAttribute(line));
            std::getline(f, line);
            last_bbox.y = ucas::str2num<float>(parseAttribute(line));
            std::getline(f, line);
            last_bbox.height = ucas::str2num<float>(parseAttribute(line)) - last_bbox.y;
            std::getline(f, line);
            last_bbox.width = ucas::str2num<float>(parseAttribute(line)) - last_bbox.x;
        }

        if (line.find("information--parking--g1") != std::string::npos) {
           
            count++;

            cv::Rect label_rect;
            label_rect.x = last_bbox.x;
            label_rect.y = last_bbox.y;
            label_rect.height = last_bbox.height;
            label_rect.width = last_bbox.width;

            #ifdef SKIP_OCCLUDED_AND_OOF
                if (last_occluded || last_out_of_frame)
                    label_occluded_rects.push_back(label_rect);
                else
                    label_rects.push_back(label_rect);
            #else
                label_rects.push_back(label_rect);
            #endif

            last_ambiguous = false;
            last_occluded = false;
            last_out_of_frame = false;
        }
    }

    #ifdef SKIP_OCCLUDED_AND_OOF
        output.push_back(label_rects);              // output [0]
        output.push_back(label_occluded_rects);     // output [1]
    #else
        output.push_back(label_rects);              // output [0]    
    #endif

    return output;
}


// calculate score of the contours
float parking::score_calculation(object obj, double maxDist)
{
    // Front
    cv::Mat img_template_front = cv::imread(TEMPLATE_FOLDER + "P-front.png", cv::IMREAD_GRAYSCALE);
    objects template_front_contours = template_countours(img_template_front);

    // Distortion to the right
    cv::Mat img_template_right = cv::imread(TEMPLATE_FOLDER + "P-right.png", cv::IMREAD_GRAYSCALE);
    objects template_right_contours = template_countours(img_template_right);

    // Distortion to the left
    cv::Mat img_template_left = cv::imread(TEMPLATE_FOLDER + "P-left.png", cv::IMREAD_GRAYSCALE);
    objects template_left_contours = template_countours(img_template_left);

    // Stretch distortion
    cv::Mat img_template_stretch = cv::imread(TEMPLATE_FOLDER + "P-stretch.png", cv::IMREAD_GRAYSCALE);
    objects template_stretch_contours = template_countours(img_template_stretch);
    
    // Stretch 45
    cv::Mat img_template_stretch_45 = cv::imread(TEMPLATE_FOLDER + "P-45.png", cv::IMREAD_GRAYSCALE);
    objects template_stretch_45_contours = template_countours(img_template_stretch_45);
    
    // Stretch 7
    cv::Mat img_template_stretch_7 = cv::imread(TEMPLATE_FOLDER + "P-7.png", cv::IMREAD_GRAYSCALE);
    objects template_stretch_7_contours = template_countours(img_template_stretch_7);
  
    // debug template contours
    /*
    cv::Mat debug_img = cv::Mat::zeros(img_template_stretch_45.size(), CV_8UC3);
    for (int i=0; i<template_stretch_7_contours.size(); i++){
        cv::drawContours(debug_img, template_stretch_7_contours, i, CV_RGB(0,255,255));
        cv::namedWindow("debug", cv::WINDOW_NORMAL);
        cv::imshow("debug", debug_img);
        cv::waitKey(0);
    }
    */

    float shape_distance_Pext_front      = cv::matchShapes(obj, template_front_contours[2],      cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_right      = cv::matchShapes(obj, template_right_contours[2],      cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_left       = cv::matchShapes(obj, template_left_contours[2],       cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_stretch    = cv::matchShapes(obj, template_stretch_contours[2],    cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_stretch_45 = cv::matchShapes(obj, template_stretch_45_contours[2], cv::CONTOURS_MATCH_I3, 0);
    float shape_distance_Pext_stretch_7  = cv::matchShapes(obj, template_stretch_7_contours[2],  cv::CONTOURS_MATCH_I3, 0);
    
    double minDist = std::min( {shape_distance_Pext_front, shape_distance_Pext_right, shape_distance_Pext_left, shape_distance_Pext_stretch, shape_distance_Pext_stretch_45, shape_distance_Pext_stretch_7} );
    
    float score = (minDist - maxDist) / -maxDist;
    
    return score;
}

// print all Hyperparameters used in that launch
void parking::print_launch_configuration() {

    std::cout << std::endl << "####################################################" << std::endl;
    std::cout << "MIN_AREA:\t"                  << MIN_AREA << std::endl;
    std::cout << "MAX_AREA:\t"                  << MAX_AREA << std::endl;

    std::cout << "MIN_PERIMETER:\t"             << MIN_PERIMETER << std::endl;
    std::cout << "MAX_PERIMETER:\t"             << MAX_PERIMETER << std::endl;

    std::cout << "MIN_CARTAGO:\t"               << MIN_CARTAGO << std::endl;
    std::cout << "MAX_CARTAGO:\t"               << MAX_CARTAGO << std::endl;


    std::cout << "MIN_BLACK:\t"                 << MIN_BLACK << std::endl;
    std::cout << "MAX_BLACK:\t"                 << MAX_BLACK << std::endl;

    std::cout << "MIN_WHITE:\t"                 << MIN_WHITE << std::endl;
    std::cout << "MAX_WHITE:\t"                 << MAX_WHITE << std::endl;

    std::cout << "MEDIUM1_BW_RATIO:\t"          << MEDIUM1_BW_RATIO << std::endl;
    std::cout << "MEDIUM2_BW_RATIO:\t"          << MEDIUM2_BW_RATIO << std::endl;
    std::cout << "MAX_BW_RATIO:\t"              << MAX_BW_RATIO << std::endl;

    std::cout << "MIN_HUE:\t"                   << MIN_HUE << std::endl;
    std::cout << "MAX_HUE:\t"                   << MAX_HUE << std::endl;

    std::cout << "MIN_SAT:\t"                   << MIN_SAT << std::endl;
    std::cout << "MAX_SAT:\t"                   << MAX_SAT << std::endl;

    std::cout << "MIN_VAL:\t"                   << MIN_VAL << std::endl;
    std::cout << "MAX_VAL:\t"                   << MAX_VAL << std::endl;


    std::cout << "MIN_TOP_BOTTOM_RATIO:\t"      << MIN_TOP_BOTTOM_RATIO << std::endl;
    std::cout << "MAX_TOP_BOTTOM_RATIO:\t"      << MAX_TOP_BOTTOM_RATIO << std::endl;

    std::cout << "MIN_LEFT_RIGHT_RATIO:\t"      << MIN_LEFT_RIGHT_RATIO << std::endl;
    std::cout << "MAX_LEFT_RIGHT_RATIO:\t"      << MAX_LEFT_RIGHT_RATIO << std::endl;

    std::cout << "MIN_TOPBOT_RIGHT_RATIO:\t"    << MIN_TOPBOT_RIGHT_RATIO << std::endl;
    std::cout << "MAX_TOPBOT_RIGHT_RATIO:\t"    << MAX_TOPBOT_RIGHT_RATIO << std::endl;

    std::cout << "MIN_TOPBOT_LEFT_RATIO:\t"     << MIN_TOPBOT_LEFT_RATIO << std::endl;
    std::cout << "MAX_TOPBOT_LEFT_RATIO:\t"     << MAX_TOPBOT_LEFT_RATIO << std::endl;

    std::cout << "MIN_BOT_RIGHT_LEFT_RATIO:\t"  << MIN_BOT_RIGHT_LEFT_RATIO << std::endl;
    std::cout << "MAX_BOT_RIGHT_LEFT_RATIO:\t"  << MAX_BOT_RIGHT_LEFT_RATIO << std::endl;


    std::cout << "MIN_CONVEXITY:\t"             << MIN_CONVEXITY << std::endl;
    std::cout << "MAX_CONVEXITY:\t"             << MAX_CONVEXITY << std::endl;


    std::cout << "MIN_RECTANGULARITY:\t"        << MIN_RECTANGULARITY << std::endl;
    std::cout << "MAX_RECTANGULARITY:\t"        << MAX_RECTANGULARITY << std::endl;


    std::cout << "MIN_BR_ASPECT_RATIO:\t"       << MIN_BR_ASPECT_RATIO << std::endl;
    std::cout << "MAX_BR_ASPECT_RATIO:\t"       << MAX_BR_ASPECT_RATIO << std::endl;
    

    std::cout << "MIN_CIRCULARITY:\t"           << MIN_CIRCULARITY << std::endl;
    std::cout << "MAX_CIRCULARITY:\t"           << MAX_CIRCULARITY << std::endl;


    std::cout << "MIN_TRIANGULARITY:\t"         << MIN_TRIANGULARITY << std::endl;
    std::cout << "MAX_TRIANGULARITY:\t"         << MAX_TRIANGULARITY << std::endl;


    std::cout << "MIN_SHAPE_I1:\t"              << MIN_SHAPE_I1 << std::endl;
    std::cout << "MAX_SHAPE_I1:\t"              << MAX_SHAPE_I1 << std::endl;


    std::cout << "MIN_SHAPE_I3:\t"              << MIN_SHAPE_I3 << std::endl;
    std::cout << "MAX_SHAPE_I3:\t"              << MAX_SHAPE_I3 << std::endl;

    return;
}
