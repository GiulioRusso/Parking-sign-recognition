/*

 EIID / AIA 2020 - 2021 UNICAS Traffic Sign Detection [parking]

 Prof. Bria Alessandro

 Students:
 - Camasso Giulio
 - Marzilli Alessandro
 - Russo Giulio

*/

#pragma once

// define the user to set own paths: GC/GR/AM/TEACHER
#define GR

// to create preprocessed datasets
//#define PREPROCESSING

// to skip the occluded and out of frame labels
#define SKIP_OCCLUDED_AND_OOF

// to use the dataset preprocessed with the clahe
//#define USE_CLAHE


// ### PATH SECTION ###

# ifdef GC
    
    // original dataset
    #define ORIGINAL_DATASET (std::string)"C:/Users/giuli/Desktop/IMMAGINI/Progetto/Dataset_Finali/subset-EIID-AIA-parking"
    // preprocessed dataset chosen for the analisys (meanshift 5-5 or meanshift 5-5 + clahe 1)
    #ifdef USE_CLAHE
        #define PREPROCESSED_DATASET (std::string)"C:/Users/giuli/Desktop/IMMAGINI/Progetto/Dataset_Finali/subset-meanshift-EIID-AIA-parking5_5_clahe1"    //meanshift 5,5 + clahe 1
    #else
        #define PREPROCESSED_DATASET (std::string)"C:/Users/giuli/Desktop/IMMAGINI/Progetto/Dataset_Finali/subset-EIID-AIA-parking-meanshift5/"            //meanshift 5,5
    #endif
    // save the contours of the signals detected
    #define OUTPUT_DATASET (std::string)"C:/Users/giuli/Desktop/IMMAGINI/Progetto/Dataset_Finali/output_dataset/"
    
    // to create a new preprocessed dataset
    #define PREPROCESSING_FOLDER (std::string)""
    // contains the .csv file used to save scores and labels(TP/FP)
    #define PERFORMANCE_FOLDER (std::string)"C:/Users/giuli/Desktop/IMMAGINI/Progetto/22-07/performance/"
    // contains the "P" templates used by matchShape() function
    #define TEMPLATE_FOLDER (std::string)"C:/Users/giuli/Desktop/IMMAGINI/Progetto/template-set-P/"

# endif

# ifdef AM

    // original dataset
    #define ORIGINAL_DATASET (std::string)"/Users/alessandromarzilli/Desktop/EIIDimmagini/subset-EIID-AIA-parking"
    // preprocessed dataset chosen for the analisys (meanshift 5-5 or meanshift 5-5 + clahe 1)
    #ifdef USE_CLAHE
        #define PREPROCESSED_DATASET (std::string)"/Users/alessandromarzilli/Desktop/EIIDimmagini/subset-meanshift-EIID-AIA-parking5_5_clahe1"        //meanshift 5,5 + clahe 1
    #else
        #define PREPROCESSED_DATASET (std::string)"/Users/alessandromarzilli/Desktop/EIIDimmagini/subset-meanshift-EIID-AIA-parking5_5"                //meanshift 5,5
    #endif
    // save the contours of the signals detected
    #define OUTPUT_DATASET (std::string)""

    // to create a new preprocessed dataset
    #define PREPROCESSING_FOLDER (std::string)""
    // contains the .csv file used to save scores and labels(TP/FP)
    #define PERFORMANCE_FOLDER (std::string)"/Users/alessandromarzilli/Desktop/EIIDimmagini/parking performance/"
    // contains the "P" templates used by matchShape() function
    #define TEMPLATE_FOLDER (std::string)"/Users/alessandromarzilli/Desktop/EIIDimmagini/template-set-P/"

# endif

