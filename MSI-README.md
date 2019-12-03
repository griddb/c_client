GridDB C Client

## Overview

The GridDB C Client provides a C interface for GridDB.
This repository also includes simple samples.
Please refer to the API(C) section in API Reference for detailed information.

## Operating environment

Library building and program execution are checked in the environment below.

    OS: Windows 10(x64) (VS2017)
    GridDB server: V4.3 CE(Community Edition) on CentOS 7.6(x64)

## Quick start

### Install C client

Install the MSI package, the package is extracted into C:/Program Files/GridDB/C Client/4.3.0 folder.

### How to create and build a sample program

Explains how to compile a program. The following is the procedure for VisualStudio2017.
* Create x64 project. Select [x64] in [Configuration Manager]-[Active Solution Platform]-[New].
* Add sample1.c to the Source Files of the project.
* Include directory settings. Specify the directory where gridstore.h exists in [Configuration Properties]-[C / C ++]-[General]-[Additional Include Directories].
* Import library (gridstore_c.lib) settings. Add to [Linker]-[Input]-[Additional Dependencies]
* Specify the directory where gridstore_c.lib exists in [Configuration Properties]-[Linker]-[General]-[Additional Library Directories].
* Build the sample project

### Execute a sample program
GridDB server need to be started in advance.

After building VS sample project, build the project and run sample.exe.

    > sample.exe <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

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
