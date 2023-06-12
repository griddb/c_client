<img src="https://griddb.org/brand-resources/griddb-logo/png/color.png" align="center" height="240" alt="GridDB"/>

# GridDB Cクライアント

## 概要

GridDB CクライアントはC言語用のインタフェースを提供します。  
また、このリポジトリには簡単なサンプルプログラムがあります。  
詳細は[C APIリファレンス](http://griddb.github.io/docs-ja/manuals/GridDB_C_API_Reference.html)を参照してください。

## 動作環境

以下の環境でCクライアントのビルドとサンプルプログラムの実行を確認しています。

    OS: CentOS 7.9(x64) (gcc 10), Windows 10(x64) (VS2017, CMake 3.15.2)
    GridDB server: V5.3 CE(Community Edition), CentOS 7.9(x64) (gcc 4.8.5)

    OS: Ubuntu 20.04(x64) (gcc 10)
    GridDB server: V5.3 CE(Community Edition), Ubuntu 18.04(x64) (gcc 4.8.5)

    OS: MacOS Catalina 10.15 (clang 11.0.3)
    GridDB server: V5.3 CE(Community Edition), CentOS 7.9(x64) (gcc 4.8.5)

## クイックスタート(CentOS, Ubuntu)

### ソースコードの利用

#### Cクライアントのビルド

    $ cd client/c
    $ ./bootstrap.sh
    $ ./configure
    $ make 
    
を実行すると、binフォルダの下に以下のファイルやリンクが作成されます。

    libgridstore.so
	libgridstore.so.0
	libgridstore.so.0.0.0

#### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

    $ cp client/c/sample/sample1.c .
    $ gcc -I./client/c/include -L./bin sample1.c -lgridstore
    $ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./bin
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(追加情報)
- client/c/includeフォルダの下に、ビルドに使われるgridstore.hファイルがあります。  
- client/c/sampleフォルダの下に、サンプルプログラムがあります。


### RPM/DEBファイルの利用

#### インストール

[CentOS, Ubuntu]

以下のURLを参照してください。  
https://software.opensuse.org/download/package?project=home:knonomura&package=griddb-c-client

#### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

    $ cp /usr/griddb_c_client-X.X.X/sample/sample1.c .
    $ gcc sample1.c -lgridstore
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(追加情報)
- /usr/includeフォルダの下に、ビルドに使われるgridstore.hファイルがあります。  
- /usr/lib64(CentOS)、/usr/lib/x86_64-linux-gnu(Ubuntu)フォルダの下に、ビルドおよび実行に使われるlibgridstore.soファイルがあります。  

## クイックスタート(MacOS)

### ソースコードの利用

#### Cクライアントのビルド

    $ cd client/c
    $ ./bootstrap.sh
    $ ./configure
    $ make 
    
を実行すると、binフォルダの下に以下のファイルやリンクが作成されます。

    libgridstore.dylib
    libgridstore.0.dylib
    libgridstore.0.0.0.dylib

#### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

    $ cp client/c/sample/sample1.c .
    $ gcc -I./client/c/include -L./bin sample1.c -lgridstore
    $ export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:./bin
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(追加情報)
- client/c/includeフォルダの下に、ビルドに使われるgridstore.hファイルがあります。  
- client/c/sampleフォルダの下に、サンプルプログラムがあります。

## クイックスタート(Windows)

### ソースコードの利用

#### CMakeによるVSソリューションファイルの生成

* <CMAKE_PATH>\bin\cmake-gui.exeをオープンします。
* "Where is the source code"に <C_CLIENT_SRC_PATH>\client\c\sample フォルダを指定します。
* "Where to build the binaries"にソリューションファイル(*.sln , *.vcxproj)の生成先フォルダを指定します。
* Configureボタンをクリック後、 "Specify the generator for this project"に"Visual Studio 15 2017 Win64"を指定します。
* Generateボタンをクリックするとファイルが生成されます。

#### VS2017によるビルド

ソリューションファイルの生成先フォルダのclient.slnファイルをクリックしてVSを起動後、clientソリューションをビルドしてください。

<C_CLIENT_SRC_PATH>\bin\x64\Releaseフォルダ(リリースモード時)、<C_CLIENT_SRC_PATH>\bin\x64\Debugフォルダ(デバッグモード時)の下に以下のファイルが作成されます。

    gridstore_c.dll
    gridstore_c.lib

### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

ソリューションファイルの生成先フォルダのsample.slnファイルをクリックしてVSを起動後、sampleプロジェクトをビルドすると、<C_CLIENT_SRC_PATH>\bin\x64\Releaseフォルダ(リリースモード時)、<C_CLIENT_SRC_PATH>\bin\x64\Debugフォルダ(デバッグモード時)の下にsample.exeファイルが生成されます。

    > sample.exe sample1 en <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

(追加情報)
- client/c/includeフォルダの下に、ビルドに使われるgridstore.hファイルがあります。  
- client/c/sampleフォルダの下に、サンプルプログラムがあります。

### MSIファイルの利用

#### インストール

MSIパッケージ(https://github.com/griddb/c_client/releases) をダウンロードして実行してください。C:/Program Files/GridDB/GridDB C Client/X.X.Xフォルダの下にインストールされます。

#### サンプルプログラムの作成、ビルド

以下は、VS2017の場合の手順です。
* x64のプロジェクトを作成。[構成マネージャ] - [アクティブソリューションプラットフォーム] - [新規作成]で "x64"を選択します。
* プロジェクトのソース ファイルにsample1.cファイルを追加します。
* Includeディレクトリの設定。[構成プロパティ] - [C/C++] - [全般] - [追加のインクルードディレクトリ]に gridstore.h が存在するディレクトリを指定します。
* インポートライブラリ(gridstore_c.lib)の設定。[リンカー] - [入力] - [追加の依存ファイル] に追加します。
* プロジェクトをビルドします。

#### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

    > sample.exe <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

## ドキュメント
  詳細は以下のドキュメントを参照してください。
  - [C APIリファレンス](http://griddb.github.io/docs-ja/manuals/GridDB_C_API_Reference.html)

※Cクライアント(Community Edition)では空間型は利用できません。  

## コミュニティ
  * Issues  
    質問、不具合報告はissue機能をご利用ください。
  * PullRequest  
    GridDB Contributor License Agreement(CLA_rev1.1.pdf)に同意して頂く必要があります。
    PullRequest機能をご利用の場合はGridDB Contributor License Agreementに同意したものとみなします。

## ライセンス
  CクライアントのライセンスはApache License, version 2.0です。  
  サードパーティのソースとライセンスについては3rd_party/3rd_party.mdを参照ください。
