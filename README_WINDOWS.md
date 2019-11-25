GridDB C Client

## Overview

The GridDB C Client provides a C interface for GridDB.  
This repository also includes simple samples.  
Please refer to the API(C) section in API Reference for detailed information.

## Operating environment

Library building and program execution are checked in the environment below.

    OS: Windows 10(x64) (VS2017)
    GridDB server: V4.2 CE(Community Edition) on CentOS 7.6(x64)
    Software: Visual Studio 2017, CMake 3.15.2

## Quick start (Windows)

### Build a C client

#### Generate standard build files from cmake

Open CMAKE_PATH/bin/cmake-gui.exe

Set link for source code in cmake-gui: <C_CLIENT_SRC_PATH>\client\c\sample

Set link for the binaries in cmake-gui: link to folder which generated files are store (*.sln , *.vcxproj files) as <PATH_TO_OUTPUT_CMAKE>

Run Configure with set Specify the generator for this project is Visual Studio 15 2017, and for Optional toolset to use is x64

Run Generate to get standard build files

#### Build with VS2017

Open <PATH_TO_OUTPUT_CMAKE>\client.sln by Visual Studio 2017
Choose build client solution to build c client. Output at <C_CLIENT_SRC_PATH>\bin\x64\Release (build with Release mode, <C_CLIENT_SRC_PATH>\bin\x64\Debug if choose Debug mode)

    gridstore_c.dll
    gridstore_c.lib

### Execute a sample program

GridDB server need to be started in advance.

After starting VS with <PATH_TO_OUTPUT_CMAKE>\sample.sln, build sample project. A sample.exe is created under the <C_CLIENT_SRC_PATH>\bin\x64\Release folder (build with Release mode, <C_CLIENT_SRC_PATH>\bin\x64\Debug if choose Debug mode).

    > sample.exe sample1 en <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]


(Additional information)
- The gridstore.h file on client\c\include folder is used for building.  
- There are some samples on client\c\sample folder.

## Document
  Refer to the file below for more detailed information.  
  - [API Reference](https://griddb.github.io/griddb_nosql/manual/GridDB_API_Reference.html)
  - [API Reference(Japanese)](https://griddb.github.io/griddb_nosql/manual/GridDB_API_Reference_ja.html)

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