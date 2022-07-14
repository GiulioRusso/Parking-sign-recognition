/*
 
 EIID / AIA 2020 - 2021 UNICAS Traffic Sign Detection [parking]
 
 Prof. Bria Alessandro
 
 Students:
 - Camasso Giulio
 - Marzilli Alessandro
 - Russo Giulio
 
*/

#include "ucasConfig.h"
#include "config.h"
#include "functions.h"

using namespace parking;


int main()
{
    try
    {
        // File names vectors
        std::vector <std::string> images;
        std::vector <std::string> jsons;


        // ### PREPROCESSING ###
        // Creates a new dataset using filters (meanshift, clahe, ecc...)
 
        # ifdef PREPROCESSING
        
        cv::glob(ORIGINAL_DATASET + "/*.jpg", images);
        cv::glob(ORIGINAL_DATASET + "/*.json", jsons);      // Jsons list in the dataset

  
        for (int subset_index = 0; images.size(); subset_index++)
        {
            cv::Mat img = cv::imread(images[subset_index], cv::IMREAD_UNCHANGED);
            if(!img.data) {
                throw ucas::Error("Cannot load image");
            }
            # ifdef USE_CLAHE
                img = clahe_preprocessing(img, 1);
            # endif
            mean_shift_preprocessing(img, subset_index, PREPROCESSING_FOLDER);
        }

        // Statistics values extraction of HSV average values on the dataset in label
        //statistics_values_extraction(images, jsons);

        # endif


        // ### ELABORATION ###
        
        // Collecting images and json from preprocesed dataset
        cv::glob(PREPROCESSED_DATASET + "/*.jpg", images);
        cv::glob(PREPROCESSED_DATASET + "/*.json", jsons);
        
        // To analyze a single image
        int num = 1;
        
        // Processing variables
        cv::Mat img_hsv;
        cv::Mat img_gray;
        cv::Mat img_enhanced;
        cv::Mat img_binary;

        objects candidate_objects;

        // Performance variables
        int TP = 0;                                     // True-Positive
        int FP = 0;                                     // False-Positive
        int FN = 0;                                     // False-Negative
        int OCC = 0;                                    // Occluded or OOF detected signs

        int taken;                                      // TP for each image
        int false_taken;                                // FP for each image
        int false_negative;                             // FN of each image
        int occluded_taken;                             // occluded or OOF signs detected for each image
        int signs_img;                                  // target signs of each image (taken from json)
        int signs_img_occluded;                         // target signs occluded or OOF of each image
        int tot_signs = 0;                              // total signs of the dataset

        std::vector<float> scores_vector;               // used to save scores
        std::vector<int> label_vector;                  // labeling: (TP=1, FP=0)

        std::vector<cv::Rect> rect_labels;              // labels for each image
        std::vector<cv::Rect> rect_occluded_labels;


        cv::Mat img_debuginlabel;                       // debug images (original image with contours)
        cv::Mat img_debuginlabel_bin;                   // debug images (only contours)
        cv::Mat final_debug;                            // to save the output images
            

        // Scanning images (from 0 to 99)
        for (int subset_index=0; subset_index<images.size(); subset_index++)
        {
            if (num == subset_index)            // To analize a single image
            {
                // 0 - Loads the image
                cv::Mat img = cv::imread(images[subset_index], cv::IMREAD_COLOR);
                if(!img.data) {
                    throw ucas::Error("Cannot load image");
                }

                // ### DEBUG ### - shows the loaded image
                /*
                cv::namedWindow("IMG", cv::WINDOW_NORMAL);
                cv::imshow("IMG", img);
                cv::waitKey(0);
                */

                // BGR to HSV conversion
                cv::cvtColor(img, img_hsv, cv::COLOR_BGR2HSV);
                
                // BGR to Gray-Scale conversion
                cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);


                // 1 - using TOP-HAT to enhance bright objects
                // NB. cv::Size(22, 115) chosen from the stem of the second letter P in image 89 (the higher one)
                cv::morphologyEx(
                    img_gray,
                    img_enhanced,
                    cv::MORPH_TOPHAT,
                    cv::getStructuringElement(cv::MORPH_RECT, cv::Size(22, 115))        
                );

                // ### DEBUG ### - shows the enhanced image
                /*
                cv::namedWindow("ENHANCED", cv::WINDOW_NORMAL);
                cv::imshow("ENHANCED", img_enhanced);
                cv::waitKey(0);
                */


                // 2 - Binarization
                cv::threshold(img_enhanced, img_binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
           
                // ### DEBUG ### - shows the binarized image
                /*
                cv::namedWindow("BINARY", cv::WINDOW_NORMAL);
                cv::imshow("BINARY", img_binary);
                cv::waitKey(0);
                */


                // 3 - Contours extraction
                candidate_objects.clear();      // to clear candidates from prev image
                cv::findContours(img_binary.clone(), candidate_objects, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

                // ### DEBUG ### - to show contours before appliyng criteria
                /*
                cv::Mat debug_contours_start = cv::Mat::zeros(img.size(), CV_8UC3);     
                for (int obj=0; obj<candidate_objects.size(); obj++)
                    cv::drawContours(debug_contours_start, candidate_objects, obj, CV_RGB(255,0,0), 3);

                cv::namedWindow("DEBUG CONTOURS BEFORE CRITERIA", cv::WINDOW_NORMAL);
                cv::imshow("DEBUG CONTOURS BEFORE CRITERIA", debug_contours_start);
                cv::waitKey(0);
                */


                // 4 - Applying criteria

                // 4.01 - cartagoCriterion
                filterObjects(candidate_objects, cartagoCriterion);

                // 4.02 - colorContrastCriterion
                colorContrastCriterion(candidate_objects, img_hsv, img_binary);

                // 4.03 - topBottomAreaCriterion
                topBottomAreaCriterion(candidate_objects, img);

                // 4.04 - convexityCriterion
                filterObjects(candidate_objects, convexityCriterion);

                // 4.05 - rectangularityCriterion
                filterObjects(candidate_objects, rectangularityCriterion);

                // 4.06 - aspectRatioCriterion
                filterObjects(candidate_objects, aspectRatioCriterion);

                // 4.07 - circularityCriterion
                filterObjects(candidate_objects, circularityCriterion);
                
                // 4.08 - triangularityCriterion
                filterObjects(candidate_objects, triangularityCriterion);
               
                // 4.09 - matchShapeCriterion
                filterObjects(candidate_objects, matchShapeCriterion);
                
                // ### DEBUG ### -  to show contours after criteria
                /*
                cv::Mat debug_contours_end = cv::Mat::zeros(img.size(), CV_8UC3);
                for (int obj=0; obj<candidate_objects.size(); obj++)
                    cv::drawContours(debug_contours_end, candidate_objects, obj, CV_RGB(255,0,0), 3);

                cv::namedWindow("DEBUG CONTOURS AFTER CRITERIA", cv::WINDOW_NORMAL);
                cv::imshow("DEBUG CONTOURS AFTER CRITERIA", debug_contours_end);
                cv::waitKey(0);
                */

                
                // ### PERFORMANCE SECTION ###


                // Loading labels for actual image
                rect_labels.clear();                                                // clearing prev image labels
                rect_occluded_labels.clear();                                       
                rect_labels = label_signs(jsons, subset_index)[0];                  // loading new labels from json file
                #ifdef SKIP_OCCLUDED_AND_OOF
                    rect_occluded_labels = label_signs(jsons, subset_index)[1];
                #endif

                // debug images, variables and print
                img_debuginlabel = img.clone();
                img_debuginlabel_bin = cv::Mat::zeros(img.size(), CV_8UC3); 

                taken = 0;
                occluded_taken = 0;
                false_taken = 0;
                signs_img = rect_labels.size();
                #ifdef SKIP_OCCLUDED_AND_OOF
                    signs_img_occluded = rect_occluded_labels.size();
                #endif
                // final debug image
                final_debug = img.clone();
                
                // if there is a signal to detect
                std::cout << "IMG " << subset_index << " -> ";

                if(!rect_labels.empty() || !rect_occluded_labels.empty())
                {
                    // true positive and double detections vector
                    objects true_positive_vector, double_detections;
                    objects occluded_true_positive_vector, occluded_double_detections;
                    // slide all the ground truth
                    for( auto & rect_label : rect_labels)
                    {
                        // flag and object for double detection
                        bool already_found = false;
                        float last_IoU = 0;
                        object last_true;
                        
                        // final debug label
                        cv::rectangle(final_debug, rect_label, CV_RGB(255,255,0), 3);
                        
                        // slide all the objects
                        for (int i=0; i<candidate_objects.size(); i++)
                        {
                            cv::Rect rect_obj = cv::boundingRect(candidate_objects[i]);

                            // Intersection of Union
                            float IoU = float((rect_obj & rect_label).area()) / float(rect_obj.area() + rect_label.area() - (rect_obj & rect_label).area());
                            
                            // if the IoU intersect the label
                            if (IoU > 0.17)
                            {
                                // ### DEBUG ### - to show in label contours
                                /*
                                cv::drawContours(img_debuginlabel, candidate_objects, i, CV_RGB(255,255,0), 3);
                                cv::rectangle(img_debuginlabel, cv::Point(rect_obj.x, rect_obj.y), cv::Point(rect_obj.x + rect_obj.width, rect_obj.y + rect_obj.height), CV_RGB(0,255,0), 1, cv::LINE_AA);
                                cv::namedWindow("IN LABEL", cv::WINDOW_NORMAL);
                                cv::imshow("IN LABEL", img_debuginlabel);
                                cv::waitKey(0);
                                 */
                                // if I haven't found a true positive yet
                                if (!already_found)
                                {
                                    // I add it into the true_positive_vector, saving it and the IoU, flag is true
                                    true_positive_vector.push_back(candidate_objects[i]);
                                    last_true = candidate_objects[i];
                                    last_IoU = IoU;
                                    already_found = true;
                                }
                                // if I have already found it
                                else
                                {
                                    // I have to check if it has got a better IoU than the previous true positive
                                    if (IoU > last_IoU)
                                    {
                                        // if it is, I save the last true positive into the double detection vector and update the true positive vector
                                        double_detections.push_back(last_true);
                                        true_positive_vector.pop_back();
                                        true_positive_vector.push_back(candidate_objects[i]);
                                        last_true = candidate_objects[i];
                                        last_IoU = IoU;
                                    }
                                }
                            }
                        }
                    }
                    
                    #ifdef SKIP_OCCLUDED_AND_OOF
                    for (auto& rect_occluded_label : rect_occluded_labels)
                    {
                        // flag and object for double detection
                        bool already_found = false;
                        float last_IoU = 0;
                        object last_occluded;

                        // final debug label
                        cv::rectangle(final_debug, rect_occluded_label, CV_RGB(255, 255, 0), 3);

                        // slide all the objects
                        for (int i = 0; i < candidate_objects.size(); i++)
                        {
                            cv::Rect rect_obj = cv::boundingRect(candidate_objects[i]);

                            // Intersection of Union
                            float IoU = float((rect_obj & rect_occluded_label).area()) / float(rect_obj.area() + rect_occluded_label.area() - (rect_obj & rect_occluded_label).area());

                            // if the IoU intersect the label
                            if (IoU > 0.17)
                            {
                                // ### DEBUG ### - to show in label contours
                                /*
                                cv::drawContours(img_debuginlabel, candidate_objects, i, CV_RGB(255,255,0), 3);
                                cv::rectangle(img_debuginlabel, cv::Point(rect_obj.x, rect_obj.y), cv::Point(rect_obj.x + rect_obj.width, rect_obj.y + rect_obj.height), CV_RGB(0,255,0), 1, cv::LINE_AA);
                                cv::namedWindow("IN LABEL", cv::WINDOW_NORMAL);
                                cv::imshow("IN LABEL", img_debuginlabel);
                                cv::waitKey(0);
                                 */
                                // if I haven't found a true positive yet
                                if (!already_found)
                                {
                                    // I add it into the true_positive_vector, saving it and the IoU, flag is true
                                    occluded_true_positive_vector.push_back(candidate_objects[i]);
                                    last_occluded = candidate_objects[i];
                                    last_IoU = IoU;
                                    already_found = true;
                                }
                                // if I have already found it
                                else
                                {
                                    // I have to check if it has got a better IoU than the previous true positive
                                    if (IoU > last_IoU)
                                    {
                                        // if it is, I save the last true positive into the double detection vector and update the true positive vector
                                        occluded_double_detections.push_back(last_occluded);
                                        occluded_true_positive_vector.pop_back();
                                        occluded_true_positive_vector.push_back(candidate_objects[i]);
                                        last_occluded = candidate_objects[i];
                                        last_IoU = IoU;
                                    }
                                }
                            }
                        }
                    }
                    #endif

                    taken = true_positive_vector.size();
                    occluded_taken = occluded_true_positive_vector.size();
                    
                    // slide all the candidate objects
                    for (int i = 0; i < candidate_objects.size(); i++)
                    {
                        // flag to see if the candidate contour is a true positive
                        bool marked = false;
                        // slide the true positive
                        for (int tp = 0; tp < true_positive_vector.size(); tp++)
                        {
                            // if the candidate contour is a true positive
                            if (candidate_objects[i] == true_positive_vector[tp])
                            {
                                // label, scores calculation and the marked flag are updated
                                label_vector.push_back(1);
                                scores_vector.push_back(score_calculation(candidate_objects[i], MAX_SHAPE_I1));
                                marked = true;
                                TP++;

                                // ### DEBUG ### - drawing the true positive in green
                                cv::drawContours(final_debug, candidate_objects, i, CV_RGB(0, 255, 0), 3);
                                /*
                                cv::namedWindow("FINAL DEBUG", cv::WINDOW_NORMAL);
                                cv::imshow("FINAL DEBUG", final_debug);
                                cv::waitKey(0);
                                */
                            }
                        }

                        // flag to see if the candidate contour is a double detection
                        bool duplicate = false;
                        // slide the double detections
                        for (int dd = 0; dd < double_detections.size(); dd++)
                        {
                            // if the candidate contour is a true positive
                            if (candidate_objects[i] == double_detections[dd])
                            {
                                duplicate = true;
                            }
                        }

                        #ifdef SKIP_OCCLUDED_AND_OOF
                        // flag to see if the candidate contour is an occluded or oof in case of we don't have to detect it
                        bool occluded = false;
                        // slide the double detections
                        for (int oo = 0; oo < occluded_true_positive_vector.size(); oo++)
                            // if the candidate contour is a true positive
                            if (candidate_objects[i] == occluded_true_positive_vector[oo]) {
                                cv::drawContours(final_debug, candidate_objects, i, CV_RGB(255, 165, 0), 3);
                                occluded = true;
                                OCC++;
                            }

                        bool double_occluded = false;
                        // slide the double detections
                        for (int oo = 0; oo < occluded_double_detections.size(); oo++)
                            // if the candidate contour is a true positive
                            if (candidate_objects[i] == occluded_double_detections[oo])
                                double_occluded = true;
                        #endif

                        // if the candidate contour didn't find a match w/ the true positive or double detections vectors elements, it's a false positive
                        #ifdef SKIP_OCCLUDED_AND_OOF
                        if (!marked && !duplicate && !occluded && !double_occluded)
                        #else
                        if (!marked && !duplicate)
                        #endif
                        {
                            // label, scores calculation are updated
                            label_vector.push_back(0);
                            scores_vector.push_back(score_calculation(candidate_objects[i], MAX_SHAPE_I1));
                            FP++;
                            false_taken++;

                            // ### DEBUG ### - drawing the false positive in green
                            cv::drawContours(final_debug, candidate_objects, i, CV_RGB(255, 0, 0), 3);
                            /*
                            cv::namedWindow("FINAL DEBUG", cv::WINDOW_NORMAL);
                            cv::imshow("FINAL DEBUG", final_debug);
                            cv::waitKey(0);
                            */
                        }
                    }
                    
                    // flag the false negative
                    false_negative = signs_img - taken;
                    FN = FN + false_negative;
                    
                    // Saving the final result of the elaboration
                    cv::imwrite(OUTPUT_DATASET + std::to_string(subset_index) + ".jpg", final_debug);
                    
                    // ### DEBUG ### - to show the final result of the elaboration
                    /*
                    cv::namedWindow("FINAL DEBUG", cv::WINDOW_NORMAL);
                    cv::imshow("FINAL DEBUG", final_debug);
                    cv::waitKey(0);
                    */
                }
                
                // ### DEBUG ### - to show the label-scores vector elements
                /*
                std::cout << label_vector.size() << " " << scores_vector.size() << std::endl;
                for (int i=0; i<label_vector.size(); i++)
                    std::cout << label_vector[i] << " " << scores_vector[i] << std::endl;
                */
                
                std::cout << taken << " / " << signs_img << " ::: FP " << false_taken << " ::: OCC " << occluded_taken << " ::: FN " << false_negative << std::endl;
                tot_signs = tot_signs + signs_img;

            } // if num
            
        } // for images

        std::cout << "TP = " << TP << " / " << tot_signs << std::endl << "FP = " << FP << std::endl << "OCC = " << OCC << std::endl << "FN = " << FN << std::endl;
        
        // writing on the .csv file to be used by PyCharm to calculate the PR and AUPR
        // if the file exist, I delete it because I don't want to overwrite the label-scores on the previous existing file
        std::ifstream infile(PERFORMANCE_FOLDER + "label-scores.csv");
        if (infile.good())
        {
            #ifndef GC
            std::__fs::filesystem::remove(PERFORMANCE_FOLDER + "label-scores.csv");
            #endif
        }
        // checking the folder is clear
        std::ifstream check(PERFORMANCE_FOLDER);
        bool isEmpty = check.peek() == EOF;
        // if it's clear, I create the two columns
        if (isEmpty)
        {
            std::ofstream output_csv_header;
            output_csv_header.open(PERFORMANCE_FOLDER, std::ios_base::app);
            output_csv_header
                            << "Label" << ","
                            << "Scores" << std::endl;
            output_csv_header.close();
        }
        std::ofstream output_csv;
        // writing into the labels and scores columns
        output_csv.open(PERFORMANCE_FOLDER + "label-scores.csv", std::ios_base::app);
        for(int i=0; i<label_vector.size(); i++)
        {
            output_csv  << std::to_string(label_vector[i]) << ","
                        << std::to_string(scores_vector[i]) << ","
                        << std::endl;
        }
        output_csv.close();
        
        return 1;
    }
    catch (aia::error &ex)
    {
        //std::cout << "EXCEPTION thrown by " << ex.getSource() << "source :\n\t|=> " << ex.what() << std::endl;
    }
    catch (ucas::Error &ex)
    {
        //std::cout << "EXCEPTION thrown by unknown source :\n\t|=> " << ex.what() << std::endl;
    }
}
