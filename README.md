<img src="https://griddb.org/brand-resources/griddb-logo/png/color.png" align="center" height="240" alt="GridDB"/>

# GridDB C Client

## Overview

The GridDB C Client provides a C interface for GridDB.  
This repository also includes simple samples.  
Please refer to [C API Reference](http://griddb.github.io/docs-en/manuals/GridDB_C_API_Reference.html) for detailed information.

## Operating environment

Library building and program execution are checked in the environment below.

    OS: CentOS 7.9(x64) (gcc 10)
    GridDB server: V5.1 CE(Community Edition), CentOS 7.9(x64) (gcc 4.8.5)

    OS: Ubuntu 20.04(x64) (gcc 10)
    GridDB server: V5.1 CE(Community Edition), Ubuntu 18.04(x64) (gcc 4.8.5)

## Quick start (CentOS, Ubuntu)

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

[CentOS, Ubuntu]

About installation information:  
https://software.opensuse.org/download/package?project=home:knonomura&package=griddb-c-client

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
