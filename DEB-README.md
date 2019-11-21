GridDB C Client

## Overview

The GridDB C Client provides a C interface for GridDB.  
This repository also includes simple samples.  
Please refer to the API(C) section in API Reference for detailed information.

## Operating environment

Program execution are checked in the environment below.

    OS: Ubuntu 18.04(x64)  
    GridDB server: V4.2 CE(Community Edition) on Ubuntu 18.04(x64)
    gcc/g++: 4.8.5

## Quick start (Ubuntu)

### Install

(A) Using apt-get command

    $ sudo sh -c "echo 'deb http://download.opensuse.org/repositories/home:/oanhltk/xUbuntu_18.04/ /' > /etc/apt/sources.list.d/home:oanhltk.list"
    $ wget -nv https://download.opensuse.org/repositories/home:oanhltk/xUbuntu_18.04/Release.key -O Release.key
    $ sudo apt-key add - < Release.key
    $ sudo apt-get update
    $ sudo apt-get install griddb-c-client

  About installation information: 
  https://software.opensuse.org/download/package?project=home:oanhltk&package=griddb-c-client

(B) Using dpkg command
- After downloading DEB file (https://github.com/griddb/c_client/releases), please run the following:

    $ sudo dpkg -i griddb-c-client-X.X.X_amd64.deb

### Execute a sample program
GridDB server need to be started in advance.

    $ cp /usr/griddb_c_client-X.X.X/sample/sample1.c .
    $ gcc sample1.c -lgridstore
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(Additional information)
- The gridstore.h file on /usr/include folder is used for building.  
- The libgridstore.so file on /usr/lib/x86_64-linux-gnu folder is used for executing.  

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
