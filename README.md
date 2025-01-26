# Parking Sign recognition
Implementation of the recognition of the parking sign with Image Analysis techniques using C++ and OpenCV library on a subset of Mapillary dataset.

Example of detected signals with green contour:

<img src="./output/04.png" width="50%"><img src="./output/10.png" width="50%"> <br>
<img src="./output/89.png" width="50%"><img src="./output/92.png" width="50%"> <br>

# Requirements
- OpenCV for C++:
    - [Download for macOS](https://docs.opencv.org/3.4/d0/db2/tutorial_macos_install.html)
    - [Download for Linux](https://docs.opencv.org/3.4/d7/d9f/tutorial_linux_install.html)
    - [Download for Windows](https://docs.opencv.org/3.4/d3/d52/tutorial_windows_install.html)

# Dataset
The dataset is composed of 100 images, in .jpg format, annotated using .json files. The images were renamed sequentially from 0 to 99 to better manage them.

The images contain some parking signs that are occluded from another object or partially out of frame. The following analisys take this in account by choosing if these signals have to be considered or not.

|                 | WITH occluded and out of frame | WITHOUT occluded and out of frame |
|-----------------|--------------------------------|-----------------------------------|
| Number of signs |             160                |                145                |

Five image annotations were corrected by adding missing parking signs that were not labeled. These are the images number 21, 25, 57, 86, 93. Thus, if you download the original .json files from the web, they will miss some annotations compared to the one in this repository.

# File organization
The following repository is organized to be built using CMake.

- **3rdparty**: set of functions for image processing
- **cmake**: CMake compiling files
- **myproject**: contains the source code of the project
    - **config.h**: configuration file where the needed path variables have to be initialized in order to run the analisys.
    - **functions.h** and **functions.cpp**: contain the definitions and implementations of the criterion applied during the analisys.
    - **main.cpp**: starting point of the project where the dataset is loaded and the filtering criterion are applied.
    - **performance.py**: performance evaluation code in Python.
- **output**: contains some example images of the dataset with the detected contours.
- **preprocessing-data-analisys**: contains a folder with the infos about the two preprocessing applied in this analisys: **meanshift-5** and **meanshift-5-clahe-1**. Each folder contain:.
    - **histograms.rtf** the values analized for every sign in the dataset.
    - **log.rtf** show the log infos for each image
    - **no-occluded-out-of-frame** and **occluded-out-of-frame**: the **log.rtf** of the experiment, the **label-scores.csv** file with the score of every detection used to evaluate the **precision-recall.png**
- **subset-EIID-AIA-parking-enum.zip**: .zip file of the 100 subset images and their corresponding .json annotations choosen for this project.
- **template-set-P**: set of stretched parking sign used for the Match Shape criterion.

    <img src="./template-set-P/P-45.png" width="100" height="100"><img src="./template-set-P/P-7.png" width="100" height="100"><img src="./template-set-P/P-left.png" width="100" height="100"><img src="./template-set-P/P-right.png" width="100" height="100"><img src="./template-set-P/P-stretch.png" width="100" height="100"> <br>

# How to run it
Build the project using CMake:
1. Clone the repository and select it as source code.
2. Select the destination folder that will contain the built project.
3. Click Configure.
4. Choose the desired generator (e.g. *Unix Makefiles* or any IDE supported by CMake)
5. Add the OpenCV path if they are not founded. Otherwise you can specify your paths inside the CMakeLists.txt.
6. Generate.
7. Open the project folder and fill the *config.h* with your path variables. Define your identifier and define under it the necessary variables.

    Choose the corresponding path variables:

    - *ORIGINAL_DATASET*: the path to the original dataset folder *subset-EIID-AIA-parking-enum*.
    - *PREPROCESSED_DATASET*: the path to the desired preprocessed dataset to analize, that is the meanshift-5 or the meanshift-5 plus clahe-1.
    - *OUTPUT_DATASET*: the path containing the entire dataset with the identified contours.
    - *PREPROCESSING_FOLDER*: the path to the preprocessed dataset to extract, that is the meanshift-5 or the meanshift-5 plus clahe-1.
    - *PERFORMANCE_FOLDER*: the path where to save the *label-score.csv* together with the *performance.py* to obtain the AUPR curve.
    - *TEMPLATE_FOLDER*: the path to the *template-set-P* folder.

    Define your intial identifier:

    - *PREPROCESSING*: if specified, apply the mean shift 5 preprocessing and save the new dataset into the specified folder.
    - *SKIP_OCCLUDED_AND_OOF*: if specified, signs labeled as occluded or out of frame are not considered.
    - *USE_CLAHE*: if specified, the analisys will consider to apply clahe 1.

8. Run the *main.cpp* file to perform the analisys.

# Performance
The best performance is achived with the mean shift 5 plus clahe 1 on the dataset without the occluded and out of frame signs. The best AUPR is 70.6% with an accuracy of 73.79%.

<img src="./preprocessing-data-analisys/meanshift-5-clahe-1/no-occluded-out-of-frame/precision-recall.png" width="70%"> <br>