# ifdef GR

    #define ORIGINAL_DATASET (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/subset-EIID-AIA-parking"
    // preprocessed dataset chosen for the analisys (meanshift 5-5 or meanshift 5-5 + clahe 1)
    #ifdef USE_CLAHE
        #define PREPROCESSED_DATASET (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/subset-EIID-AIA-parking-meanshift5-clahe1"    //meanshift 5,5 + clahe 1
    #else
        #define PREPROCESSED_DATASET (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/subset-EIID-AIA-parking-meanshift5"            //meanshift 5,5
    #endif
    // save the contours of the signals detected
    #define OUTPUT_DATASET (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/debug-dataset/"

    // to create a new preprocessed dataset
    #define PREPROCESSING_FOLDER (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/subset-preprocessed-EIID-AIA-parking"
    // contains the .csv file used to save scores and labels(TP/FP)
    #define PERFORMANCE_FOLDER (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/parking-performance/"
    // contains the "P" templates used by matchShape() function
    #define TEMPLATE_FOLDER (std::string)"/Users/giuliorusso/Documents/Istruzione/Università/Magistrale/Advanced Image Analysis/Progetto/template-set-P/"

# endif

# ifdef TEACHER

    #define ORIGINAL_DATASET (std::string)""
    // preprocessed dataset chosen for the analisys (meanshift 5-5 or meanshift 5-5 + clahe 1)
    #ifdef USE_CLAHE
        #define PREPROCESSED_DATASET (std::string)""            //meanshift 5,5 + clahe 1
    #else
        #define PREPROCESSED_DATASET (std::string)""            //meanshift 5,5
    #endif
    // save the contours of the signals detected
    #define OUTPUT_DATASET (std::string)""

    // to create a new preprocessed dataset
    #define PREPROCESSING_FOLDER (std::string)""
    // contains the .csv file used to save scores and labels(TP/FP)
    #define PERFORMANCE_FOLDER (std::string)""
    // contains the "P" templates used by matchShape() function
    #define TEMPLATE_FOLDER (std::string)""

# endif


// ### CRITERIA CONSTRAINTS SECTION ###

