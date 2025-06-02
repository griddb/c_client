<img src="https://griddb.org/brand-resources/griddb-logo/png/color.png" align="center" height="240" alt="GridDB"/>

# GridDB C Client

## Overview

The GridDB C Client provides a C interface for GridDB.  
This repository also includes simple samples.  
Please refer to [C API Reference](http://griddb.github.io/docs-en/manuals/GridDB_C_API_Reference.html) for detailed information.

## Operating environment

Library building and program execution are checked in the environment below.

    OS: Ubuntu 22.04(x64) (gcc 11), Windows 11(x64) (VS2017), MacOS Catalina 12 (clang 13.1.6)
    GridDB server: V5.8 CE(Community Edition), Ubuntu 22.04(x64)

    OS: RockyLinux 9.4(x64) (gcc 11)
    GridDB server: V5.8 CE(Community Edition), RockyLinux 9.4(x64)

## Quick start (Ubuntu, RockyLinux)

### Using source code

#### Build
    
Run the make command like the following:
    
    $ cd client/c
    $ ./bootstrap.sh
    $ ./configure
    $ make 
    
and create the following file and links under the bin/ folder.
    
    libgridstore.so
	libgridstore.so.0
	libgridstore.so.0.0.0

#### Execute a sample program
GridDB server need to be started in advance.

    $ cp client/c/sample/sample1.c .
    $ gcc -I./client/c/include -L./bin sample1.c -lgridstore
    $ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./bin
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(Additional information)
- The gridstore.h file on client/c/include folder is used for building.  
- There are some samples on client/c/sample folder.


### Using RPM or DEB

#### Install

[Ubuntu, RockyLinux]

Install the RPM/DEB package (https://github.com/griddb/c_client/releases).

#### Execute a sample program
GridDB server need to be started in advance.

    $ cp /usr/griddb_c_client-X.X.X/sample/sample1.c .
    $ gcc sample1.c -lgridstore
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(Additional information)
- The gridstore.h file on /usr/include folder is used for building.  
- The libgridstore.so file on /usr/lib64 (CentOS), /usr/lib/x86_64-linux-gnu (Ubuntu) folder is used for building and executing.  

## Quick start (MacOS)

### Using source code

#### Build

Run the make command like the following:

    $ cd client/c
    $ ./bootstrap.sh
    $ ./configure
    $ make

and create the following file and links under the bin/ folder.

    libgridstore.dylib
    libgridstore.0.dylib
    libgridstore.0.0.0.dylib

#### Execute a sample program
GridDB server need to be started in advance.

    $ cp client/c/sample/sample1.c .
    $ gcc -I./client/c/include -L./bin sample1.c -lgridstore
    $ export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:./bin
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(Additional information)
- The gridstore.h file on client/c/include folder is used for building.
- There are some samples on client/c/sample folder.

## Quick start (Windows)

### Using source code

#### Generate standard build files from cmake

* Open CMAKE_PATH/bin/cmake-gui.exe.
* Set link for source code in cmake-gui: <C_CLIENT_SRC_PATH>\client\c\sample.
* Set link for the binaries in cmake-gui: link to folder which generated files are store (*.sln , *.vcxproj files) as <PATH_TO_OUTPUT_CMAKE>.
* Run Configure with set Specify the generator for this project is Visual Studio 15 2017 Win64.
* Run Generate to get standard build files.

#### Build with VS2017

Open <PATH_TO_OUTPUT_CMAKE>\client.sln by VS2017.

Choose build client solution to build c client.

Output at <C_CLIENT_SRC_PATH>\bin\x64\Release (build with Release mode, <C_CLIENT_SRC_PATH>\bin\x64\Debug if choose Debug mode).

    gridstore_c.dll
    gridstore_c.lib

#### Execute a sample program

GridDB server need to be started in advance.

After starting VS with <PATH_TO_OUTPUT_CMAKE>\sample.sln, build sample project.

A sample.exe is created under the <C_CLIENT_SRC_PATH>\bin\x64\Release folder (build with Release mode, <C_CLIENT_SRC_PATH>\bin\x64\Debug if choose Debug mode).

    > sample.exe sample1 en <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(Additional information)
- The gridstore.h file on client/c/include folder is used for building.  
- There are some samples on client/c/sample folder.


### Using MSI

#### Install

Install the MSI package (https://github.com/griddb/c_client/releases), the package is extracted into C:/Program Files/GridDB/GridDB C Client/X.X.X folder.

#### How to create and build a sample program

Explains how to compile a program. The following is the procedure for VS2017.
* Create x64 project. Select [x64] in [Configuration Manager]-[Active Solution Platform]-[New].
* Add sample1.c to the Source Files of the project.
* Include directory settings. Specify the directory where gridstore.h exists in [Configuration Properties]-[C / C ++]-[General]-[Additional Include Directories].
* Import library (gridstore_c.lib) settings. Add to [Linker]-[Input]-[Additional Dependencies]
* Specify the directory where gridstore_c.lib exists in [Configuration Properties]-[Linker]-[General]-[Additional Library Directories].
* Build the sample project

#### Execute a sample program
GridDB server need to be started in advance.

After building VS sample project, build the project and run sample.exe.

    > sample.exe <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

## Document
  Refer to the file below for more detailed information.  
  - [C API Reference](http://griddb.github.io/docs-en/manuals/GridDB_C_API_Reference.html)

Note: GEOMETRY type is not available for C Client CE(Community Edition).  

## Community
  * Issues  
    Use the GitHub issue function if you have any requests, questions, or bug reports. 
  * PullRequest  
    Use the GitHub pull request function if you want to contribute code.
    You'll need to agree GridDB Contributor License Agreement(CLA_rev1.1.pdf).
    By using the GitHub pull request function, you shall be deemed to have agreed to GridDB Contributor License Agreement.

## License
  The C client source license is Apache License, version 2.0.  
  See 3rd_party/3rd_party.md for the source and license of the third party.
