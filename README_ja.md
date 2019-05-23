GridDB Cクライアント

## 概要

GridDB CクライアントはC言語用のインタフェースを提供します。  
また、このリポジトリには簡単なサンプルプログラムがあります。  
詳細はAPIリファレンスのAPI(C言語)の章を参照してください。

## 動作環境

以下の環境でCクライアントのビルドとサンプルプログラムの実行を確認しています。

    OS: CentOS 7.6(x64) (gcc 4.8.5), Windows 10(x64) (VS2017)
    GridDB server: V4.2 CE(Community Edition), CentOS 7.6(x64)

## クイックスタート(CentOS)

### Cクライアントのビルド

    $ cd client/c
    $ ./bootstrap.sh
    $ ./configure
    $ make 
    
を実行すると、binフォルダの下に以下のファイルやリンクが作成されます。

    libgridstore.so
	libgridstore.so.0
	libgridstore.so.0.0.0

### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

    $ cp client/c/sample/sample1.c .
    $ gcc -I./client/c/include -L./bin sample1.c -lgridstore
    $ export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./bin
    $ ./a.out <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]

## クイックスタート(Windows)

### Cクライアントのビルド

client.slnファイルをクリックしてVSを起動後、clientプロジェクトをビルドしてください。

bin/x64フォルダの下に以下のファイルが作成されます。

    gridstore_c.dll
    gridstore_c.lib

### サンプルプログラムの実行
事前にGridDBサーバを起動しておく必要があります。

client.slnファイルをクリックしてVSを起動後、sampleプロジェクトをビルドすると、bin/x64フォルダの下にsample.exeファイルが作成されます。

    > sample.exe sample1 en <GridDB notification address(default is 239.0.0.1)> <GridDB notification port(default is 31999)>
      <GridDB cluster name> <GridDB user> <GridDB password>
      -->Person: name=name02 status=false count=2 lob=[65, 66, 67, 68, 69, 70, 71, 72, 73, 74]


(追加情報)
- client/c/includeフォルダの下に、ビルドに使われるgridstore.hファイルがあります。  
- client/c/sampleフォルダの下に、サンプルプログラムがあります。

## ドキュメント
  詳細は以下のドキュメントを参照してください。
  - [APIリファレンス](https://griddb.github.io/griddb_nosql/manual/GridDB_API_Reference_ja.html)

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