#ifdef USE_CLAHE // (42, 93) are lost

    // N.1  CARTAGO CRITERION
    #define MIN_AREA 139.5                  // IMG 34 (69.5 black = 0) --> IMG 69 (139.5) --> IMG 81 (142.5)
    #define MAX_AREA 6814                     // IMG 89.1 (6814)

    #define MIN_PERIMETER 53.213            // IMG 34 (36.7279 black = 0) --> IMG 81 (53.2132)
    #define MAX_PERIMETER 384.712           // IMG 89.1 (384.711)

    #define MIN_CARTAGO 0.056               // IMG 89.1 (0.0564589)
    #define MAX_CARTAGO 0.390               // IMG 34 (0.528459 black = 0) --> IMG 69 (0.389855)


    // N.2  COLOR CONTRAST CRITERION
    #define MIN_BLACK 8                     // IMG 30 (8) --> IMG 48 (14)
    #define MAX_BLACK 2501                  // IMG 99 (2501)

    #define MIN_WHITE 134                   // IMG 34 (87 black = 0) --> IMG 81 (134)
    #define MAX_WHITE 5720                  // IMG 89 (5720)

    #define MEDIUM1_BW_RATIO 0.963          // IMG 32 (0.962406)
    #define MEDIUM2_BW_RATIO 2.064          // IMG 25 (2.06449)
    #define MAX_BW_RATIO 6.839              // IMG 46 (6.83893)

    #define MIN_HUE 93                      // IMG 42.2 (89) -> IMG 17 (no P) -> IMG 3.2 (93)
    #define MAX_HUE 128                     // IMG 42.1 (128)

    #define MIN_SAT 48                      // IMG 42.2 (12) --> IMG 66 (48) --> IMG 19 (57)
    #define MAX_SAT 231                     // IMG 44 (231)

    #define MIN_VAL 40                      // IMG 19 (40)
    #define MAX_VAL 250                     // IMG 54 (250)


    // N.3  TOP/BOTTOM CRITERION
    #define MIN_TOP_BOTTOM_RATIO 1.180      // IMG 89.2 (1.01865 black = 0) --> IMG 48 (1.18073)
    #define MAX_TOP_BOTTOM_RATIO 2.97       // IMG 17 (no P) --> IMG 61 (2.96943)

    #define MIN_LEFT_RIGHT_RATIO 0.786      // IMG 93 (0.402908) —-> IMG 89.2 (0.750943 black = 0) —-> IMG 3.2 (0.786451)
    #define MAX_LEFT_RIGHT_RATIO 1.797      // IMG 48 (1.79621)

    #define MIN_TOPBOT_RIGHT_RATIO 1.046    // IMG 89.2 (1.0407 black = 0) —-> IMG 48 (1.04613) -> IMG 29 (1.36129)
    #define MAX_TOPBOT_RIGHT_RATIO 10.31    // IMG 84 (10.3)

    #define MIN_TOPBOT_LEFT_RATIO 0.761     // IMG 93 (0.65812) —> IMG 61 (0.761021)
    #define MAX_TOPBOT_LEFT_RATIO 2.424     // IMG 17 (no P) --> IMG 3.2 (2.42308)

    #define MIN_BOT_RIGHT_LEFT_RATIO 0.133  // IMG 84 (0.133136) —> IMG 38 (0.155738)
    #define MAX_BOT_RIGHT_LEFT_RATIO 1.359  // IMG 17 (no P) --> IMG 93 (1.39316) -> IMG 3.2 (1.35897)


    // N.4  CONVEXITY CRITERION
    #define MIN_CONVEXITY 0.685             // IMG 17 (no P) --> IMG 93 (0.6844) —-> IMG 84 (0.685274)
    #define MAX_CONVEXITY 0.943             // IMG 70 (0.942213)


    // N.5  RECTANGULARITY CRITERION
    #define MIN_RECTANGULARITY 0.537        //  IMG 17 (no P) --> IMG 46 (0.537918)
    #define MAX_RECTANGULARITY 0.796        //  IMG 70 (0.795902)

    #define MIN_MAR_ASPECT_RATIO 0.213      // IMG 45.1 (0.213746) —-> IMG 45.2 (0.221636)
    #define MAX_MAR_ASPECT_RATIO 0.990      // IMG 48 (0.989796)


    // N.6  ASPECT-RATIO CRITERION
    #define MIN_BR_ASPECT_RATIO 0.225       // IMG 45.1 (0.225806) —-> IMG 45.2 (0.230769)
    #define MAX_BR_ASPECT_RATIO 1.038       // IMG 17 (no P) --> IMG 93 (1.2029) -> IMG 66 (1.03774)


    // N.7  CIRCULARITY CRITERION
    #define MIN_CIRCULARITY 0.165           // IMG 61 (0.165162)
    #define MAX_CIRCULARITY 0.649           // IMG 66 (0.648435)


    // N.9  TRIANGULARITY CRITERION
    #define MIN_TRIANGULARITY 0.489         // IMG 17 (no P) --> IMG 93 (0.461261) —-> IMG 89.2 (0.488642 black = 0) —-> IMG 84 (0.489207)
    #define MAX_TRIANGULARITY 0.773         // IMG 91 (0.772036)

    
    // N.9  SHAPE CRITERION
    #define MIN_SHAPE_I1 0.0139             // IMG 52 (0.0139356)
    #define MAX_SHAPE_I1 0.934              // IMG 17 (no P) —-> IMG 45.1 (0.933724) —-> IMG 45.2 (0.58536)

    #define MIN_SHAPE_I3 0.013              // IMG 19 (0.0139109)
    #define MAX_SHAPE_I3 0.41               // IMG 89.2 (0.493573 black = 0) --> IMG 29 (0.409815)


