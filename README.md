# CPE-462-Final-Project

CPE 462 Final Project Image Enhancement  
Christopher Kurtz, Jonathan Melamed, Jonah Kramer  
I pledge my honor that I have abided by the Stevens Honor System  

To compile ImgProcSuite.cpp on Windows (there is only a single .cpp file so there is only need to find OpenCV libs):
   Using CMake:
    Modify CMakeLists.txt to point to the OpenCV directory on your system
   Using Make:
    Modify the sample Makefile to point to the OpenCV directory on your system
   Using Visual Studio:
     Set the build target for your project to x64
     In your project, navigate to Project Properties and then to VC++ Directories
     Under "Include Directories" add c:\opencv\build\include (assuming you have installed OpenCV in your root directory)
     Under "Library Directories" add c:\opencv\build\x64\vc15\lib (also assuming you have installed OpenCV in your root directory)
     Navigate to the Linker settings and under "Additional Dependencies" add opencv_world412d.lib
  The .cpp file should compile successfully given that these steps have been followed
  
  Program command line arguments:
    The program takes in 4 arguments when called in the command line
      1. input image filename (put the image in the same folder that the executable is in for ease of use)
      2. output image filename
      3. image width
      4. image height
      Example command line call: ./IPRFin sampleIMG.jpg newIMG.jpg 2316 3088
