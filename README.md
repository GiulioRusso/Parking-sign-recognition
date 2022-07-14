# Parking-sign-recognition
C++ and OpenCV implementation of the recognition of the parking sign with Image Analysis techniques

# How to run it
Generate OpenCV project

1. Download the latest stable Cmake version (Last Release from the official site)

2. Source folder: project setup (ATTENTION !!! The project setup folder after generating the Xcode project is MODIFIED, so FOR EVERY project you create you must have a DIFFERENT project setup folder. So when you need to generate a new project, make a copy the free-project-setup-opencv4 folder, rename it, and move it to where you want it to be - along with the project folder that will be generated)

In this case it is: parking-project-setup-opencv4

Destination folder: where you want the project to be created

In this case it is: parking

3. Configure (if it doesn't find the OpenCV_DIR folder, click on the three dots on the right and select the build_opencv folder)

N.B. The OpenCV library needs to be installed on the machine, otherwise it doesn't know where to get it
N.B. If it fails to create the project due to a compiler misrecognition error, launch it from the terminal

sudo xcode-select -s /Applications/Xcode.app/Contents/Developer

which is needed to use the Xcode Developer tools

4. Generate

N.B. After doing Generate, both the project setup and the destination must NOT change path in the folders and you must NOT change the names of the folders or the project, otherwise nothing will work anymore

5. In the destination folder click on the Xcode project, at the top choose the exec, click on the Xcode Play

N.B. If, following a software update, Xcode should give errors, regenerate the project from scratch (saving the .cpp and .h and then replacing them in the myproject folder of the project setup)



MISTAKES:

- PhaseScriptExecution failed with a nonzero exit code: I had regenerated the project after changing the name of a directory (from “Magistrale” to “Magistrale (LM-32)”). Apparently those round brackets gave error to line 5 in a .sh file that it generated automatically in parking / aia.build / Debug / ZERO_CHECK.build / Script-. . . The line corresponded to the path of the project and said that there was an "expected token (". So I put the name back to "Magistrale" and everything worked

- "unable to find sdk": As soon as the project with the correct directory name was generated, I pressed on "Build" and it disappeared immediately giving me "Build Succeeded". I have not yet understood what it is



Install OpenCV library on Mac

1. Check the python and python 3 version installed on the Mac with

python —-version
python3 —-version

* need python 2.7 and 3.8, but it should be installed on the machine automatically

2. Install numpy with

pip3 install numpy —-user

* --user is used to execute the superuser command

3. Check the git version, as from there you will download the OpenCV library to compile on the machine

git —-version
cmake —-version
brew install cmake
/ bin / bash -c "$ (curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

4. Create an OpenCV folder (which will contain the source and compiled library) in Applications. Reach this folder from the terminal (pwd - shows you where you are - cd - makes you move to the directory if you can reach it from where you are - cd. - makes you go back -)

5. On the terminal, reached the OpenCV folder in Applications launch

git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

We will create an opencv and opencv_contrib folder in the OpenCV folder you created

6. Open the CMake application, select the source folder the opencv folder installed by the first git clone of the previous step, select the build_opencv destination folder that you create in the OpenCV folder in Applications. Use as a Unix compiler



Import the OpenCV library into an Xcode project WITHOUT Cmake

N.B. the OpenCV library must be installed as shown in the previous steps

1. On Xcode, look for the project file with the .proj extension and click on it once

2. Search for "Search Paths" and enter in:

Header Search Paths: /usr/local/Cellar/opencv/4.5.2/include
Library Search Paths: /usr/local/Cellar/opencv/4.5.2/lib

N.B. insert after opencv / the version of OpenCV installed on your computer, not necessarily 4.5.2. To see it you need to locate yourself in the folder specified by the Path

3. Install pkg-config by running the following terminal commands

$ brew install pkg-config
$ pkg-config —cflags —libs opencv

The output will be: -I / usr / local / Cellar. . .

-I / usr / local / Cellar / opencv / 4.5.2 / include / opencv4
-L / usr / local / Cellar / opencv / 4.5.2 / lib
-lopencv_gapi
-lopencv_stitching
-lopencv_alphamat
-lopencv_aruco
-lopencv_bgsegm
-lopencv_bioinspired
-lopencv_ccalib
-lopencv_dnn_objdetect
-lopencv_dnn_superres
-lopencv_dpm
-lopencv_face
-lopencv_freetype
-lopencv_fuzzy
-lopencv_hfs
-lopencv_img_hash
-lopencv_intensity_transform
-lopencv_line_descriptor
-lopencv_mcc
-lopencv_quality
-lopencv_rapid
-lopencv_reg
-lopencv_rgbd
-lopencv_saliency
-lopencv_sfm
-lopencv_stereo
-lopencv_structured_light
-lopencv_phase_unwrapping
-lopencv_superres
-lopencv_optflow
-lopencv_surface_matching
-lopencv_tracking
-lopencv_highgui
-lopencv_datasets
-lopencv_text
-lopencv_plot
-lopencv_videostab
-lopencv_videoio
-lopencv_viz
-lopencv_wechat_qrcode
-lopencv_xfeatures2d
-lopencv_shape
-lopencv_ml
-lopencv_ximgproc
-lopencv_video
-lopencv_dnn
-lopencv_xobjdetect
-lopencv_objdetect
-lopencv_calib3d
-lopencv_imgcodecs
-lopencv_features2d
-lopencv_flann
-lopencv_xphoto
-lopencv_photo
-lopencv_imgproc
-lopencv_core

Everything needs to be copied, and pasted on the Xcode entry: Linking -> Other Linker Flags