#else // only meanshift 5,5   (42, 13) are lost

    // N.1  CARTAGO CRITERION
    #define MIN_AREA 118.5                  // IMG 13 (38.5) --> IMG 34 (64.5 black = 0) --> IMG 81 (118.5)
    #define MAX_AREA 7117.5                 // IMG 89 (7117.5)

    #define MIN_PERIMETER 51.556            // IMG 13 (25.070) --> IMG 34 (33.0711 black = 0) --> IMG 44 (48.6247 black = 0) --> IMG 81 (51.5564)
    #define MAX_PERIMETER 572.854           // IMG 64 (572.854)

    #define MIN_CARTAGO 0.055               // IMG 89 (0.055)
    #define MAX_CARTAGO 0.4351               // IMG 13 (0.652) --> IMG 81 (0.435075)


    // N.2  COLOR CONTRAST CRITERION
    #define MIN_BLACK 10                    // IMG 13 (8) --> IMG 30 (10)
    #define MAX_BLACK 2599                  // IMG 99 (2599)

    #define MIN_WHITE 102                   // IMG  13 (43) --> IMG 81 (102)
    #define MAX_WHITE 6040                  // IMG 89 (6040)

    #define MEDIUM1_BW_RATIO 0.993          // IMG 32 (0.993)
    #define MEDIUM2_BW_RATIO 2.061          // IMG 25 (2.061)
    #define MAX_BW_RATIO 6.729              // IMG 46 (6.729)

    #define MIN_HUE 94                      // IMG 17 (no p) --> IMG 3 (94)
    #define MAX_HUE 129                     // IMG 19 (129)

    #define MIN_SAT 56                      // IMG 42 (13) --> IMG 13 (49) --> IMG 66 (56)
    #define MAX_SAT 236                     // IMG 41 (236)

    #define MIN_VAL 35                      // IMG 19 (35)
    #define MAX_VAL 230                     // IMG 51 (230)


    // N.3  TOP/BOTTOM CRITERION
    #define MIN_TOP_BOTTOM_RATIO 1.254      // IMG 13 (1.039) --> IMG 34 (1.13158 black=0) --> IMG 48 (1.25416)
    #define MAX_TOP_BOTTOM_RATIO 2.958      // IMG 17 (no P) --> IMG 61 (2.95745)

    #define MIN_LEFT_RIGHT_RATIO 0.402      // IMG 93 (0.402)
    #define MAX_LEFT_RIGHT_RATIO 1.883      // IMG 48 (1.883)

    #define MIN_TOPBOT_RIGHT_RATIO 1.195    // IMG 48 (1.195)
    #define MAX_TOPBOT_RIGHT_RATIO 10.185   // IMG 84 (10.185)

    #define MIN_TOPBOT_LEFT_RATIO 0.655     // IMG 93 (0.655)
    #define MAX_TOPBOT_LEFT_RATIO 2.240     // IMG 17 (no P) --> IMG 3 (2.23952)

    #define MIN_BOT_RIGHT_LEFT_RATIO 0.133  // IMG 84 (0.133)
    #define MAX_BOT_RIGHT_LEFT_RATIO 1.393  // IMG 17 (no P) --> IMG 93 (1.393)


    // N.4  CONVEXITY CRITERION
    #define MIN_CONVEXITY 0.495             // IMG 17 (no P) --> IMG 96 (0.495) --> IMG 64 (0.6211)
    #define MAX_CONVEXITY 0.941             // IMG 13 (0.951) --> IMG 70 (0.940467)


    // N.5  RECTANGULARITY CRITERION
    #define MIN_RECTANGULARITY 0.385        //  IMG 96 (0.385) --> IMG 17 (no P) --> IMG 64 (0.5130)
    #define MAX_RECTANGULARITY 0.805        //  IMG 70 (0.805)

    #define MIN_MAR_ASPECT_RATIO 0.220      // IMG 45 (0.220) --> IMG 45 (0.222222) --> IMG 58 (0.241407)
    #define MAX_MAR_ASPECT_RATIO 0.984      // IMG 33 (0.984)


    // N.6  ASPECT-RATIO CRITERION
    #define MIN_BR_ASPECT_RATIO 0.230       // IMG 45 (0.230) --> IMG 45 (0.232) --> IMG 58 (0.261905)
    #define MAX_BR_ASPECT_RATIO 1.17        // IMG 17 (no P) --> IMG 93 (1.16901)


    // N.7  CIRCULARITY CRITERION
    #define MIN_CIRCULARITY 0.169           // IMG 61 (0.169)
    #define MAX_CIRCULARITY 0.622           // IMG 66 (0.622)


    // N.8  TRIANGULARITY CRITERION
    #define MIN_TRIANGULARITY 0.397         // IMG 17 (no P) --> IMG 96 (0.397212) --> IMG 93 (0.462822)
    #define MAX_TRIANGULARITY 0.766         // IMG 42 (0.769) —-> IMG 91 (0.765957)


    // N.9  SHAPE CRITERION
    #define MIN_SHAPE_I1 0.0129             // IMG 19 (0.0129)
    #define MAX_SHAPE_I1 1.484              // IMG 96 (1.484)

    #define MIN_SHAPE_I3 0.008              // IMG 19 (0.00814831)
    #define MAX_SHAPE_I3 0.379              // IMG 17 (no p) --> IMG 96 (0.378792) --> IMG 29 (0.371937)

#endif
