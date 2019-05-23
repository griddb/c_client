GridDB C Client

## Overview

The GridDB C Client provides a C interface for GridDB.  
This repository also includes simple samples.  
Please refer to the API(C) section in API Reference for detailed information.

## Operating environment

Library building and program execution are checked in the environment below.

    OS: CentOS 7.6(x64) (gcc 4.8.5), Windows 10(x64) (VS2017)
    GridDB server: V4.2 CE(Community Edition) on CentOS 7.6(x64)

## Quick start (CentOS)

### Build a C client
    
Run the make command like the following:
    
    $ cd client/c
    $ ./bootstrap.sh
    $ ./configure
    $ make 
    
and create the following file and links under the bin/ folder.
    
    libgridstore.so
	libgridstore.so.0
	libgridstore.so.0.0.0

### Execute a sample program
GridDB server need to be started in advance.

    $ cp client/c/sample/sample1.c .
    $ gcc -I./client/c/include -L./bin sample1.c -lgridstore
    $ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./bin
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

## Quick start (Windows)

### Build a C client

After starting VS with client.sln, build client project.

The following files are created under the bin/x64 folder.

    gridstore_c.dll
    gridstore_c.lib

### Execute a sample program
GridDB server need to be started in advance.

After starting VS with client.sln, build sample project. A sample.exe is created under the bin/x64 folder.

    > sample.exe sample1 en <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]


(Additional information)
- The gridstore.h file on client/c/include folder is used for building.  
- There are some samples on client/c/sample folder.

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
