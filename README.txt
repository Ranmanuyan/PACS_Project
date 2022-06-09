** README ** 

This project is the exam project "Implementation of branch and bound algorithm for stochastic scheduling problems---using bob++ library" for course Advanced Programming for Scientific Computing of POLIMI.
All the codes are wrote and tested in  g++ under Mac/Unix system.
Please make sure everything is downloaded, including the "." hidden files.


Installation of libraries and guideline for codes are presented here.


** *Installation* ** 
Firstly, to run this project, several packages(bobpp, Eigen and Boost) need to be installed:

1, bob++ lirary is a library for the development of branch and bound algorithm, which is provided in this project, i.e., the folder bobpp_lib.
2, Eigen library is a standard linear algebra library used for the matrix operations.
3, Boost library is a standard library providing templates for scientific computing.


The Installation of bob++ lirary should follow the steps:

1, Open the Terminal, cd go to folder "bobpp_lib", input: "./configure; make; make install"(wiithout""), press enter.
This will configure, build and install this package, no further work described in the INSTALL file need to be done.


2, Check the installation
    By default, `make install' installs the package's commands under `/usr/local/bin', and include files under `/usr/local/include', etc.
    we can check:
    a. INCLUDE DIR: Go to the folder "/usr/local/include”, you will find a folder named “bobpp-1,0/bobpp”, here you will find the needed headers (.h files) that are needed to include the bob library in your projects.
    b. LIB DIR: Folder "/usr/local/lib", here you will find a set of libraries (e.g., libbobpp-1.0.dylib) already compiled, to be added to your projects 

If everything is ok, the installation of bobpp should be successful.


The Installation of Eigen library should follow the steps:

1, In order to use Eigen, you just need to download and extract Eigen's source code, the only thing is remember the installation path (If it has already been installed, check the installation path), and match it in source code files, i.e., /src/pfs.h, line 
lb.h" and "pfs.h", make sure the Eigen related library import path is correct, i.e. "#include <Eigen/unsupported/Eigen/MatrixFunctions>", "#include <Eigen/Core>", "#include <Eigen/Dense>"...


Boost library is optional in this project, several alternative functions used it, it can be commented and run without boost library.
1, If you didn't install boost in your laptop, you can download it https://www.boost.org/users/history/version_1_79_0.html

2, unpack it, and "cd" go to the folder, then run "./bootstrap.sh", then run  "./b2", and then run "./b2 install".

3, remember the insatll path of boost library, and add it in Makefile, which is located src/Makefile, line 113, CXX = g++   -I


Now all the related libraries are installed.


** *Run the codes* ** 
How to run the project code?

1, Open Terminal, cd into the src folder.

2, Compile the project with the command: $ make

3, After successful compiling, we can see exectuable fils "pfs" and "pfsthr" in this same folder, otherwise the compile failed. 

4, Now, run the executable file, $ ./pfs

5, Data file name will be asked to be input, I have put the data file in project folder, so input $: ../data.txt, then return.

6, The algorithm is running in the Terminal, and the results are collected in result.txt in project folder.
