/*------------------------------------------------------------------*/
// Copyright (c) 2017 TOSHIBA Digital Solutions Corporation. All Rights Reserved.
/*------------------------------------------------------------------*/
/*!
	@JP
	@file
	@brief GridDBのC言語向け公開API

	@EN
	@file
	@brief GridDB public API in C

	@ENDL
*/
#ifndef GRIDSTORE_H_
#define GRIDSTORE_H_

/*!
	@JP
	@page GridDB C API

	GridDBの公開C言語インタフェースを定義します

	- インタフェース型
		- @ref Group_GSAggregationResult
		- @ref Group_GSCollection
		- @ref Group_GSContainer
		- @ref Group_GSGridStore
		- @ref Group_GSGridStoreFactory
		- @ref Group_GSQuery
		- @ref Group_GSRowSet
		- @ref Group_GSTimeSeries
		- @ref Group_GSRow
		- @ref Group_GSRowKeyPredicate
		- @ref Group_GSPartitionController

	- ユーティリティ
		- @ref Group_GSTimestamp
		- @ref Group_ErrorHandling

	- ヘッダファイル
		- @ref gridstore.h

	@EN
	@page GridDB C API

	Defines GridDB public interface in C programming language.

	- Interface Types
		- @ref Group_GSAggregationResult
		- @ref Group_GSCollection
		- @ref Group_GSContainer
		- @ref Group_GSGridStore
		- @ref Group_GSGridStoreFactory
		- @ref Group_GSQuery
		- @ref Group_GSRowSet
		- @ref Group_GSTimeSeries
		- @ref Group_GSRow
		- @ref Group_GSRowKeyPredicate
		- @ref Group_GSPartitionController

	- Utilities
		- @ref Group_GSTimestamp
		- @ref Group_ErrorHandling

	- Header Files
		- @ref gridstore.h

	@ENDL
 */

#include <stdlib.h>		// NULL, size_t

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#include "gsext/stdint.h"
#else
#include <stdint.h>
#endif

#ifndef GS_CLIENT_VERSION_MAJOR
/*!
	@JP
	@brief GridDBクライアントのメジャーバージョンを表す数値です。

	@EN
	@brief Represents the major version of GridDB client

	@ENDL
*/
#define GS_CLIENT_VERSION_MAJOR 4
#endif

#ifndef GS_CLIENT_VERSION_MINOR
/*!
	@JP
	@brief GridDBクライアントのマイナーバージョンを表す数値です。

	@EN
	@brief Represents the minor version of GridDB client

	@ENDL
*/
#define GS_CLIENT_VERSION_MINOR 2
#endif

// C API
#ifdef __cplusplus
extern "C" {
#endif

#ifndef GS_INTERNAL_DEFINITION_VISIBLE
#define GS_INTERNAL_DEFINITION_VISIBLE 1
#endif

#if GS_INTERNAL_DEFINITION_VISIBLE

#if defined(GS_DLL_INSIDE) && defined(__GNUC__) && !defined(_WIN32)
#define GS_DLL_PUBLIC __attribute__ ((visibility("default")))
#else
#define GS_DLL_PUBLIC
#endif

#ifdef _WIN32
#define GS_API_CALL __stdcall
#else
#define GS_API_CALL
#endif

#if defined(__GNUC__) && !defined(GS_WARN_UNUSED)
#define GS_STATIC_HEADER_FUNC_ATTR __attribute__((unused))
#else
#define GS_STATIC_HEADER_FUNC_ATTR
#endif

#define GS_STATIC_HEADER_FUNC_SPECIFIER GS_STATIC_HEADER_FUNC_ATTR static

#ifndef GS_DEPRECATION_IGNORABLE
#ifdef GS_DLL_INSIDE
#define GS_DEPRECATION_IGNORABLE 1
#else
#define GS_DEPRECATION_IGNORABLE 0
#endif
#endif

#if !GS_DEPRECATION_IGNORABLE && defined(__GNUC__)
#define GS_DEPRECATED_SYMBOL1(symbol) symbol __attribute__((deprecated))
#elif !GS_DEPRECATION_IGNORABLE && defined(_MSC_VER)
#define GS_DEPRECATED_SYMBOL1(symbol) __declspec(deprecated) symbol
#else
#define GS_DEPRECATED_SYMBOL1(symbol) symbol
#endif

#define GS_DEPRECATED_FUNC(func) GS_DEPRECATED_SYMBOL1(func)
#define GS_DEPRECATED_VAR(func) GS_DEPRECATED_SYMBOL1(func)

#ifndef GS_DEPRECATED_FUNC_ENABLED
#define GS_DEPRECATED_FUNC_ENABLED 1
#endif

#ifndef GS_EXPERIMENTAL_TOOL_ENABLED
#define GS_EXPERIMENTAL_TOOL_ENABLED 0
#endif

#if GS_DEPRECATED_FUNC_ENABLED

// 旧バージョンのファクトリAPIの有効化設定
#ifndef GS_COMPATIBILITY_FACTORY_BETA_0_3
#define GS_COMPATIBILITY_FACTORY_BETA_0_3 0
#endif

// 旧バージョンの時系列構成オプション使用プログラムのコンパイル用。
// オプションを指定しての実行はできない
#ifndef GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10
#define GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10 0
#endif

// 旧バージョンのサンプリングAPIの有効化設定
#ifndef GS_COMPATIBILITY_TIME_SERIES_SAMPLING_BETA_0_1
#define GS_COMPATIBILITY_TIME_SERIES_SAMPLING_BETA_0_1 0
#endif

// 旧バージョンの一括ロウ作成APIの有効化設定
#ifndef GS_COMPATIBILITY_GET_MULTIPLE_ROWS_BETA_0_3
#define GS_COMPATIBILITY_GET_MULTIPLE_ROWS_BETA_0_3 0
#endif

#ifndef GS_COMPATIBILITY_VALUE_1_1_106
#define GS_COMPATIBILITY_VALUE_1_1_106 0
#endif

#ifndef GS_COMPATIBILITY_MULTIPLE_CONTAINERS_1_1_106
#define GS_COMPATIBILITY_MULTIPLE_CONTAINERS_1_1_106 0
#endif

#ifndef GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE
#define GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE 1
#endif

#endif	// GS_DEPRECATED_FUNC_ENABLED

#if !defined(GS_COMPATIBILITY_SUPPORT_1_5) && \
	(GS_CLIENT_VERSION_MAJOR > 1 || \
	(GS_CLIENT_VERSION_MAJOR == 1 && GS_CLIENT_VERSION_MINOR >= 5))
#define GS_COMPATIBILITY_SUPPORT_1_5 1
#else
#define GS_COMPATIBILITY_SUPPORT_1_5 0
#endif

#if !defined(GS_COMPATIBILITY_SUPPORT_2_0) && \
	(GS_CLIENT_VERSION_MAJOR > 2 || \
	(GS_CLIENT_VERSION_MAJOR == 2 && GS_CLIENT_VERSION_MINOR >= 0))
#define GS_COMPATIBILITY_SUPPORT_2_0 1
#else
#define GS_COMPATIBILITY_SUPPORT_2_0 0
#endif

#if !defined(GS_COMPATIBILITY_SUPPORT_2_1) && \
	(GS_CLIENT_VERSION_MAJOR > 2 || \
	(GS_CLIENT_VERSION_MAJOR == 2 && GS_CLIENT_VERSION_MINOR >= 1))
#define GS_COMPATIBILITY_SUPPORT_2_1 1
#else
#define GS_COMPATIBILITY_SUPPORT_2_1 0
#endif

#if !defined(GS_COMPATIBILITY_SUPPORT_3_5) && \
	(GS_CLIENT_VERSION_MAJOR > 3 || \
	(GS_CLIENT_VERSION_MAJOR == 3 && GS_CLIENT_VERSION_MINOR >= 5))
#define GS_COMPATIBILITY_SUPPORT_3_5 1
#else
#define GS_COMPATIBILITY_SUPPORT_3_5 0
#endif

#if !defined(GS_COMPATIBILITY_SUPPORT_4_0) && \
	(GS_CLIENT_VERSION_MAJOR > 4 || \
	(GS_CLIENT_VERSION_MAJOR == 4 && GS_CLIENT_VERSION_MINOR >= 0))
#define GS_COMPATIBILITY_SUPPORT_4_0 1
#else
#define GS_COMPATIBILITY_SUPPORT_4_0 0
#endif

#if !defined(GS_COMPATIBILITY_SUPPORT_4_1) && \
	(GS_CLIENT_VERSION_MAJOR > 4 || \
	(GS_CLIENT_VERSION_MAJOR == 4 && GS_CLIENT_VERSION_MINOR >= 1))
#define GS_COMPATIBILITY_SUPPORT_4_1 1
#else
#define GS_COMPATIBILITY_SUPPORT_4_1 0
#endif

#if !defined(GS_COMPATIBILITY_SUPPORT_4_2) && \
	(GS_CLIENT_VERSION_MAJOR > 4 || \
	(GS_CLIENT_VERSION_MAJOR == 4 && GS_CLIENT_VERSION_MINOR >= 2))
#define GS_COMPATIBILITY_SUPPORT_4_2 1
#else
#define GS_COMPATIBILITY_SUPPORT_4_2 0
#endif

#endif	// GS_INTERNAL_DEFINITION_VISIBLE

/*!
	@JP
	@brief GridDB APIで使用される標準の文字の型です。
	@par
		この型の文字列エンコーディングは常にUTF-8です。

	@EN
	@brief Standard character type used in GridDB API.
	@par
		The character encoding is always UTF-8.

	@ENDL
*/
typedef char GSChar;

/*!
	@JP
	@brief GridDB APIで使用されるブール型です。
	@par
		GridDB上のBOOL型と対応します。

	@EN
	@brief Boolean type used in GridDB API.
	@par
		This type corresponds to the BOOL type in GridDB.

	@ENDL
*/
typedef char GSBool;

/*!
	@JP
	@brief 真であることを示すブール型値です。

	@EN
	@brief Indicates the true on boolean data type.

	@ENDL
*/
#define GS_TRUE 1

/*!
	@JP
	@brief 偽であることを示すブール型値です。

	@EN
	@brief Indicates the false on boolean data type.

	@ENDL
*/
#define GS_FALSE 0

/*!
	@JP
	@brief 列挙型

	@EN
	@brief Enumeration Type

	@ENDL
*/
typedef int32_t GSEnum;

/*!
	@JP
	@defgroup Group_GSTimestamp GSTimestamp

	@EN
	@defgroup Group_GSTimestamp GSTimestamp

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSTimestamp
	@brief GridDB上のTIMESTAMP型と対応する時刻型です。
		ミリ秒単位のUNIX時刻を保持します。

	@EN
	@ingroup Group_GSTimestamp
	@brief Time data type corresponding to TIMESTAMP type on GridDB.
		The value keeps Unix time in milliseconds.

	@ENDL
*/
typedef int64_t GSTimestamp;

/*!
	@JP
	@defgroup Group_GSGridStoreFactory GSGridStoreFactory

	@EN
	@defgroup Group_GSGridStoreFactory GSGridStoreFactory

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSGridStoreFactory
	@brief @ref GSGridStore インスタンスを管理します。
	@par
		@ref GSGridStore インスタンス共通のクライアント設定や
		使用済みのコネクションを管理します。
	@par
		GridDBにアクセスするためには、このファクトリを介して
		@ref GSGridStore インスタンスを取得する必要があります。
	@par
		この型のポインタを第一引数とする関数は、すべてスレッド安全です。

	@EN
	@ingroup Group_GSGridStoreFactory
	@brief Manage a @ref GSGridStore instance.
	@par
		It manages the client settings shared by @ref GSGridStore instances and
		used connections.
	@par
		To access GridDB, you need to get a @ref GSGridStore instance using
		this Factory.
	@par
		All the functions which have a pointer of this type in the first
		argument are thread safe.

	@ENDL
*/
typedef struct GSGridStoreFactoryTag GSGridStoreFactory;

/*!
	@JP
	@defgroup Group_GSGridStore GSGridStore

	@EN
	@defgroup Group_GSGridStore GSGridStore

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 1つのGridDBシステムが管理するデータ全体を操作するための機能を提供します。
	@par
		コレクションや時系列といったコンテナの追加・削除・構成変更、ならびに、
		コンテナを構成するロウの操作機能を提供します。
	@par
		コンテナ種別などの違いによらず、1つのデータベースのコンテナ間で、
		ASCIIの大文字・小文字表記だけが異なる名前のものを複数定義することは
		できません。コンテナ名は、ベースコンテナ名単独、もしくは、ベースコンテナ名の
		後ろにノードアフィニティ名をアットマーク「@」で連結した形式で表記します。
		その他、コンテナの定義において使用できるコンテナ名の文字種や長さには制限が
		あります。具体的には、GridDBテクニカルリファレンスを参照してください。
		特に記載のない限り、コンテナ名を指定する操作では、ASCIIの大文字・小文字
		表記の違いは区別されません。
	@par
		このインタフェースまたはこのインタフェースを通じて得られたインスタンスの
		インタフェースに対する操作を通じてエラーが発生した場合、エラーに関する
		次のパラメータを取得できることがあります。
		<table>
		<tr><th>パラメータ名</th><th>説明</th></tr>
		<tr><td>address</td><td>接続先クラスタノードのアドレス・ポート。ホスト名
		またはIPアドレスとポート番号とをコロン「:」で連結した文字列により
		構成されます。このインタフェースまたはこのインタフェースを通じて得られた
		インスタンスのインタフェースにおいて、クラスタへのアクセスを伴う操作を
		呼び出した際にエラーを検知すると、このパラメータを含むことがあります。
		このパラメータを含む場合、パラメータが示すクラスタノードにおいてエラーの
		詳細が記録されていることがあります。</td></tr>
		<tr><td>container</td><td>エラーに関係しうるコンテナの名前。任意個数の
		コンテナを一括して扱う操作において、そのうち少なくとも一つのコンテナに
		ついての操作を行えないことが判明した場合に、このパラメータを含むことが
		あります。任意個数のコンテナを扱う具体的な操作については、個々の
		インタフェースの定義を参照してください。クラスタノードへのリクエスト
		準備段階でのリソース不足など、どのコンテナの問題か特定し切れないことが
		あるため、どのようなエラーでもこのパラメータを含むとは限りません。
		また、複数のコンテナについて操作できない可能性があったとしても、
		パラメータに含まれるのは高々一つのコンテナの名前のみです。</td></tr>
		</table>
	@par
		この型のポインタを第一引数とする関数のスレッド安全性は保証されません。
	@see GSCollection
	@see GSTimeSeries
	@see GSContainer
	@see gsGetErrorParameterCount

	@EN
	@ingroup Group_GSGridStore
	@brief Provides functions to manipulate the entire data managed in one
		GridDB system.
	@par
		A function to add, delete, or change the composition of Collection and
		TimeSeries Containers as well as to process the Rows constituting a
		Container is provided.
	@par
		Regardless of container types, etc., multiple
		container names different only in uppercase and
		lowercase ASCII characters cannot be defined in a
		database. A container name is represented by only a
		base container name or by connecting the base name and
		a node affinity name with '@'. See the GridDB
		Technical Reference for the details. In the
		operations specifying a container name, uppercase and
		lowercase ASCII characters are identified as the same
		unless otherwise noted.
	@par
		If an error occurs by this interface or the interface of the instance
		which is acquired through this interface, the error information may
		contain the following parameters related to the error.
		<table>
		<tr><th>Parameter name</th><th>Description</th></tr>
		<tr><td>address</td><td>Address and port of connecting cluster
		node. It is a string connecting the host name or the IP address
		and the port number with a colon ":". In this interface or the
		interface of the instance which is acquired through this interface,
		when an error is detected in invoking an operation including a
		cluster access, this parameter may be contained. In that case, the
		details of the error may be logged in the cluster node shown by this
		parameter.</td></tr>
		<tr><td>container</td><td>The name of container which may relate
		the error. When operating an arbitrary number of containers and
		detected that the operation cannot be performed for one of the
		containers, this parameter may be contained. For instance of such
		operations, see the definition of each interface. For such as
		resource shortage in preparing requests to cluster nodes, it may
		not be possible to determine which container is the cause, so this
		parameter may not be contained in some error cases. And even if it is
		not possible to operate multiple containers, this parameter contains
		only one container name at most.</td></tr>
		</table>
	@par
		Functions which have a pointer of this type in the first argument are
		NOT thread safe.
	@see GSCollection
	@see GSTimeSeries
	@see GSContainer
	@see gsGetErrorParameterCount

	@ENDL
*/
typedef struct GSGridStoreTag GSGridStore;

/*!
	@JP
	@defgroup Group_GSContainer GSContainer

	@EN
	@defgroup Group_GSContainer GSContainer

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSContainer
	@brief 同一タイプのロウ集合からなるGridDBの構成要素に対しての、
		管理機能を提供します。
	@par
		ロウオブジェクトを入出力の基本単位として、各種管理機能を提供します。
		ロウオブジェクトとGridDB上のロウは、指定のロウオブジェクト型と
		GridDB上のスキーマとの対応関係に基づいて、相互にマッピングされます。
	@par
		GridDB上のスキーマを構成する各カラムは、対応する
		@ref GS_STRUCT_BINDING の内容に基づき決定されます。1つのコンテナは
		1つ以上のカラムにより構成されます。
	@par
		1つのコンテナのカラム間で、ASCIIの大文字・小文字表記だけが異なる名前
		のものを複数定義することはできません。その他、コンテナ定義における
		カラム名の文字種や長さ、カラム数には制限があります。具体的には、GridDB
		テクニカルリファレンスを参照してください。特に記載のない限り、カラム名を
		指定する操作では、ASCIIの大文字・小文字表記の違いは区別されません。
	@par
		カラムの型と、ロウオブジェクト内の各値の型との対応は、それぞれ次の通りです。
		カラム型 | ロウオブジェクト内の各値の型
		- | -
		STRING | @ref GSChar*
		BOOL | @ref GSBool
		BYTE | int8_t
		SHORT | int16_t
		INTEGER | int32_t
		LONG | int64_t
		FLOAT | float
		DOUBLE | double
		TIMESTAMP | @ref GSTimestamp
		GEOMETRY | @ref GSChar*
		BLOB | @ref GSBlob
		STRING配列 | @ref GSChar**
		BOOL配列 | @ref GSBool*
		BYTE配列 | int8_t*
		SHORT配列 | int16_t*
		INTEGER配列 | int32_t*
		LONG配列 | int64_t*
		FLOAT配列 | float*
		DOUBLE配列 | double*
		TIMESTAMP配列 | @ref GSTimestamp*
	@par
		フィールドの値の表現範囲やサイズには制限があります。
		具体的には、付録の章の値の範囲の説明、ならびに、GridDBテクニカル
		リファレンスを参照してください。制限に反する値をコンテナに格納することは
		できません。
	@par
		ロウキーとして許可されている型や、ロウキーに対応するカラムの有無、
		ロウ更新の可否といった制約は、このコンテナ型から派生した個別の型の定義
		によって異なります。
	@par
		GridDB上のロウにおけるNULLは、NOT NULL制約が設定されていない
		限り保持することができます。NULLは、@ref GSRow を通じて格納や
		取り出しを行うことができます。一方、@ref GS_STRUCT_BINDING と
		対応付くロウオブジェクトにおいては、常に後述の空の値にマッピング
		されます。
	@par
		ロウオブジェクト型におけるNOT NULL制約は、@ref GS_TYPE_OPTION_NULLABLE
		ならびに@ref GS_TYPE_OPTION_NOT_NULL により明示的に指定できます。
		NOT NULL制約がいずれの指定対象にも指定されていない場合、ロウキー以外の
		カラムはNOT NULL制約なしであるとみなされます。ロウキーは暗黙的にNOT NULL
		制約が設定された状態となっており、この制約を外すような指定はできません。
		また、同一指定対象での矛盾したNOT NULL制約は指定できません。
		NOT NULL制約は、@ref GSColumnInfoTag::options を通じて指定することが
		できます。
	@par
		空の値は、@ref GSRow の作成など各種操作の初期値などとして用いられる
		ことのある、フィールド値の一種です。以下のように、カラム型ごとに値が
		定義されています。
		カラム型 | 空の値
		- | -
		STRING | @c "" (長さ0の文字列)
		BOOL | 偽(@ref GS_FALSE)
		数値型 | @c 0
		TIMESTAMP | <tt>1970-01-01T00:00:00Z</tt>
		GEOMETRY | @c POINT(EMPTY)
		BLOB | 長さ0のBLOBデータ
		配列型 | 要素数0の配列
	@par
		トランザクション処理では、デフォルトで自動コミットモードが有効になっています。
		自動コミットモードでは、変更操作は逐次確定し、明示的に取り消すことができません。
		手動コミットモードにおいて、@ref GSContainer インスタンスを介した操作により
		クラスタノード上でエラーが検出された場合、コミット前の更新操作はすべて取り消されます。
		トランザクション分離レベルはREAD COMMITTEDのみをサポートします。
		ロック粒度は、コンテナの種別によって異なります。
	@par
		ロウの更新・追加・削除、ならびに更新用ロック獲得を行った場合、
		内部でトランザクションが生成されます。このトランザクションには、
		有効期限が存在します。これらの操作をある@ref GSContainer インスタンスに
		対してはじめて行った時刻を起点として、GridDB上で定められている期間だけ
		経過した後に、さらに同様の操作やトランザクション操作を行おうとすると、
		該当する@ref GSContainer インスタンスを介した以降の操作は
		常に失敗するようになります。
	@par
		あるコンテナへの操作要求に対するクラスタノード上での処理が開始され、
		終了するまでの間、同一のコンテナに対する操作が待機させられる場合が
		あります。ここでの操作には、コンテナのスキーマや索引などの定義変更、
		コンテナ情報の参照、ロウ操作などが含まれます。一貫性レベルが
		@c IMMEDIATE の@ref GSGridStore インスタンスを通じてコンテナを操作する
		場合、同一のコンテナに対する@c IMMEDIATE 設定での他の操作処理の途中、
		原則としては待機させられます。また、コンテナに対する他の操作処理の
		途中の状態に基づいて処理が行われることは、原則としてはありません。
		例外事項については、個別の操作ごとの説明を参照してください。

	@EN
	@ingroup Group_GSContainer
	@brief Provides management functions for sets of row having same type.
	@par
		Provides several management functions for row objects which is a
		fundamental unit for input/output. A row object and a row on GridDB are
		mapped to each other based on the relationship between a specified row
		object type and a GridDB schema.
	@par
		Each column in GridDB schema is defined by a content in the
		corresponding @ref GS_STRUCT_BINDING. Each container consists of one or
		more columns.
	@par
		Multiple column names that are different only in
		upper- and lowercase letters cannot be defined in a
		table. Further the allowed characters, the length of
		column names and the number of columns are limited.
		See the GridDB Technical Reference for the details.
		In the operations specifying column names, ASCII
		uppercase and lowercase characters are identified as
		same unless otherwise noted.
	@par
		Mapping table between column type and value in a row object is
		following:
		Column type | value in a row object
		- | -
		STRING | @ref GSChar*
		BOOL | @ref GSBool
		BYTE | int8_t
		SHORT | int16_t
		INTEGER | int32_t
		LONG | int64_t
		FLOAT | float
		DOUBLE | double
		TIMESTAMP | @ref GSTimestamp
		GEOMETRY | @ref GSChar*
		BLOB | @ref GSBlob
		STRING Array | @ref GSChar**
		BOOL Array | @ref GSBool*
		BYTE Array | int8_t*
		SHORT Array | int16_t*
		INTEGER Array | int32_t*
		LONG Array | int64_t*
		FLOAT Array | float*
		DOUBLE Array | double*
		TIMESTAMP Array | @ref GSTimestamp*
	@par
		The column value has limitations for rage and size. Please refer
		to appendix and GridDB Technical Reference and for more detail.
		Cannot store a value exceeding these limitations.
	@par
		A limitation about a row key type, presence of column corresponding to
		a row key, and availability of row value updates, may differ for each
		type derived from the container type.
	@par
		In GridDB, as long as NOT NULL constraint is not set, NULL can be saved.
		NULL can be stored and retrieved by @ref GSRow. On the other hand, row
		object associated with @ref GS_STRUCT_BINDING is always mapped to an empty
		value.
	@par
		In row object, NOT NULL constraint can be explicitly specified by
		@ref GS_TYPE_OPTION_NULLABLE and @ref GS_TYPE_OPTION_NOT_NULL.
		If NOT NULL constraint is not specified for any target, the column other
		than the row key will be considered to be without NOT NULL constraint.
		It is not possible to exclude NOT NULL constraint in the row key as it
		has been implicitly set.
		If there is an inconsistent NOT NULL constraint on the same target,
		NOT NULL constraint that cannot be specified can be instead set using
		@ref GSColumnInfoTag::options.
	@par
		Empty value is a type of field value that are sometimes used as initial
		values of various operations such as @ref GSRow creation, etc.
		Column type | Empty value
		STRING | @c "" (0-length string)
		BOOL | False(@ref GS_FALSE)
		Numeric type | @c 0
		TIMESTAMP | <tt>1970-01-01T00:00:00Z</tt>
		GEOMETRY | @c POINT(EMPTY)
		BLOB | 0-length BLOB data
		Array type | An array with no element
	@par
		About transaction, auto commit mode is active as a default. In the auto
		commit mode, each transaction is processed sequentially, and cannot be
		canceled. For manual commit mode, transactions before a commit is
		canceled if there is an error on a cluster node during the transaction
		via @ref GSContainer instances. Transaction isolation level supports
		only READ COMMITTED. Lock granularity may differ for each container
		type.
	@par
		When a row is updated, added, deleted, and got a lock for updates, a
		transaction is generated internally. This transaction has a valid
		period. After some period defined by GridDB is passed from the timing of
		this transaction for @ref GSContainer instance, any same type of
		transactions will be not accepted.
	@par
		When there is an operation request to a certain container, operation in
		the same container will be put on hold until the processing on the cluster
		node is started. Operations in this context include changes in the definition
		of container schema and index, container information reference, row operation,
		etc. When running the containers through @ref GSGridStore instances with
		consistency level @c IMMEDIATE, the operation will be queued behind other
		processing in the same container with the @c IMMEDIATE configuration.
		Processing is not performed based on the state of other on-going operation.
		For irregular case, please see the description of each individual operation.

	@ENDL
 */
typedef struct GSContainerTag GSContainer;

/*!
	@JP
	@defgroup Group_GSQuery GSQuery

	@EN
	@defgroup Group_GSQuery GSQuery

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSQuery
	@brief 特定の@ref GSContainer に対応付けられたクエリを保持し、
		結果取得方法の設定ならびに実行・結果取得を行う機能を持ちます。

	@EN
	@ingroup Group_GSQuery
	@brief Provides the functions of holding the information about a query
		related to a specific @ref GSContainer, specifying the options for
		fetching and retrieving the result.

	@ENDL
 */
typedef struct GSQueryTag GSQuery;

/*!
	@JP
	@defgroup Group_GSRowSet GSRowSet

	@EN
	@defgroup Group_GSRowSet GSRowSet

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSRowSet
	@brief クエリ実行より求めたロウの集合を管理します。
	@par
		ロウ単位・ロウフィールド単位での操作機能を持ち、対象とするロウを指し示すための
		カーソル状態を保持します。初期状態のカーソルは、ロウ集合の先頭より手前に
		位置しています。

	@EN
	@ingroup Group_GSRowSet
	@brief Manages a set of Rows obtained by a query.
	@par
		It has a function of per-Row and per-Row-field manipulation and holds a
		cursor state to specify a target Row. The cursor is initially located
		just before the head of a Row set.

	@ENDL
 */
typedef struct GSRowSetTag GSRowSet;

/*!
	@JP
	@defgroup Group_GSAggregationResult GSAggregationResult

	@EN
	@defgroup Group_GSAggregationResult GSAggregationResult

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSAggregationResult
	@brief 集計演算の結果を保持します。
	@par
		@ref gsGetNextAggregation もしくは@ref gsAggregateTimeSeries
		により取得できる、集計演算の結果を保持します。
		整数型カラムに対する演算結果を浮動小数点型として、また、有効桁数の
		少ない数値型のカラムに対する演算結果をより桁数の多い数値型として
		受け取ることができます。
	@par
		保持する型は、集計演算の種別や集計対象のカラムの型によって決定されます。
		具体的な規則は@ref GSAggregation またはTQLの仕様を参照してください。
	@par
		取り出しできる型は、保持する型によって決まります。
		保持する型が数値型の場合はDOUBLE型またはLONG型、TIMESTAMP型の
		場合はTIMESTAMP型の値としてのみ取り出しできます。

	@EN
	@ingroup Group_GSAggregationResult
	@brief Stores the result of an aggregation operation.
	@par
		Stores the result returned by @ref gsGetNextAggregation or
		@ref gsAggregateTimeSeries. A floating-point-type result can be obtained
		from an operation on a numeric-type Column, and a higher-precision
		result can be obtained from an operation on a numeric-type Column with a
		small number of significant digits.
	@par
		The type of the stored result depends on the type of aggregation
		operation and the type of the target Columns. For specific rules, see
		@ref GSAggregation or the TQL specifications.
	@par
		The type of obtaining value depends on the stored type. DOUBLE type and
		LONG type are only available when a result is of numeric type, and
		TIMESTAMP type when a result is of TIMESTAMP type.

	@ENDL
 */
typedef struct GSAggregationResultTag GSAggregationResult;

/*!
	@JP
	@defgroup Group_GSCollection GSCollection

	@EN
	@defgroup Group_GSCollection GSCollection

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSCollection
	@brief ロウ集合を汎用的に管理するためのコンテナです。
	@par
		ロウキーには次の型が使用できます。
		- STRING型(@ref GSChar*)
		- INTEGER型(int32_t)
		- LONG型(int64_t)
		- TIMESTAMP型(@ref GSTimestamp)
	@par
		ロウキーの設定は必須ではありません。
	@par
		ロウ操作について、コンテナ固有の制限は設けられていません。
	@par
		@ref gsQuery もしくは@ref gsGetMultipleContainerRows などより
		複数のロウの内容を一度に取得する場合、特に指定がなければ、
		返却されるロウの順序は不定となります。
	@par
		ロック粒度はロウ単位です。

	@EN
	@ingroup Group_GSCollection
	@brief A general-purpose Container for managing a set of Rows.
	@par
		The following types are available as a Row key.
		- STRING type (@ref GSChar*)
		- INTEGER type (int32_t)
		- LONG type (int64_t)
		- TIMESTAMP type (@ref GSTimestamp)
	@par
		It is not mandatory to set a Row key.
	@par
		There is no Container-specific constraint on Row operations.
	@par
		A set of Rows returned by @ref gsQuery or
		@ref gsGetMultipleContainerRows etc. in no particular order, when order
		is not specified.
	@par
		The granularity of locking is a Row.

	@ENDL
 */
typedef GSContainer GSCollection;

/*!
	@JP
	@defgroup Group_GSTimeSeries GSTimeSeries

	@EN
	@defgroup Group_GSTimeSeries GSTimeSeries

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 時刻をロウキーとする、時系列処理に特化したコンテナです。
	@par
		一般的に、範囲取得や集計演算といった処理は、@ref GSCollection
		よりも効率的な実装が選択されます。
	@par
		ロウ操作については、@ref GSCollection と異なり一部制限が
		設けられています。
		@ref GSTimeSeriesProperties に基づき圧縮オプションが
		設定されている場合、次のロウ操作を行えません。
		- 指定ロウの更新
		- 指定ロウの削除
		- 指定時刻より新しい時刻のロウが存在する場合の、ロウの新規作成
	@par
		@ref gsQuery もしくは@ref gsGetMultipleContainerRows などより
		複数のロウの内容を一度に取得する場合、特に指定がなければ、
		返却されるロウの順序はロウキーの時刻を基準として@ref GS_ORDER_ASCENDING
		相当の順序に整列されます。
	@par
		ロック粒度は、1つ以上のロウ集合をひとまとまりとする内部格納
		単位となります。したがって、特定ロウについてロックする際、そのロウが属する
		内部格納単位上の他のロウも同時にロックしようとします。

	@EN
	@ingroup Group_GSTimeSeries
	@brief A specialized Container with a time-type Row key for TimeSeries data
		operation.
	@par
		Generally, in extraction of a specific range and aggregation operations
		on TimeSeries, more efficient implementation is selected than on
		@ref GSCollection.
	@par
		There are some restrictions on Row operation unlike @ref GSCollection.
		If the compression option based on the @ref GSTimeSeriesProperties has
		been set, following Row operations are not permitted.
		- Update operation to specified Row
		- Delete operation to specified Row
		- Create operation if newer Row than the specified time is existed
	@par
		If the order of Rows requested by @ref gsQuery or
		@ref gsGetMultipleContainerRows is not specified, the Rows in a result
		set are sorted in @ref GS_ORDER_ASCENDING order based on the Row key
		time.
	@par
		The granularity of locking is an internal storage unit, i.e., a set of
		one or more Rows. Accordingly, when locking a specific Row, GridDB will
		attempt to lock other Rows in the same internal storage unit as the Row.

	@ENDL
 */
typedef GSContainer GSTimeSeries;

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@defgroup Group_GSRow GSRow

	@EN
	@defgroup Group_GSRow GSRow

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSRow
	@brief 任意のスキーマについて汎用的にフィールド操作できるロウです。
	@par
		NULLが設定されたフィールドに対して型指定のフィールド値取得機能を
		用いた場合、NULLの代わりに@ref GSContainer にて定義されている空の値が
		求まります。たとえば文字列型カラムに対応するフィールドにNULLが設定
		されており、かつ、@ref gsGetRowFieldAsString を用いた場合、
		@c NULL アドレスではなく、空の値である長さ@c 0 の文字列を指すアドレスが
		求まります。
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief A general-purpose Row for managing fields in any schema.
	@par
		For fields where NULL is set, when the field value acquisition function
		is used, an empty value defined in @ref GSContainer is obtained instead
		of NULL. For example, if a string type column field is set to NULL,
		@ref gsGetRowFieldAsString is used, the address is pointed to an empty
		value of string length @c 0 instead of @c NULL.
	@since 1.5

	@ENDL
 */
typedef struct GSRowTag GSRow;

/*!
	@JP
	@defgroup Group_GSRowKeyPredicate GSRowKeyPredicate

	@EN
	@defgroup Group_GSRowKeyPredicate GSRowKeyPredicate

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief ロウキーの合致条件を表します。
	@par
		@ref gsGetMultipleContainerRows における取得条件を
		構成するために使用できます。
	@par
		条件の種別として、範囲条件と個別条件の2つの種別があります。
		両方の種別の条件を共に指定することはできません。
		条件の内容を何も指定しない場合、対象とするすべてのロウキーに
		合致することを表します。
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Represents the condition that a row key satisfies.
	@par
		This is used as the search condition in @ref gsGetMultipleContainerRows
	@par
		There are two types of conditions, range condition and individual
		condition. The two types of conditions cannot be specified at the same
		time. If the condition is not specified, it means that the condition is
		satisfied in all the target row keys.
	@since 1.5

	@ENDL
 */
typedef struct GSRowKeyPredicateTag GSRowKeyPredicate;

/*!
	@JP
	@defgroup Group_GSPartitionController GSPartitionController

	@EN
	@defgroup Group_GSPartitionController GSPartitionController

	@ENDL
 */
/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief パーティション状態の取得や操作のためのコントローラです。
	@par
		パーティションとは、データを格納する論理的な領域です。
		GridDBクラスタ内のデータ配置に基づいた操作を行う
		ために使用します。
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Controller for acquiring and processing the partition status.
	@par
		A partition is a theoretical region where data is stored. It is used to
		perform operations based on the data arrangement in a GridDB cluster.
	@since 1.5

	@ENDL
 */
typedef struct GSPartitionControllerTag GSPartitionController;

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@brief GridDBに対する命令の実行結果コードの型です。

	@EN
	@brief The type of result code for GridDB instructions.

	@ENDL
 */
typedef int32_t GSResult;

/*!
	@JP
	@brief GridDBに対する命令の実行に成功したことを示す、実行結果コードの値です。
	@see GSResult

	@EN
	@brief The value of result code which indicates the successful execution for
		GridDB instructions.
	@see GSResult

	@ENDL
 */
#define GS_RESULT_OK 0

/*!
	@JP
	@brief 実行結果コードに基づきGridDBに対する命令の実行に成功したかどうかの
		ブール値を求めるマクロです。
	@see GS_RESULT_OK
	@see GSResult

	@EN
	@brief The macro to find the Boolean value of success or failure in the
		execution of the instruction for the GridDB based on the result code.
	@see GS_RESULT_OK
	@see GSResult

	@ENDL
 */
#define GS_SUCCEEDED(result) ((result) == GS_RESULT_OK)

/*!
	@JP
	@brief ロウオブジェクトにおけるBLOB構造を表します。

	@EN
	@brief Represents the BLOB structure in a Row object.

	@ENDL
 */
typedef struct GSBlobTag {

	/*!
		@JP
		@brief BLOBデータのサイズです。

		@EN
		@brief The size of the BLOB data.

		@ENDL
	 */
	size_t size;

	/*!
		@JP
		@brief BLOBデータの格納先ポインタです。

		@EN
		@brief The pointer to the BLOB data.

		@ENDL
	 */
	const void *data;

} GSBlob;

/*!
	@JP
	@brief プロパティの構成エントリです。

	@EN
	@brief The entry of the property configuration.

	@ENDL
 */
typedef struct GSPropertyEntryTag {

	/*!
		@JP
		@brief プロパティエントリの名前です。

		@EN
		@brief The name of the property entry.

		@ENDL
	 */
	const GSChar *name;

	/*!
		@JP
		@brief プロパティエントリの値です。

		@EN
		@brief The value of the property entry.

		@ENDL
	 */
	const GSChar *value;

} GSPropertyEntry;

/*!
	@JP
	@brief クエリ実行結果を取得する際のオプション項目です。

	@EN
	@brief The options for fetching the result of a query.

	@ENDL
 */
enum GSFetchOptionTag {

	/*!
		@JP
		@brief 取得するロウの数の最大値を設定するために使用します。
		@par
			実行結果のロウ数が最大値を超えた場合、@ref GSRowSet で得られる順番で
			先頭から最大値の分だけが取得できます。それ以降のロウは取得できません。
		@par
			サポートされる設定値の型は、INTEGERまたはLONGです。
			負の値は指定できません。設定が省略された場合、上限は設定されません。

		@EN
		@brief Used to set the maximum number of Rows to be fetched.
		@par
			If the number of Rows in a result exceeds the maximum, the maximum
			number of Rows counting from the 0-th in @ref GSRowSet are fetched.
			The rest of the Rows in the result cannot be fetched.
		@par
			The supported types of values are INTEGER and LONG. Negative values
			are not available. If the setting is omitted, the limit is not
			defined.

		@ENDL
	 */
	GS_FETCH_LIMIT,

#if GS_COMPATIBILITY_SUPPORT_1_5

#if GS_INTERNAL_DEFINITION_VISIBLE
#if !GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE
	/*!
		@JP
		@brief 非公開のオプションです。
		@deprecated
		@attention
			十分検証されていないため、意図しない挙動を示す可能性があります。
		@since 1.5

		@EN
		@brief This option is planned to be supported in the future version.
		@deprecated
		@attention
			Since it has not been fully verified, it could indicate an
			unintended behavior.
		@since 1.5

		@ENDL
	 */
	GS_FETCH_SIZE = (GS_FETCH_LIMIT + 1),
#endif
#endif

#if GS_COMPATIBILITY_SUPPORT_4_0
	/*!
		@JP
		@brief 部分実行モードを設定するために使用します。
		@par
			部分実行モードでは、クエリの中間処理や結果送受信に用いる
			バッファのサイズなどがなるべく一定の範囲に収まるよう、必要に応じて
			実行対象のデータ範囲を分割し、この部分範囲ごとに実行とフェッチを
			リクエストすることがあります。そのため、@ref GSRowSet を取得した時点で
			一部の範囲の結果が求まっていないことや、結果ロウを順に参照していく
			段階で、残りの範囲を部分的に実行していくことがあります。
		@par
			部分実行モードは、現バージョンでは次の条件すべてを満たすクエリに
			使用できます。また、@ref GS_FETCH_LIMIT オプションと併用することが
			できます。条件を満たさない場合でも、各種フェッチオプションの設定時点
			ではエラーを検知しない場合があります。
			- TQL文からなるクエリであること
			- TQL文において、選択式が「*」のみからなり、ORDER BY節を含まないこと
			- 対応する@ref GSContainer が個々の部分的なクエリ実行時点において
				常に自動コミットモードに設定されていること
		@par
			部分実行モードでは、対応する@ref GSContainer のトランザクション
			分離レベルや状態に基づき、個々の部分的なクエリ実行時点において
			参照可能なロウが使用されます。ただし、クエリ全体の実行開始時点で
			存在しないロウは、実行対象から外れる場合があります。
		@par
			部分実行モードを有効にした場合に@ref GSRowSet に対して使用
			できない操作や特有の挙動については、個別の定義を参照してください。
		@par
			サポートされる設定値の型は、BOOLのみです。
			部分実行モードを有効にするには、@ref GS_TRUE と一致する値を
			指定します。現バージョンでは、未設定の場合には部分実行モードを有効に
			しません。
		@since 4.0
		@EN
		@brief Used to set the partial execution mode.
		@par
			In the partial execution mode, it is trying
			for the buffer size of the intermediate query
			processing and data transfer, etc. to fit
			inside a fixed size by dividing the target
			data and fetching the query results in each
			divided range. Therefore the results for some
			data ranges may not be determined when the
			@ref GSRowSet is obtained, and in the middle
			of getting the results, there are the cases
			that the query is executed partially for the
			rest of the ranges.
		@par
			In this version, the partial execution mode
			can be used for queries satisfying all the
			following conditions. And it can be used in
			combination with @ref GS_FETCH_LIMIT option. If
			not satisfying the conditions, the error may
			not be detected at the setting of the fetch
			option.
			- The query must be specified by TQL
			- The SELECT clause must be consisted of
			only '*' and an ORDER BY clause must not be
			specified.
			- The container must have been set to the auto
			commit mode at the each partial execution of the
			query.
		@par
			In the partial execution mode, rows that can
			be fetched at the each partial execution of
			the query based on the separation level and
			the status of the corresponding transaction
			are used. However rows that don't exist at
			the first time of the whole query execution
			may not be reflected to the results.
		@par
			For inhibited operations and behaviors
			on @ref GSRowSet in this mode, see the
			individual definitions.
		@par
			The only supported type for this setting is
			BOOL. The value matching to @ref GS_TRUE
			must be specified to activate
			this mode. In this version, the partial
			execution mode is not effective unless setting
			the mode explicitly.
		@ENDL
	 */
	GS_FETCH_PARTIAL_EXECUTION = (GS_FETCH_LIMIT + 2)
#endif	// GS_COMPATIBILITY_SUPPORT_4_0

#endif	// GS_COMPATIBILITY_SUPPORT_1_5
};

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE
#if GS_DEPRECATED_FUNC_ENABLED
static const enum GSFetchOptionTag GS_DEPRECATED_VAR(GS_FETCH_SIZE) =
		(enum GSFetchOptionTag) (GS_FETCH_LIMIT + 1);
#endif
#endif
#endif

/*!
	@JP
	@see GSFetchOptionTag

	@EN
	@see GSFetchOptionTag

	@ENDL
 */
typedef GSEnum GSFetchOption;

/*!
	@JP
	@brief クエリにおける要求ロウ順序を表します。
	@par
		各種クエリ機能別に定義される判定対象を基準として、順序指定を行うために使
		用します。具体的な判定対象は、個別の機能によって異なります。

	@EN
	@brief Represents the order of Rows requested by a query.
	@par
		It is used to specify the order of Rows targeted by each query function.
		Specific targets differ with individual functions.

	@ENDL
 */
enum GSQueryOrderTag {

	/*!
		@JP
		@brief 要求ロウ順序が昇順であることを表します。

		@EN
		@brief Indicates the requested order of Rows is an ascending order.

		@ENDL
	 */
	GS_ORDER_ASCENDING,

	/*!
		@JP
		@brief 要求ロウ順序が降順であることを表します。

		@EN
		@brief Indicates the requested order of Rows is a descending order.

		@ENDL
	 */
	GS_ORDER_DESCENDING

};

/*!
	@JP
	@see GSQueryOrderTag

	@EN
	@see GSQueryOrderTag

	@ENDL
 */
typedef GSEnum GSQueryOrder;

/*!
	@JP
	@brief @ref GSContainer に設定する索引の種別を示します。

	@EN
	@brief Represents the type(s) of indexes set on a @ref GSContainer.

	@ENDL
 */
enum GSIndexTypeFlagTag {

	/*!
		@JP
		@brief デフォルトの索引を示します。
		@par
			この索引種別は、特定の種別を明示せずに索引の操作を行う必要が
			ある場合に用いられるものであり、実在する索引はこの種別以外の種別に分類
			されます。

		@EN
		@brief Indicates a default index.
		@par
			For this index type, when you need to manipulate the index without
			specifying a type, the existing index will be classified as another
			type other than this type.

		@ENDL
	 */
	GS_INDEX_FLAG_DEFAULT = -1,

	/*!
		@JP
		@brief ツリー索引を示します。
		@par
			この索引種別は、時系列におけるロウキーと対応するカラムを除く
			任意の種別のコンテナにおける、次の型のカラムに対して使用できます。
			- STRING
			- BOOL
			- BYTE
			- SHORT
			- INTEGER
			- LONG
			- FLOAT
			- DOUBLE
			- TIMESTAMP

		@EN
		@brief Indicates a tree index.
		@par
			This index type can be applied to following types of Columns of any type
			of Container, except the Column corresponding to the Row key of
			TimeSeries.
			- STRING
			- BOOL
			- BYTE
			- SHORT
			- INTEGER
			- LONG
			- FLOAT
			- DOUBLE
			- TIMESTAMP

		@ENDL
	 */
	GS_INDEX_FLAG_TREE = 1 << 0,

	/*!
		@JP
		@brief ハッシュ索引を示します。
		@par
			この索引は、@ref GSCollection における次の型のカラムに対して設定できます。
			- STRING
			- BOOL
			- BYTE
			- SHORT
			- INTEGER
			- LONG
			- FLOAT
			- DOUBLE
			- TIMESTAMP
		@par
			@ref GSTimeSeries に対して設定することはできません。

		@EN
		@brief Indicates a hash index.
		@par
			This index type can be set on the following types of Columns in
			@ref GSCollection.
			- STRING
			- BOOL
			- BYTE
			- SHORT
			- INTEGER
			- LONG
			- FLOAT
			- DOUBLE
			- TIMESTAMP
		@par
			It cannot be set on Columns in @ref GSTimeSeries.

		@ENDL
	 */
	GS_INDEX_FLAG_HASH = 1 << 1,

	/*!
		@JP
		@brief 空間索引を示します。
		@par
			この索引種別は、@ref GSCollection におけるGEOMETRY型のカラムに
			対してのみ使用できます。@ref GSTimeSeries に対して設定することは
			できません。

		@EN
		@brief Indicates a spatial index.
		@par
			This index type can be applied to only GEOMETRY type of Columns in
			@ref GSCollection.
			It cannot be set on Columns in @ref GSTimeSeries.

		@ENDL
	 */
	GS_INDEX_FLAG_SPATIAL = 1 << 2

};

/*!
	@JP
	@see GSIndexTypeFlagTag

	@EN
	@see GSIndexTypeFlagTag

	@ENDL
 */
typedef int32_t GSIndexTypeFlags;

/*!
	@JP
	@brief ロウ集合またはその特定のカラムに対する、集計演算の方法を示します。
	@par
		現バージョンでは、@ref GSTimeSeries に対してのみ使用できます。
	@par
			重み付きの演算の場合、キーの値に基づき重み付け値を決定します。
			@ref GSTimeSeries に対する重み付きの演算の場合、前後それぞれの
			時刻のロウとの中間時刻間の期間を特定の単位で換算したものを、
			重み付け値として使用します。
			ただし、前後いずれかの時刻のロウのみが存在しない場合は、存在しないロウの
			代わりに重み付け対象のロウを用いて求めた重み付け値を使用します。
			前後いずれの時刻のロウも存在しない場合は、重み付け値として@c 1
			(単位は前後いずれかのロウが存在する場合と同一)を使用します。
	@par
			演算の内部処理にてオーバーフローが発生した場合、浮動小数点数型では
			負または正の無限大、整数型では未定義の値が求まります。また、浮動小数点数型にて
			演算対象に非数(@c NaN)が含まれていた場合、非数が求まります。

	@EN
	@brief Represents the methods of aggregation operation on a set of Rows or
		their specific Columns.
	@par
		Available only to @ref GSTimeSeries in the current version.
	@par
			In a weighted operation, a weighted value is determined based on a
			key value. In a weighted operation on a @ref GSTimeSeries, a
			weighted value is obtained by calculating half the time span between
			the adjacent Rows before and after a Row in terms of the specific
			unit. However, if a Row has only one adjacent Row, the time span
			from the adjacent Row is considered, and if no adjacent Rows exist,
			@c 1 (Unit is the same as when there is either a row before and
			after) is used as a weighted value.
	@par
			If an overflow occurs in an internal operation, -INF or INF is
			returned for floating point operation, and the value "undefined" is
			returned for integer operation. And if @c NaN is given as an operand
			for floating-point operation, @c NaN is returned.

	@ENDL
 */
enum GSAggregationTag {

	/*!
		@JP
		@brief 最小値を求める演算です。
		@par
			大小比較できる型、すなわち数値型や時刻型のカラムに対してのみ使用できます。
			演算結果の型は、対象のカラムと同一の型となります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain a minimum value.
		@par
			Available only to Columns of numeric and time types allowing
			magnitude comparison. The type of a returned value is the same as
			that of a specified Column. If no target Row exists, the aggregation
			result will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_MINIMUM,

	/*!
		@JP
		@brief 最大値を求める演算です。
		@par
			大小比較できる型、すなわち数値型や時刻型のカラムに対してのみ使用できます。
			演算結果の型は、対象のカラムと同一の型となります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain a maximum value.
		@par
			Available only to Columns of numeric and time types allowing
			magnitude comparison. The type of a returned value is the same as
			that of a specified Column. If no target Row exists, the aggregation
			result will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_MAXIMUM,

	/*!
		@JP
		@brief 合計を求める演算です。
		@par
			数値型のカラムに対してのみ使用できます。
			演算結果の型は、対象のカラムが整数型の場合LONG、
			浮動小数点型の場合DOUBLEとなります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain a sum total.
		@par
			Available only to numeric-type Columns. The type of a returned value
			is LONG if a specified Column is of integer type, and DOUBLE if it
			is of floating-point type. If no target Row exists, the aggregation
			result will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_TOTAL,

	/*!
		@JP
		@brief 平均を求める演算です。
		@par
			数値型のカラムに対してのみ使用できます。
			演算結果の型は常にDOUBLEとなります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain an average.
		@par
			Available only to numeric-type Columns. The type of a returned value
			is always DOUBLE. If no target Row exists, the aggregation result
			will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_AVERAGE,

	/*!
		@JP
		@brief 分散を求める演算です。
		@par
			数値型のカラムに対してのみ使用できます。
			演算結果の型は常にDOUBLEとなります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain a variance.
		@par
			Available only to numeric-type Columns. The type of a returned value
			is always DOUBLE. If no target Row exists, the aggregation result
			will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_VARIANCE,

	/*!
		@JP
		@brief 標準偏差を求める演算です。
		@par
			数値型のカラムに対してのみ使用できます。
			演算結果の型は常にDOUBLEとなります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain a standard deviation.
		@par
			Available only to numeric-type Columns. The type of a returned value
			is always DOUBLE. If no target Row exists, the aggregation result
			will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_STANDARD_DEVIATION,

	/*!
		@JP
		@brief 標本数、すなわちロウ数を求める演算です。
		@par
			任意のカラムに対して使用できます。
			演算結果の型は常にLONGとなります。
			対象となるロウが1つも存在しない場合、演算結果の値は0となります。

		@EN
		@brief An operation to obtain the number of samples, i.e., the number of
			Rows.
		@par
			Available to any kinds of Columns. The type of a returned value is
			always LONG. If no target Row exists, the value of the operation
			result is 0.

		@ENDL
	 */
	GS_AGGREGATION_COUNT,

	/*!
		@JP
		@brief 重み付きで平均を求める演算です。
		@par
			各標本値と重み付け値との積の合計を、各重み付け値の合計で割ることにより
			求めます。重み付け値の計算方法は、@ref GSAggregationTag の説明を
			参照してください。
		@par
			この演算は、数値型のカラムに対してのみ使用できます。
			演算結果の型は常にDOUBLEとなります。
			対象となるロウが1つも存在しない場合、演算結果は設定されません。

		@EN
		@brief An operation to obtain a weighted average.
		@par
			The weighted average is calculated by dividing the sum of products
			of sample values and their respective weighted values by the sum of
			weighted values. For the method of calculating a weighted value, see
			the description of @ref GSAggregationTag.
		@par
			This operation is only available to numeric-type Columns. The type
			of a returned value is always DOUBLE. If no target Row exists, the
			aggregation result will not be stored in a result object.

		@ENDL
	 */
	GS_AGGREGATION_WEIGHTED_AVERAGE,

};

/*!
	@JP
	@see GSAggregationTag

	@EN
	@see GSAggregationTag

	@ENDL
 */
typedef GSEnum GSAggregation;

/*!
	@JP
	@brief ロウの補間方法の種別を表します。
	@par
		時系列ロウの補間機能で使用されます。

	@EN
	@brief Represents the type of interpolation of Rows.
	@par
		It is used by the function of interpolating TimeSeries Rows.

	@ENDL
 */
enum GSInterpolationModeTag {

	/*!
		@JP
		@brief カラムに応じて線形補間または直前ロウの値による補間を行うことを示します。
		@par
			補間機能にて指定されたカラムについては、補間対象時刻の
			前後時刻のロウの値により線形補間を行います。対象とするカラムの型は
			数値型でなければなりません。
		@par
			補間機能にて特に指定されていないカラムについては、補間対象時刻と
			隣接する直前の時刻のロウの値を補間値として使用します。対象とするカラムの
			型に制限はありません。

		@EN
		@brief Indicates performing linear interpolation or interpolation with
			the value of a previous Row on Columns.
		@par
			The Column specified the interpolation function is linearly
			interpolated with the Row values before and after the target time of
			interpolation. The target Column must be of numeric type.
		@par
			The Columns not specified as interpolation targets are interpolated
			with adjacent Row values just before the target time of
			interpolation. Those Columns can be of any type.

		@ENDL
	 */
	GS_INTERPOLATION_LINEAR_OR_PREVIOUS,

	/*!
		@JP
		@brief 空の値を補間値として用いることを示します。
		@par
			ロウキーを除くすべてのロウフィールドについて、@ref GSContainer にて
			定義されている空の値を補間値として用いることを示します。

		@EN
		@brief Indicates using an empty value as an interpolated value.
		@par
			It indicates that an empty value defined in @ref GSContainer is used
			as an interpolated value for all Row fields except Row keys.

		@ENDL
	 */
	GS_INTERPOLATION_EMPTY

};

/*!
	@JP
	@see GSInterpolationModeTag

	@EN
	@see GSInterpolationModeTag

	@ENDL
 */
typedef GSEnum GSInterpolationMode;

/*!
	@JP
	@brief @ref GSTimeSeries のキー時刻に基づく、ロウの特定方法を表します。
	@par
		別途指定する時刻と組み合わせることで、最も近い時刻のキーを持つロウなどを
		特定できます。該当するロウが存在しない場合の扱いは、この列挙型を使用する
		それぞれの機能により異なります。

	@EN
	@brief Represents how to specify a Row based on a key timestamp of
		@ref GSTimeSeries.
	@par
		It can be used in combination with the timestamp (specified separately)
		to specify a Row with the nearest timestamp etc. When no relevant Row
		exists, it behaves differently depending on the function using this
		enumeration type.

	@ENDL
 */
enum GSTimeOperatorTag {

	/*!
		@JP
		@brief 指定時刻と同一またはより前の時刻のロウのうち、最も新しいものを求めます。

		@EN
		@brief Returns the newest among the Rows whose timestamp are identical
			with or earlier than the specified time.

		@ENDL
	 */
	GS_TIME_OPERATOR_PREVIOUS,

	/*!
		@JP
		@brief 指定より前の時刻のロウのうち、最も新しいものを求めます。

		@EN
		@brief Returns the newest among the Rows whose timestamp are earlier
			than the specified time.

		@ENDL
	 */
	GS_TIME_OPERATOR_PREVIOUS_ONLY,

	/*!
		@JP
		@brief 指定時刻同一またはより後の時刻のロウのうち、最も古いものを求めます。

		@EN
		@brief Returns the oldest among the Rows whose timestamp are identical
			with or later than the specified time.

		@ENDL
	 */
	GS_TIME_OPERATOR_NEXT,

	/*!
		@JP
		@brief 指定時刻より後の時刻のロウのうち、最も古いものを求めます。

		@EN
		@brief Returns the oldest among the Rows whose timestamp are later than
			the specified time.

		@ENDL
	 */
	GS_TIME_OPERATOR_NEXT_ONLY

};

/*!
	@JP
	@see GSTimeOperatorTag

	@EN
	@see GSTimeOperatorTag

	@ENDL
 */
typedef GSEnum GSTimeOperator;

/*!
	@JP
	@brief 空間範囲同士の関係性についての制約を定義します。
	@par
		空間範囲検索の条件指定のために使用します。

	@EN
	@brief Defines a restriction to the relationship of each spatial region.
	@par
		This is used to set a search condition for the spatial region search.

	@ENDL
 */
enum GSGeometryOperatorTag {

	/*!
		@JP
		@brief 双方の空間範囲またはその外接構造が交差する関係にあることを示します。
		@par
			双方の外接直方体(Minimum Bounding Box)、もしくは外接直方体と
			2次曲面が交差する関係にあることを示します。
			交差判定の条件は、TQLの@c ST_MBRIntersects 関数、
			もしくは@c ST_QSFMBRIntersects 関数と同一です。

		@EN
		@brief Shows that each spatial region or each spatial region's
			circumscribing object has an intersection.
		@par
			Shows that each Minimum Bounding Box or Minimum Bounding Box and
			Quadric surface have an intersection. Condition of the cross-section
			determination is the same as TQL @c ST_MBRIntersects function or
			@c ST_QSFMBRIntersects function.

		@ENDL
	 */
	GS_GEOMETRY_OPERATOR_INTERSECT

};

/*!
	@JP
	@see GSGeometryOperatorTag

	@EN
	@see GSGeometryOperatorTag

	@ENDL
 */
typedef GSEnum GSGeometryOperator;

/*!
	@JP
	@brief 圧縮方式の種別を表します。
	@par
		時系列圧縮設定を行う際に使用します。

	@EN
	@brief Shows a type of compression methods.
	@par
		Used to set a configuration of compression.

	@ENDL
 */
enum GSCompressionMethodTag {
#if GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10
	GS_COMPRESSION_NONE,
	GS_COMPRESSION_LOSSLESS,
	GS_COMPRESSION_LOSSY
#else

	/*!
		@JP
		@brief 無圧縮であることを示します。

		@EN
		@brief Indicates it is a non-compression.

		@ENDL
	 */
	GS_COMPRESSION_NO,

	/*!
		@JP
		@brief 誤差なし間引き圧縮方式であることを示します。
		@par
			誤差なし間引き圧縮では、直前及び直後に登録したロウと同じデータを
			持つロウは省かれます。
			省かれたデータはinterpolateやsample処理の際に、誤差を発生することなく
			復元されます。

		@EN
		@brief Indicates it is a thinning compression method without error.
		@par
			In this compression, the Row with the same
			data as the Row registered just before and immediately after will be
			omitted.
			omitted data will be restored without error at the process of interpolate
			and sample processing.

		@ENDL
	 */
	GS_COMPRESSION_SS,

	/*!
		@JP
		@brief 誤差あり間引き圧縮方式であることを示します。
		@par
			誤差あり間引き圧縮では、前回まで及び直後に登録したデータと同じ傾斜を
			表すロウは省かれます。
			同じ傾斜かを判定する条件はユーザが指定できます。
			指定されたカラムが条件を満たし、それ以外のカラムの値が前回のデータと
			同じ場合のみ省かれます。
			省かれたデータはinterpolateやsample処理の際に、指定された誤差の範囲内で
			復元されます。

		@EN
		@brief Indicates it is a thinning compression method with error.
		@par
			In this compression, The Row which show the same slope as the data
			registered up to the last time and immediately after will be omitted.
			Conditions for determining whether the same slope can be specified by
			the user.
			It will be omitted only the case that the specified column meets the conditions
			and the value of the other columns are the same as the last data.
			The omitted data will be restored at a process of interpolate and sample
			within the specified error range.

		@ENDL
	 */
	GS_COMPRESSION_HI

#endif
};

/*!
	@JP
	@see GSCompressionMethodTag

	@EN
	@see GSCompressionMethodTag

	@ENDL
 */
typedef GSEnum GSCompressionMethod;

/*!
	@JP
	@brief 時系列処理で用いる時間の単位を示します。

	@EN
	@brief Represents the time unit(s) used in TimeSeries data operation.

	@ENDL
 */
enum GSTimeUnitTag {
	/*!
		@JP
		@brief 年の単位です。

		@EN
		@brief Unit of the year.

		@ENDL
	 */
	GS_TIME_UNIT_YEAR,

	/*!
		@JP
		@brief 月の単位です。

		@EN
		@brief Unit of the month.

		@ENDL
	 */
	GS_TIME_UNIT_MONTH,

	/*!
		@JP
		@brief 日の単位です。

		@EN
		@brief Unit of the day.

		@ENDL
	 */
	GS_TIME_UNIT_DAY,

	/*!
		@JP
		@brief 時の単位です。

		@EN
		@brief Unit of the hour.

		@ENDL
	 */
	GS_TIME_UNIT_HOUR,

	/*!
		@JP
		@brief 分の単位です。

		@EN
		@brief Unit of the minute.

		@ENDL
	 */
	GS_TIME_UNIT_MINUTE,

	/*!
		@JP
		@brief 秒の単位です。

		@EN
		@brief Unit of the second.

		@ENDL
	 */
	GS_TIME_UNIT_SECOND,

	/*!
		@JP
		@brief ミリ秒の単位です。

		@EN
		@brief Unit of the millisecond.

		@ENDL
	 */
	GS_TIME_UNIT_MILLISECOND
};

/*!
	@JP
	@see GSTimeUnitTag

	@EN
	@see GSTimeUnitTag

	@ENDL
 */
typedef GSEnum GSTimeUnit;

/*!
	@JP
	@brief コンテナの種別を表します。

	@EN
	@brief Represents the type(s) of a Container.

	@ENDL
 */
enum GSContainerTypeTag {

	/*!
		@JP
		@brief 対象のコンテナがコレクションであることを示します。

		@EN
		@brief Indicates the target Container is a Collection.

		@ENDL
	 */
	GS_CONTAINER_COLLECTION,

	/*!
		@JP
		@brief 対象のコンテナが時系列であることを示します。

		@EN
		@brief Indicates the target Container is a TimeSeries.

		@ENDL
	 */
	GS_CONTAINER_TIME_SERIES,

};

/*!
	@JP
	@see GSContainerTypeTag

	@EN
	@see GSContainerTypeTag

	@ENDL
 */
typedef GSEnum GSContainerType;

/*!
	@JP
	@brief GridDB上のフィールド値の型を表します。

	@EN
	@brief Represents the type(s) of field values in GridDB.

	@ENDL
 */
enum GSTypeTag {
	/*!
		@JP
		@brief STRING型です。

		@EN
		@brief STRING type.

		@ENDL
	 */
	GS_TYPE_STRING,

	/*!
		@JP
		@brief BOOL型です。

		@EN
		@brief BOOL type.

		@ENDL
	 */
	GS_TYPE_BOOL,

	/*!
		@JP
		@brief BYTE型です。

		@EN
		@brief BYTE type.

		@ENDL
	 */
	GS_TYPE_BYTE,

	/*!
		@JP
		@brief SHORT型です。

		@EN
		@brief SHORT type.

		@ENDL
	 */
	GS_TYPE_SHORT,

	/*!
		@JP
		@brief INTEGER型です。

		@EN
		@brief INTEGER type.

		@ENDL
	 */
	GS_TYPE_INTEGER,

	/*!
		@JP
		@brief LONG型です。

		@EN
		@brief LONG type.

		@ENDL
	 */
	GS_TYPE_LONG,

	/*!
		@JP
		@brief FLOAT型です。

		@EN
		@brief FLOAT type.

		@ENDL
	 */
	GS_TYPE_FLOAT,

	/*!
		@JP
		@brief DOUBLE型です。

		@EN
		@brief DOUBLE type.

		@ENDL
	 */
	GS_TYPE_DOUBLE,

	/*!
		@JP
		@brief TIMESTAMP型です。

		@EN
		@brief TIMESTAMP type.

		@ENDL
	 */
	GS_TYPE_TIMESTAMP,

	/*!
		@JP
		@brief GEOMETRY型です。

		@EN
		@brief GEOMETRY type.

		@ENDL
	 */
	GS_TYPE_GEOMETRY,

	/*!
		@JP
		@brief BLOB型です。

		@EN
		@brief BLOB type.

		@ENDL
	 */
	GS_TYPE_BLOB,

	/*!
		@JP
		@brief STRING型配列です。

		@EN
		@brief STRING array type.

		@ENDL
	 */
	GS_TYPE_STRING_ARRAY,

	/*!
		@JP
		@brief BOOL型配列です。

		@EN
		@brief BOOL array type.

		@ENDL
	 */
	GS_TYPE_BOOL_ARRAY,

	/*!
		@JP
		@brief BYTE型配列です。

		@EN
		@brief BYTE array type.

		@ENDL
	 */
	GS_TYPE_BYTE_ARRAY,

	/*!
		@JP
		@brief SHORT型配列です。

		@EN
		@brief SHORT array type.

		@ENDL
	 */
	GS_TYPE_SHORT_ARRAY,

	/*!
		@JP
		@brief INTEGER型配列です。

		@EN
		@brief INTEGER array type.

		@ENDL
	 */
	GS_TYPE_INTEGER_ARRAY,

	/*!
		@JP
		@brief LONG型配列です。

		@EN
		@brief LONG array type.

		@ENDL
	 */
	GS_TYPE_LONG_ARRAY,

	/*!
		@JP
		@brief FLOAT型配列です。

		@EN
		@brief FLOAT array type.

		@ENDL
	 */
	GS_TYPE_FLOAT_ARRAY,

	/*!
		@JP
		@brief DOUBLE型配列です。

		@EN
		@brief DOUBLE array type.

		@ENDL
	 */
	GS_TYPE_DOUBLE_ARRAY,

	/*!
		@JP
		@brief TIMESTAMP型配列です。

		@EN
		@brief TIMESTAMP array type.

		@ENDL
	 */
	GS_TYPE_TIMESTAMP_ARRAY,

#if GS_COMPATIBILITY_SUPPORT_3_5
	/*!
		@JP
		@brief ロウフィールドにNULLが設定されていることを示します。
		@par
			カラムの型として用いることはできません。
		@since 3.5

		@EN
		@brief Indicates the Row field is set to null.
		@par
			Cannot be used as Column type.
		@since 3.5

		@ENDL
	*/
	GS_TYPE_NULL = -1
#endif
};

/*!
	@JP
	@see GSTypeTag

	@EN
	@see GSTypeTag

	@ENDL
 */
typedef GSEnum GSType;

/*!
	@JP
	@brief カラムに関するオプション設定を示します。
	@see GSTypeOption

	@EN
	@brief Indicates optional settings for Column.
	@see GSTypeOption

	@ENDL
*/
enum GSTypeOptionTag {

#if GS_INTERNAL_DEFINITION_VISIBLE
	GS_TYPE_OPTION_KEY = 1 << 0,
#endif

#if GS_COMPATIBILITY_SUPPORT_3_5

	/*!
		@JP
		@brief NOT NULL制約を持たないカラムであることを示します。
		@since 3.5

		@EN
		@brief Indicates a Column without NOT NULL constraint.
		@since 3.5

		@ENDL
	*/
	GS_TYPE_OPTION_NULLABLE = 1 << 1,

	/*!
		@JP
		@brief NOT NULL制約を持つカラムであることを示します。
		@since 3.5

		@EN
		@brief Indicates a Column with NOT NULL constraint.
		@since 3.5

		@ENDL
	*/
	GS_TYPE_OPTION_NOT_NULL = 1 << 2,

#if GS_COMPATIBILITY_SUPPORT_4_1
	/*!
		@JP
		@brief 初期値としてNULLを使用するカラムであることを示します。
		@since 4.1

		@EN
		@brief Indicates use of NULL for the initial value.
		@since 4.1

		@ENDL
	*/
	GS_TYPE_OPTION_DEFAULT_VALUE_NULL = 1 << 3,

	/*!
		@JP
		@brief 初期値としてNULLを使用しないカラムであることを示します。
		@since 4.1

		@EN
		@brief Indicates no use of NULL for the initial value.
		@since 4.1

		@ENDL
	*/
	GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL = 1 << 4

#endif	// GS_COMPATIBILITY_SUPPORT_4_1

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

};

/*!
	@JP
	@brief カラムに関するオプション設定を示すフラグ値のビット和です。
	@par
		ある設定項目について、対応するフラグ値が複数含まれていた場合に、
		オプション設定が矛盾しているとみなされるものが存在します。
		それらの設定項目のうち、対応するフラグ値が一つも含まれていないものは、
		未設定状態であるとみなされます。この制約に該当する設定項目と
		フラグ値との対応は次の通りです。
		<table>
		<tr><th>設定項目</th><th>フラグ値</th></tr>
		<tr>
		<td>NOT NULL制約</td>
		<td>
		- @ref GS_TYPE_OPTION_NULLABLE
		- @ref GS_TYPE_OPTION_NOT_NULL
		</td>
		</tr>
		<tr>
		<td>初期値でのNULL使用有無</td>
		<td>
		- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NULL
		- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL
		</td>
		</tr>
		</table>
	@see GSTypeOptionTag

	@EN
	@brief Sum of bits of value of the flag indicating the option setting
		for Column.
	@par
		There are setting items that when more than one flag values for a
		setting item are included, the option setting is considered
		inconsistent. A setting item that neither of corresponding flag values
		is included is considered to be in an unconfigured state. Following
		flag values related to the setting items have those restrictions.
		<table>
		<tr><th>Setting item</th><th>Flag values</th></tr>
		<tr>
		<td>NOT NULL constraint state</td>
		<td>
		- @ref GS_TYPE_OPTION_NULLABLE
		- @ref GS_TYPE_OPTION_NOT_NULL
		</td>
		</tr>
		<tr>
		<td>Whether to use of NULL for the initial value</td>
		<td>
		- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NULL
		- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL
		</td>
		</tr>
		</table>
	@see GSTypeOptionTag

	@ENDL
*/
typedef int32_t GSTypeOption;

/*!
	@JP
	@brief @ref GSRowSet から取り出すことのできる内容の種別です。

	@EN
	@brief The type of content that can be extracted from @ref GSRowSet.

	@ENDL
 */
enum GSRowSetTypeTag {
	/*!
		@JP
		@brief クエリ実行対象のコンテナと対応する型のロウデータからなる@ref GSRowSet
			であることを示します。

		@EN
		@brief Indicate it is @ref GSRowSet consist of the type of the Row data
			corresponding with container the target of query execution.

		@ENDL
	 */
	GS_ROW_SET_CONTAINER_ROWS,

	/*!
		@JP
		@brief 集計演算からなる@ref GSRowSet であることを示します。
		@see GSAggregationResult

		@EN
		@brief Indicates it is @ref GSRowSet consisting aggregation operation.
		@see GSAggregationResult

		@ENDL
	 */
	GS_ROW_SET_AGGREGATION_RESULT,

	/*!
		@JP
		@brief EXPLAIN文ならびEXPLAIN ANALYZE文の実行結果エントリからなる
			@ref GSRowSet であることを示します。
		@see GSQueryAnalysisEntry

		@EN
		@brief Indicates it is @ref GSRowSet consisting of the execution result entry of
			EXPLAIN statement and EXPLAIN ANALYZE statement.
		@see GSQueryAnalysisEntry

		@ENDL
	 */
	GS_ROW_SET_QUERY_ANALYSIS
};

/*!
	@JP
	@see GSRowSetTypeTag

	@EN
	@see GSRowSetTypeTag

	@ENDL
 */
typedef GSEnum GSRowSetType;

/*!
	@JP
	@brief 特定のカラムの圧縮設定を表します。
	@par
		時系列を対象とした誤差あり間引き圧縮のカラム別設定に使用します。

	@EN
	@brief Represents the compression settings for a particular column.
	@par
		Use for the column-specific settings for thinning compression with error
		intended for the TimeSeries.

	@ENDL
 */
typedef struct GSColumnCompressionTag {

	/*!
		@JP
		@brief 設定対象のカラムの名前です。
		@par
			間引き圧縮方式(@ref GS_COMPRESSION_HI)を選択し、次の型の
			カラムに対して指定した場合のみ、カラム別の圧縮設定として使用できます。
			- @ref GS_TYPE_BYTE
			- @ref GS_TYPE_SHORT
			- @ref GS_TYPE_INTEGER
			- @ref GS_TYPE_LONG
			- @ref GS_TYPE_FLOAT
			- @ref GS_TYPE_DOUBLE

		@EN
		@brief Name of setting target column.
		@par
			Possible to be used as column-specific compression settings only if
			it selects thinning compression method (@ref GS_COMPRESSION_HI) and
			specified for the following type of column.
			- @ref GS_TYPE_BYTE
			- @ref GS_TYPE_SHORT
			- @ref GS_TYPE_INTEGER
			- @ref GS_TYPE_LONG
			- @ref GS_TYPE_FLOAT
			- @ref GS_TYPE_DOUBLE

		@ENDL
	 */
	const GSChar *columnName;

#if GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10
	GSCompressionMethod method;
	double threshold;
	GSBool thresholdRelative;
#else

	/*!
		@JP
		@brief 間引き圧縮における判定基準値として、相対誤差を適用するかどうかを
			示します。
		@par
			間引き圧縮方式(@ref GS_COMPRESSION_HI)以外を選択した場合は無視されます。

		@EN
		@brief Indicates whether or not to apply the relative error as a criterion
			value in the thinning compression.
		@par
			If the non-thinning compression method (@ref GS_COMPRESSION_HI) is selected
			it will be ignored.

		@ENDL
	 */
	GSBool relative;

	/*!
		@JP
		@brief 相対誤差あり間引き圧縮における、値がとりうる範囲を基準とした
			誤差境界値の比率です。
		@par
			値がとりうる範囲は、@c span メンバと対応します。
			また、比率は@c 0 以上@c 1 以下でなければ、時系列を作成することが
			できません。
		@par
			誤差あり間引き圧縮方式(@ref GS_COMPRESSION_HI)において、
			判定基準値として相対誤差を選択(@ref GSColumnCompressionTag::relative
			に @ref GS_TRUE を指定)した場合のみ有効です。

		@EN
		@brief Ratio of the error boundary value on the basis of the range of values can
			take in the relative error thinning compression.
		@par
			Range that value can take correspond to the @c span members.
			In addition, unless the ratio is between @c 0 and @c 1, TimeSeries will not
			be able to create.
		@par
			In error thinning compression method(@ref GS_COMPRESSION_HI), it is
			only valid if it selects the relative error as a criterion value (specify
			@ref GS_TRUE to @ref GSColumnCompressionTag::relative).

		@ENDL
	 */
	double rate;

	/*!
		@JP
		@brief 相対誤差あり間引き圧縮における、値がとりうる範囲の最大値と
			最小値の差です。
		@par
			誤差あり間引き圧縮方式(@ref GS_COMPRESSION_HI)において、
			判定基準値として相対誤差を選択(@ref GSColumnCompressionTag::relative
			に @ref GS_TRUE を指定)した場合のみ有効です。

		@EN
		@brief It is the difference between the maximum value and the minimum value of
			the range of values can take in the relative error thinning compression.
		@par
			In error thinning compression method(@ref GS_COMPRESSION_HI), it is
			only valid if it selects the relative error as a criterion value (specify
			@ref GS_TRUE to @ref GSColumnCompressionTag::relative).

		@ENDL
	 */
	double span;

	/*!
		@JP
		@brief 絶対誤差あり間引き圧縮における、誤差境界の幅です。
		@par
			誤差あり間引き圧縮方式(@ref GS_COMPRESSION_HI)において、
			判定基準値として相対誤差を選択(@ref GSColumnCompressionTag::relative
			に @ref GS_TRUE を指定)した場合のみ有効です。

		@EN
		@brief Width of the error boundary in absolute error thinning compression.
		@par
			In error thinning compression method(@ref GS_COMPRESSION_HI), it is
			only valid if it selects the relative error as a criterion value (specify
			@ref GS_TRUE to @ref GSColumnCompressionTag::relative).

		@ENDL
	 */
	double width;

#endif
} GSColumnCompression;

#if GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10
#define GS_COLUMN_COMPRESSION_INITIALIZER \
	{ NULL, GS_COMPRESSION_NONE, 0, GS_FALSE }
#else
/*!
	@JP
	@brief @ref GSColumnCompression の初期化子です。

	@EN
	@brief Initializer of @ref GSColumnCompression.

	@ENDL
 */
#define GS_COLUMN_COMPRESSION_INITIALIZER \
	{ NULL, GS_FALSE, 0, 0, 0 }
#endif

/*!
	@JP
	@brief コレクションの構成オプションを表します。
	@note 現バージョンでは使用されておりません。

	@EN
	@brief Represents a collection of configuration options.
	@note Not used in the current version

	@ENDL
 */
typedef struct GSCollectionPropertiesTag {
#if GS_INTERNAL_DEFINITION_VISIBLE
	struct {
		int8_t unused;
	} internal;
#endif
} GSCollectionProperties;

/*!
	@JP
	@brief @ref GSCollectionProperties の初期化子です。

	@EN
	@brief Initializer of @ref GSCollectionProperties.

	@ENDL
 */
#define GS_COLLECTION_PROPERTIES_INITIALIZER \
	{ 0 }

/*!
	@JP
	@brief 時系列を新規作成または変更する際に使用される、オプションの構成情報を
		表します。
	@par
		カラム名の表記、もしくは、個別に圧縮設定できるカラム数の上限などの
		内容の妥当性について、必ずしも検査するとは限りません。

	@EN
	@brief Represents the information about optional configuration settings
		used for newly creating or updating a TimeSeries.
	@par
		It does not guarantee the validity of values e.g. the column
		names and the upper limit of the column number that can be
		individually compressed.

	@ENDL
 */
typedef struct GSTimeSeriesPropertiesTag {

	/*!
		@JP
		@brief ロウの有効期限の基準となる経過期間です。
		@par
			ロウの有効期限の時刻は、ロウキーの時刻から指定の経過期間を加算することで
			求まります。有効期限の時刻がGridDB上の現在時刻よりも古いロウは、
			有効期限の切れたロウとみなされます。
			期限切れのロウは、検索や更新といったロウ操作の対象から外れ、存在しないものと
			みなされます。
			対応するGridDB上の内部データは、随時削除されます。
		@attention
			有効期限超過の判定に使用される現在時刻は、GridDBの各ノードの
			実行環境に依存します。
			したがって、ネットワークの遅延や実行環境の時刻設定のずれなどにより、
			このプロセスの実行環境の現在時刻より前に期限切れ前のロウにアクセスできなくなる
			場合や、この現在時刻より後に期限切れロウにアクセスできる場合があります。
			意図しないロウの喪失を避けるために、最低限必要な期間よりも大きな値を
			設定することを推奨します。
		@par
			作成済みの時系列の設定を変更することはできません。
		@par
			値が負の場合、有効期限はないものとみなされ、明示的に削除操作を
			行わない限りロウが削除されなくなります。
			@c 0 が設定されたオプションを指定して時系列を作成することは
			できません。

		@EN
		@brief The elapsed time period of a Row to be used as the basis of the
			validity period.
		@par
			The validity period of a Row can be obtained by adding the specified
			elapsed time period to the timestamp of the Row key. Rows whose
			expiration times are older than the current time on GridDB are
			treated as expired Rows.
			Expired Rows are treated as non-existent and are not used for
			search, update or other Row operations.
			The corresponding internal data in GridDB will be deleted as needed.
		@attention
			The current time used for expiry check is dependent on the runtime
			environment of each node of GridDB.
			Therefore, there may be cases where unexpired Rows cannot be
			accessed before the current time of the process on execution
			environment, or expired Rows can be accessed after the current time
			because of a network delay or a discrepancy in the time setting of
			the runtime environment.
			In order to avoid unintended loss of Rows, you should set a value
			larger than the minimum required period of time.
		@par
			The setting for an already-created TimeSeries cannot be changed.
		@par
			If the value is less than @c 0, it is determined there is no expiration
			date and unless you execute deleting operation explicitly, Rows will not
			be deleted. A TimeSeries cannot be created with specifying that the value is @c 0.

		@ENDL
	 */
	int32_t rowExpirationTime;

	/*!
		@JP
		@brief ロウの有効期限の基準とする経過時間の単位です。
		@par
			有効期限の期間値が設定されていた場合、@ref GS_TIME_UNIT_YEAR
			または@ref GS_TIME_UNIT_MONTH が設定されたオプションを指定して、
			時系列を作成することはできません。

		@EN
		@brief The unit of elapsed time referenced for the expiration date of a Row.
		@par
			When the elapsed time value has been set, a TimeSeries cannot be
			created with specifying @ref GS_TIME_UNIT_YEAR or @ref GS_TIME_UNIT_MONTH
			as the value of this option.

		@ENDL
	 */
	GSTimeUnit rowExpirationTimeUnit;

	/*!
		@JP
		@brief 間引き圧縮において連続して間引きされるロウの最大期間です。
		@par
			この期間が設定された時系列のロウについて、前方のロウと指定の期間
			以上時刻が離れていた場合、間引き圧縮として間引き可能な条件を満たして
			いたとしても、間引かれなくなります。
		@par
			時系列圧縮方式として @ref GS_COMPRESSION_NO が設定されていた
			場合、この期間の設定は無視されます。
		@par
			時系列圧縮方式として @ref GS_COMPRESSION_HI または
			@ref GS_COMPRESSION_SS が設定されており、この期間として@c 0 以下の
			値が設定された場合、TIMESTAMP型の取りうる値の範囲全体が期間として
			設定されたとみなされます。
		@par
			前方のロウと指定の期間以上時刻が離れておらず、かつ、間引き圧縮として
			間引き可能な条件を満たしていたとしても、格納先の内部の配置などに
			よっては間引かれない場合があります。

		@EN
		@brief The maximum period of the Row which is thinned continuously in thinning
			compression.
		@par
			If the period is set to a TimeSeries and the time of the specified period is apart from
			the previous Row of the TimeSeries, it will not be thinned even if it satisfies possible
			conditions for thinning as thinning compression.
		@par
			If @ref GS_COMPRESSION_NO has been set as the
			compression method, the setting of the maximum
			periods is ignored.
		@par
			If @ref GS_COMPRESSION_HI or @ref GS_COMPRESSION_SS
			has been set, in the case that this setting is
			not specified, the maximum value of TIMESTAMP
			is set as the period.
		@par
			Even if the row is in the period from the
			previous data and it satisfies the conditions
			of the compression, it may not be thinned by
			the stored location, etc.
		@par
			If the value is @c 0 or less, the period of the Row thinning in succession
			will be unlimited.

		@ENDL
	 */
	int32_t compressionWindowSize;

	/*!
		@JP
		@brief 間引き圧縮において連続して間引きされるロウの最大期間の
			単位です。
		@par
			最大期間の値が明示的に設定されていた場合、@ref GS_TIME_UNIT_YEAR
			または@ref GS_TIME_UNIT_MONTH が設定されたオプションを指定して、
			時系列を作成することはできません。

		@EN
		@brief A unit of the maximum period of the Row thinned continuously in thinning
			compression.
		@par
			When the maximum period has been specified
			explicitly, a TimeSeries cannot be created
			with specifying @ref GS_TIME_UNIT_YEAR or
			@ref GS_TIME_UNIT_MONTH as the value of this option.

		@ENDL
	 */
	GSTimeUnit compressionWindowSizeUnit;

#if !(GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10)

	/*!
		@JP
		@brief 時系列圧縮方式の種別です。

		@EN
		@brief Type of TimeSeries compression method.

		@ENDL
	 */
	GSCompressionMethod compressionMethod;

#endif

	/*!
		@JP
		@brief カラム別圧縮設定(@c compressionList)のエントリ数です。
		@par
			1つの時系列に対してパラメータ設定できるカラム の上限数については、
			GridDBテクニカルリファレンスを参照してください。
			上限を超えたオプションを指定して時系列を作成することはできません。

		@EN
		@brief Number of entries of compression settings by column (@c compressionList).
		@par
			Refer to the GridDB Technical Reference for the maximum number of columns
			that can set one of TimeSeries parameters.
			A TimeSeries with the option that exceeds the upper limit cannot be created.

		@ENDL
	 */
	size_t compressionListSize;

	/*!
		@JP
		@brief カラム別の圧縮設定です。
		@par
			エントリ数が@c 0 の場合、時系列を新規作成または更新する際に
			無視されます。

		@EN
		@brief Compression settings by column
		@par
			If the number of entries is @c 0, it is ignored when the TimeSeries is
			newly created or updated.

		@ENDL
	 */
	GSColumnCompression *compressionList;

#if GS_COMPATIBILITY_SUPPORT_2_0

	/*!
		@JP
		@brief 期限に到達したロウデータの解放単位と対応する、有効期間に対しての
			分割数です。
		@par
			分割数を設定すると、期限に到達したロウデータの管理領域を
			解放するための条件を制御できます。期限に到達したロウデータが
			分割数に相当する期間だけ集まった時点で解放しようとします。
		@par
			分割数の上限については、GridDBテクニカルリファレンスを参照
			してください。上限を超えたオプションを指定して時系列を作成
			することはできません。
		@par
			値が負の場合、分割数が設定されていないことを示します。
			@c 0 が設定されたオプションを指定して時系列を作成することは
			できません。
		@par
			ロウの有効期限の基準となる経過期間の設定がない場合、この分割数の
			設定は無視されます。
			一方、ロウの有効期限の基準となる経過期間の設定がある場合にこの
			分割数の設定を省略すると、作成される時系列にはGridDBクラスタ上の
			デフォルトの分割数が設定されます。
		@since 2.0

		@EN
		@brief Sets the division number for the validity period as the number of
			expired Row data units to be released.
		@par
			The division number set is used to control the conditions for releasing
			Row data management areas that have expired the period. Expired Row data
			shall be released at the point they are collected only when the period
			equivalent to the division number is reached.
		@par
			Refer to the GridDB Technical Reference for the division number upper limit.
			A TimeSeries with the option that exceeds the upper limit cannot be created.
		@par
			If the value is negative, it indicates the division number has not been set.
			A TimeSeries cannot be specified with specifying @c 0 as the value of this option.
		@par
			If the elapsed time period is not set, the
			setting of division number is ignored. If the
			elapsed time period has been set and the setting of
			division number is not set, the division
			number for the TimeSeries to be created is set
			to the default value of the connected GridDB server.
		@since 2.0

		@ENDL
	 */
	int32_t expirationDivisionCount;

#endif	// GS_COMPATIBILITY_SUPPORT_2_0

} GSTimeSeriesProperties;

#if GS_COMPATIBILITY_SUPPORT_2_0
/*!
	@JP
	@brief @ref GSTimeSeriesProperties の初期化子です。
	@par
		ロウの有効期限ならびに圧縮ロウの間引き連続制限は無効、
		時系列圧縮方式は無圧縮に設定されます。

	@EN
	@brief Initializer of @ref GSTimeSeriesProperties.
	@par
		Invalid the validity period of a Row and the continuous thinning limitation of
		a compression Row. TimeSeries compression method is set to no compression.

	@ENDL
 */
#define GS_TIME_SERIES_PROPERTIES_INITIALIZER \
	{ -1, GS_TIME_UNIT_DAY, -1, GS_TIME_UNIT_DAY, \
	GS_COMPRESSION_NO, 0, NULL, -1 }
#elif !GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10
#define GS_TIME_SERIES_PROPERTIES_INITIALIZER \
	{ -1, GS_TIME_UNIT_DAY, -1, GS_TIME_UNIT_DAY, \
	GS_COMPRESSION_NO, 0, NULL }
#else
#define GS_TIME_SERIES_PROPERTIES_INITIALIZER \
	{ -1, GS_TIME_UNIT_DAY, -1, GS_TIME_UNIT_DAY, 0, NULL }
#endif

/*!
	@JP
	@brief カラムのスキーマに関する情報を表します。

	@EN
	@brief Represents the information about the schema of a Column.

	@ENDL
 */
typedef struct GSColumnInfoTag {

	/*!
		@JP
		@brief カラム名です。

		@EN
		@brief Name of a Column.

		@ENDL
	*/
	const GSChar *name;

	/*!
		@JP
		@brief カラムの型、すなわち、カラムに対応する各フィールド値の型です。

		@EN
		@brief Type of a Column, i.e., the type of each field value
			corresponding to a Column.

		@ENDL
	 */
	GSType type;

#if GS_COMPATIBILITY_SUPPORT_1_5

	/*!
		@JP
		@brief 索引種別を示すフラグ値のビット和です。
		@par
			@ref GS_INDEX_FLAG_DEFAULT は索引種別が未設定であることを
			示します。
		@since 1.5

		@EN
		@brief Sum of bits of value of the flag indicating the index type
		@par
			@ref GS_INDEX_FLAG_DEFAULT indicates the index type is not set.
		@since 1.5

		@ENDL
	*/
	GSIndexTypeFlags indexTypeFlags;

#if GS_COMPATIBILITY_SUPPORT_3_5

	/*!
		@JP
		@brief カラムに関するオプション設定を示すフラグ値のビット和です。
		@par
			現バージョンでは、NOT NULL制約または初期値に関連する、以下の
			フラグ値のみを含めることができます。
			- @ref GS_TYPE_OPTION_NULLABLE
			- @ref GS_TYPE_OPTION_NOT_NULL
			- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NULL
			- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL
		@since 3.5

		@EN
		@brief Sum of bits of value of the flag indicating the option setting
			for Column.
		@par
			In the current version, it can only contain the following flag
			values for the NOT NULL constraint or the initial value.
			- @ref GS_TYPE_OPTION_NULLABLE
			- @ref GS_TYPE_OPTION_NOT_NULL
			- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NULL
			- @ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL
		@since 3.5

		@ENDL
	*/
	GSTypeOption options;
#endif	// GS_COMPATIBILITY_SUPPORT_3_5

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

} GSColumnInfo;

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief @ref GSColumnInfo の初期化子です。

	@EN
	@brief Initializer of @ref GSColumnInfo.

	@ENDL
 */
#define GS_COLUMN_INFO_INITIALIZER \
	{ NULL, GS_TYPE_STRING, GS_INDEX_FLAG_DEFAULT, 0 }

#elif GS_COMPATIBILITY_SUPPORT_1_5

#define GS_COLUMN_INFO_INITIALIZER \
	{ NULL, GS_TYPE_STRING, GS_INDEX_FLAG_DEFAULT }

#else

#define GS_COLUMN_INFO_INITIALIZER \
	{ NULL, GS_TYPE_STRING }

#endif	// not GS_COMPATIBILITY_SUPPORT_1_5

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@brief トリガの種別を表します。

	@EN
	@brief Represent the trigger type.

	@ENDL
 */
enum GSTriggerTypeTag {
	/*!
		@JP
		@brief コンテナの更新時にRESTで通知するトリガ種別を示します。

		@EN
		@brief Indicate the trigger type to notify in a REST notification when
			a Container is updated.

		@ENDL
	 */
	GS_TRIGGER_REST,

	/*!
		@JP
		@brief コンテナの更新時にJava Message Service(JMS)で通知するトリガ種別を示します。

		@EN
		@brief Indicate the trigger type to notify in a Java Message Service
			(JMS) notification when a Container is updated.

		@ENDL
	 */
	GS_TRIGGER_JMS

};

/*!
	@JP
	@see GSTriggerTypeTag

	@EN
	@see GSTriggerTypeTag

	@ENDL
 */
typedef GSEnum GSTriggerType;

/*!
	@JP
	@brief トリガで監視対象とする更新操作種別を表します。

	@EN
	@brief Represent the update operation type subject to monitoring by the
		trigger.

	@ENDL
 */
enum GSTriggerEventTypeFlagTag {
	/*!
		@JP
		@brief コンテナに対するロウ新規作成または更新を示します。

		@EN
		@brief Indicate the creation of a new Row or update of an existing Row
			for a Container.

		@ENDL
	 */
	GS_TRIGGER_EVENT_PUT = 1 << 0,

	/*!
		@JP
		@brief コンテナに対するロウ削除を示します。

		@EN
		@brief Indicate the deletion of a Row for a Container.

		@ENDL
	 */
	GS_TRIGGER_EVENT_DELETE = 1 << 1

};

/*!
	@JP
	@see GSTriggerEventTypeFlagTag

	@EN
	@see GSTriggerEventTypeFlagTag

	@ENDL
 */
typedef int32_t GSTriggerEventTypeFlags;

/*!
	@JP
	@brief トリガに関する情報を表します。
	@since 1.5

	@EN
	@brief Represent the trigger information.
	@since 1.5

	@ENDL
 */
typedef struct GSTriggerInfoTag {

	/*!
		@JP
		@brief トリガ名です。

		@EN
		@brief The trigger name.

		@ENDL
	 */
	const GSChar *name;

	/*!
		@JP
		@brief トリガ種別です。

		@EN
		@brief The trigger type.

		@ENDL
	 */
	GSTriggerType type;

	/*!
		@JP
		@brief 通知先URIです。

		@EN
		@brief The notification destination URI.

		@ENDL
	 */
	const GSChar *uri;

	/*!
		@JP
		@brief 監視対象とする更新操作種別です。

		@EN
		@brief The update operation type subject to monitoring by the trigger.

		@ENDL
	 */
	GSTriggerEventTypeFlags eventTypeFlags;

	/*!
		@JP
		@brief 通知対象とするカラム名の集合です。

		@EN
		@brief A set of column names to be notified.

		@ENDL
	 */
	const GSChar *const *columnSet;

	/*!
		@JP
		@brief 通知対象とするカラム名の数です。

		@EN
		@brief The number of column names to be notified.

		@ENDL
	 */
	size_t columnCount;

	/*!
		@JP
		@brief JMS通知で使用するJMSデスティネーション種別です。

		@EN
		@brief The JMS destination type used in a JMS notification.

		@ENDL
	 */
	const GSChar *jmsDestinationType;

	/*!
		@JP
		@brief JMS通知で使用するJMSデスティネーション名です。

		@EN
		@brief The JSM destination name used in a JMS notification.

		@ENDL
	 */
	const GSChar *jmsDestinationName;

	/*!
		@JP
		@brief 通知先サーバに接続する際のユーザ名です。

		@EN
		@brief The user name when connecting to a notification destination
			server.

		@ENDL
	 */
	const GSChar *user;

	/*!
		@JP
		@brief 通知先サーバに接続する際のパスワードです。

		@EN
		@brief The password when connecting to a notification destination
			server.

		@ENDL
	 */
	const GSChar *password;

} GSTriggerInfo;

/*!
	@JP
	@brief @ref GSTriggerInfo の初期化子です。

	@EN
	@brief Initializer of @ref GSTriggerInfo.

	@ENDL
 */
#define GS_TRIGGER_INFO_INITIALIZER \
	{ NULL, GS_TRIGGER_REST, NULL, \
	0, NULL, \
	NULL, NULL, NULL, NULL }

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief 索引の設定内容を表します。
	@since 3.5

	@EN
	@brief Indicates the contents of index setting.
	@since 3.5

	@ENDL
*/
typedef struct GSIndexInfoTag {

	/*!
		@JP
		@brief 索引名です。
		@since 3.5

		@EN
		@brief Index name
		@since 3.5

		@ENDL
	*/
	const GSChar *name;

	/*!
		@JP
		@brief 索引種別を示すフラグ値です。
		@par
			デフォルトまたは任意個数の索引種別を含めることができます。
			複数個の索引種別を含める場合は、各種別のフラグ値のビット和により
			表現します。コンテナ情報の一部として得られた索引情報では、
			デフォルトを除くいずれか一つの索引種別のみが設定されます。
		@since 3.5

		@EN
		@brief This is a flag value which shows index classification.
		@par
			Default or any number of index types can be included. When
			multiple index types are included, it is expressed by sum of bits
			of various flag values. For index information obtained as part
			of container information, only one index type except the default is set.
		@since 3.5

		@ENDL
	*/
	GSIndexTypeFlags type;

	/*!
		@JP
		@brief 索引に対応するカラムのカラム番号です。
		@since 3.5

		@EN
		@brief Column number
		@since 3.5

		@ENDL
	*/
	int32_t column;

	/*!
		@JP
		@brief 索引に対応するカラムのカラム名です。
		@since 3.5

		@EN
		@brief Column name
		@since 3.5

		@ENDL
	*/
	const GSChar *columnName;

} GSIndexInfo;

#define GS_INDEX_INFO_INITIALIZER \
	{ NULL, GS_INDEX_FLAG_DEFAULT, -1, NULL }

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief 特定のコンテナに関する情報を表します。

	@EN
	@brief Represents the information about a specific Container.

	@ENDL
 */
typedef struct GSContainerInfoTag {

	/*!
		@JP
		@brief コンテナ名です。

		@EN
		@brief Name of container.

		@ENDL
	 */
	const GSChar *name;

	/*!
		@JP
		@brief コンテナの種別です。

		@EN
		@brief Type of container.

		@ENDL
	 */
	GSContainerType type;

	/*!
		@JP
		@brief カラム数です。

		@EN
		@brief Number of columns

		@ENDL
	 */
	size_t columnCount;

	/*!
		@JP
		@brief カラム情報のリストです。
		@par
			カラム数と同一の長さの@ref GSColumnInfo の配列です。
			各要素はカラムの定義順と対応します。

		@EN
		@brief List of the information of Columns.
		@par
			The @ref GSColumnInfo array which has the same length as column number.
			Each element corresponds to the defining order of columns.

		@ENDL
	 */
	const GSColumnInfo *columnInfoList;

	/*!
		@JP
		@brief ロウキーに対応するカラムが設定されているかどうかを示す真偽値です。
		@par
			コンテナがロウキーを持つ場合、対応するカラム番号は0です。

		@EN
		@brief The boolean value indicating whether the Row key Column is assigned.
		@par
			If the Container has a Row key, the number of its corresponding Column is 0.

		@ENDL
	 */
	GSBool rowKeyAssigned;

#if GS_COMPATIBILITY_SUPPORT_1_5

	/*!
		@JP
		@brief カラム順序が無視できるかどうかを示す真偽値です。
		@see gsPutContainerGeneral
		@since 1.5

		@EN
		@brief The boolean value indicating whether the order of Columns can be ignored.
		@see gsPutContainerGeneral
		@since 1.5

		@ENDL
	 */
	GSBool columnOrderIgnorable;

	/*!
		@JP
		@brief 時系列構成オプションです。
		@since 1.5

		@EN
		@brief Option of TimeSeries configuration.
		@since 1.5

		@ENDL
	 */
	const GSTimeSeriesProperties *timeSeriesProperties;

	/*!
		@JP
		@brief トリガ情報のエントリ数です。
		@since 1.5

		@EN
		@brief The number of entries of the trigger information.
		@since 1.5

		@ENDL
	 */
	size_t triggerInfoCount;

	/*!
		@JP
		@brief トリガ情報の一覧です。
		@since 1.5

		@EN
		@brief A list of the trigger information.
		@since 1.5

		@ENDL
	 */
	const GSTriggerInfo *triggerInfoList;

#if GS_COMPATIBILITY_SUPPORT_2_1
	/*!
		@JP
		@brief データ配置最適化のために用いられる、コンテナ間の類似性(データ
			アフィニティ)を示す文字列です。
		@par
			同一クラスタノード上の同一管理領域内に格納されるコンテナについて、
			配置先を最適化するために使用されます。
		@par
			データアフィニティが同一のコンテナの内容は、近接する配置先に格納される
			可能性が高くなります。また、解放期限が設定され、近接する配置先に
			格納された時系列について、登録頻度などの変更パターンが類似している場合、
			解放期限に到達したロウの解放処理が効率的に行われる可能性が
			高くなります。
		@par
			コンテナの定義において使用できるデータアフィニティ文字列の文字種や
			長さには制限があります。具体的には、GridDBテクニカルリファレンスを参照
			してください。ただし、文字列を設定した時点で必ずしもすべての制限を
			検査するとは限りません。特に記載のない限り、データアフィニティ文字列が
			使用される操作では、ASCIIの大文字・小文字表記の違いが区別されます。
		@par
			値が@c NULL の場合、標準設定を優先することを示します。
		@since 2.1

		@EN
		@brief A strings indicating affinity between Containers (data affinity)
			for data location optimizing.
		@par
			Use for optimizing the arrangement of Containers stored in the same
			management area on the same cluster node.
		@par
			Containers which have the same data affinity string may be stored near
			each other. Therefore the efficiency for the expiration of Rows may be
			improved by using the same data affinity string for TimeSeries Containers
			which includes Rows with similar elapsed time periods.
		@par
			An empty string is not acceptable. A data affinity string must be composed
			of characters same as a Container name. See "System limiting values" in the
			GridDB Technical Reference for the maximum length of the string. A Container with
			a Container name longer than the maximum length cannot be created.
		@par
			There are the limitations, allowed characters
			and maximum length, for the data affinity
			string. See the GridDB Technical Reference for
			the details. All the restrictions may not be
			checked when setting the string. The data
			affinity string is case sensitive unless
			otherwise noted.
		@par
			If null is specified, the Container will be stored as usual.
		@since 2.1

		@ENDL
	 */
	const GSChar *dataAffinity;

#if GS_COMPATIBILITY_SUPPORT_3_5

	/*!
		@JP
		@brief 索引情報のエントリ数です。
		@since 3.5

		@EN
		@brief Number of @ref GSIndexInfo object in a list
		@since 3.5

		@ENDL
	*/
	size_t indexInfoCount;

	/*!
		@JP
		@brief 索引情報の一覧です。
		@since 3.5

		@EN
		@brief List of @ref GSIndexInfo object
		@since 3.5

		@ENDL
	*/
	const GSIndexInfo *indexInfoList;

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

#endif	// GS_COMPATIBILITY_SUPPORT_2_1

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

} GSContainerInfo;

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief @ref GSContainerInfo の初期化子です。

	@EN
	@brief Initializer of @ref GSContainerInfo.

	@ENDL
 */
#define GS_CONTAINER_INFO_INITIALIZER \
	{ NULL, GS_CONTAINER_COLLECTION, 0, NULL, GS_FALSE, \
	GS_FALSE, NULL, 0, NULL, NULL, 0, NULL }

#elif GS_COMPATIBILITY_SUPPORT_2_1

#define GS_CONTAINER_INFO_INITIALIZER \
	{ NULL, GS_CONTAINER_COLLECTION, 0, NULL, GS_FALSE, \
	GS_FALSE, NULL, 0, NULL, NULL }

#elif GS_COMPATIBILITY_SUPPORT_1_5

#define GS_CONTAINER_INFO_INITIALIZER \
	{ NULL, GS_CONTAINER_COLLECTION, 0, NULL, GS_FALSE, \
	GS_FALSE, NULL, 0, NULL }

#else

#define GS_CONTAINER_INFO_INITIALIZER \
	{ NULL, GS_CONTAINER_COLLECTION, 0, NULL, GS_FALSE }

#endif	// not GS_COMPATIBILITY_SUPPORT_1_5

#if GS_INTERNAL_DEFINITION_VISIBLE
typedef struct GSBindingEntryTag {
	const GSChar *columnName;
	GSType elementType;
	size_t offset;
	size_t arraySizeOffset;
	GSTypeOption options;		// GSTypeOption
} GSBindingEntry;
#endif

/*!
	@JP
	@brief ロウオブジェクトとロウデータとの対応関係を表すバインディング情報です。

	@EN
	@brief The binding information representing the correspondence between
		a Row objects and a Row data.

	@ENDL
 */
typedef struct GSBindingTag {
#if GS_INTERNAL_DEFINITION_VISIBLE
	GSBindingEntry *entries;
	size_t entryCount;
#endif
} GSBinding;

/*!
	@JP
	@brief クエリプランならびにクエリ処理解析結果を構成する
		一連の情報の一つを示します。
	@par
		TQLのEXPLAIN文ならびEXPLAIN ANALYZE文の実行結果を保持するために
		使用します。1つの実行結果は、このエントリの列により表現されます。

	@EN
	@brief Represents one of information entries composing a query plan
		and the results of analyzing a query operation.
	@par
		Used to hold the result of executing an EXPLAIN statement or
		an EXPLAIN ANALYZE statement in TQL. One execution result is represented
		by an array of entries.

	@ENDL
 */
typedef struct GSQueryAnalysisEntryTag {

	/*!
		@JP
		@brief 一連のエントリ列における、このエントリの位置を示すIDです。
		@par
			TQLを実行した結果を受け取る場合、IDは1から順に割り当てられます。

		@EN
		@brief the ID indicating the location of an entry in an array of entries.
		@par
			In a result set of executing a TQL query, IDs are assigned serially
			starting from 1.

		@ENDL
	 */
	int32_t id;

	/*!
		@JP
		@brief 他のエントリとの関係を表すための、深さです。
		@par
			このエントリより小さな値のIDを順にたどり、このエントリの深さより1だけ
			浅いエントリが存在した場合、このエントリは、該当するエントリの内容を
			より詳しく説明するためのものであることを意味します。

		@EN
		@brief the depth indicating the relation to other entries.
		@par
			If there is found an entry whose depth is smaller than that of a target
			entry by one, through checking entries one by one whose
			IDs are smaller than that of the target entry, it means that the target
			entry describes the content of the found entry in more detail.

		@ENDL
	 */
	int32_t depth;

	/*!
		@JP
		@brief このエントリが示す情報の種別です。
		@par
			実行時間といった解析結果の種別、クエリプランの構成要素の種別などを
			表します。

		@EN
		@brief The type of information indicated by an entry
		@par
			Represents the type of an analysis result of execution time, the type
			of a component of a query, etc.

		@ENDL
	 */
	const GSChar *type;

	/*!
		@JP
		@brief このエントリが示す情報に対応付けられた値の型名です。
		@par
			実行時間といった解析結果などに対応する値の型名です。
			型の名称は、TQLで定義された基本型のうち次のものです。
			- STRING
			- BOOL
			- BYTE
			- SHORT
			- INTEGER
			- LONG
			- FLOAT
			- DOUBLE

		@EN
		@brief The type name of the value corresponding to the information indicated by
			an entry
		@par
			The type name of the value corresponding to an analysis result (e.g., execution time)
			etc.
			The following types (primitive types defined by TQL) are supported:
			- STRING
			- BOOL
			- BYTE
			- SHORT
			- INTEGER
			- LONG
			- FLOAT
			- DOUBLE

		@ENDL
	 */
	const GSChar *valueType;

	/*!
		@JP
		@brief このエントリが示す情報に対応付けられた値の文字列表現です。

		@EN
		@brief A string representation of the value corresponding to the information indicated by
			an entry.

		@ENDL
	 */
	const GSChar *value;

	/*!
		@JP
		@brief このエントリが示す情報に対応するTQL文の一部です。

		@EN
		@brief a part of a TQL statement corresponding to the information indicated by an entry.

		@ENDL
	 */
	const GSChar *statement;

} GSQueryAnalysisEntry;

/*!
	@JP
	@brief @ref GSQueryAnalysisEntry の初期化子です。

	@EN
	@brief Initializer of @ref GSQueryAnalysisEntry.

	@ENDL
 */
#define GS_QUERY_ANALYSIS_ENTRY_INITIALIZER \
	{ 0, 0, NULL, NULL, NULL, NULL }

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@brief 複数のコンテナの複数のロウを一括して操作する場合に用いる、
		コンテナ別のロウ内容エントリです。
	@since 1.5

	@EN
	@brief The Row contents entry by a container used when operating collectively
		a plurality of Rows of a plurality of containers.
	@since 1.5

	@ENDL
 */
typedef struct GSContainerRowEntryTag {

	/*!
		@JP
		@brief コンテナ名です。

		@EN
		@brief Name of container.

		@ENDL
	 */
	const GSChar *containerName;

	/*!
		@JP
		@brief ロウオブジェクトへのアドレスのリストです。
		@par
			現バージョンでは、@ref GSRow のアドレスのみを要素として含めることができます。

		@EN
		@brief The list to addresses to Row objects.
		@par
			Possible to include only @ref GSRow addresses as an element in the current version.

		@ENDL
	 */
	void *const *rowList;

#if GS_COMPATIBILITY_MULTIPLE_CONTAINERS_1_1_106
	size_t rowListSize;
#else
	/*!
		@JP
		@brief ロウオブジェクトの個数です。

		@EN
		@brief Number of Row objects.

		@ENDL
	 */
	size_t rowCount;
#endif

} GSContainerRowEntry;

/*!
	@JP
	@brief @ref GSContainerRowEntry の初期化子です。
	@since 1.5

	@EN
	@brief Initializer of @ref GSContainerRowEntry.
	@since 1.5

	@ENDL
 */
#define GS_CONTAINER_ROW_ENTRY_INITIALIZER \
	{ NULL, NULL, 0 }

/*!
	@JP
	@brief 複数のコンテナに対する取得条件を表すための、コンテナ別の
		合致条件エントリです。
	@since 1.5

	@EN
	@brief The specified condition entry by a container for representing the acquisition
		conditions for a plurality of containers.
	@since 1.5

	@ENDL
 */
typedef struct GSRowKeyPredicateEntryTag {

	/*!
		@JP
		@brief コンテナ名です。

		@EN
		@brief Name of container.

		@ENDL
	 */
	const GSChar *containerName;

	/*!
		@JP
		@brief コンテナのロウキーについての合致条件です。

		@EN
		@brief The specified condition for Row key of container.

		@ENDL
	 */
	GSRowKeyPredicate *predicate;

} GSRowKeyPredicateEntry;

/*!
	@JP
	@brief @ref GSRowKeyPredicateEntry の初期化子です。
	@since 1.5

	@EN
	@brief Initializer of @ref GSRowKeyPredicateEntry.
	@since 1.5

	@ENDL
 */
#define GS_ROW_KEY_PREDICATE_ENTRY_INITIALIZER \
	{ NULL, NULL }

/*!
	@JP
	@brief ロウフィールドに格納できるいずれかの型の値です。
	@since 1.5

	@EN
	@brief One of the type of value that can be stored in the Row field.
	@since 1.5

	@ENDL
 */
typedef union GSValueTag {

	/*!
		@JP
		@brief STRING型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of STRING type.

		@ENDL
	 */
	const GSChar *asString;

	/*!
		@JP
		@brief BOOL型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of BOOL type.

		@ENDL
	 */
	GSBool asBool;

	/*!
		@JP
		@brief BYTE型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of BYTE type.

		@ENDL
	 */
	int8_t asByte;

	/*!
		@JP
		@brief SHORT型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of SHORT type.

		@ENDL
	 */
	int16_t asShort;

	/*!
		@JP
		@brief INTEGER型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of INTEGER type.

		@ENDL
	 */
	int32_t asInteger;

	/*!
		@JP
		@brief LONG型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of LONG type.

		@ENDL
	 */
	int64_t asLong;

	/*!
		@JP
		@brief FLOAT型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of FLOAT type.

		@ENDL
	 */
	float asFloat;

	/*!
		@JP
		@brief DOUBLE型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of DOUBLE type.

		@ENDL
	 */
	double asDouble;

	/*!
		@JP
		@brief TIMESTAMP型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of TIMESTAMP type.

		@ENDL
	 */
	GSTimestamp asTimestamp;

	/*!
		@JP
		@brief GEOMETRY型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of GEOMETRY type.

		@ENDL
	 */
	const GSChar *asGeometry;

	/*!
		@JP
		@brief BLOB型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to a Row field of BLOB type.

		@ENDL
	 */
	GSBlob asBlob;

#if GS_COMPATIBILITY_VALUE_1_1_106

	struct {
		size_t size;
		const GSChar *const *elements;
	} asStringArray;

	struct {
		size_t size;
		const GSBool *elements;
	} asBoolArray;

	struct {
		size_t size;
		const int8_t *elements;
	} asByteArray;

	struct {
		size_t size;
		const int16_t *elements;
	} asShortArray;

	struct {
		size_t size;
		const int32_t *elements;
	} asIntegerArray;

	struct {
		size_t size;
		const int64_t *elements;
	} asLongArray;

	struct {
		size_t size;
		const float *elements;
	} asFloatArray;

	struct {
		size_t size;
		const double *elements;
	} asDoubleArray;

	struct {
		size_t size;
		const GSTimestamp *elements;
	} asTimestampArray;

#else

	struct {

		/*!
			@JP
			@brief 配列の要素数です。

			@EN
			@brief The number of elements in an array.

			@ENDL
		 */
		size_t length;

		union {

			/*!
				@JP
				@brief 配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in an array.

				@ENDL
			 */
			const void *data;

			/*!
				@JP
				@brief STRING型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in STRING type array.

				@ENDL
			 */
			const GSChar *const *asString;

			/*!
				@JP
				@brief BOOL型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in BOOL type array.

				@ENDL
			 */
			const GSBool *asBool;

			/*!
				@JP
				@brief BYTE型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in BYTE type array.

				@ENDL
			 */
			const int8_t *asByte;

			/*!
				@JP
				@brief SHORT型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in SHORT type array.

				@ENDL
			 */
			const int16_t *asShort;

			/*!
				@JP
				@brief INTEGER型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in INTEGER type array.

				@ENDL
			 */
			const int32_t *asInteger;

			/*!
				@JP
				@brief LONG型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in LONG type array.

				@ENDL
			 */
			const int64_t *asLong;

			/*!
				@JP
				@brief FLOAT型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in FLOAT type array.

				@ENDL
			 */
			const float *asFloat;

			/*!
				@JP
				@brief DOUBLE型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in DOUBLE type array.

				@ENDL
			 */
			const double *asDouble;

			/*!
				@JP
				@brief TIMESTAMP型配列の要素列へのアドレスです。

				@EN
				@brief The address of the element column in TIMESTAMP type
					array.

				@ENDL
			 */
			const GSTimestamp *asTimestamp;

		}
		/*!
			@JP
			@brief 配列の要素です。

			@EN
			@brief The element in an array.

			@ENDL
		 */
		elements;

	}
	/*!
		@JP
		@brief 配列型のロウフィールドに対応する値です。

		@EN
		@brief The value corresponding to the Row field of the array type.

		@ENDL
	 */
	asArray;

#endif

} GSValue;

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSTimestamp
	@brief TIMESTAMP型値の文字列表現を格納するための文字列バッファにおける、
		終端文字を含むバイト単位での最大サイズです。
	@see gsFormatTime

	@EN
	@ingroup Group_GSTimestamp
	@brief The maximum size of string buffer in bytes, including the termination
		character to store a string representation of the TIMESTAMP-type value.
	@see gsFormatTime

	@ENDL
 */
#define GS_TIME_STRING_SIZE_MAX 32

//
// GridStoreFactory API
//

/*!
	@JP
	@ingroup Group_GSGridStoreFactory
	@brief 必要に応じ、指定の@ref GSGridStoreFactory に関連するリソースを
		クローズします。
	@note
		現バージョンでは、何もクローズ処理を行いません。
	@param [in,out] factory
		対象とする@ref GSGridStoreFactory インスタンスのポインタ変数へのポインタ値
	@param [in] allRelated
		現バージョンでは、結果に影響しません。

	@EN
	@ingroup Group_GSGridStoreFactory
	@brief Closes the resources which are related to specified
		@ref GSGridStoreFactory as needed.
	@note
		In the current version, there is nothing to do in close processing.
	@param [in,out] factory
		The pointer to a pointer variable that refers to @ref GSGridStoreFactory
		instance to be closed.
	@param [in] allRelated
		In the current version, this parameter does not affect the result

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseFactory(
		GSGridStoreFactory **factory, GSBool allRelated);

/*!
	@JP
	@ingroup Group_GSGridStoreFactory
	@brief デフォルトの@ref GSGridStoreFactory インスタンスを取得します。
	@return @ref GSGridStoreFactory インスタンスへのポインタ値

	@EN
	@ingroup Group_GSGridStoreFactory
	@brief Returns a default @ref GSGridStoreFactory instance.
	@return The pointer to a @ref GSGridStoreFactory instance

	@ENDL
 */
GS_DLL_PUBLIC GSGridStoreFactory* GS_API_CALL gsGetDefaultFactory();

/*!
	@JP
	@ingroup Group_GSGridStoreFactory
	@brief 指定のプロパティを持つ@ref GSGridStore を取得します。
	@par
		@ref GSGridStore を取得した時点では、各@ref GSContainer を
		管理するマスタノード(以下、マスタ)のアドレス探索を必要に応じて行うだけであり、
		認証処理は行われません。
		実際に各@ref GSContainer に対応するノードに接続する必要が生じた
		タイミングで、認証処理が行われます。
	@par
		以下のプロパティを指定できます。サポート外の名称のプロパティは無視されます。
		<table>
		<tr><th>名称</th><th>説明</th></tr>
		<tr>
		<td>host</td>
		<td>接続先ホスト名。IPアドレス(IPV4のみ)も可。
		マスタを手動指定する場合は必須。マスタを自動検出する場合は設定しない。
		notificationMemberおよびnotificationProviderと同時に指定することはできない</td>
		</tr>
		<tr>
		<td>port</td>
		<td>接続先ポート番号。@c 0 から@c 65535 までの数値の文字列表現。
		マスタを手動指定する場合は必須。マスタを自動検出する場合は設定しない</td>
		</tr>
		<tr><td>notificationAddress</td>
		<td>マスタ自動検出に用いられる通知情報を受信するためのIPアドレス(IPV4のみ)。
		省略時はデフォルトのアドレスを使用</td>
		</tr>
		<tr>
		<td>notificationPort</td>
		<td>マスタ自動検出に用いられる通知情報を受信するためのポート番号。
		@c 0 から@c 65535 までの数値の文字列表現。
		省略時はデフォルトのポートを使用</td>
		</tr>
		<tr>
		<td>clusterName</td>
		<td>クラスタ名。接続先のクラスタに設定されているクラスタ名と一致するか
		どうかを確認するために使用される。省略時もしくは空文字列を
		指定した場合、クラスタ名の確認は行われない。</td>
		</tr>
		<tr>
		<td>database</td>
		<td>接続先のデータベース名。省略時は全てのユーザがアクセス可能な「public」
		データベースに自動接続される。接続ユーザは接続データベースに属するコンテナを
		操作できる。</td>
		</tr>
		<tr>
		<td>user</td>
		<td>ユーザ名</td>
		</tr>
		<tr>
		<td>password</td>
		<td>ユーザ認証用のパスワード</td>
		</tr>
		<tr>
		<td>consistency</td>
		<td>以下のいずれかの一貫性レベル。デフォルトでは@c IMMEDIATE を適用
		- @c IMMEDIATE
			- 他のクライアントからの更新結果は、該当トランザクションの完了後
				即座に反映される
		- @c EVENTUAL
			- 他のクライアントからの更新結果は、該当トランザクションが完了した後
				でも反映されない場合がある。
				@ref GSContainer に対する更新操作は実行できない
		</td>
		</tr>
		<tr>
		<td>transactionTimeout</td>
		<td>トランザクションタイムアウト時間の最低値。
		関係する@ref GSContainer における各トランザクションの開始時点から適用。
		@c 0 からINTEGER型の最大値までの値の文字列表現であり、単位は秒。
		ただし、タイムアウト時間として有効に機能する範囲に上限があり、
		上限を超える指定は上限値が指定されたものとみなされる。
		@c 0 の場合、後続のトランザクション処理がタイムアウトエラーになるかどうかは
		常に不定となる。省略時は接続先GridDB上のデフォルト値を使用</td>
		</tr>
		<tr>
		<td>failoverTimeout</td>
		<td>フェイルオーバ処理にて新たな接続先が見つかるまで待機する時間の最低値。
		@c 0 からINTEGER型の最大値までの数値の文字列表現であり、単位は秒。
		@c 0 の場合、フェイルオーバ処理を行わない。省略時は指定のファクトリの
		設定値を使用</td>
		</tr>
		<tr>
		<td>containerCacheSize</td>
		<td>コンテナキャッシュに格納するコンテナ情報の最大個数。
		@c 0 からINTEGER型の最大値までの数値の文字列表現。
		値が@c 0 の場合、コンテナキャッシュを使用しないことを意味する。
		@ref GSContainer を取得する際にキャッシュにヒットした場合は、
		GridDBへのコンテナ情報の問い合わせを行わない。
		省略時は既存の設定値を使用。バージョン1.5よりサポート</td>
		</tr>
		<tr>
		<td>notificationMember</td>
		<td>固定リスト方式を使用して構成されたクラスタに接続する場合に、
		クラスタノードのアドレス・ポートのリストを次のように指定する。
		<pre>(アドレス1):(ポート1),(アドレス2):(ポート2),...</pre>
		notificationAddressおよびnotificationProviderと同時に指定する
		ことはできない。バージョン2.9よりサポート</td>
		</tr>
		<tr>
		<td>notificationProvider</td>
		<td>プロバイダ方式を使用して構成されたクラスタに接続する場合に、
		アドレスプロバイダのURLを指定する。
		notificationAddressおよびnotificationMemberと同時に指定する
		ことはできない。バージョン2.9よりサポート</td>
		</tr>
		<tr>
		<td>applicationName</td>
		<td>アプリケーションの名前。アプリケーションの識別を補助するための
		情報として、接続先のクラスタ上での各種管理情報の出力の際に
		含められる場合がある。ただし、アプリケーションの同一性をどのように
		定義するかについては関与しない。省略時はアプリケーション名の
		指定がなかったものとみなされる。空文字列は指定できない。
		バージョン4.2よりサポート</td>
		</tr>
		</table>
	@par
		クラスタ名、データベース名、ユーザ名、パスワードについては、
		ASCIIの大文字・小文字表記の違いがいずれも区別されます。その他、
		これらの定義に使用できる文字種や長さの上限などの制限については、
		GridDBテクニカルリファレンスを参照してください。ただし、制限に反する
		文字列をプロパティ値として指定した場合、各ノードへの接続のタイミングまで
		エラーが検知されないことや、認証情報の不一致など別のエラーになる
		ことがあります。
	@par
		取得のたびに、新たな@ref GSGridStore インスタンスが生成されます。
		異なる@ref GSGridStore インスタンスならびに関連するリソースに対する操作は、
		スレッド安全です。すなわち、ある2つのリソースがそれぞれ@ref GSGridStore
		インスタンスを基にして生成されたものまたは@ref GSGridStore インスタンスそのものであり、
		かつ、該当する関連@ref GSGridStore インスタンスが異なる場合、一方のリソースに
		対してどのスレッドからどのタイミングで関連する関数が呼び出されていたとしても、
		他方のリソースの関連する関数を呼び出すことができます。
		ただし、@ref GSGridStore 自体のスレッド安全性は保証されていないため、
		同一@ref GSGridStore インスタンスに対して複数スレッドから任意のタイミングで
		関連する関数を呼び出すことはできません。
	@param [in] factory
		取得元の@ref GSGridStoreFactory インスタンス。@c NULL の場合、
		@ref gsGetDefaultFactory により得られるインスタンスと同一のものが使用されます。
	@param [in] properties
		取得設定を指示するためのプロパティ。
		@ref GSPropertyEntry の配列により構成されます。
		エントリ数が@c 0 の場合、この配列を参照することはなく、
		@c NULL を指定することもできます。
		エントリを構成する名前もしくは値に@c NULL を含めることはできません。
	@param [in] propertyCount
		@c properties として引数に指定するプロパティのエントリ数。
	@param [out] store
		@ref GSGridStore インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のホストについて名前解決できなかった場合
		- 指定のプロパティが上で説明した形式・制限に合致しないことを検知できた
			場合
		- @c store に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStoreFactory
	@brief Returns a @ref GSGridStore with the specified properties.
	@par
		When obtaining @ref GSGridStore, it just searches for the name of a
		master node (hereafter, a master) administering each @ref GSContainer
		as necessary, but authentication is not performed. When a client really
		needs to connect to a node corresponding to each @ref GSContainer,
		authentication is performed.
	@par
		The following properties can be specified. Unsupported property names
		are ignored.
		<table>
		<tr><th>Property</th><th>Description</th></tr>
		<tr>
		<td>host</td>
		<td>A destination host name.
		An IP address (IPV4 only) is also available.
		Mandatory for manually setting a master.
		For autodetection of a master, omit the setting.
		This property cannot be specified with neither notificationMember nor
		notificationProvider properties at the same time.</td>
		</tr>
		<tr>
		<td>port</td>
		<td>A destination port number.
		A string representing of a number from @c 0 to @c 65535.
		Mandatory for manually setting a master.
		For autodetection of a master, omit the setting.</td>
		</tr>
		<tr><td>notificationAddress</td>
		<td>An IP address (IPV4 only) for receiving a notification used for
		autodetection of a master.
		A default address is used if omitted.</td>
		</tr>
		<tr>
		<td>notificationPort</td>
		<td>A port number for receiving a notification used for autodetection
		of a master. A string representing of a number from @c 0 to @c 65535.
		A default port number is used if omitted.</td>
		</tr>
		<tr>
		<td>clusterName</td>
		<td>A cluster name. It is used to verify whether it matches the cluster
		name assigned to the destination cluster. If it is omitted or an empty
		string is specified, cluster name verification is not performed.</td>
		</tr>
		<tr>
		<td>database</td>
		<td>A database name to be connected. If omitted, it is automatically
		connected to "public" database which can be accessed by all users.
		The connected user can operate the Container belonging to the connected
		database.</td>
		</tr>
		<tr>
		<td>user</td>
		<td>A user name.</td>
		</tr>
		<tr>
		<td>password</td>
		<td>A password for user authentication.</td>
		</tr>
		<tr>
		<td>consistency</td>
		<td>Either one of the following consistency levels.
		By default, @c IMMEDIATE is selected.
		- @c IMMEDIATE
			- The updates by other clients are committed immediately after a
				relevant transaction completes.
		- @c EVENTUAL
			- The updates by other clients may not be committed even after a
			relevant transaction completes.
			No update operation cannot be applied to @ref GSContainer.
		</td>
		</tr>
		<tr>
		<td>transactionTimeout</td>
		<td>The minimum value of transaction timeout time.
		The transaction timeout is counted from the beginning of each
		transaction in a relevant @ref GSContainer.
		A string representing of a number from @c 0 to maximum value of
		INTEGER-type in seconds.
		If a value specified over the internal upper limit of timeout, timeout
		will occur at the internal upper limit value.
		The value @c 0 indicates that it is always uncertain whether a timeout
		error will occur during a subsequent transaction. If omitted, the
		default value used by a destination GridDB is applied.</td>
		</tr>
		<tr>
		<td>failoverTimeout</td>
		<td>The minimum value of waiting time until a new destination is found
		in a failover. A numeric string representing from @c 0 to maximum value
		of INTEGER-type in seconds.
		The value @c 0 indicates that no failover is performed. If omitted, the
		default value used by the specified Factory is applied.</td>
		</tr>
		<tr>
		<td>containerCacheSize</td>
		<td>The maximum number of Container information on the Container cache.
		A string representing of a number from @c 0 to maximum value of
		INTEGER-type. The Container cache is not used if the value is @c 0.
		To obtain a @ref GSContainer, its Container information might be
		obtained from the Container cache instead of request to GridDB.
		A default number is used if omitted.
		This property is supported on version 1.5 or later.</td>
		</tr>
		<tr>
		<td>notificationMember</td>
		<td>A list of address and port pairs in cluster. It is used to connect
		to cluster which is configured with FIXED_LIST mode, and specified as
		follows.
		<pre>(Address1):(Port1),(Address2):(Port2),...</pre>
		This property cannot be specified with neither notificationAddress nor
		notificationProvider properties at the same time.
		This property is supported on version 2.9 or later.</td>
		</tr>
		<tr>
		<td>notificationProvider</td>
		<td>A URL of address provider. It is used to connect to cluster which is
		configured with PROVIDER mode.
		This property cannot be specified with neither notificationAddress nor
		notificationMember properties at the same time.
		This property is supported on version 2.9 or later.</td>
		</tr>
		<tr>
		<td>applicationName</td>
		<td>Name of an application. It may be contained in various information
		for management on the connected cluster. However, the cluster shall
		not be involved with the identity of applications. If the property is
		omitted, it is regarded that the name is not specified. Empty string
		cannot be specified. This property is supported on version 4.2 or
		later.</td>
		</tr>
		</table>
	@par
		Cluster names, database names, user names and passwords
		are case-sensitive. See the GridDB Technical Reference for
		the details of the limitations, such as allowed characters
		and maximum length. When a name violating
		the limitations has been specified as a property
		value, the error detection may not be delayed until
		the authentication processing. And there are the cases that
		the error is identified as an authentication error,
		etc., not a violation error for the limitations.
	@par
		A new @ref GSGridStore instance is created by each call of this method.
		Operations on different @ref GSGridStore instances and related resources
		are thread safe. That is, if some two resources are each created based
		on @ref GSGridStore instances or they are just @ref GSGridStore
		instances, and if they are related to different @ref GSGridStore
		instances respectively, any function related to one resource can be
		called, no matter when a function related to the other resource may be
		called from any thread.
		However, since thread safety is not guaranteed for @ref GSGridStore
		itself, it is not allowed to call a method of a single @ref GSGridStore
		instance from two or more threads at an arbitrary time.
	@param [in] factory
		@ref GSGridStoreFactory instance of acquisition source.
		In case of @c NULL, the same instance obtained by calling
		@ref gsGetDefaultFactory will be used.
	@param [in] properties
		Properties specifying the settings for the object to be obtained.
		This parameter is composed by the array of @ref GSPropertyEntry.
		If the number of entries is @c 0, it is possible to specify @c NULL
		since the array is not accessed in this function.
		It is prohibited to include @c NULL in the name or value which
		configures the entry.
	@param [in] propertyCount
		The number of entries specified @c properties argument.
	@param [out] store
		The pointer to a pointer variable to store @ref GSGridStore instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if host name resolution fails
		- if any specified property that does not match the format or
			the limitations shown above is detected.
		- if @c NULL is specified to @c store argument

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetGridStore(
		GSGridStoreFactory *factory,
		const GSPropertyEntry *properties, size_t propertyCount,
		GSGridStore **store);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsCompatibleFunc_GetGridStore1(
		GSGridStoreFactory *factory, const GSPropertyEntry *properties,
		GSGridStore **store));
#endif

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_COMPATIBILITY_FACTORY_BETA_0_3
#define gsGetGridStore(factory, properties, store) \
		gsCompatibleFunc_GetGridStore1(factory, properties, store)
#else
#define gsGetGridStore(factory, properties, propertyCount, store) \
		gsGetGridStore(factory, properties, propertyCount, store)
#endif
#endif	// not GS_INTERNAL_DEFINITION_VISIBLE

/*!
	@JP
	@ingroup Group_GSGridStoreFactory
	@brief 指定のファクトリの設定を変更します。
	@par
		設定の変更は、指定のファクトリより生成された@ref GSGridStore 、
		ならびに、今後指定のファクトリで生成される@ref GSGridStore に反映されます。
	@par
		以下のプロパティを指定できます。サポート外の名称のプロパティは無視されます。
		<table>
		<tr><th>名称</th><th>説明</th></tr>
		<tr>
		<td>maxConnectionPoolSize</td>
		<td>内部で使用されるコネクションプールの最大コネクション数。
		@c 0 からINTEGER型の最大値までの数値の文字列表現。
		値が@c 0 の場合、コネクションプールを使用しないことを意味する。
		省略時は既存の設定値を使用</td>
		</tr>
		<tr>
		<td>failoverTimeout</td>
		<td>フェイルオーバ処理にて新たな接続先が見つかるまで待機する時間の最低値。
		@c 0 からINTEGER型の最大値までの数値の文字列表現であり、単位は秒。
		@c 0 の場合、フェイルオーバ処理を行わない。省略時は既存の設定値を使用</td>
		</tr>
		</table>
	@param [in] factory
		取得元の@ref GSGridStoreFactory インスタンス。@c NULL の場合、
		@ref gsGetDefaultFactory により得られるインスタンスと同一のものが使用されます。
	@param [in] properties
		取得設定を指示するためのプロパティ。
		@ref GSPropertyEntry の配列により構成されます。
		エントリ数が@c 0 の場合、@c NULL を指定することもできます。
		エントリを構成する名前もしくは値に@c NULL を含めることはできません。
	@param [in] propertyCount
		@c properties として引数に指定するプロパティのエントリ数。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のプロパティが上で説明した形式に合致しない場合
		- @c properties に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStoreFactory
	@brief Changes the settings for specified Factory.
	@par
		The changed settings will be reflected in @ref GSGridStore which is
		already created by the specified Factory and @ref GSGridStore which
		will be created later by the specified Factory.
	@par
		The following properties can be specified. Unsupported property names
		are ignored.
		<table>
		<tr><th>Property</th><th>Description</th></tr>
		<tr>
		<td>maxConnectionPoolSize</td>
		<td>The maximum number of connections in the connection pool used
		inside.
		A numeric string representing from @c 0 to maximum value of
		INTEGER-type.
		The value @c 0 indicates no use of the connection pool.
		If omitted, the default value is used.</td>
		</tr>
		<tr>
		<td>failoverTimeout</td>
		<td>The minimum value of waiting time until a new destination is found
		in a failover.
		A numeric string representing from @c 0 to maximum value of
		INTEGER-type in seconds.
		The value 0 indicates that no failover is performed.
		If omitted, the default value is used.</td>
		</tr>
		</table>
	@param [in] factory
		@ref GSGridStoreFactory instance of acquisition source.
		In case of @c NULL, the same instance obtained by calling
		@ref gsGetDefaultFactory will be used.
	@param [in] properties
		Properties specifying the settings for the object to be obtained.
		This parameter is composed by the array of @ref GSPropertyEntry.
		If the number of entries is @c 0, it is possible to specify @c NULL.
		It is prohibited to include @c NULL in the name or value which
		configures the entry.
	@param [in] propertyCount
		The number of entries specified @c properties argument.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if any specified property does not match the format shown above
		- if @c NULL is specified to @c properties argument

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetFactoryProperties(
		GSGridStoreFactory *factory,
		const GSPropertyEntry *properties, size_t propertyCount);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsCompatibleFunc_SetFactoryProperties1(
		GSGridStoreFactory *factory, const GSPropertyEntry *properties));
#endif

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_COMPATIBILITY_FACTORY_BETA_0_3
#define gsSetFactoryProperties(factory, propertyCount) \
		gsCompatibleFunc_SetFactoryProperties1(factory, propertyCount)
#else
#define gsSetFactoryProperties(factory, properties, propertyCount) \
		gsSetFactoryProperties(factory, properties, propertyCount)
#endif
#endif	// not GS_INTERNAL_DEFINITION_VISIBLE

//
// GridStore API
//

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の@ref GSGridStore インスタンスについて、対応するGridDBクラスタとの
		接続状態を解除し、必要に応じて指定のインスタンスならびに関連するリソースを解放します。
	@param [in,out] store
		クローズ対象の@ref GSGridStore インスタンスを指しているポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSGridStore インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。
	@param [in] allRelated
		指定の@ref GSGridStore と関連する下位のリソースのうち、未クローズのものすべてを
		クローズするかどうか。
		関連する下位のリソースとは、指定の@ref GSGridStore を介して取得した
		@ref GSCollection 、@ref GSTimeSeries 、ならびに、これらのリソースと関連する
		下位のリソースのことを指します。
		@ref GS_FALSE を指定した場合、指定の@ref GSGridStore を介して取得した
		リソースを個別にクローズする必要があり、すべてクローズした時点で
		指定の@ref GSGridStore 自体のリソースが解放されます。

	@EN
	@ingroup Group_GSGridStore
	@brief Closes the connection between the specified @ref GSGridStore
		instance and its corresponding GridDB cluster, and releases the
		specified instance and related resources as necessary.
	@param [in,out] store
		the pointer to a pointer variable that refers to @ref GSGridStore
		instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSGridStore instance which was
		already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.
	@param [in] allRelated
		Indicates whether all unclosed resources in the lower resources related
		to the specified @ref GSGridStore will be closed or not.
		The related lower resources point out @ref GSCollection,
		@ref GSTimeSeries, and its resources which are obtained via specified
		@ref GSGridStore.
		If @ref GS_FALSE is specified, it is necessary to individually close
		the acquired resources through @ref GSGridStore. And the specified
		@ref GSGridStore itself will be released when all resources are closed.

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseGridStore(
		GSGridStore **store, GSBool allRelated);

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の名前を持つコレクションを削除します。
	@par
		削除済みの場合の扱い、トランザクションの扱い、削除要求完了直後の
		状態に関しては、@ref gsDropContainer と同様です。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコレクションの名前
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 種別の異なるコンテナを削除しようとした場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Deletes a Collection with the specified name.
	@par
		The treatment of deleted cases, transaction handling, and the state
		immediately after completion of deletion request are the same as
		@ref gsDropContainer.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Collection name to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the Container type is unmatched
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDropCollection(
		GSGridStore *store, const GSChar *name);

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の名前を持つ時系列を削除します。
	@par
		削除済みの場合は何も変更しません。
	@par
		削除済みの場合の扱い、トランザクションの扱い、削除要求完了直後の
		状態に関しては、@ref gsDropContainer と同様です。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象の時系列の名前
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 種別の異なるコンテナを削除しようとした場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Deletes a TimeSeries with the specified name.
	@par
		If the specified TimeSeries is already deleted, nothing is changed.
	@par
		The treatment of deleted cases, transaction handling, and the state
		immediately after completion of deletion request are the same as
		@ref gsDropContainer.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		TimeSeries name to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the Container type is unmatched
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDropTimeSeries(
		GSGridStore *store, const GSChar *name);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(GSResult GS_API_CALL gsGetRowByPath(
		GSGridStore *store, const GSChar *pathKey, void *rowObj,
		GSBool *exists));
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の名前のコレクションを操作するための@ref GSCollection インスタンスを
		取得します。
	@par
		指定の型とカラムレイアウトとの対応関係については、@ref GSContainer の定義を
		参照してください。
	@param [in] store
		処理対象のコレクションが格納されている@ref GSGridStore
	@param [in] name
		処理対象のコレクションの名前
	@param [in] binding
		ユーザ定義構造体とカラムレイアウトとのバインディング情報
	@param [out] collection
		@ref GSCollection インスタンスを格納するためのポインタ変数へのポインタ値。
		指定の名前のコレクションが存在しない場合、@c NULL が設定されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 同名の時系列が存在する場合
		- 指定の型と既存のカラムレイアウトが一致しない場合
		- 指定の型がロウオブジェクトの型として適切でない場合。
			詳しくは@ref GSContainer の定義を参照してください。
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Return a @ref GSCollection instance to manipulate a Collection with
		the specified name.
	@par
		For the correspondence between a specified type and a Column layout,
		see the description of Container.
	@param [in] store
		@ref GSGridStore which contains the Collection to be processed
	@param [in] name
		Collection name to be processed
	@param [in] binding
		The binding information between the user-defined structure and the
		column layout
	@param [out] collection
		The pointer to a pointer variable to store @ref GSCollection instance.
		@c NULL is set to this pointer if there is no Collection which has the
		specified name.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a TimeSeries with the same name exists
		- if the specified type and the existing Column layout conflict each
			other
		- if the specified type is not proper as a type of a Row object.
			For more information, see the description of Container.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetCollection(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, GSCollection **collection);

#if GS_COMPATIBILITY_SUPPORT_3_5

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfoV3_3(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の名前のコンテナに関する情報を取得します。
	@par
		返却される@ref GSContainerInfo に含まれるコンテナ名は、GridDB上に
		格納されているものが設定されます。したがって、指定したコンテナ名と比較すると、
		ASCIIの大文字・小文字表記が異なる場合があります。
	@par
		カラム順序を無視するかどうかについては、無視しない状態に設定されます。
		この設定は、@ref GSContainerInfo::columnOrderIgnorable を通じて
		確認できます。
	@par
		現バージョンでは、初期値でのNULL使用有無は未設定状態で求まります。
		この設定は、@ref GSColumnInfo::options を通じて確認できます。
	@attention
		カラム情報の列などの可変長データを格納するために、指定の@ref GSGridStore
		インスタンス上で管理される一時的なメモリ領域を使用します。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] store
		処理対象のコンテナが格納されている@ref GSGridStore
	@param [in] name
		処理対象のコンテナの名前
	@param [out] info
		指定の名前のコンテナに関する情報を格納するための@ref GSContainerInfo
		へのポインタ値。指定の名前のコンテナが存在しない場合、もしくは、
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_CONTAINER_INFO_INITIALIZER
		と同一の内容の初期値が格納されます。
	@param [out] exists
		処理対象のコンテナが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理のタイムアウト、接続障害が発生した場合
		- @c exists 以外の引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Get information related to a Container with the specified name.
	@par
		A name stored in GridDB is set for the Container name to be included in
		a returned @ref GSContainerInfo. Therefore, compared to the specified
		Container name, the notation of the ASCII uppercase characters and
		lowercase characters may differ.
	@par
		The column sequence is set to Do Not Ignore. This setting can be
		verified through @ref GSContainerInfo::columnOrderIgnorable.
	@par
		In the current version, whether to use of NULL for the initial value
		is not set. Note that it may be set in the future version. This
		information can be acquired through @ref GSColumnInfo::options.
	@attention
		In order to store the variable-length data such as the column of column
		information, it uses a temporary memory area which is managed by the
		specified @ref GSGridStore instance.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] store
		@ref GSGridStore which contains the Container to be processed
	@param [in] name
		Container name to be processed
	@param [out] info
		The pointer value to @ref GSContainerInfo for storing information about
		the Container with the specified name.
		If the Container with the specified name does not exist, or if
		@ref GS_RESULT_OK is not returned as the execution result, the initial
		value which is same as @ref GS_CONTAINER_INFO_INITIALIZER is stored.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Container exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to arguments except @c exists

	@ENDL
 */
GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsGetContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return gsGetContainerInfoV3_3(store, name, info, exists);
}

#elif GS_COMPATIBILITY_SUPPORT_2_1

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfoV2_1(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsGetContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return gsGetContainerInfoV2_1(store, name, info, exists);
}

#elif GS_COMPATIBILITY_SUPPORT_1_5

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfoV1_5(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsGetContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists) {
	return gsGetContainerInfoV1_5(store, name, info, exists);
}

#else

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerInfo(
		GSGridStore *store, const GSChar *name, GSContainerInfo *info,
		GSBool *exists);

#endif	// not GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の名前の時系列を操作するための@ref GSTimeSeries インスタンスを
		取得します。
	@par
		指定の型とカラムレイアウトとの対応関係については、@ref GSContainer の定義を
		参照してください。
	@param [in] store
		処理対象の時系列が格納されている@ref GSGridStore
	@param [in] name
		処理対象の時系列の名前
	@param [in] binding
		ユーザ定義構造体とカラムレイアウトとのバインディング情報
	@param [out] timeSeries
		@ref GSTimeSeries インスタンスを格納するためのポインタ変数へのポインタ値。
		指定の名前の時系列が存在しない場合、@c NULL が設定されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 同名のコレクションが存在する場合
		- 指定の型と既存のカラムレイアウトが一致しない場合
		- 指定の型がロウオブジェクトの型として適切でない場合。
			詳しくは@ref GSContainer の定義を参照してください。
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Returns a @ref GSTimeSeries instance to manipulate a TimeSeries with
		the specified name.
	@par
		For the correspondence between a specified type and a Column layout,
		see the description of Container.
	@param [in] store
		@ref GSGridStore which contains the TimeSeries to be processed
	@param [in] name
		TimeSeries name to be processed
	@param [in] binding
		The binding information between the user-defined structure and the
		column layout
	@param [out] timeSeries
		The pointer to a pointer variable to store @ref GSTimeSeries instance.
		@c NULL is set to this pointer if there is no TimeSeries which has the
		specified name.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a Collection with the same name exists
		- if the specified type and the existing Column layout conflict each
			other
		- if the specified type is not proper as a type of a Row object.
			For more information, see the description of Container.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetTimeSeries(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, GSTimeSeries **timeSeries);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(GSResult GS_API_CALL gsPutRowByPath(
		GSGridStore *store, const GSChar *pathKey, const void *rowObj,
		GSBool *exists));
#endif

#if GS_COMPATIBILITY_SUPPORT_2_1
/*!
	@JP
	@ingroup Group_GSGridStore
	@brief バインディング情報と@ref GSContainerInfo を指定して、コンテナを
		新規作成または変更します。
	@par
		主に、バインディング情報を指定して、追加設定を持つコンテナを新規作成
		する場合に使用します。
	@par
		カラムレイアウトならびにカラム順序の無視設定を@ref GSContainerInfo に
		指定できない点を除けば、@ref gsPutContainerGeneral と同様です。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコンテナの名前
	@param [in] binding
		ユーザ定義構造体とカラムレイアウトとのバインディング情報
	@param [in] info
		処理対象のコンテナの情報。@c NULL の場合は無視される
	@param [in] modifiable
		既存コレクションのカラムレイアウト変更を許可するかどうか
	@param [out] container
		@ref GSContainer インスタンスを格納するためのポインタ変数へのポインタ値。
		@ref GS_CONTAINER_COLLECTION を指定した場合は@ref GSCollection 、
		@ref GS_CONTAINER_TIME_SERIES を指定した場合は@ref GSTimeSeries
		固有の機能が使用できます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c name ならびに@c info 引数の内容が規則に合致しない場合。
			また、指定のコンテナ種別に対応する コンテナ新規作成・変更関数の
			規則に合致しない場合
		- 指定の型がロウオブジェクトの型として適切でない場合。
			詳しくは@ref GSContainer の定義を参照してください。
		- この処理のタイムアウト、接続障害が発生した場合
		- @c name 以外のポインタ型引数に@c NULL が指定された場合
	@see gsPutCollection
	@see gsPutTimeSeries
	@see gsPutContainerGeneral

	@EN
	@ingroup Group_GSGridStore
	@brief Newly creates or update a Container with the specified binding
		information and @ref GSContainerInfo.
	@par
		Mainly used to create a new container with additional settings by
		specifying the binding information.
	@par
		The behavior will be the same as @ref gsPutContainerGeneral except for
		points such that the settings to ignore the column layout and column
		order cannot be specified to @ref GSContainerInfo.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Container name to be processed
	@param [in] binding
		The binding information between the user-defined structure and the
		column layout
	@param [in] info
		Container information to be processed. Ignored if @c NULL is specified
	@param [in] modifiable
		Indicates whether the column layout of the existing Collection can be
		modified or not
	@param [out] container
		The pointer to a pointer variable to store @ref GSContainer instance.
		@ref GSCollection specific function can be used if
		@ref GS_CONTAINER_COLLECTION is specified, or @ref GSTimeSeries
		specific function can be used if @ref GS_CONTAINER_TIME_SERIES is
		specified.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the contents of the arguments @c name and @c info do not conform to
			the rules. Also if the contents do not conform to the rules of the
			new Container creation and update function for the specified
			Container type
		- if the specified type is not proper as a type of a Row object.
			For more information, see the description of Container.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to pointer type arguments except @c name
	@see gsPutCollection
	@see gsPutTimeSeries
	@see gsPutContainerGeneral

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainer(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);
#endif	// GS_COMPATIBILITY_SUPPORT_2_1

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief コレクションを新規作成または変更します。
	@par
		同名のコンテナが存在しない場合、指定のバインディング情報により定義された
		カラムレイアウトに従い、新規にコレクションを作成します。
		すでに同名のコレクションが存在し、既存のカラムレイアウトの内容がすべて
		一致する場合、実行中のトランザクションを待機する点を除き
		@ref gsGetCollection と同様に振る舞います。
	@par
		@c modifiable が@ref GS_TRUE であり、すでに同名のコレクションが
		存在する場合、必要に応じカラムレイアウトを変更します。
		変更する際、要求したカラムと同一の名前・型の既存のカラムは保持されます。
		一致しないカラムのうち、既存のコレクションにない名前のカラムは追加し、要求側にない
		カラムはデータも含め削除します。型が異なる同名のカラムが存在する場合は
		失敗します。また、ロウキーに対応するカラムの追加と削除はできません。
	@par
		コンテナにトリガが設定されており、カラムレイアウト変更によってトリガが
		通知対象としているカラムが削除された場合、そのカラムはトリガの通知対象
		から削除されます。
	@par
		新たに追加されるカラムの値は、@ref GSContainer にて定義されている
		空の値を初期値として初期化されます。
	@par
		指定の型とカラムレイアウトとの対応関係については、@ref GSContainer の定義を
		参照してください。
	@par
		すでに同名のコレクションが存在し、かつ、該当するコレクションにおいて実行中の
		トランザクションが存在する場合、それらの終了を待機してから処理を行います。
	@par
		ロウキーを持つコレクションを新規に作成する場合、ロウキーに対し、
		@ref gsCreateIndex にて定義されているデフォルト種別の索引が作成されます。
		この索引は、削除することができます。
	@par
		現バージョンでは、コンテナの規模など諸条件を満たした場合、
		カラムレイアウトの変更開始から終了までの間に、処理対象のコンテナに対して
		コンテナ情報の参照、更新ロックなしでのロウの参照操作を行える場合が
		あります。それ以外の操作は、@ref GSContainer での定義通り待機させる
		場合があります。カラムレイアウトの変更途中に別の操作が行われる場合は、
		変更前のカラムレイアウトが使用されます。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコレクションの名前
	@param [in] binding
		ユーザ定義構造体とカラムレイアウトとのバインディング情報
	@param [in] properties
		コレクションの構成オプション。@c NULL を指定できます。
		現バージョンでは使用されておらず、内容はチェックされません。
	@param [in] modifiable
		既存コレクションのカラムレイアウト変更を許可するかどうか
	@param [out] collection
		@ref GSCollection インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 同名の時系列が存在する場合。
		- @c modifiable が@ref GS_FALSE であり、既存の同名のコレクションに関して
			カラムレイアウトの内容が一致しない場合
		- @c modifiable が@ref GS_TRUE であり、既存の同名のコレクションに関して
			変更できない項目を変更しようとした場合
		- 指定の型がロウオブジェクトの型として適切でない場合。
			詳しくは@ref GSContainer の定義を参照してください。
		- この処理のタイムアウト、接続障害が発生した場合
		- @c properties 以外のポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Newly creates or update a Collection.
	@par
		If a Container with the specified name does not exist, it newly creates
		a Collection based on the Column layout defined by the specified binding
		information. If a Container with the specified name already exists and
		its whole Column layout matches the specified type, it behaves in the
		same way as @ref gsGetCollection, except that it waits for active
		transactions to be completed.
	@par
		If @c modifiable is @ref GS_TRUE and a Collection with the specified
		name exists, it changes its layout as necessary. When changing the
		layout, it keeps the existing Columns which have the same name and type
		with requested Columns. If a Column name defined by the class is not
		found in the existing Collection, it creates a Column with the name; and
		it deletes other Columns in the existing Collection as well as their
		data. It fails if a Column with the same name but of a different type
		exists. It is not possible to create or delete a Column corresponding to
		a Row key.
	@par
		If a trigger is set in a Container, and if a column whose trigger is
		subject to notification is deleted due to a change in the column layout,
		the column will be deleted from the list of triggers subject to
		notification.
	@par
		The values of Columns to be newly created are initialized with
		the empty value defined by @ref GSContainer.
	@par
		For the correspondence between a specified type and a Column layout,
		see the description of Container.
	@par
		If a Collection with the specified name exists and if a transaction(s)
		is active in the Collection, it does the operation after waiting for the
		transaction completion.
	@par
		When creating a Collection with Row key, an index of default type of the
		Row key which is defined by @ref gsCreateIndex is created. The index is
		removable.
	@par
		In the current version, when various conditions such as the size of
		container are fulfilled, it is possible to reference container information
		to the container to be processed and reference operation of the row without
		updating the lock, from the start to the end of column layout change. Other
		than these operations, you may have to wait as defined in @ref GSContainer.
		If another operation is performed while changing the column layout, the column
		layout prior to the change will be used.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Collection name to be processed
	@param [in] binding
		The binding information between the user-defined structure and the
		column layout
	@param [in] properties
		The configuration options to Collection. @c NULL can be specified.
		This parameter is not used in current version, therefore the content
		is not checked.
		configuration options. @c You can specify a NULL.
	@param [in] modifiable
		Indicates whether the column layout of the existing Collection can be
		modified or not
	@param [out] collection
		The pointer to a pointer variable to store @ref GSCollection instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a TimeSeries with the same name exists
		- if @ref GS_FALSE is set to @c modifiable and if the column layout in
			existed Collection with the same name does not match
		- if @ref GS_TRUE is set to @c modifiable and if the unchangeable item
			in existed Collection with the same name is tried to be changed
		- if the specified type is not proper as a type of a Row object.
			For more information, see the description of Container.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to pointer type arguments except @c properties

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollection(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSCollectionProperties *properties,
		GSBool modifiable, GSCollection **collection);

#if GS_COMPATIBILITY_SUPPORT_2_0

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesV2_0(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries);
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 時系列を新規作成または変更します。
	@par
		同名のコンテナが存在しない場合、指定のバインディング情報により定義された
		カラムレイアウトに従い、新規に時系列を作成します。
		すでに同名の時系列が存在し、既存のカラムレイアウトの内容がすべて
		一致する場合、実行中のトランザクションを待機する点を除き
		@ref gsGetTimeSeries と同様に振る舞います。
	@par
		@c modifiable が@ref GS_TRUE であり、すでに同名の時系列が
		存在する場合、必要に応じカラムレイアウトを変更します。
		変更する際、要求したカラムと同一の名前・型の既存のカラムは保持されます。
		一致しないカラムのうち、既存の時系列にない名前のカラムは追加し、要求側にない
		カラムはデータも含め削除します。型が異なる同名のカラムが存在する場合は
		失敗します。また、ロウキーに対応するカラムの追加と削除、時系列構成オプションの
		変更はできません。時系列構成オプションを指定する場合は、既存の設定内容と
		すべて同値にする必要があります。
	@par
		コンテナにトリガが設定されており、カラムレイアウト変更によってトリガが
		通知対象としているカラムが削除された場合、そのカラムはトリガの通知対象
		から削除されます。
	@par
		新たに追加されるカラムの値は、@ref gsPutCollection の定義を参照してください。
	@par
		指定の型とカラムレイアウトとの対応関係については、@ref GSContainer の定義を
		参照してください。
	@par
		すでに同名の時系列が存在し、かつ、該当する時系列において実行中の
		トランザクションが存在する場合、それらの終了を待機してから処理を行います。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象の時系列の名前
	@param [in] binding
		ユーザ定義構造体とカラムレイアウトとのバインディング情報
	@param [in] properties
		時系列の構成オプション。@c NULL を指定すると、同名の時系列が存在する場合は
		既存の設定が継承され、存在しない場合は初期設定を指定したものとみなされます。
		初期設定とは、@ref GS_TIME_SERIES_PROPERTIES_INITIALIZER により
		初期化した時系列構成オプションと同値の設定のことです。
	@param [in] modifiable
		既存時系列のカラムレイアウト変更を許可するかどうか
	@param [out] timeSeries
		@ref GSTimeSeries インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 同名の時系列が存在する場合。
		- @c modifiable が@ref GS_FALSE であり、既存の同名の時系列に関して
			カラムレイアウトの内容が一致しない場合
		- @c modifiable が@ref GS_TRUE であり、既存の同名の時系列に関して
			変更できない項目を変更しようとした場合
		- 指定の型がロウオブジェクトの型として適切でない場合。
			詳しくは@ref GSContainer の定義を参照してください。
		- この処理のタイムアウト、接続障害が発生した場合
		- @c properties 以外のポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSGridStore
	@brief Newly creates or updates a TimeSeries.
	@par
		If a Container with the specified name does not exist, it newly creates
		a TimeSeries based on the Column layout defined by the specified binding
		information. If a TimeSeries with the specified name already exists and
		its whole Column layout matches the specified type, it behaves in the
		same way as @ref gsGetTimeSeries, except that it waits for active
		transactions to be completed.
	@par
		If @c modifiable is @ref GS_TRUE and a TimeSeries with the specified
		name exists, it changes its layout as necessary. When changing the
		layout, it keeps the existing Columns which have the same name and type
		with requested Columns. If a Column name defined by the class is not
		found in the existing TimeSeries, it creates a Column with the name; and
		it deletes other Columns in the existing TimeSeries as well as their
		data. It fails if a Column with the same name but of a different type
		exists. It is not possible to create or delete a Column corresponding to
		a Row key or change the options for configuring a TimeSeries. When
		specifying some options for configuring a TimeSeries, specified values
		must be the same as the current settings.
	@par
		If a trigger is set in a Container, and if a column whose trigger is
		subject to notification is deleted due to a change in the column layout,
		the column will be deleted from the list of triggers subject to
		notification.
	@par
		For the initial values for newly created Columns, see the description of
		@ref gsPutCollection.
	@par
		For the correspondence between a specified type and a Column layout,
		see the description of Container.
	@par
		If a TimeSeries with the specified name exists and if a transaction(s)
		is active in the TimeSeries, it does the operation after waiting for the
		transaction completion.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		TimeSeries name to be processed
	@param [in] binding
		The binding information between the user-defined structure and the
		column layout
	@param [in] properties
		The configuration options to TimeSeries. If @c NULL is specified and
		the TimeSeries with same name exists, its existing configuration is
		inherited. If @c NULL is specified and the TimeSeries with same name
		does not exist, it is assumed that the initial configuration is
		specified.
		The initial configuration has the same values with TimeSeries
		configuration initialized by
		@ref GS_TIME_SERIES_PROPERTIES_INITIALIZER.
	@param [in] modifiable
		Indicates whether the column layout of the existing TimeSeries can be
		modified or not
	@param [out] timeSeries
		The pointer to a pointer variable to store @ref GSTimeSeries instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a TimeSeries with the same name exists
		- if @ref GS_FALSE is set to @c modifiable and if the column layout in
			existed TimeSeries with the same name does not match
		- if @ref GS_TRUE is set to @c modifiable and if the unchangeable item
			in existed TimeSeries with the same name is tried to be changed
		- if the specified type is not proper as a type of a Row object.
			For more information, see the description of Container.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to pointer type arguments except @c properties

	@ENDL
 */
GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutTimeSeries(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	return gsPutTimeSeriesV2_0(
			store, name, binding, properties, modifiable, timeSeries);
}

#elif !GS_COMPATIBILITY_TIME_SERIES_PROPERTIES_0_0_10

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeries(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries);

#else

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsCompatibleFunc_PutTimeSeries1(
		GSGridStore *store, const GSChar *name,
		const GSBinding *binding, const GSTimeSeriesProperties *properties,
		GSBool modifiable, GSTimeSeries **timeSeries));
#endif

#define gsPutTimeSeries( \
				store, name, binding, properties, modifiable, timeSeries) \
		gsCompatibleFunc_PutTimeSeries1( \
				store, name, binding, properties, modifiable, timeSeries)

#endif

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsRemoveRowByPath(
		GSGridStore *store, const GSChar *pathKey, GSBool *exists));
#endif

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsDeleteRowByPath(
		GSGridStore *store, const GSChar *pathKey, GSBool *exists));
#endif

#if GS_COMPATIBILITY_SUPPORT_1_5

#if GS_COMPATIBILITY_SUPPORT_3_5

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSContainerInfo を指定して、コンテナを新規作成または変更します。
	@par
		次の点を除き、@ref gsPutCollection もしくは@ref gsPutTimeSeries
		と同様に振る舞います。
		- @ref GSContainerInfo を用いてコンテナ種別、カラムレイアウト、
			ならびに、必要に応じ時系列構成オプションを指定する
		- 返却される @ref GSContainer のロウオブジェクトの型が常に@ref GSRow
			となる
		それぞれの同名の引数@c modifiable の用法についても同様です。
	@par
		コンテナに関する情報の指定方法の一覧は次の通りです。
		<table>
		<tr><th>項目</th><th>引数</th><th>説明</th></tr>
		<tr><td>コンテナ名</td><td>@c name または@c info</td>
		<td>少なくともいずれかの引数に@c NULL ではない値を指定する。
		両方に指定する場合、異なる値を指定してはならない。</td></tr>
		<tr><td>コンテナ種別</td><td>@c info</td>
		<td>@ref GS_CONTAINER_COLLECTION を指定した場合、
		@ref gsPutCollection と同様の振る舞いとなる。
		@ref GS_CONTAINER_TIME_SERIES を指定した場合、
		@ref gsPutTimeSeries
		と同様の振る舞いとなる。</td></tr>
		<tr><td>カラムレイアウト</td><td>@c info</td>
		<td>@ref GSContainer にて規定された制約に合致するよう
		@ref GSColumnInfo のリストならびにロウキーの有無を設定する。
		ただし現バージョンでは、初期値でのNULL使用有無が設定された
		@ref GSColumnInfo::options を持つ@ref GSColumnInfo を含めることは
		できない。</td></tr>
		<tr><td>カラム順序の無視</td><td>@c info</td>
		<td>無視する場合、同名の既存のコンテナのカラム順序と一致するかどうかを
		検証しない。</td></tr>
		<tr><td>時系列構成オプション</td><td>@c info</td>
		<td>コンテナ種別が@ref GS_CONTAINER_TIME_SERIES の場合のみ、
		@c NULL ではない値を指定できる。</td></tr>
		<tr><td>索引設定</td><td>@c info</td>
		<td>現バージョンでは無視される。
		今後のバージョンでは、@ref gsCreateIndex の規則に合致しない
		設定が含まれていた場合、エラーとなる可能性がある。</td></tr>
		<tr><td>トリガ設定</td><td>@c info</td>
		<td>現バージョンでは無視される。
		今後のバージョンでは、@ref gsCreateTrigger の規則に合致しない
		設定が含まれていた場合、エラーとなる可能性がある。</td></tr>
		<tr><td>コンテナ類似性</td><td>@c info</td>
		<td>@c NULL 以外を指定し新規作成する場合、指定の内容が反映される。
		既存コンテナの設定を変更することはできない。@c NULL を指定した
		場合は無視される。</td></tr>
		</table>
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコンテナの名前
	@param [in] info
		処理対象のコンテナの情報
	@param [in] modifiable
		既存コンテナのカラムレイアウト変更を許可するかどうか
	@param [out] container
		@ref GSContainer インスタンスを格納するためのポインタ変数へのポインタ値。
		@ref GS_CONTAINER_COLLECTION を指定した場合は@ref GSCollection 、
		@ref GS_CONTAINER_TIME_SERIES を指定した場合は@ref GSTimeSeries
		固有の機能が使用できます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c name ならびに@c info 引数の内容が規則に合致しない場合。
			また、指定のコンテナ種別に対応する コンテナ新規作成・変更関数の
			規則に合致しない場合
		- この処理のタイムアウト、接続障害が発生した場合
		- @c name 以外のポインタ型引数に@c NULL が指定された場合
	@see gsPutCollection
	@see gsPutTimeSeries
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Newly creates or update a Container with the specified
		@ref GSContainerInfo.
	@par
		The behavior will be the same as @ref gsPutCollection or
		@ref gsPutTimeSeries except for points below.
		- Use @ref GSContainerInfo to specify the Container type, column layout,
			as well as the TimeSeries composition option as necessary
		- The Row object type of the returned @ref GSContainer will be always
			@ref GSRow
		Arguments @c modifiable with the same respective name are used in the
		same way as well.
	@par
		A list of the methods to specify Container-related information is given
		below.
		<table>
		<tr><th>Item</th><th>Argument</th><th>Description</th></tr>
		<tr><td>Container name</td><td>@c name or @c info</td>
		<td>Specify a value that is not @c NULL in at least one of the
		arguments. A different value must be specified when specifying
		both sides.</td></tr>
		<tr><td>Container type</td><td>@c info</td>
		<td>If @ref GS_CONTAINER_COLLECTION is specified, the behavior will be
		the same as @ref gsPutCollection.
		If @ref GS_CONTAINER_TIME_SERIES is	specified, the behavior will be
		the same as @ref gsPutTimeSeries.</td></tr>
		<tr><td>column layout</td><td>@c info</td>
		<td>Set the @ref GSColumnInfo list and whether there is any Row key
		so as to conform to the restrictions stipulated in @ref GSContainer.
		However, in the current version, it is not allowed that the list
		includes one or more @ref GSColumnInfo which has option
		whether to use of NULL for the initial value in
		@ref GSColumnInfo::options.
		</td></tr>
		<tr><td>ignore column order</td><td>@c info</td>
		<td>If ignored, no verification of the conformance with the column order
		of existing Containers with the same name will be carried out.</td></tr>
		<tr><td>TimeSeries composition option</td><td>@c info</td>
		<td>A value that is not @c NULL can be specified only if the Container
		type is @ref GS_CONTAINER_TIME_SERIES.</td></tr>
		<tr><td>index setting</td><td>@c info</td>
		<td>Ignored in the current version. In future versions, if settings that
		do not conform to the rules of @ref gsCreateIndex are included,
		an error may be occurred.</td></tr>
		<tr><td>trigger setting</td><td>@c info</td>
		<td>Ignored in the current version. In future versions, if settings that
		do not conform to the rules of @ref gsCreateTrigger are included,
		an error may be occurred.</td></tr>
		<tr><td>Container similarity</td><td>@c info</td>
		<td>The specified contents will be reflected if a setting other than
		@c NULL is specified and newly created. The settings of an existing
		Container cannot be changed. The settings are ignored if @c NULL is
		specified.</td></tr>
		</table>
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Container name to be processed
	@param [in] info
		Container information to be processed
	@param [in] modifiable
		Indicates whether the column layout of the existing Container can be
		modified or not
	@param [out] container
		The pointer to a pointer variable to store @ref GSContainer instance.
		@ref GSCollection specific function can be used if
		@ref GS_CONTAINER_COLLECTION is specified, or @ref GSTimeSeries
		specific function can be used if @ref GS_CONTAINER_TIME_SERIES is
		specified.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the contents of the arguments @c name and @c info do not conform to
			the rules. Also if the contents do not conform to the rules of the
			new Container creation and update function for the specified
			Container type
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to pointer type arguments except @c name
	@see gsPutCollection
	@see gsPutTimeSeries
	@since 1.5

	@ENDL
 */
GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutContainerGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return gsPutContainerGeneralV3_3(
			store, name, info, modifiable, container);
}

#elif GS_COMPATIBILITY_SUPPORT_2_1

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutContainerGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return gsPutContainerGeneralV2_1(
			store, name, info, modifiable, container);
}

#elif GS_COMPATIBILITY_SUPPORT_2_0

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneralV2_0(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutContainerGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container) {
	return gsPutContainerGeneralV2_0(
			store, name, info, modifiable, container);
}

#else

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutContainerGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);

#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSRow によりロウ操作できる@ref GSContainer インスタンスを
		取得します。
	@par
		次の点を除き、@ref gsGetCollection もしくは@ref gsGetTimeSeries
		と同様に振る舞います。
		- 既存のコンテナの種別ならびにカラムレイアウトに基づき@ref GSContainer
			インスタンスを返却する
		- コンテナの種別ならびにカラムレイアウトを指定しないため、これらの
			不一致に伴うエラーが発生しない
		- 返却される @ref GSContainer のロウオブジェクトの型が常に@ref GSRow
			となる
		それぞれの同名の引数@c name の用法についても同様です。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコンテナの名前
	@param [out] container
		@ref GSContainer インスタンスを格納するためのポインタ変数へのポインタ値。
		指定の名前のコンテナが存在しない場合、@c NULL が設定されます。
		指定の名前のコンテナが存在し、種別が
		@ref GS_CONTAINER_COLLECTION であった場合は@ref GSCollection 、
		@ref GS_CONTAINER_TIME_SERIES であった場合は@ref GSTimeSeries
		固有の機能が使用できます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合
	@see gsGetCollection
	@see gsGetTimeSeries
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Get a @ref GSContainer instance whose Rows can be processed using a
		@ref GSRow.
	@par
		The behavior will be the same as @ref gsGetCollection or
		@ref gsGetTimeSeries except for points below.
		- Return a @ref GSContainer instance based on the existing Container
			type and column layout
		- No error accompanying these non-conformances will occur as the
			Container type and column layout are not specified
		- The Row object type of the returned @ref GSContainer will be always
			@ref GSRow
		Arguments @c modifiable with the same respective name are used in the
		same way as well.
		Arguments @c name with the same respective name are used in the
		same way as well.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Container name to be processed
	@param [out] container
		The pointer to a pointer variable to store @ref GSContainer instance.
		@c NULL is set to this pointer if there is no Container which has the
		specified name.
		@ref GSCollection specific function can be used if the Container with
		the specified name exists and its type is @ref GS_CONTAINER_COLLECTION,
		or @ref GSTimeSeries specific function can be used if the Container with
		the specified name exists and its type is @ref GS_CONTAINER_TIME_SERIES.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)
	@see gsGetCollection
	@see gsGetTimeSeries
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerGeneral(
		GSGridStore *store, const GSChar *name, GSContainer **container);

#if GS_COMPATIBILITY_SUPPORT_3_5

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollectionGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSContainerInfo を指定して、コレクションを新規作成または変更します。
	@par
		コンテナ種別が@ref GS_CONTAINER_COLLECTION に限定され、
		@ref GSContainer インスタンスが格納される点を除き、
		@ref gsPutContainerGeneral と同様に振る舞います。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコレクションの名前
	@param [in] info
		処理対象のコレクションの情報。コンテナ種別には常に
		@ref GS_CONTAINER_COLLECTION を指定
	@param [in] modifiable
		既存コレクションのカラムレイアウト変更を許可するかどうか
	@param [out] collection
		@ref GSCollection インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- コンテナ種別以外の指定内容に関して、@ref gsPutContainerGeneral
			の規則に合致しない場合。また、コンテナ種別に関する制限に合致しない場合
		- この処理のタイムアウト、接続障害が発生した場合
		- @c name 以外のポインタ型引数に@c NULL が指定された場合
	@see gsPutContainerGeneral
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Newly creates or update a Collection with the specified
		@ref GSContainerInfo.
	@par
		The behavior will be the same as @ref gsPutContainerGeneral except for
		points where the Container type is limited to
		@ref GS_CONTAINER_COLLECTION and the @ref GSContainer instance is
		stored.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Collection name to be processed
	@param [in] info
		Collection information to be processed. Always specify
		@ref GS_CONTAINER_COLLECTION in the Container type
	@param [in] modifiable
		Indicates whether the column layout of the existing Collection can be
		modified or not
	@param [out] collection
		The pointer to a pointer variable to store @ref GSCollection instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- If specifications other than the Container type do not conform to the
			rules of @ref gsPutContainerGeneral. If the specifications do not
			conform to the restrictions related to the Container type as well
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to pointer type arguments except @c name
	@see gsPutContainerGeneral
	@since 1.5

	@ENDL
 */
GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutCollectionGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection) {
	return gsPutCollectionGeneralV3_3(
			store, name, info, modifiable, collection);
}

#elif GS_COMPATIBILITY_SUPPORT_2_1

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollectionGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSContainer **container);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutCollectionGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection) {
	return gsPutCollectionGeneralV2_1(
			store, name, info, modifiable, collection);
}

#else

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutCollectionGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSCollection **collection);

#endif	// not GS_COMPATIBILITY_SUPPORT_2_1

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSRow によりロウ操作できる@ref GSCollection インスタンスを
		取得します。
	@par
		期待するコンテナ種別が@ref GS_CONTAINER_COLLECTION に限定され、
		常に@ref GSContainer インスタンスが格納される点を除き、
		@ref gsGetContainerGeneral と同様に振る舞います。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコレクションの名前
	@param [out] collection
		@ref GSCollection インスタンスを格納するためのポインタ変数へのポインタ値。
		指定の名前のコレクションが存在しない場合、@c NULL が設定されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 同名の時系列が存在する場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合
	@see gsGetContainerGeneral
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Get a @ref GSCollection instance whose Rows can be processed using a
		@ref GSRow.
	@par
		The behavior will be the same as @ref gsGetContainerGeneral except for
		points where the expected Container type is limited to
		@ref GS_CONTAINER_COLLECTION and the @ref GSContainer instance is
		always stored.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Collection name to be processed
	@param [out] collection
		The pointer to a pointer variable to store @ref GSCollection instance.
		@c NULL is set to this pointer if there is no Collection which has the
		specified name.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a TimeSeries with the same name exists
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)
	@see gsGetContainerGeneral
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetCollectionGeneral(
		GSGridStore *store, const GSChar *name, GSCollection **collection);

#if GS_COMPATIBILITY_SUPPORT_3_5

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneralV3_3(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSContainerInfo を指定して、時系列を新規作成または変更します。
	@par
		コンテナ種別が@ref GS_CONTAINER_TIME_SERIES に限定され、
		@ref GSTimeSeries インスタンスが格納される点を除き、
		@ref gsPutTimeSeriesGeneral と同様に振る舞います。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象の時系列の名前
	@param [in] info
		処理対象の時系列の情報。コンテナ種別には常に
		@ref GS_CONTAINER_TIME_SERIES を指定
	@param [in] modifiable
		既存時系列のカラムレイアウト変更を許可するかどうか
	@param [out] timeSeries
		@ref GSTimeSeries インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- コンテナ種別以外の指定内容に関して、@ref gsPutContainerGeneral
			の規則に合致しない場合。また、コンテナ種別に関する制限に合致しない場合
		- この処理のタイムアウト、接続障害が発生した場合
		- @c name 以外のポインタ型引数に@c NULL が指定された場合
	@see gsPutContainerGeneral
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Newly creates or update a TimeSeries with the specified
		@ref GSContainerInfo.
	@par
		The behavior will be the same as @ref gsPutTimeSeriesGeneral except for
		points where the Container type is limited to
		@ref GS_CONTAINER_TIME_SERIES and the @ref GSTimeSeries instance is
		stored.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		TimeSeries name to be processed
	@param [in] info
		TimeSeries information to be processed. Always specify
		@ref GS_CONTAINER_TIME_SERIES in the Container type
	@param [in] modifiable
		Indicates whether the column layout of the existing TimeSeries can be
		modified or not
	@param [out] timeSeries
		The pointer to a pointer variable to store @ref GSTimeSeries instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specifications other than the Container type do not conform to the
			rules of @ref gsPutContainerGeneral. If the specifications do not
			conform to the restrictions related to the Container type as well
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified to pointer type arguments except @c name
	@see gsPutContainerGeneral
	@since 1.5

	@ENDL
 */
GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	return gsPutTimeSeriesGeneralV3_3(
			store, name, info, modifiable, timeSeries);
}

#elif GS_COMPATIBILITY_SUPPORT_2_1

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneralV2_1(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	return gsPutTimeSeriesGeneralV2_1(
			store, name, info, modifiable, timeSeries);
}

#elif GS_COMPATIBILITY_SUPPORT_2_0

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneralV2_0(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsPutTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries) {
	return gsPutTimeSeriesGeneralV2_0(
			store, name, info, modifiable, timeSeries);
}

#else

GS_DLL_PUBLIC GSResult GS_API_CALL gsPutTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name,
		const GSContainerInfo *info,
		GSBool modifiable, GSTimeSeries **timeSeries);

#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSRow によりロウ操作できる@ref GSTimeSeries インスタンスを
		取得します。
	@par
		期待するコンテナ種別が@ref GS_CONTAINER_TIME_SERIES に限定され、
		常に@ref GSTimeSeries インスタンスが格納される点を除き、
		@ref gsGetContainerGeneral と同様に振る舞います。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象の時系列の名前
	@param [out] timeSeries
		@ref GSCollection インスタンスを格納するためのポインタ変数へのポインタ値。
		指定の名前の時系列が存在しない場合、@c NULL が設定されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 同名のコレクションが存在する場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合
	@see gsGetContainerGeneral
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Get a @ref GSTimeSeries instance whose Rows can be processed using a
		@ref GSRow.
	@par
		The behavior will be the same as @ref gsGetContainerGeneral except for
		points where the expected Container type is limited to
		@ref GS_CONTAINER_TIME_SERIES and the @ref GSTimeSeries instance is
		always stored.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		TimeSeries name to be processed
	@param [out] timeSeries
		The pointer to a pointer variable to store @ref GSCollection instance.
		@c NULL is set to this pointer if there is no TimeSeries which has the
		specified name.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a Collection with the same name exists
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)
	@see gsGetContainerGeneral
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetTimeSeriesGeneral(
		GSGridStore *store, const GSChar *name, GSTimeSeries **timeSeries);

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の名前を持つコンテナを削除します。
	@par
		削除済みの場合は何も変更しません。
	@par
		処理対象のコンテナにおいて実行中のトランザクションが存在する場合、
		それらの終了を待ってから削除を行います。
	@par
		コンテナの削除要求が完了した直後は、削除したコンテナの索引や
		ロウなどのために使用されていたメモリやストレージ領域を他の用途に
		ただちに再利用できない場合があります。また、削除処理に関連した処理が
		クラスタ上で動作することにより、削除前と比べて負荷が高まる期間が
		一定程度継続する場合があります。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] name
		処理対象のコンテナの名前
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理のタイムアウト、接続障害が発生した場合
		- 引数に@c NULL が指定された場合
	@see gsDropCollection
	@see gsTimeSeries
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Delete a Container with the specified name.
	@par
		If the specified Container is already deleted, nothing is changed.
	@par
		When a transaction(s) is active in a target Container, it deletes the
		Container after waiting for the transaction completion.
	@par
		Immediately after the container deletion request is completed, the index
		of deleted container, the memory used for row, and the storage area may
		not be immediately reused for other usage. When cluster operation related
		to deletion is performed, you may experience an increase of load for
		a certain period of time as compared to prior the deletion.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] name
		Container name to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if @c NULL is specified in the argument(s)
	@see gsDropCollection
	@see gsTimeSeries
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDropContainer(
		GSGridStore *store, const GSChar *name);

#if GS_COMPATIBILITY_SUPPORT_3_5

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateRowByStoreV3_3(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row);
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief @ref GSContainerInfo を指定して、@ref GSRow を新規作成します。
	@par
		@ref GSContainer にて規定された制約に合致するよう、
		@ref GSColumnInfo のリストならびにロウキーの有無を含む
		カラムレイアウトを@ref GSContainerInfo に指定します。
	@par
		また、コンテナ種別を@ref GSContainerInfo に含めることで、
		特定のコンテナ種別固有の制約に合致するかどうかを検証できます。
		ただし、作成された@ref GSRow に対して@ref gsGetRowSchema
		を呼び出したとしても、常に固定の値である@ref GS_CONTAINER_COLLECTION
		がコンテナ種別として設定された@ref GSContainerInfo が求まります。
	@par
		作成された@ref GSRow の各フィールドには、@ref GSContainerInfo に
		含まれる各カラムの@ref GSColumnInfo に基づいた初期値が設定されます。
		初期値として、@ref GSColumnInfo::options に含まれるオプションに応じた
		次の値が使用されます。
		<table>
		<tr>
		<th>オプション</th><th>初期値</th>
		</tr>
		<tr>
		<td>@ref GS_TYPE_OPTION_DEFAULT_VALUE_NULL</td>
		<td>NULL。ただし制約に反するロウは作成できない。</td>
		</tr>
		<tr>
		<td>@ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL</td>
		<td>空の値。@ref GSContainer の定義を参照。</td>
		</tr>
		<tr>
		<td>(上記いずれも指定なし)</td>
		<td>現バージョンでは、@ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL
		が指定された場合と同様。</td>
		</tr>
		<tr>
		<td>(上記オプションを両方指定)</td>
		<td>@ref GSTypeOption の定義に基づき矛盾する設定と見なされ、ロウを作成
		できない。</td>
		</tr>
		</table>
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] info
		カラムレイアウトを含むコンテナ情報。その他の内容は無視される
	@param [out] row
		@ref GSRow インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- コンテナ種別もしくはカラムレイアウトの制約に合致しない場合
		- 引数に@c NULL が指定された場合
	@see GSContainer
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Creates a new @ref GSRow with the specified @ref GSContainerInfo.
	@par
		Include the @ref GSColumnInfo list and whether there is any Row key
		so as to conform to the restrictions stipulated in @ref GSContainer.
		Specify the column layout in @ref GSContainerInfo.
	@par
		In addition, by including the Container type in @ref GSContainerInfo, it
		can be verified whether the restrictions unique to a specific Container
		type are conformed to or not. However, the @ref GSContainerInfo which
		has @ref GS_CONTAINER_COLLECTION in Container type will be returned even
		if a @ref gsGetRowSchema is invoked against the created @ref GSRow.
	@par
		Each existing field of the created @ref GSRow is initialized with the empty
		value defined by @ref GSContainer.
	@par
		Each field will be set to the initial value which is based on
		@ref GSColumnInfo of each column in the specified
		@ref GSContainerInfo. The initial value corresponding to the option in
		@ref GSColumnInfo::options is selected by the following way.
		<table>
		<tr>
		<th>Option</th><th>Initial value</th>
		</tr>
		<tr>
		<td>@ref GS_TYPE_OPTION_DEFAULT_VALUE_NULL</td>
		<td>NULL. However a row which violates constraints can not be
		created.</td>
		</tr>
		<tr>
		<td>@ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL</td>
		<td>The empty value. See the definition of @ref GSContainer.</td>
		</tr>
		<tr>
		<td>(None of above)</td>
		<td>In the current version, same as
		@ref GS_TYPE_OPTION_DEFAULT_VALUE_NOT_NULL.</td>
		</tr>
		<tr>
		<td>(Both of above)</td>
		<td>According to definition of @ref GSTypeOption, a row can not be
		created because of conflicted options.</td>
		</tr>
		</table>
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] info
		Container information including the column layout. Other contents are
		ignored
	@param [out] row
		the pointer to a pointer variable to store @ref GSRow instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if restrictions regarding to the Container type or the column layout
			are not satisfied
		- if @c NULL is specified in the argument(s)
	@see GSContainer
	@since 1.5

	@ENDL
 */
GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsCreateRowByStore(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row) {
	return gsCreateRowByStoreV3_3(store, info, row);
}

#else

GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateRowByStore(
		GSGridStore *store, const GSContainerInfo *info, GSRow **row);

#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定された任意個数の@ref GSQuery について、可能な限りリクエスト単位を
		大きくしてクエリ実行とフェッチを行います。
	@par
		指定のクエリ列に含まれる各@ref GSQuery について、個別に@ref gsFetch
		を行った場合と同様にクエリ実行とフェッチを行い、結果の@ref GSRowSet
		を設定します。
		@ref GSQuery の実行結果を取り出すには、@ref gsGetRowSet
		を使用します。ただし、個別に行う場合と違い、同一の格納先などの可能な限り
		大きな単位で対象ノードに対しリクエストしようとします。これにより、リストの要素数が
		多くなるほど、対象ノードとやりとりする回数が削減される可能性が高くなります。
		リスト内の@ref GSQuery の実行順序は不定です。
	@par
		指定のクエリ列には、指定の@ref GSGridStore インスタンスを介して得られた、
		対応する@ref GSContainer がクローズされていない@ref GSQuery
		のみを含めることができます。
		@ref gsFetch と同様、各@ref GSQuery が持つ最後に生成された
		@ref GSRowSet を介するロウ操作ができなくなります。
		同一のインスタンスが配列に複数含まれていた場合、それぞれ異なる
		インスタンスであった場合と同様に振る舞います。
	@par
		他のコンテナ・ロウ操作と同様、異なるコンテナ間での整合性は保証されません。
		したがって、あるコンテナに対する処理の結果は、その処理の開始前に完了した
		他の操作命令の影響を受けることがあります。
	@par
		指定の@ref GSQuery に対応する各@ref GSContainer のコミットモードが
		自動コミットモード、手動コミットモードのいずれであったとしても、使用できます。
		トランザクション状態はクエリの実行結果に反映されます。
		正常に操作が完了した場合、トランザクションタイムアウト時間に到達しない限り、
		対応する各@ref GSContainer のトランザクションをアボートすることはありません。
	@par
		各@ref GSQuery に対する処理の途中でエラーが発生した場合、
		一部の@ref GSQuery についてのみ新たな@ref GSRowSet
		が設定されることがあります。また、指定の@ref GSQuery に対応する
		各@ref GSContainer の未コミットのトランザクションについては、アボートされる
		ことがあります。
	@par
		一度に大量のロウを取得しようとした場合、GridDBノードが管理する
		通信バッファのサイズの上限に到達し、失敗することがあります。
		上限サイズについては、GridDBテクニカルリファレンスを参照してください。
	@par
		この操作によりエラーが発生した場合、エラー情報には@c container
		パラメータが含まれることがあります。エラーに関するパラメータの詳細は、
		@ref GSGridStore の定義を参照してください。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] queryList
		対象とするクエリ列。@ref GSQuery へのポインタ値の配列により構成されます。
		@c queryCount が@c 0 の場合、この配列を参照することはなく、
		@c NULL を指定することもできます。
	@param [in] queryCount
		対象とするクエリ列の要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定の@ref GSGridStore インスタンスを介して得られた@ref GSQuery 以外の
			@ref GSQuery が含まれていた場合
		- 正しくないパラメータ・構文・命令を含むクエリを実行しようとした場合。
			たとえば、TQLでは、関数の引数に対応しない型のカラムを指定した場合。
			具体的な制約は、指定のクエリを作成する機能の各種定義を参照してください
		- この処理または関連するトランザクションのタイムアウト、対応するコンテナの削除もしくは
			スキーマ変更、接続障害が発生した場合、または対応するコンテナのクローズ後に
			呼び出された場合
		- @c queryCount が正の値であるにもかかわらず、@c queryList
			に@c NULL が指定された場合
		- クエリ列を構成する配列要素に@c NULL が含まれていた場合
	@see gsFetch
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Query execution and fetch is carried out on a specified arbitrary
		number of @ref GSQuery, with the request unit enlarged as much as
		possible.
	@par
		For each @ref GSQuery included in a specified query column, perform a
		similar query execution and fetch as when @ref gsFetch is performed
		individually and set the @ref GSRowSet in the results.
		Use @ref gsGetRowSet to extract the execution results of each
		@ref GSQuery. However, unlike the case when carried out individually,
		the target node is requested for the same storage destination, etc. with
		a unit that is as large as possible. Based on this, the larger the
		number of elements in the list, the higher is the possibility that the
		number of correspondences with the target node will be reduced.
		@ref GSQuery in a list are not executed in any particular order.
	@par
		Only a @ref GSQuery that has not been closed, including corresponding
		@ref GSContainer acquired via the specified @ref GSGridStore instance,
		can be included in a specified query column.
		Like a @ref gsFetch, the Row operations via @ref GSRowSet finally
		generated and held by each @ref GSQuery will be unavailable.
		If the same instance is included multiple times in an array, the
		behavior will be the same as the case in which the respective instances
		differ.
	@par
		Like other Container or Row operations, consistency between Containers
		is not guaranteed. Therefore, the processing results for a certain
		Container may be affected by other operation commands that have been
		completed prior to the start of the process.
	@par
		The commit mode of each @ref GSContainer corresponding to the specified
		@ref GSQuery can be used in either the auto commit mode or manual commit
		mode.
		The transaction status is reflected in the execution results of the
		query.
		If the operation is completed normally, the corresponding transaction of
		each @ref GSContainer will not be aborted so long as the transaction
		timeout time has not been reached.
	@par
		If an error occurs in the midst of processing each @ref GSQuery, a
		new @ref GSRowSet may be set for only some of the @ref GSQuery.
		In addition, uncommitted transactions of each @ref GSQuery corresponding
		to the designated @ref GSContainer may be aborted.
	@par
		If the system tries to acquire a large number of Rows all at once, the
		upper limit of the communication buffer size managed by the GridDB node
		may be reached, possibly resulting in a failure.
		Refer to the GridDB Technical Reference for the upper limit
		size.
	@par
		If an error occurs in this operation, the error information may contain
		@c container parameter. For the details of the parameters related to
		the error, see the definition of @ref GSGridStore.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] queryList
		The target query column. It consists of the array of pointers to
		@ref GSQuery.
		If @c 0 is set to @c queryCount, @c NULL can be specified since the
		array is not referred in this function.
	@param [in] queryCount
		The number of elements in target query column
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a @ref GSQuery other than a @ref GSQuery obtained via the
			specified @ref GSGridStore instance is included
		- if the target query contains any wrong parameter, syntax, or
			directive. For example, in the case of TQL, if the type of a
			specified Column does not match the parameter of a function. For
			detailed restrictions, see the descriptions of the functions to
			create a specified query
		- if a timeout occurs during this operation or related transaction,
			the relevant Container is deleted, its schema is changed,
			a connection failure occurs, or this functions is called after the
			relevant Container is closed
		- if @c NULL is specified to @c queryList despite @c queryCount
			is a positive value
		- if @c NULL is included in the array element which consists of the
			query column
	@see gsFetch
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsFetchAll(
		GSGridStore *store, GSQuery *const *queryList, size_t queryCount);

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 任意のコンテナの任意個数のロウについて、可能な限りリクエスト単位を大きくして
		新規作成または更新操作を行います。
	@par
		指定のエントリ列に含まれる各ロウオブジェクトについて、個別に@ref gsPutRow
		を呼び出した場合と同様に新規作成または更新操作を行います。
		ただし、個別に行う場合と違い、同一の格納先などの可能な限り大きな単位で
		対象ノードに対しリクエストしようとします。これにより、対象コンテナの総数や
		指定のロウオブジェクトの総数が多くなるほど、対象ノードとやりとりする回数が
		削減される可能性が高くなります。
	@par
		指定のエントリ列は、コンテナ名とロウオブジェクト列との組からなる任意個数の
		エントリから構成されます。対象とするコンテナとして、コンテナ種別やカラムレイアウトが
		異なるものを混在させることができます。ただし、すでに存在するコンテナで
		なければなりません。エントリ列のコンテナ名として@c NULL を設定することは
		できません。また、ロウオブジェクト列の要素数が正ならば、ロウオブジェクト列の
		配列アドレスとして@c NULL を設定することはできません。
	@par
		各ロウオブジェクト列には、対象のコンテナと同一のカラムレイアウトの
		@ref GSRow のみを任意個数含めることができます。
		現バージョンでは、カラム順序についてもすべて同一でなければなりません。
		ロウオブジェクト列の要素として@c NULL を含めることはできません。
	@par
		コンテナの種別ならびに設定によっては、操作できるロウの内容について
		@ref gsPutRow と同様の制限が設けられています。
		具体的な制限事項は、@ref gsPutRow の定義を参照してください。
	@par
		指定のエントリ列内に同一コンテナを対象とした同一ロウキーを持つ複数の
		ロウオブジェクト列が存在する場合、異なるリスト間であればエントリ列の要素順、
		同一ロウオブジェクト列内であればその要素順を基準として、
		同値のロウキーを持つ最も後方にあるロウオブジェクトの内容が反映されます。
	@par
		トランザクションを維持し、ロックを保持し続けることはできません。
		ただし、既存のトランザクションが対象ロウに影響するロックを確保している場合、
		すべてのロックが解放されるまで待機し続けます。
	@par
		他のコンテナ・ロウ操作と同様、異なるコンテナ間での整合性は保証されません。
		したがって、あるコンテナに対する処理の結果は、その処理の開始前に完了した
		他の操作命令の影響を受けることがあります。
	@par
		各コンテナならびにロウに対する処理の途中でエラーが発生した場合、
		一部のコンテナの一部のロウに対する操作結果のみが反映されたままとなることが
		あります。
	@par
		この操作によりエラーが発生した場合、エラー情報には@c container
		パラメータが含まれることがあります。エラーに関するパラメータの詳細は、
		@ref GSGridStore の定義を参照してください。
	@attention
		エントリ列に含まれるロウオブジェクトへのアドレスとして@ref GSRow 以外の
		ものが含まれており、異常を検知できなかった場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] entryList
		対象とするコンテナの名前とロウオブジェクト列からなるエントリの列。
		@ref GSContainerRowEntry の配列により構成されます。
		@c entryCount が@c 0 の場合、この配列を参照することはなく、
		@c NULL を指定することもできます。
	@param [in] entryCount
		エントリ列の要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 対象とするコンテナが存在しない場合、また、対象とする コンテナとロウオブジェクトとの
			カラムレイアウトが一致しない場合
		- 特定コンテナ種別固有の制限に反する操作を行った場合
		- この処理または関連するトランザクションのタイムアウト、接続障害が発生した場合、
			またはサポート範囲外の値が ロウオブジェクトに含まれていた場合
		- @c store が@c NULL の場合
		- @c queryCount が正の値であるにもかかわらず、@c queryList
			に@c NULL が指定された場合
		- エントリ列に含まれるロウオブジェクトへのアドレスとして@ref GSRow 以外の
			ものが含まれており、異常検知に成功した場合
		- エントリ列を構成するエントリのコンテナ名として@c NULL が含まれていた場合、
			ロウオブジェクト列の要素数が正であるにも関わらずロウオブジェクト列の
			配列アドレスとして@c NULL が含まれていた場合、また、ロウオブジェクト列の
			要素として@c NULL が含まれていた場合
	@see gsPutRow
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief New creation or update operation is carried out on an arbitrary
		number of rows of a Container, with the request unit enlarged as much as
		possible.
	@par
		For each Row object included in a specified entry column, a new creation
		or update operation is carried out just like the case when @ref gsPutRow
		is invoked individually.
		However, unlike the case when carried out individually, the target node
		is requested for the same storage destination, etc. with a unit that is
		as large as possible. Based on this, the larger the total number of Row
		objects specified and the larger the total number of target Containers,
		the higher is the possibility that the number of correspondences with
		the target node will be reduced.
	@par
		A specified entry column is composed of an arbitrary number of entries
		that adopt the Container name as its key and the column of Row objects
		as its value. A subject Container may be a mixture of different
		Container types and column layouts. However, the Containers must already
		exist. @c NULL can not be set as the Container name in the entry column.
		Also @c NULL can not be set as the array address to the column of Row
		objects if the number of elements in the column of Row objects is
		positive value.
	@par
		An arbitrary number of @ref GSRow with the same column layout as the
		subject Container can be included in each column of Row objects. In the
		current version, all the column order must also be the same. The
		Container cannot include @c NULL as an element of the column of Row
		objects.
	@par
		Depending on the Container type and setting, the same restrictions as
		@ref gsPutRow are established for the contents of Rows that can be
		operated.
		Refer to the definition of @ref gsPutRow for the specific restrictions.
	@par
		If there are multiple columns of Row objects having the same Row key
		targeting the same Container in the designated entry column, the
		contents of the rear-most Row object having a Row key with the same
		value will be reflected using the element order of entry column as a
		reference if it is between different lists, or the element order of the
		column of Row object as a reference if it is within the same column of
		Row object.
	@par
		The transaction cannot be maintained and the lock cannot continue to be
		retained.
		However, if the lock that affects the target Row is secured by an
		existing transaction, the system will continue to wait for all the
		locks to be released.
	@par
		Like other Container or Row operations, consistency between Containers
		is not guaranteed. Therefore, the processing results for a certain
		Container may be affected by other operation commands that have been
		completed prior to the start of the process.
	@par
		If an error occurs in the midst of processing a Container and its Rows,
		only the results for some of the Rows of some of the Containers may
		remain reflected.
	@par
		If an error occurs in this operation, the error information may contain
		@c container parameter. For the details of the parameters related to
		the error, see the definition of @ref GSGridStore.
	@attention
		The behavior is undefined if the address of Row object excepting
		@ref GSRow is included in the entry columns and failed to detect
		the abnormality.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] entryList
		A entry column made up of a column of Row objects and target Container
		names.
		It consists of the array of @ref GSContainerRowEntry.
		If @c 0 is set to @c entryCount, @c NULL can be specified since the
		array is not referred in this function.
	@param [in] entryCount
		The number of elements in the entry column
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the target Container does not exist, or if the column layouts of
			the target Container and Row object do not match
		- When an operation violating the restrictions unique to a specific
			Container type is carried out
		- if a timeout occurs during this operation or the transaction, or a
			connection failure occurs, or if a value outside the supported range
			is included in the Row object
		- if @c NULL is specified to @c store
		- if @c NULL is specified to @c queryList despite @c queryCount
			is a positive value
		- if the address of Row object excepting @ref GSRow is included in the
			entry columns and succeeded to detect the abnormality
		- if @c NULL is included in the Container name in entry which make up
			the entry column, or if @c NULL is specified as an array address of
			the column of Row object despite the number of elements in the
			column of Row object is positive, or if @c NULL is included as the
			element in the column of Row object
	@see gsPutRow
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutMultipleContainerRows(
		GSGridStore *store, const GSContainerRowEntry *entryList,
		size_t entryCount);

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_COMPATIBILITY_MULTIPLE_CONTAINERS_1_1_106
#define gsPutMultipleContainerRow gsPutMultipleContainerRows
#endif
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の条件に基づき、任意のコンテナの任意個数・範囲のロウについて、
		可能な限りリクエスト単位を大きくして取得します。
	@par
		指定のエントリ列に含まれる条件に従い、個別に@ref gsGetRow もしくは
		@ref gsFetch を呼び出した場合と同様に、ロウの内容を取得します。
		ただし、個別に行う場合と違い、同一の格納先などの可能な限り大きな単位で
		対象ノードに対しリクエストしようとします。これにより、対象コンテナの総数や
		条件に合致するロウの総数が多くなるほど、対象ノードとやりとりする回数が
		削減される可能性が高くなります。
	@par
		指定の条件エントリ列は、コンテナ名と、@ref GSRowKeyPredicate で
		表現される取得条件との組からなる任意個数の条件エントリから構成されます。
		同一の@ref GSRowKeyPredicate インスタンスを複数含めることもできます。
		また、対象とするコンテナとして、コンテナ種別やカラムレイアウトが異なるものを
		混在させることができます。
		ただし、コンテナの構成によっては評価できない取得条件が存在します。
		具体的な制限については、@ref GSRowKeyPredicate に対する各種設定機能の
		定義を参照してください。
		コンテナ名または取得条件として@c NULL を設定することはできません。
	@par
		取得するエントリ列は、コンテナ名とロウオブジェクト列との組からなる
		エントリにより構成されます。また、取得するエントリ列には、取得条件として
		指定したエントリ列のうち、リクエスト時点で実在するコンテナに関する
		エントリのみが含まれます。
		同一のコンテナを指す複数のエントリが指定の条件エントリ列に含まれていた場合、
		取得するエントリ列にはこれらを1つにまとめたエントリが格納されます。
		同一のリストに複数のロウオブジェクトが含まれる場合、格納される順序は
		コンテナ種別と対応する@ref GSContainer から派生した個別のコンテナ型の
		定義に従います。
		指定のコンテナに対応するロウが1つも存在しない場合、対応する
		ロウオブジェクト列の要素数は@c 0 となります。
	@par
		他のコンテナ・ロウ操作と同様、異なるコンテナ間での整合性は保証されません。
		したがって、あるコンテナに対する処理の結果は、その処理の開始前に完了した
		他の操作命令の影響を受けることがあります。
	@par
		@ref gsGetRowForUpdate もしくは@ref gsFetch のように、
		トランザクションを維持し、更新用ロックを要求することはできません。
	@par
		一度に大量のロウを取得しようとした場合、GridDBノードが管理する
		通信バッファのサイズの上限に到達し、失敗することがあります。
		上限サイズについては、GridDBテクニカルリファレンスを参照してください。
	@par
		この操作によりエラーが発生した場合、エラー情報には@c container
		パラメータが含まれることがあります。エラーに関するパラメータの詳細は、
		@ref GSGridStore の定義を参照してください。
	@attention
		取得するエントリ列ならびにその中に含まれるコンテナ名やロウオブジェクト列の
		可変長のデータを格納するために、指定の@ref GSGridStore インスタンス上で
		管理される、一時的なメモリ領域を使用します。この領域は、対応する
		@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] predicateList
		対象とするコンテナの名前と取得条件との組からなる条件エントリの列。
		@ref GSContainerRowEntry の配列により構成されます。
		@c predicateCount が@c 0 の場合、この配列を参照することはなく、
		@c NULL を指定することもできます。
	@param [in] predicateCount
		条件エントリ列の要素数
	@param [out] entryList
		取得結果エントリ列のアドレスを格納するためのポインタ変数へのポインタ値。
		取得結果エントリ列は@ref GSContainerRowEntry の配列により構成されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@param [out] entryCount
		取得結果エントリ列の要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナに関して評価できない取得条件が指定された場合
		- 特定コンテナ種別固有の制限に反する操作を行った場合
		- この処理または関連するトランザクションのタイムアウト、接続障害が発生した場合
		- @c predicateList 以外のポインタ型引数に@c NULL が指定された場合
		- @c predicateCount が正の値であるにもかかわらず、@c predicateList
			に@c NULL が指定された場合
		- エントリ列を構成するエントリのコンテナ名もしくは取得条件として@c NULL
			が含まれていた場合
	@see gsGetRow
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Returns an arbitrary number and range of Rows in any Container based
		on the specified conditions, with the request unit enlarged as much as
		possible.
	@par
		Returns the Row contents in accordance with the conditions included in
		the specified entry column, similar to invoking @ref gsGetRow or
		@ref gsFetch individually.
		However, unlike the case when carried out individually, the target node
		is requested for the same storage destination, etc. with a unit that is
		as large as possible. Based on this, the larger the total number of Rows
		conforming to the conditions and the larger the total number of target
		Containers, the higher is the possibility that the number of
		correspondences with the target node will be reduced.
	@par
		A specified condition entry column is composed of an arbitrary number
		of condition entries that adopt the Container name as the key and the
		acquisition condition represented by @ref GSRowKeyPredicate as the
		value. Multiple instances with the same @ref GSRowKeyPredicate can also
		be included. In addition, a subject Container may be a mixture of
		different Container types and column layouts.
		However, there are some acquisition conditions that cannot be evaluated
		due to the composition of the Container. Refer to the definitions of the
		various setting functions for @ref GSRowKeyPredicate for the specific
		restrictions.
		In addition, the specified Container name must be a real Container.
		It is prohibited to set @c NULL in the Container name or the acquisition
		condition.
	@par
		An acquired entry column is composed of entries that adopt the Container
		name as its key and column of Row objects as its value.
		Only the entries for real Containers at the request included
		in specified entries as acquisition conditions are included in
		acquired entries.
		If multiple entries pointing the same Container are included in a
		specified condition entry column, a single entry consolidating these is
		stored in the acquired entry column. If multiple Row objects are
		included in the same list, the stored order follows the Container type
		and the definition of the individual Container type derived from the
		corresponding @ref GSContainer. If there is no Row corresponding to the
		specified Container, the number of elements in corresponding column of
		Row object will be @c 0.
	@par
		Like other Container or Row operations, consistency between Containers
		is not guaranteed. Therefore, the processing results for a certain
		Container may be affected by other operation commands that have been
		completed prior to the start of the process.
	@par
		Like @ref gsGetRowForUpdate or @ref gsFetch, a transaction cannot be
		maintained and requests for updating locks cannot be made.
	@par
		If the system tries to acquire a large number of Rows all at once, the
		upper limit of the communication buffer size managed by the GridDB node
		may be reached, possibly resulting in a failure.
		Refer to the GridDB Technical Reference for the upper limit size.
	@par
		If an error occurs in this operation, the error information may contain
		@c container parameter. For the details of the parameters related to
		the error, see the definition of @ref GSGridStore.
	@attention
		In order to store the variable-length data such as the entry column to
		be obtained and the Container name or the column of Row object included
		in the entry column, it uses a temporary memory area which is managed by
		the specified @ref GSGridStore instance.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] predicateList
		The column of condition entry consisting of a combination of the target
		Container name and the acquisition condition.
		It consists of the array of @ref GSContainerRowEntry.
		If @c 0 is set to @c predicateCount, @c NULL can be specified since the
		array is not referred in this function.
	@param [in] predicateCount
		The number of elements in the condition entry column
	@param [out] entryList
		The pointer to a pointer variable to store the address of entry column
		for acquisition result.
		The entry column for acquisition result consists of the array of
		@ref GSContainerRowEntry.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@param [out] entryCount
		The pointer to a variable to store the number of elements in the entry
		column for the acquisition result.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if acquisition conditions
			which cannot be evaluated with a specified Container are specified
		- When an operation violating the restrictions unique to a specific
			Container type is carried out
		- if a timeout occurs during this operation or related transaction, or
			a connection failure occurs
		- if @c NULL is specified to pointer type arguments except
			@c predicateList
		- if @c NULL is specified to @c predicateList despite @c predicateCount
			is a positive value
		- if @c NULL is included in the Container name or the acquisition
			condition of entry which make up the entry column
	@see gsGetRow
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetMultipleContainerRows(
		GSGridStore *store,
		const GSRowKeyPredicateEntry *const *predicateList,
		size_t predicateCount,
		const GSContainerRowEntry **entryList, size_t *entryCount);

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_COMPATIBILITY_MULTIPLE_CONTAINERS_1_1_106
#define gsGetMultipleContainerRow gsGetMultipleContainerRows
#endif
#endif

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 対応するGridDBクラスタについての@ref GSPartitionController
		を取得します。
	@par
		指定の@ref GSGridStore をクローズした時点で使用できなくなります。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [out] partitionController
		@ref GSPartitionController インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@see GSPartitionController
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Returns @ref GSPartitionController corresponding to GridDB cluster.
	@par
		It can be used until the specified @ref GSGridStore is closed.
	@param [in] store
		@ref GSGridStore to be processed
	@param [out] partitionController
		the pointer to a pointer variable to store @ref GSPartitionController
		instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@see GSPartitionController
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionController(
		GSGridStore *store, GSPartitionController **partitionController);

/*!
	@JP
	@ingroup Group_GSGridStore
	@brief 指定の@ref GSType をロウキーの型とする合致条件を作成します。
	@par
		合致条件の評価対象とするコンテナは、ロウキーを持ち、かつ、ロウキーの型が指定の
		@ref GSType と同一の型でなければなりません。
	@par
		設定可能なロウキーの型は、@ref GSContainer から派生した個別の
		コンテナ型にて許容されている型のみです。
	@param [in] store
		処理対象の@ref GSGridStore
	@param [in] keyType
		合致条件の評価対象とするロウキーの型
	@param [out] predicate
		@ref GSRowKeyPredicate インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定された型がロウキーとして常にサポート外となる場合
		- ポインタ型引数に@c NULL が指定された場合
	@see GSContainer
	@since 1.5

	@EN
	@ingroup Group_GSGridStore
	@brief Creates a matching condition with the specified @ref GSType as the
		type of Row key.
	@par
		The target Container must have a Row key, and it must be the same type
		as the specified @ref GSType.
	@par
		The type of Row key that can be set must be the same type that is
		allowed by the individual Container type derived from @ref GSContainer.
	@param [in] store
		@ref GSGridStore to be processed
	@param [in] keyType
		the type of Row key used as a matching condition
	@param [out] predicate
		the pointer to a pointer variable to store @ref GSRowKeyPredicate
		instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the specified type is always unsupported as a Row key
		- if @c NULL is specified to pointer type arguments
	@see GSContainer
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateRowKeyPredicate(
		GSGridStore *store, GSType keyType, GSRowKeyPredicate **predicate);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

//
// Container API
//

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定の@ref GSContainer インスタンスについて、必要に応じこのインスタンス
		ならびに関連するリソースを解放します。
	@par
		トランザクションを保持している場合、未コミットの更新内容はすべて元に戻されます。
	@par
		この処理を行うために接続障害が発生したとしても、ローカルリソースの解放処理は
		適宜実施されます。ただし、GridDB上のトランザクション状態などは状態などは
		残る可能性があります。
	@param [in,out] container
		クローズ対象の@ref GSContainer インスタンスを指しているポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSContainer インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。
	@param [in] allRelated
		指定の@ref GSContainer と関連する下位のリソースのうち、未クローズのものすべてを
		クローズするかどうか。
		関連する下位のリソースとは、指定の@ref GSContainer を介して取得した
		@ref GSQuery 、@ref GSAggregationResult 、ならびに、これらのリソースと関連する
		下位のリソースのことを指します。
		@ref GS_FALSE を指定した場合、指定の@ref GSContainer を介して取得した
		リソースを個別にクローズする必要があり、すべてクローズした時点で
		指定の@ref GSContainer 自体のリソースが解放されます。

	@EN
	@ingroup Group_GSContainer
	@brief Releases the specified @ref GSContainer instance and related
		resources as necessary.
	@par
		When a transaction is held, uncommitted updates will be rolled back.
	@par
		Even if a connection failure occurred while processing this request,
		the local resources will be released properly. However, the transaction
		state might remain in GridDB.
	@param [in,out] container
		the pointer to a pointer variable that refers to @ref GSContainer
		instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSContainer instance which was
		already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.
	@param [in] allRelated
		Indicates whether all unclosed resources in the lower resources related
		to the specified @ref GSContainer will be closed or not.
		The related lower resources point out @ref GSQuery,
		@ref GSAggregationResult, and its resources which are obtained via
		specified @ref GSContainer.
		If @ref GS_FALSE is specified, it is necessary to individually close
		the acquired resources through @ref GSContainer. And the specified
		@ref GSContainer itself will be released when all resources are closed.

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseContainer(
		GSContainer **container, GSBool allRelated);

#if GS_DEPRECATED_FUNC_ENABLED
/*!
	@JP
	@ingroup Group_GSContainer
	@brief 変更結果の通知先の一つを設定します。
	@par
		指定のコンテナに変更があると、指定のURLにリクエストが送信されるようになります。
		作成済みの場合は何も変更しません。
	@par
		リクエストの通知条件・内容の詳細は次の通りです。
		- 通知条件
			- 指定のコンテナに対するロウ新規作成・更新・削除操作命令の実行後、
				結果が確定(コミット)した場合。自動コミットの場合は、該当するロウ操作命令が
				実行されるたびに通知。複数ロウ一括操作の場合、1回の命令ごとに1度だけ通知。
				結果として内容に変更がない場合でも、通知されることがある。また、設定されている
				URLに通知を行っても一定時間以内に応答がない場合、タイムアウトする。
		- プロトコル
			- HTTP 1.1 (SSLは未サポート)
		- リクエストメソッド
			- POST
		- URL書式
			-
	‾‾‾
	http://(ホスト名):(ポート番号)/(パス)
	‾‾‾
		- MIMEタイプ
			- application/json
		- リクエスト内容
			-
	‾‾‾
	{
		"update" : {
			"container" : "(コンテナ名)"
		}
	}
	‾‾‾
	@par
		GridDBからの通知の際に、設定されている通知先URLにリクエストに対応する
		HTTPサーバが応答しない場合、タイムアウト時刻までの待機処理が発生します。
		この待機処理は、指定のコンテナならびに他の一部のコンテナに対するロウの
		新規作成・更新・削除操作のレスポンスが低下する要因となります。
		したがって、無効となった通知先URLは@ref gsDropEventNotification
		により削除することが推奨されます。
	@par
		処理対象のコンテナにおいて実行中のトランザクションが存在する場合、
		それらの終了を待機してから設定を行います。
	@param [in] container
		設定対象の@ref GSContainer
	@param [in] url
		設定対象の通知先URL文字列
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のURLが規定の構文に合致しない場合
		- この処理のタイムアウト、指定のコンテナの削除、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Sets one of the notification destination for the change results.
	@par
		If there are any changes in the specified Container, a request will be
		sent to the specified URL.
		If it is already created, nothing is changed.
	@par
		The details of the notification condition and the content for requests
		are as shown below.
		- Notification condition
			- After executing the create/update/delete Row operations to the
			specified Container, notification will be sent if results are
			committed. In auto commit mode, notification will be sent whenever
			the corresponding Row operation is executed. If bulk operations for
			multiple Row are specified, notification will be sent for each
			instruction. Even if there is no change in the content as a result,
			notification may be sent. In addition, if there is no response
			within a certain period of time for the notification which has been
			sent to the specified URL, the notification timeout occurs.
		- Protocol
			- HTTP 1.1 (SSL is not supported)
		- Request methods
			- POST
		- URL format
			-
	‾‾‾
	http://(host name):(port number)/(path)
	‾‾‾
		- MIME type
			- application/json
		- Request content
			-
	‾‾‾
	{
		"update" : {
			"container" : "(container name)"
		}
	}
	‾‾‾
	@par
		If the HTTP server does not respond to a request sent to the
		notification destination URL that has been set up when sending a
		notification from GridDB, standby processing will occur until the
		process times out.
		This standby process becomes a cause of the lower response to a Row
		operations such as creation, updates, or deletion in the specified
		Container as well as some other Containers.
		Therefore, an invalid notification destination URL is recommended
		to be deleted by using @ref gsDropEventNotification.
	@par
		When a transaction(s) is active in a target Container, it configures
		the settings after waiting for the transaction(s) to complete.
	@param [in] container
		Target @ref GSContainer to be set
	@param [in] url
		Target string of notification destination URL to be set
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the specified URL does not conform to the stipulated syntax
		- if a timeout occurs during this operation, a specified Container is
			deleted, a connection failure occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
	GSResult GS_API_CALL gsCreateEventNotification(
	GSContainer *container, const GSChar *url));

#endif	// GS_DEPRECATED_FUNC_ENABLED

#if GS_COMPATIBILITY_SUPPORT_1_5
/*!
	@JP
	@ingroup Group_GSContainer
	@brief トリガを設定します。
	@par
		指定のコンテナに対して特定の種別の更新操作が行われた場合に、
		指定のURIに通知が送信されるようになります。
		指定されたトリガと同名のトリガが存在した場合、設定内容が上書きされます。
	@par
		トリガ設定内容の詳細は、@ref GSTriggerInfo の定義を参照してください。
		トリガ名、トリガ種別、通知条件、通知先URI、通知内容の詳細は以下の通り
		です。

	@par トリガ名
		トリガ種別や通知条件などの違いによらず、1つのコンテナのトリガ間で、
		ASCIIの大文字・小文字表記を含め同一の名前のものを複数定義することは
		できません。その他、トリガの定義において使用できるトリガ名の文字種や
		長さには制限があります。具体的には、GridDBテクニカルリファレンスを
		参照してください。特に記載のない限り、トリガ名を指定する操作では、
		ASCIIの大文字・小文字表記の違いが区別されます。

	@par トリガ種別
		次のトリガ種別をサポートします。
		<table>
		<tr><th>名称</th><th>説明</th></tr>
		<tr>
		<td>REST</td>
		<td>コンテナに指定された種別の更新操作が行われた際に、
		指定されたURIにREST(HTTP POSTメソッド)で通知するトリガです。</td>
		</tr>
		<tr>
		<td>Java Message Service(JMS)</td>
		<td>コンテナに指定された種別の更新操作が行われた際に、
		指定されたURIのJMSサーバへJMSメッセージを通知するトリガです。
		JMSプロバイダとしてApache ActiveMQを使用します。</td>
		</tr>
		</table>

	@par 通知条件
		指定のコンテナに対するロウ新規作成/更新
		(@ref gsPutRow 、@ref gsPutMultipleRows 、
		@ref gsPutMultipleContainerRows 、 @ref gsUpdateCurrentRow)・
		削除(@ref gsDeleteRow 、@ref gsDeleteCurrentRow)
		操作命令の実行直後に通知を行います。
		監視対象として複数の操作が指定された場合は、
		そのうちのいずれかが実行された際に通知を行います。
	@par
		通知を行った時点でのレプリケーションの完了は保証されません。
		自動コミットモード無効で実行されたロウ新規作成/更新・
		削除命令に対応する通知については、
		通知を行った時点でトランザクションが未コミットであったり、
		通知後にトランザクションがアボートされたりした場合、
		通知を受けた時点で通知に含まれるデータが取得できないことがあります。
	@par
		複数ロウ一括操作の場合、1件のロウ操作ごとに通知を行います。
		指定されたURIに通知を行っても一定時間以内に応答がない場合、
		タイムアウトし再送は行いません。
		GridDBクラスタに障害が発生した場合、
		ある更新操作に対応する通知が行われないことのほか、
		複数回通知されることがあります。

	@par 通知先URI
		通知先URIは次の書式で記述します。
	~~~
		(メソッド名)://(ホスト名):(ポート番号)/(パス)
	~~~
		ただし、トリガ種別がRESTの場合、メソッド名にはhttpのみ指定できます。

	@par 通知内容
		更新が行われたコンテナ名、更新操作名、
		更新されたロウデータの指定したカラムの値を通知します。
		更新操作名は、ロウ新規作成/更新では@c "put" 、
		削除では@c "delete" となります。
	@par
		通知する値は、ロウ新規作成では新規作成直後、更新では更新後、
		削除では削除前のロウデータについての、指定カラムの値となります。
		カラムの型がTIMESTAMPの場合、<tt>1970-01-01T00:00:00Z</tt>
		からの経過ミリ秒を示す整数が値として設定されます。
		カラムの型が、BLOB型、GEOMETRY型、配列型の場合、
		空文字列が値として設定されます。

	@par 通知方法―RESTの場合
		以下のようなJSON文字列を、MIMEタイプapplication/jsonで送信します。
	~~~
		{
			"container" : "(コンテナ名)",
			"event" : "(更新操作名)",
			"row" : {
				"(カラム名)" : (カラムデータ),
				"(カラム名)" : (カラムデータ),
				...
			}
		}
	~~~

	@par 通知方法―JMSの場合
		javax.jms.TextMessageを、指定されたデスティネーション種別・
		デスティネーション名で送信します。
	@par
		コンテナ名は、
		<tt>javax.jms.Message#setStringProperty("@container", "(コンテナ名)")</tt>
		で設定されます。
		更新操作名は、
		<tt>javax.jms.Message#setStringProperty("@event", "(更新操作名)")</tt>
		で設定されます。
	@par
		カラムの値は、カラムの型に応じた
		<tt>javax.jms.Message#setXXXProperty("(カラム名)", (カラムデータ))</tt>
		で設定されます。
	@par
		トリガが設定されているコンテナに対して@ref gsPutCollection 、@ref gsPutTimeSeries
		などによりカラムレイアウトが変更された際に、
		トリガの通知対象となっているカラムの削除または名称変更があった場合、
		該当するカラムはトリガの通知対象から削除されます。
	@par
		GridDBからの通知の際に、設定されている通知先URIへのリクエストに
		対応するサーバが応答しなかった場合、タイムアウト時刻までの待機処理が
		発生します。この待機処理は、このコンテナならびに他の一部のコンテナの
		更新に対する通知が遅れる要因となります。
		したがって、無効となった通知先URIを持つトリガは
		@ref gsDropTrigger により削除することが推奨されます。
	@par
		一つのコンテナに対して設定できるトリガの最大数、ならびに、トリガの各種
		設定値の上限については、GridDBテクニカルリファレンスを参照してください。

	@param [in] container
		設定対象の@ref GSContainer
	@param [in] info
		設定対象のトリガ情報
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- トリガ名が@c NULL 空、またはその他の規則に合致しない場合
		- 監視対象更新操作の指定がない場合
		- 通知先のURIが規定の構文に合致しない場合
		- トリガ種別でJMSが指定され、かつJMSデスティネーション種別が@c NULL 、
			または空、または指定の書式に合致しない場合
		- トリガ種別でJMSが指定され、かつJMSデスティネーション名が@c NULL 、
			または空の場合
		- この処理のタイムアウト、指定のコンテナの削除、接続障害が発生した場合
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSContainer
	@brief Set the trigger.
	@par
		If an update operation of a specific type is carried out on the specified
		Container, a notification will be sent to the specified URI. If a
		trigger with the same name as the specified trigger exists, the setting
		will be overwritten.
	@par
		Refer to the definition of @ref GSTriggerInfo for the detailed trigger
		settings. The details of the trigger name, the trigger type, notification conditions,
		notification destination URI and notification contents are as shown
		below.

	@par Trigger name
		Multiple trigger names which are identified as the same,
		including the names only different in ASCII uppercase and
		lowercase characters, even if they have different types or
		notification conditions, in a container cannot be
		defined. And there are the limitations, the allowed characters
		and the length, on the trigger names. See the GridDB
		Technical Reference for the details. Trigger names are
		case-sensitive unless otherwise noted.

	@par Trigger type
		The following trigger types are supported.
		<table>
		<tr><th>Name</th><th>Description</th></tr>
		<tr>
		<td>REST</td>
		<td>Trigger to notify the specified URI with a REST notification (HTTP
		POST method) when an update operation of the specified type is carried
		out on a Container.</td>
		</tr>
		<tr>
		<td>Java Message Service(JMS)</td>
		<td>Trigger to send a JMS message to the JMS server of the specified URI
		when an update operation of the specified type is carried out on a
		Container. Apache ActiveMQ is used as the JMS provider.</td>
		</tr>
		</table>

	@par Notification condition
		Create new Row/ update Row for the specified Container (@ref gsPutRow,
		@ref gsPutMultipleRows, @ref gsPutMultipleContainerRows,
		@ref gsUpdateCurrentRow), delete (@ref gsDeleteRow,
		@ref gsDeleteCurrentRow) Perform notification immediately after
		executing operation command. If multiple operations are specified as
		monitoring targets, perform notification after executing any one of
		these operations.
	@par
		Completion of replication at the point notification is carried out is
		not guaranteed. For notifications corresponding to a create new Row/
		update Row or delete command that has been executed by disabling the
		auto commit mode, if the transaction is not committed at the point of
		the notification, or if the transaction is aborted after the
		notification, it may not be possible to get the data included in the
		notification at the point the notification is received.
	@par
		For batch operations involving multiple Rows, notification is carried
		out for each Row operation. If there is no response within a specified
		time even if notification has been sent to the specified URl, time out
		is performed and it will not be sent again. If a failure occurs in a
		GridDB cluster, in addition to not sending any notification to support
		a certain update operation, multiple notifications may be sent.

	@par Notification destination URI
		A notification destination URI is described in the following format.
	~~~
		(method name)://(host name):(port number)/(path)
	~~~
		However, if the trigger type is REST, only http can be specified in the
		method name.

	@par Notification contents
		Provide notification of the updated Container name, update operation
		name, and specified column value of the updated Row data. For the update
		operation name, use @c "put" to create a new Row/ update Row and
		@c "delete" to delete.
	@par
		The notification value shall be the specified column value of the Row
		data that is newly created immediately after a new Row is created, or
		updated in an update operation, or before deletion in a delete
		operation. If the column type is TIMESTAMP, an integer to indicate the
		time passed in milliseconds starting from <tt>1970-01-01T00:00:00Z</tt>
		is set as the value. If the column type if BLOB, GEOMETRY, or array, a
		blank character string will be set as the value.

	@par Notification method - For REST
		JSON character strings such as those shown below are sent with the MIME
		type application/json.
	~~~
		{
			"container" : "(container name)",
			"event" : "(update operation name)",
			"row" : {
				"(column name)" : (column data),
				"(column name)" : (column data),
				...
			}
		}
	~~~

	@par Notification method - For JMS
		A javax.jms.TextMessage is sent with the specified destination type and
		destination name.
	@par
		The container name is set by
		<tt>javax.jms.Message#setStringProperty("@container", "(container name)")</tt>.
		The update operation name is set by
		<tt>javax.jms.Message#setStringProperty("@event", "(update operation name)")</tt>.
	@par
		The column value is set with a
		<tt>javax.jms.Message#setXXXProperty("(column name)", (column data))</tt>
		in accordance with the column type.
	@par
		When the column layout is changed by a @ref gsPutCollection,
		@ref gsPutTimeSeries, etc. in relation to a Container with a set
		trigger, if a column subject to trigger notification is deleted or if
		its name is changed, the corresponding column will be deleted from the
		trigger notification targets.
	@par
		If the server does not respond to a request sent to the notification
		destination URI that has been set up when sending a notification from
		GridDB, standby processing will occur until the process times out. This
		standby process becomes a cause for the delay in serving notification of
		an update in the Container as well as some other containers. Therefore,
		a trigger having an invalid notification destination URI is recommended
		to be deleted by using @ref gsDropTrigger.
	@par
		Refer to the GridDB Technical Reference for the maximum number
		of triggers that can be set for a single Container and the upper limit
		of the values for various trigger settings.

	@param [in] container
		Target @ref GSContainer to be set
	@param [in] info
		Target trigger information to be set
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the trigger name is @c NULL, or blank
		- if the update operation subject to monitoring is not specified
		- if the notification destination URI does not conform to the stipulated
			syntax
		- if the JMS is specified by the trigger type, and the JMS destination
			type is @c NULL, or is blank, or does not conform to the specified
			format
		- if the JMS is specified by the trigger type, and the JMS destination
			name is @c NULL, or is blank
		- if a timeout occurs during this operation, a specified Container is
			deleted, a connection failure occurs
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateTrigger(
		GSContainer *container, const GSTriggerInfo *info);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定された名前のカラムに対し、指定された種別で名前のない索引を
		作成します。
	@par
		カラム名と種別のみが設定された@ref GSIndexInfo を指定して
		@ref gsCreateIndexDetail を呼び出した場合と同様に振る舞います。
		ただし、@c flags にデフォルト種別を含め一つも種別が指定されていない
		場合、索引は作成されません。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] columnName
		処理対象のカラムの名前
	@param [in] flags
		作成する索引種別のフラグ値のビット和。
		指定できる値は@ref gsCreateIndexDetail の場合と同様です
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のカラム名と種別が@ref gsCreateIndexDetail の規則に合致しない場合
		- この処理のタイムアウト、指定のコンテナの削除もしくはスキーマ変更、接続障害が
			発生した場合
		- @c flags 以外の引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Create an unnamed index with the specified type for column with a specified name.
	@par
		It behaves as if @ref gsCreateIndexDetail is called with @ref GSIndexInfo with only
		column name and type set.
		However, if none of the default types are included in @c flags, no index is created.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] columnName
		Column name to be processed
	@param [in] flags
		Sum of bits of flag value of index type to be created.
		Possible values are the same as in @ref gsCreateIndexDetail.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- If the specified column name and type does not match the rule of @ref gsCreateIndexDetail.
		- if a timeout occurs during this operation, a specified Container is
			deleted or its schema is changed, a connection failure occurs
		- if @c NULL is specified to arguments except @c flags

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateIndex(
		GSContainer *container,
		const GSChar *columnName, GSIndexTypeFlags flags);

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief @ref GSIndexInfo で設定されている内容に従い、索引を作成します。
	@par
		作成対象の索引のカラムについては、カラム名またはカラム番号の
		少なくとも一方が設定されており、かつ、対応するコンテナにおいて実在する
		ものが設定されている必要があります。カラム名とカラム番号が共に設定
		されていた場合、対応するカラムが互いに一致しなければなりません。
	@par
		索引種別が一つも設定されていないか@ref GS_INDEX_FLAG_DEFAULT が設定
		されていた場合、後述の基準に従い、デフォルト種別の索引が選択されます。
		それ以外の場合、対象のカラムにおいて許されている索引種別である限り、
		一つ以上の種別を指定できます。複数個の種別が設定されていた場合、
		作成途中に一部の索引のみが作成された状態のコンテナ情報を参照できる
		ことや、エラーが生じるとその状態まま作成操作が終了することがあります。
	@par
		1つのコンテナの索引間で、ASCIIの大文字・小文字表記だけが異なる
		名前のものを複数定義することはできません。その他、索引の定義において
		使用できる索引名の文字種や長さには制限があります。具体的には、
		GridDBテクニカルリファレンスを参照してください。特に記載のない限り、
		索引名を指定する操作では、ASCIIの大文字・小文字表記の違いは
		区別されません。
	@par
		既存の同名の索引が存在した場合、後述の条件を満たす同一設定の
		@ref GSIndexInfo を指定しなければならず、その場合新たな索引は作成
		されません。一方、既存の異なる名前の索引または名前のない索引と同一
		設定の@ref GSIndexInfo を指定することはできません。
	@par
		索引名が設定されていない場合は、名前のない索引の作成が要求された
		ものとみなされます。名前を除いて同一設定の索引がすでに存在していた場合、
		名前のない索引でなければならず、その場合新たな索引は作成されません。
	@par
		現バージョンでは、少なくとも@ref GSContainer を通じて作成された
		索引において、次の条件を満たす場合に索引名を除いて同一設定の
		索引であるとみなされます。
		- 索引対象のカラムが一致すること。カラム名、カラム番号といった、
			カラムの指定方法の違いは無視される
		- 索引種別が一致すること。デフォルト指定の有無といった索引種別の
			指定方法の違いは無視される
	@par
		現バージョンにおける、@ref gsGetDefaultFactory を基に生成された
		@ref GSContainer インスタンスでは、コンテナの種別、対応するカラムの
		型などに基づき、次の索引種別がデフォルトとして選択されます。
		カラムの型 | コレクション | 時系列
		-|-|-
		STRING | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE
		BOOL | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE
		数値型 | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE
		TIMESTAMP | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE ※制限あり
		GEOMETRY | @ref GS_INDEX_FLAG_SPATIAL | (なし)
		BLOB | (なし) | (なし)
		配列型 | (なし) | (なし)
	@par
		時系列のロウキー(TIMESTAMP型)には索引を設定できません。
	@par
		この@ref GSContainer インスタンスが未コミットのトランザクションを
		保持していた場合、コミットしてから作成を行います。処理対象のコンテナ
		において実行中の他のトランザクションが存在する場合、それらの終了を
		待機してから作成を行います。すでに索引が存在しており新たな索引が
		作成されなかった場合、他のトランザクションによって待機するかどうかは
		未定義です。またこの場合、この@ref GSContainer インスタンスが保持
		している未コミットのトランザクションが常にコミットされるかどうかは
		未定義です。
	@par
		現バージョンでは、コンテナの規模など諸条件を満たした場合、索引の
		作成開始から終了までの間に、処理対象のコンテナに対してコンテナ情報の
		参照、一部の索引操作、トリガ操作、ロウ操作(更新含む)を行える場合が
		あります。それ以外の操作は、@ref GSContainer での説明通り待機させる
		場合があります。索引の作成途中に別の操作が行われる場合は、
		作成途中の索引に関する情報はコンテナ情報には含まれません。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] info
		処理対象の索引の情報
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- 作成対象のカラム、索引名が上記の規則に合致しない場合
		- この処理のタイムアウト、指定のコンテナの削除もしくはスキーマ変更、接続障害が
			発生した場合
		- 指定のカラムにおいてサポートされていない索引種別が指定された場合
		- 引数に@c NULL が指定された場合
	@since 3.5

	@EN
	@brief Create index according to the contents set in @ref GSIndexInfo.
	@par
		For the column of the index to be created, at least one of the column name
		and column number must be set, and the actual container must be set in the
		corresponding container.If both column name and column number are set,
		the corresponding columns must match each other.
	@par
		If no index type is set but @ref GS_INDEX_FLAG_DEFAULT is set, the default
		type index is selected according to the criteria described below.
		In other cases, you can specify one or more types as long as it is an allowed
		index type in the target column. If more than one type is set, it is possible
		to refer to container information in which only a part of the index is created
		during creation, or the creation operation may be terminated in that state
		if an error occurs.
	@par
		If an index name is set, a new index will be created only if there is no index
		with the same name in the target container. Only ASCII alphanumeric characters
		and underscores ("_") are allowed in index names. However, numbers cannot be used
		at the beginning. Please also note that the name is case-insensitive.
		Please refer to the GridDB Technical Reference for the upper limit of name length.
	@par
		If an existing index with the same name exists, you must specify @ref GSIndexInfo
		of the same setting that satisfies the conditions described below. In this case
		no new index will be created. On the other hand, it is not possible to specify
		@ref GSIndexInfo with the same setting as the existing index of different name
		or unnamed.
	@par
		When the index name is not set, it will be treated as an unnamed index. Excluding names,
		if an index with the same setting already exists, it is considered as an unnamed index,
		which in this case no new index will be created.
	@par
		In the current version, the index created using @ref GSContainer is considered to
		be identical if the following conditions are satisfied.
		- The columns to be indexed are matching. The differences in column specification
		methods, such as column names and column numbers are not taken into consideration.
		- Index types are matching. Differences in the specification method of index type
		such as existence of default designation are not taken into consideration.
	@par
		In the current version, for @ref GSContainer instances generated based on
		@ref gsGetDefaultFactory, the following index types are selected as defaults
		depending on the type of container, the type of the corresponding column, and so on.
		Column type | collection | time series
		-|-|-
		STRING | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE
		BOOL | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE
		NUMBER | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE
		TIMESTAMP | @ref GS_INDEX_FLAG_TREE | @ref GS_INDEX_FLAG_TREE ※there is a limitation
		GEOMETRY | @ref GS_INDEX_FLAG_SPATIAL | (None)
		BLOB | (None) | (None)
		ARRAY | (None) | (None)
	@par
		Index cannot be set for time-series row keys (TIMESTAMP).
	@par
		If this @ref GSContainer instance holds an uncommitted transaction, it will be created
		after the commit. If there are other transactions being executed in the container, wait
		for them to finish prior to the creation. If an index already exists and a new index is
		not created, it is undefined whether to wait by another transaction. Also in this case
		it is undefined whether uncommitted transactions held by this @ref GSContainer instance
		will always be committed or not.
	@par
		In the current version, when various conditions such as the size of a container are satisfied,
		during the period from the creation of the index to the end, it may be possible to reference
		container information, some index operations, trigger operations, row operations
		(including updating) for the container to be processed. For other operations, as described in
		@ref GSContainer, you may have to wait. If another operation is performed during index creation,
		the index information will not be included in the container information.
	@param [in] container
		@ref GSContainer to be processed.
	@param [in] info
		Information on index to be processed.
	@return code number of the results. In the following case, a value other than @ref GS_RESULT_OK
		will be returned.
		- If the column or index name to be created does not conform to the above rule
		- When a connection failure, deletion of specified container, schema change or timeout occurs.
		- When an unsupported index type is specified in the specified column
		- When @c NULL is specified as an argument
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateIndexDetail(
		GSContainer *container, const GSIndexInfo *info);

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

#if GS_DEPRECATED_FUNC_ENABLED
/*!
	@JP
	@ingroup Group_GSContainer
	@brief 変更結果の通知先の一つを削除します。
	@par
		削除済みの場合は何も変更しません。
	@par
		処理対象のコンテナにおいて実行中のトランザクションが存在する場合、
		それらの終了を待機してから削除を行います。
	@param [in] container
		削除対象の@ref GSContainer
	@param [in] url
		削除対象の通知先URL文字列
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理のタイムアウト、指定のコンテナの削除、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Delete one of the notification destination for update operation.
	@par
		If the notification destination is already deleted, nothing is changed.
	@par
		When a transaction(s) is active in a target Container, it removes the
		index after waiting for the transaction(s) to be completed.
	@param [in] container
		@ref GSContainer to be deleted
	@param [in] url
		string of notification destination URL to be deleted
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation, a specified Container is
			deleted, a connection failure occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
	GSResult GS_API_CALL gsDropEventNotification(
	GSContainer *container, const GSChar *url));

#endif	// GS_DEPRECATED_FUNC_ENABLED

#if GS_COMPATIBILITY_SUPPORT_1_5
/*!
	@JP
	@ingroup Group_GSContainer
	@brief トリガを削除します。
	@par
		指定された名前のトリガが存在しない場合は何も削除しません。
	@param [in] container
		削除対象の@ref GSContainer
	@param [in] name
		削除対象のトリガ名
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理のタイムアウト、指定のコンテナの削除、接続障害が発生した場合
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSContainer
	@brief Delete a trigger.
	@par
		Nothing is changed if the trigger which has the specified name does not
		exist.
	@param [in] container
		@ref GSContainer to be deleted
	@param [in] name
		Trigger name to be deleted
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation, a specified Container is
			deleted, a connection failure occurs
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDropTrigger(
		GSContainer *container, const GSChar *name);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定された名前のカラムのうち、指定された種別の索引のみを削除します。
	@par
		カラム名と種別のみが設定された@ref GSIndexInfo を指定して
		@ref gsDropIndexDetail を呼び出した場合と同様に振る舞います。
		ただし、@c flags にデフォルト種別を含め一つも種別が指定されていない
		場合、いずれの索引も削除対象にはなりません。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] columnName
		処理対象のカラムの名前
	@param [in] flags
		削除する索引種別のフラグ値のビット和。
		指定できる値は@ref gsDropIndexDetail の場合と同様です
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のカラム名と種別が@ref gsDropIndexDetail の規則に合致しない場合
		- この処理のタイムアウト、指定のコンテナの削除もしくはスキーマ変更、接続障害が
			発生した場合
		- @c flags 以外の引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Removes the index of the specified type from the columns with the specified name.
	@par
		Specifying @ref gsDropIndexDetail with setting only the column name and type
		is the same as calling @ref GSIndexInfo.
		However, if no category is specified for @c flags including the default type,
		neither index will be deleted.
	@par
		When a transaction(s) is active in a target Container, it removes the
		index after waiting for the transaction(s) to be completed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] columnName
		Column name to be processed
	@param [in] flags
		Sum of bits of flag value of index type to be deleted.
		The possible values are the same as @ref gsDropIndexDetail
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- If the specified column name and type does not match the rule of @ref gsDropIndexDetail
		- if a timeout occurs during this operation, a specified Container is
			deleted or its schema is changed, a connection failure occurs
		- if @c NULL is specified to arguments except @c flags

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDropIndex(
		GSContainer *container,
		const GSChar *columnName, GSIndexTypeFlags flags);

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief @ref GSIndexInfo で設定されている内容に一致する、すべての索引を
		削除します。
	@par
		@ref GSIndexInfo の設定内容は、削除対象の索引を絞り込む条件として
		使用されます。絞り込み条件は、カラム、索引種別、索引名の3つに分類
		されます。それぞれ設定するかどうかは任意です。いずれも設定されていない
		場合は、作成済みのすべての索引が削除されます。
	@par
		カラム名 またはカラム番号が設定されている場合、対応するコンテナにおいて
		実在するものである必要があります。カラム名とカラム番号が共に設定されている
		場合、対応するカラムが互いに一致している必要があります。カラム名ならびに
		カラム番号が共に設定されていない場合、他の絞り込み条件(索引種別、
		索引名)を満たす任意のカラムに対する索引が削除対象となります。
	@par
		索引種別が設定されている場合、指定の種別の索引のみが削除対象と
		なります。@ref GS_INDEX_FLAG_DEFAULT が設定されている場合、
		@ref gsCreateIndexDetail の基準に従い、デフォルト種別の索引が
		選択されます。それ以外の場合、対象のカラムにおいて許されている索引種別
		である限り、任意個数の種別を指定できます。複数個の種別が設定されていた
		場合、削除途中に一部の索引のみが削除された状態のコンテナ情報を参照
		できることや、エラーが生じるとその状態まま削除操作が終了することが
		あります。索引をサポートしていないカラムや指定の種別の索引をサポート
		していないカラムについては、削除対象にはなりません。索引種別が設定されて
		いない場合、他の絞り込み条件(カラム、索引名)を満たす任意の種別の索引が
		削除対象となります。
	@par
		索引名が設定されている場合、指定の名前の索引のみが削除対象と
		なります。索引名の同一性は、@ref gsCreateIndexDetail の基準に
		従います。索引名が設定されていない場合、他の絞り込み条件(カラム、
		索引種別)を満たす、任意の名前の索引ならびに名前のない索引が削除対象
		となります。
	@par
		削除対象となる索引が一つも存在しない場合、索引の削除は行われません。
	@par
		トランザクションの扱いは、@ref gsCreateIndexDetail と同様です。また、
		索引種別としてデフォルト種別または単一の種別が設定されており、かつ、
		複数の索引が削除対象となった場合に、一部の索引のみが 削除された状態で
		他のトランザクションが実行されることがありうるかどうかは未定義です。
	@par
		索引の削除要求の完了直後の状態に関しては、@ref gsDropContainer
		と同様です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] info
		処理対象の索引の情報
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- 削除対象のカラム、索引名が上記の規則に合致しない場合
		- この処理のタイムアウト、指定のコンテナの削除もしくはスキーマ変更、接続障害が
			発生した場合
		- 引数に@c NULL が指定された場合
	@since 3.5

	@EN
	@brief Delete all indexes that match the contents set in @ref GSIndexInfo
	@par
		The contents of @ref GSIndexInfo are used as a condition to narrow down
		the index to be deleted. Filtering conditions are classified into three categories:
		column, index type, and index name. It is optional to set these categories.
		If none of them are set, all created indexes will be deleted.
	@par
		When setting column name or column number, it must be done within the
		corresponding container. When both column name and column number are set,
		the corresponding columns must match each other. If neither the column name
		nor the column number are set, the index for any column that meets other
		narrowing conditions (index type, index name) will be deleted.
	@par
		When index type is set, only the index of the specified type will be deleted.
		If @ref GS_INDEX_FLAG_DEFAULT is set, the default type index is selected
		according to the criteria of @ref gsCreateIndexDetail. Otherwise, you can
		specify any number of types as long as it is the allowed index type of the target
		column. When multiple types are set, the container information in which only
		some indexes are deleted during the deletion can be referenced, and when an error
		occurs, the deletion operation may end in that state. Columns that do not support
		indexes nor support indices of the specified type cannot be deleted. If the index
		type is not set, index of any type satisfying other narrowing conditions (column,
		index name) will be deleted.
	@par
		If an index name is set, only the index with the specified name will be deleted.
		The identity of the index name follows the criteria of @ref gsCreateIndexDetail.
		If an index name is not set, an index with an arbitrary name and an unnamed index
		satisfying other refinement conditions (column, index type) will be deleted.
	@par
		If there is no index to be deleted, the index will not be deleted。
	@par
		Transaction handling is similar to @ref gsCreateIndexDetail. In the case when
		default or single is set as the index type and multiple indexes are to be deleted,
		it is not clear whether other transactions could be performed or not.
	@par
		In regards to the immediate state after index deletion request is completed,
		it is the same as @ref gsDropContainer.
	@param [in] container
		to be processed @ref GSContainer
	@param [in] info
		information about the index to be processed.
	@return The execution result of code number. In the next case it will return a value
		other than @ref GS_RESULT_OK.
		- When the column or index name to be deleted does not conform to the above rule
		- Processing timeout, deletion of a specified container or schema changes, connection failure
		- when @c NULL is specified as an argument.
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsDropIndexDetail(
		GSContainer *container, const GSIndexInfo *info);

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@ingroup Group_GSContainer
	@brief これまでの更新結果をSSDなどの不揮発性記憶媒体に書き出し、
		すべてのクラスタノードが突然停止したとしても内容が失われないようにします。
	@par
		通常より信頼性が要求される処理のために使用します。
		ただし、頻繁に実行すると性能低下を引き起こす可能性が高まります。
	@par
		書き出し対象のクラスタノードの範囲など、挙動の詳細はGridDB上の
		設定によって変化します。
	@param [in] container
		処理対象の@ref GSContainer
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理のタイムアウト、指定のコンテナの削除もしくはスキーマ変更、接続障害が
			発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Writes the results of earlier updates to a non-volatile storage
		medium, such as SSD, so as to prevent the data loss even if all cluster
		nodes stop suddenly.
	@par
		It can be used for operations which require higher reliability than
		usual. However, frequent execution of this operation would potentially
		cause degradation in response time.
	@par
		The details of behavior, such as the scope of cluster nodes which are
		the data export targets, will be changed depending on the configuration
		of GridDB.
	@param [in] container
		@ref GSContainer to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation, a specified Container is
			deleted or its schema is changed, a connection failure occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsFlush(GSContainer *container);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief ロウキーに対応するロウの内容を取得します。
	@par
		ロウキーに対応するカラムが存在する場合のみ使用できます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		同様に、ロウキーの型が一致しない場合の動作も未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSContainer
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキーが格納された変数へのポインタ値。
		@ref GSContainer において定義されているコンテナ上のロウキーの型と
		この引数の型との関係は次のようになります。
		コンテナ上の型 | 引数の型
		-|-
		STRING | GSChar** <em>※GSChar*との取り違えに注意</em>
		INTEGER | int32_t*
		LONG | int64_t*
		TIMESTAMP | GSTimeStamp*
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして設定されていた場合
		- @c exists 以外の引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Returns the content of a Row corresponding to the specified Row key.
	@par
		It can be used only if a Column exists which corresponds to the
		specified Row key.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		In the same way as above, the behavior is undefined if the type of
		those Row keys does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSContainer.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		The pointer to a variable to store the target Row key.
		The relationship between the type of Row key in Container defined by
		@ref GSContainer and the type of argument is as follows.
		Type of Row key in Container | Type of argument
		-|-
		STRING | GSChar** <em>※Be careful not to confuse with GSChar*</em>
		INTEGER | int32_t*
		LONG | int64_t*
		TIMESTAMP | GSTimeStamp*
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no Column exists which corresponds to the specified Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRow(
		GSContainer *container, const void *key, void *rowObj, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 必要に応じ別途ロウキーを指定して、ロウを新規作成または更新します。
	@par
		ロウキーに対応するカラムが存在する場合、ロウキーとコンテナの状態を
		基に、ロウを新規作成するか、更新するかを決定します。この際、対応する
		ロウがコンテナ内に存在しない場合は新規作成、存在する場合は更新します。
		ロウオブジェクトとは別にロウキーを指定した場合、ロウオブジェクト内の
		ロウキーより優先して使用されます。
	@par
		ロウキーに対応するカラムを持たない場合、常に新規のロウを作成します。
		別途指定するロウキーには、常に@c NULL を指定します。
	@par
		ただし、コンテナの種別ならびに設定によっては、制限が設けられています。
		指定のコンテナが時系列であり、圧縮オプションが設定されている場合、
		以下の操作のみを条件付きで行うことができます。
		- 新規作成
			- 最も新しい時刻を持つ既存ロウより新しい時刻のロウを指定した場合のみ
		- 既存ロウの内容の保持
			- 最も新しい時刻を持つ既存ロウの時刻が指定の時刻と一致する場合のみ
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		同様に、ロウキーの型が一致しない場合の動作も未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキーが格納された変数へのポインタ値。
		@ref GSContainer において定義されているコンテナ上のロウキーの型と
		この引数の型との関係は、@ref gsGetRow の場合と同様です。
		ロウキーに対応するカラムが存在しない場合、もしくは指定のロウオブジェクト内の
		キーを用いる場合は@c NULL を指定します。
	@param [in] rowObj
		新規作成または更新するロウの内容と対応するロウオブジェクト
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するカラムが存在しないにもかかわらず、キーが指定された場合
		- 特定コンテナ固有の制限に反する操作を行った場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーまたはロウオブジェクトに含まれていた場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、指定のロウオブジェクト内のロウキー以外のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合。もしくは、ロウキーに対応するカラムが
			存在し@c key が@c NULL であるにもかかわらず、ロウキーの フィールドに
			同様に@c NULL が含まれていた場合

	@EN
	@ingroup Group_GSContainer
	@brief Newly creates or update a Row by specifying the Row key as needed.
	@par
		If a Column exists which corresponds to the specified Row key, it
		determines whether to newly create or update a Row, based on the Row key
		and the state of the Container. If there is no corresponding Row in the
		Container, it determines to newly create a Row; otherwise, it updates a
		relevant Row. If a Row key is specified besides a Row object, the
		specified Row key is used in preference to the Row key in the Row
		object.
	@par
		If no Column exists which corresponds to the specified Row key, it
		always creates a new Row. In such a case, specify @c NULL as key.
	@par
		However, there are some restrictions depending on the type of Container
		and its settings. It can be used only for the following operations
		conditionally if the specified Container is TimeSeries and its
		compression option is enabled.
		- Create New
			- Only if newer Row than the existing Row which has the most recent
				time is specified
		- Keep the contents of the existing Row
			- Only if the time of the existing Row which has the most recent
				time and the specified time matches
	@par
		In the manual commit mode, the target Row is locked.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		In the same way as above, the behavior is undefined if the type of
		those Row keys does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		the pointer to a variable to store the target Row key.
		The relationship between the type of Row key in Container defined by
		@ref GSContainer and the type of argument is same as in the case of
		@ref gsGetRow.
		@c NULL should be specified if the column corresponding to Row key is
		not existed, or if Row key in specified Row object is used.
	@param [in] rowObj
		A Row object representing the content of a Row to be newly created or
		updated.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a Row key is specified although no Column exists which corresponds
			to the key
		- if its operation is contrary to the restrictions specific to a
			particular Container
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is set in the Row key or the Row object
		- if @c NULL is specified to arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the fields other than Row key in the
			specified Row object, or even though the column corresponding to
			Row key exists and @c key is @c NULL, @c NULL is included in the
			field of Row key as well

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutRow(
		GSContainer *container, const void *key, const void *rowObj,
		GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定のロウオブジェクト集合に基づき、任意個数のロウをまとめて
		新規作成または更新します。
	@par
		指定のロウオブジェクト集合の各ロウについて、配列要素の順序にしたがって
		@ref gsPutRow を呼び出した場合と同様に新規作成または更新操作を行います。
	@par
		指定のロウオブジェクト集合内に同一のロウキーを持つ複数のロウが存在する場合、
		ロウオブジェクト集合を構成する配列要素の順序を基準として、
		同一のロウキーを持つ最も後方にあるロウオブジェクトの内容が反映されます。
	@par
		コンテナの種別ならびに設定によっては、操作できるロウの内容について
		@ref gsPutRow と同様の制限が設けられています。
		具体的な制限事項は、個別のコンテナ種別の定義を参照してください。
	@par
		手動コミットモードの場合、対象のロウがロックされます。
	@par
		自動コミットモードのときに、コンテナならびにロウに対する処理の途中で
		エラーが発生した場合、コンテナの一部のロウに対する操作結果のみが反映された
		ままとなることがあります。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		同様に、ロウキーの型が一致しない場合の動作も未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] rowObjs
		新規作成するロウ集合の内容と対応するロウオブジェクト列。
		このロウオブジェクト列は、個々のロウオブジェクトへのポインタ値の配列により
		構成されます。
		@c rowCount が@c 0 の場合、この配列を参照することはなく、
		@c NULL を指定することもできます。
	@param [in] rowCount
		新規作成するロウの個数。@c 0 の場合、ロウを新規作成せず正常に処理を終えます。
	@param [out] exists
		現バージョンでは、ポインタ値が@c NULL ではない限り常に@ref GS_FALSE が
		格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 特定コンテナ種別固有の制限に反する操作を行った場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がロウオブジェクトに含まれていた場合
		- @c container が@c NULL の場合
		- @c exists 以外のポインタ型引数に@c NULL が指定された場合。
			また、指定のロウオブジェクト内のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合
		- @c rowCount が正の値であるにもかかわらず、@c rowObjs
			に@c NULL が指定された場合
		- ロウオブジェクト列を構成する配列要素に@c NULL が含まれていた場合

	@EN
	@ingroup Group_GSContainer
	@brief Newly creates or updates an arbitrary number of Rows together based on the
		specified Row objects group.
	@par
		For each row of the specified row object set, when calling @ref gsPutRow,
		create or update is performed according to the order of the array elements.
	@par
		When there are multiple rows with the same row key in the specified row object
		set, the contents of the same most rear row object with the same row key are
		reflected based on the order of the array elements constituting the row object set.
	@par
		Depending on the container type and setting, the content of operable row have
		the same restriction as @ref gsPutRow.
		Please refer to the definition of individual container type for details of the
		restrictions.
	@par
		In the manual commit mode, the target Row is locked.
	@par
		In automatic commit mode, when an error occurs during containers and
		rows' processing, only the operation result for some of the containers'
		row will be reflected.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		In the same way as above, the behavior is undefined if the type of
		those Row keys does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] rowObjs
		The column of Row object corresponding to contents of newly created Row
		collection.
		This column of Row object consists of the array of pointers to each Row
		objects.
		If @c 0 is set to @c rowCount, @c NULL can be specified since the
		array is not referred in this function.
	@param [in] rowCount
		The number of Row to be newly created. If @c 0 is specified, the
		process will complete successfully without creating a new Row.
	@param [out] exists
		In the current version, @ref GS_FALSE is always stored unless the
		pointer value is not @c NULL.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- When an operation violating the restrictions unique to a specific
			Container type is carried out
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is included in the Row object
		- if @c NULL is specified to @c container
		- if @c NULL is specified to pointer type arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the fields within the specified Row
			object
		- if @c NULL is specified to @c rowObjs despite @c rowCount
			is a positive value
		- if @c NULL is included in the array element which consists of the
			column of Row objects

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutMultipleRows(
		GSContainer *container, const void *const *rowObjs, size_t rowCount,
		GSBool *exists);

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsCompatibleFunc_PutMultipleRows1(
		GSContainer *container, size_t rowCount, const void *const *rowObjs,
		GSBool *exists));
#endif

#if GS_COMPATIBILITY_GET_MULTIPLE_ROWS_BETA_0_3
#define gsPutMultipleRows(container, rowCount, rowObjs, exists) \
		gsCompatibleFunc_PutMultipleRows1(container, rowCount, rowObjs, exists)
#else
#define gsPutMultipleRows(container, rowObjs, rowCount, exists) \
		gsPutMultipleRows(container, rowObjs, rowCount, exists)
#endif
#endif	// not GS_INTERNAL_DEFINITION_VISIBLE

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定のTQL文を実行するためのクエリを作成します。
	@par
		@ref gsFetch を通じてロウ集合を求める際に更新用ロックのオプションを有効できるのは、
		指定のコンテナ上に実在しないロウが選択されることのないクエリのみです。
		たとえば、補間演算を含むクエリに対しては有効にできません。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] queryString
		TQL文
	@param [out] query
		@ref GSQuery インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Creates a query to execute the specified TQL statement.
	@par
		When obtaining a set of Rows using @ref gsFetch, the option of locking
		for update can be enabled only for the queries that will not select Rows
		which do not exist in specified Container. For example, it cannot be
		enabled for a query containing an interpolation operation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] queryString
		TQL statement
	@param [out] query
		the pointer to a pointer variable to store @ref GSQuery instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsQuery(GSContainer *container,
		const GSChar *queryString, GSQuery **query);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsRemoveRow(
		GSContainer *container, const void *key, GSBool *exists));
#endif

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定のロウキーに対応するロウを削除します。
	@par
		ロウキーに対応するカラムが存在する場合のみ使用できます。
		対応するロウが存在しない場合は何も変更しません。
	@par
		ただし、コンテナの種別ならびに設定によっては、制限が設けられています。
		圧縮オプションが設定された状態の時系列に対しては使用できません。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトのロウキーの型と
		指定のロウキーの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキーが格納された変数へのポインタ値。
		@ref GSContainer において定義されているコンテナ上のロウキーの型と
		この引数の型との関係は、@ref gsGetRow の場合と同様です。
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するカラムが存在しない場合
		- 特定コンテナ固有の制限に反する操作を行った場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして指定された場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、@c key に対応する文字列キーのポインタ値が@c NULL の場合

	@EN
	@ingroup Group_GSContainer
	@brief Deletes a Row corresponding to the specified Row key.
	@par
		It can be used only if a Column exists which corresponds to the
		specified Row key. If no corresponding Row exists, nothing is changed.
	@par
		However, there are some restrictions depending on the type of Container
		and its settings. It cannot be used to the TimeSeries whose compression
		option is enabled.
	@par
		In the manual commit mode, the target Row is locked.
	@attention
		The behavior is undefined if the type of Row key in the Row object
		bound to the specified @ref GSContainer and the type of specified Row
		key does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		the pointer to a variable to store the target Row key.
		The relationship between the type of Row key in Container defined by
		@ref GSContainer and the type of argument is same as in the case of
		@ref gsGetRow.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no Column exists which corresponds to the specified Row key
		- if its operation is contrary to the restrictions specific to a
			particular Container
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists; or if @c NULL
			is specified to pointer value of string key which corresponds to
			@c key

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDeleteRow(
		GSContainer *container, const void *key, GSBool *exists);

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定のコンテナの種別を取得します。
	@par
		現バージョンでは、インスタンス生成時点で常に種別が確定するため、
		この操作によりGridDBクラスタに問い合わせを行うことはありません。
	@param [in] container
		処理対象の@ref GSContainer
	@param [out] type
		指定のコンテナの種別を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_CONTAINER_COLLECTION が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSContainer
	@brief Return the type of specified Container.
	@par
		In the current version, no inquiry is sent to the GridDB cluster by this
		operation since the type is always decided when the instance is
		generated.
	@param [in] container
		@ref GSContainer to be processed
	@param [out] type
		the pointer to a variable to store the type of the specified Container.
		@ref GS_CONTAINER_COLLECTION is stored if non-@ref GS_RESULT_OK is
		returned as the execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetContainerType(
		GSContainer *container, GSContainerType *type);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 指定のコンテナのカラムレイアウトに基づき、ロウオブジェクトを新規作成します。
	@par
		作成される@ref GSRow の各フィールドには@ref gsCreateRowByStore
		により作成した場合と同様に既定の初期値が設定されます。
	@param [in] container
		処理対象の@ref GSContainer
	@param [out] row
		@ref GSRow インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSContainer
	@brief Create a new Row object based on the column layout of specified
		Container.
	@par
		The default initial values are set in each field of the created
		@ref GSRow, similar to the case when it is created using a
		@ref gsCreateRowByStore.
	@param [in] container
		@ref GSContainer to be processed
	@param [out] row
		the pointer to a pointer variable to store @ref GSRow instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsCreateRowByContainer(
		GSContainer *container, GSRow **row);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

//
// Container API (Transaction)
//

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 手動コミットモードにおいて、現在のトランザクションの操作結果を元に戻し、
		新たなトランザクションを開始します。
	@param [in] container
		処理対象の@ref GSContainer
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 自動コミットモードでないにもかかわらず呼び出した場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Rolls back the result of the current transaction and starts a new
		transaction in the manual commit mode.
	@param [in] container
		@ref GSContainer to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if called not in the auto commit mode
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, a connection failure occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAbort(GSContainer *container);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief 手動コミットモードにおいて、現在のトランザクションにおける操作結果を確定させ、
		新たなトランザクションを開始します。
	@param [in] container
		処理対象の@ref GSContainer
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 自動コミットモードでないにもかかわらず呼び出した場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除、接続障害が発生した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Commits the result of the current transaction and start a new
		transaction in the manual commit mode.
	@param [in] container
		@ref GSContainer to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if called not in the auto commit mode
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, a connection failure occurs
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsCommit(GSContainer *container);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief ロウキーに対応するロウについて、更新用ロックを獲得し内容を取得します。
	@par
		ロウキーに対応するカラムが存在する場合、かつ、
		手動コミットモードの場合のみ使用できます。
	@par
		トランザクションが終了するかタイムアウトするまで対象ロウのロックを維持します。
		ロックされたロウに対する他のトランザクションからの更新・削除操作は、
		このトランザクションが終了するかタイムアウトするまで待機するようになります。
		対象ロウが削除されたとしても、ロックは維持されます。
	@par
		取得結果のロウオブジェクトに含まれる文字列や配列などの可変長サイズの
		データのリソースは、指定の@ref GSContainer を直接介した次回のロウオブジェクト
		取得処理を実行するまで維持されます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		同様に、ロウキーの型が一致しない場合の動作も未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSContainer
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキーが格納された変数へのポインタ値。
		@ref GSContainer において定義されているコンテナ上のロウキーの型と
		この引数の型との関係は、@ref gsGetRow の場合と同様です。
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するカラムが存在しない場合
		- 自動コミットモードの場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして設定されていた場合
		- @c exists 以外の引数に@c NULL が指定された場合
	@see gsGetRow

	@EN
	@ingroup Group_GSContainer
	@brief Acquires a lock for update and returns the content of a Row
		corresponding to the specified Row key.
	@par
		It can be used only if a Column exists which corresponds to a specified
		Row key and the manual commit mode is enabled.
	@par
		It will hold the lock until a relevant transaction completes or a
		timeout occurs.
		The update or deletion operation by any other transaction on the locked
		Row will wait until the transaction completes or a timeout occurs.
		The lock will be held even if the target Row is deleted.
	@par
		Resources of data whose size is variable-length like string or array
		that is included in the Row object as acquisition result are maintained
		until the next Row object acquisition via the specified
		@ref GSContainer is executed.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		In the same way as above, the behavior is undefined if the type of
		those Row keys does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSContainer.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		The pointer to a variable to store the target Row key.
		The relationship between the type of Row key in Container defined by
		@ref GSContainer and the type of argument is same as in the case of
		@ref gsGetRow.
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no Column exists which corresponds to the specified Row key
		- if the auto commit mode is enabled
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists
	@see gsGetRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowForUpdate(
		GSContainer *container, const void *key, void *rowObj, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief コミットモードの設定を変更します。
	@par
		自動コミットモードでは、直接トランザクション状態を制御できず、
		変更操作が逐次コミットされます。
		自動コミットモードが有効でない場合、すなわち手動コミットモードの場合は、
		直接@ref gsCommit を呼び出すかトランザクションがタイムアウトしない限り、
		指定のコンテナ内で同一のトランザクションが使用され続け、
		変更操作はコミットされません。
	@par
		自動コミットモードが無効から有効に切り替わる際、未コミットの変更内容は
		暗黙的にコミットされます。コミットモードに変更がない場合、
		トランザクション状態は変更されません。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] enabled
		自動コミットモードを有効にするかどうか。@ref GS_TRUE の場合は自動コミットモード、
		@ref GS_FALSE の場合は手動コミットモードが有効になります。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- モード変更に伴いコミット処理を要求した際に、この処理または
			トランザクションのタイムアウト、指定のコンテナの削除もしくは
			スキーマ変更、接続障害が発生した場合
		- ポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSContainer
	@brief Change the setting of the commit mode.
	@par
		In the auto commit mode, the transaction state cannot be controlled
		directly and change operations are committed sequentially. If the auto
		commit mode is disabled, i.e. in the manual commit mode, as long as the
		transaction has not timed out or @ref gsCommit has been invoked
		directly, the same transaction will continue to be used in this
		Container and change operations will not be committed.
	@par
		When the auto commit mode is switched from disabled to enabled,
		uncommitted updates are committed implicitly. Unless the commit mode is
		changed, the state of the transaction will not be changed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] enabled
		indicates whether it enables auto commit mode or not. if @ref GS_TRUE,
		auto commit mode is enabled. if @ref GS_FALSE, manual commit mode is
		enabled.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, or a connection failure occurs,
			when a commit is requested after a mode change
		- if @c NULL is specified to pointer type arguments

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetAutoCommit(
		GSContainer *container, GSBool enabled);

//
// Container API (Type safe key)
//

/*!
	@JP
	@ingroup Group_GSContainer
	@brief INTEGER型のロウキーに対応するロウの内容を取得します。
	@par
		ロウキーに対応するINTEGER型のカラムが存在する場合のみ使用できます。
	@par
		手動コミットモードにおいて更新用ロックを要求した場合、
		トランザクションが終了するかタイムアウトするまで対象ロウのロックを維持します。
		ロックされたロウに対する他のトランザクションからの更新・削除操作は、
		このトランザクションが終了するかタイムアウトするまで待機するようになります。
		対象ロウが削除されたとしても、ロックは維持されます。
	@par
		自動コミットモードの場合、更新用ロックを要求できません。
	@par
		取得結果のロウオブジェクトに含まれる文字列や配列などの可変長サイズの
		データのリソースは、指定の@ref GSContainer を直接介した次回のロウオブジェクト
		取得処理を実行するまで維持されます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSContainer
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [in] forUpdate
		更新用ロックを要求するかどうか
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するINTEGER型のカラムが存在しない場合
		- 自動コミットモードでないにもかかわらず、更新用ロックを要求しようとした場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして設定されていた場合
		- @c exists 以外の引数に@c NULL が指定された場合
	@see gsGetRow

	@EN
	@ingroup Group_GSContainer
	@brief Returns the content of a Row corresponding to the INTEGER-type Row
		key.
	@par
		It can be used only if a INTEGER-type Column exists which corresponds
		to the specified Row key.
	@par
		If it requests a lock for update in the manual commit mode, it will hold
		the lock until a relevant transaction completes or a timeout occurs. The
		update or deletion operation by any other transaction on the locked Row
		will wait until the transaction completes or a timeout occurs. The lock
		will be held even if the target Row is deleted.
	@par
		In the autocommit mode, it cannot request a lock for update.
	@par
		Resources of data whose size is variable-length like string or array
		that is included in the Row object as acquisition result are maintained
		until the next Row object acquisition via the specified
		@ref GSContainer is executed.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSContainer.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [in] forUpdate
		indicates whether it requests a lock for update or not
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no INTEGER-type Column exists which corresponds to the specified
			Row key
		- if it requests a lock for update in the auto commit mode
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists
	@see gsGetRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowByInteger(
		GSContainer *container, int32_t key, void *rowObj,
		GSBool forUpdate, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief LONG型のロウキーに対応するロウの内容を取得します。
	@par
		ロウキーに対応するLONG型のカラムが存在する場合のみ使用できます。
	@par
		手動コミットモードにおいて更新用ロックを要求した場合、
		トランザクションが終了するかタイムアウトするまで対象ロウのロックを維持します。
		ロックされたロウに対する他のトランザクションからの更新・削除操作は、
		このトランザクションが終了するかタイムアウトするまで待機するようになります。
		対象ロウが削除されたとしても、ロックは維持されます。
	@par
		自動コミットモードの場合、更新用ロックを要求できません。
	@par
		取得結果のロウオブジェクトに含まれる文字列や配列などの可変長サイズの
		データのリソースは、指定の@ref GSContainer を直接介した次回のロウオブジェクト
		取得処理を実行するまで維持されます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSContainer
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [in] forUpdate
		更新用ロックを要求するかどうか
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するLONG型のカラムが存在しない場合
		- 自動コミットモードでないにもかかわらず、更新用ロックを要求しようとした場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして設定されていた場合
		- @c exists 以外の引数に@c NULL が指定された場合
	@see gsGetRow

	@EN
	@ingroup Group_GSContainer
	@brief Returns the content of a Row corresponding to the LONG-type Row key.
	@par
		It can be used only if a LONG-type Column exists which corresponds
		to the specified Row key.
	@par
		If it requests a lock for update in the manual commit mode, it will hold
		the lock until a relevant transaction completes or a timeout occurs. The
		update or deletion operation by any other transaction on the locked Row
		will wait until the transaction completes or a timeout occurs. The lock
		will be held even if the target Row is deleted.
	@par
		In the autocommit mode, it cannot request a lock for update.
	@par
		Resources of data whose size is variable-length like string or array
		that is included in the Row object as acquisition result are maintained
		until the next Row object acquisition via the specified
		@ref GSContainer is executed.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSContainer.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [in] forUpdate
		indicates whether it requests a lock for update or not
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no LONG-type Column exists which corresponds to the specified
			Row key
		- if it requests a lock for update in the auto commit mode
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists
	@see gsGetRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowByLong(
		GSContainer *container, int64_t key, void *rowObj,
		GSBool forUpdate, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief TIMESTAMP型のロウキーに対応するロウの内容を取得します。
	@par
		ロウキーに対応するTIMESTAMP型のカラムが存在する場合のみ使用できます。
	@par
		手動コミットモードにおいて更新用ロックを要求した場合、
		トランザクションが終了するかタイムアウトするまで対象ロウのロックを維持します。
		ロックされたロウに対する他のトランザクションからの更新・削除操作は、
		このトランザクションが終了するかタイムアウトするまで待機するようになります。
		対象ロウが削除されたとしても、ロックは維持されます。
	@par
		自動コミットモードの場合、更新用ロックを要求できません。
	@par
		取得結果のロウオブジェクトに含まれる文字列や配列などの可変長サイズの
		データのリソースは、指定の@ref GSContainer を直接介した次回のロウオブジェクト
		取得処理を実行するまで維持されます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSContainer
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [in] forUpdate
		更新用ロックを要求するかどうか
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するTIMESTAMP型のカラムが存在しない場合
		- 自動コミットモードでないにもかかわらず、更新用ロックを要求しようとした場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして設定されていた場合
		- @c exists 以外の引数に@c NULL が指定された場合
	@see gsGetRow

	@EN
	@ingroup Group_GSContainer
	@brief Returns the content of a Row corresponding to the TIMESTAMP-type Row
		key.
	@par
		It can be used only if a TIMESTAMP-type Column exists which corresponds
		to the specified Row key.
	@par
		If it requests a lock for update in the manual commit mode, it will hold
		the lock until a relevant transaction completes or a timeout occurs. The
		update or deletion operation by any other transaction on the locked Row
		will wait until the transaction completes or a timeout occurs. The lock
		will be held even if the target Row is deleted.
	@par
		In the autocommit mode, it cannot request a lock for update.
	@par
		Resources of data whose size is variable-length like string or array
		that is included in the Row object as acquisition result are maintained
		until the next Row object acquisition via the specified
		@ref GSContainer is executed.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSContainer.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [in] forUpdate
		indicates whether it requests a lock for update or not
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no TIMESTAMP-type Column exists which corresponds to the specified
			Row key
		- if it requests a lock for update in the auto commit mode
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists
	@see gsGetRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowByTimestamp(
		GSContainer *container, GSTimestamp key, void *rowObj,
		GSBool forUpdate, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief STRING型のロウキーに対応するロウの内容を取得します。
	@par
		ロウキーに対応するSTRING型のカラムが存在する場合のみ使用できます。
	@par
		手動コミットモードにおいて更新用ロックを要求した場合、
		トランザクションが終了するかタイムアウトするまで対象ロウのロックを維持します。
		ロックされたロウに対する他のトランザクションからの更新・削除操作は、
		このトランザクションが終了するかタイムアウトするまで待機するようになります。
		対象ロウが削除されたとしても、ロックは維持されます。
	@par
		自動コミットモードの場合、更新用ロックを要求できません。
	@par
		取得結果のロウオブジェクトに含まれる文字列や配列などの可変長サイズの
		データのリソースは、指定の@ref GSContainer を直接介した次回のロウオブジェクト
		取得処理を実行するまで維持されます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSContainer
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [in] forUpdate
		更新用ロックを要求するかどうか
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するSTRING型のカラムが存在しない場合
		- 自動コミットモードでないにもかかわらず、更新用ロックを要求しようとした場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして設定されていた場合
		- @c exists 以外の引数に@c NULL が指定された場合
	@see gsGetRow

	@EN
	@ingroup Group_GSContainer
	@brief Returns the content of a Row corresponding to the STRING-type Row
		key.
	@par
		It can be used only if a STRING-type Column exists which corresponds
		to the specified Row key.
	@par
		If it requests a lock for update in the manual commit mode, it will hold
		the lock until a relevant transaction completes or a timeout occurs. The
		update or deletion operation by any other transaction on the locked Row
		will wait until the transaction completes or a timeout occurs. The lock
		will be held even if the target Row is deleted.
	@par
		In the autocommit mode, it cannot request a lock for update.
	@par
		Resources of data whose size is variable-length like string or array
		that is included in the Row object as acquisition result are maintained
		until the next Row object acquisition via the specified
		@ref GSContainer is executed.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSContainer.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [in] forUpdate
		indicates whether it requests a lock for update or not
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no STRING-type Column exists which corresponds to the specified
			Row key
		- if it requests a lock for update in the auto commit mode
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
		- if @c NULL is specified to arguments except @c exists
	@see gsGetRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowByString(
		GSContainer *container, const GSChar *key, void *rowObj,
		GSBool forUpdate, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief INTEGER型のロウキーを指定して、ロウを新規作成または更新します。
	@par
		ロウキーに対応するINTEGER型のカラムが存在する場合のみ使用できます。
	@par
		ロウキーとコンテナの状態を基に、ロウを新規作成するか更新するかを決定します。
		この際、対応するロウがコンテナ内に存在しない場合は新規作成、
		存在する場合は更新します。ロウオブジェクト内のロウキーは無視されます。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [in] rowObj
		新規作成または更新するロウの内容と対応するロウオブジェクト
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するINTEGER型のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がロウオブジェクトに含まれていた場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、指定のロウオブジェクト内のロウキー以外のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合。もしくは、ロウキーに対応するカラムが
			存在し@c key が@c NULL であるにもかかわらず、ロウキーの フィールドに
			同様に@c NULL が含まれていた場合
	@see gsPutRow

	@EN
	@ingroup Group_GSContainer
	@brief Newly creates or update a Row by specifying the INTEGER type Row key.
	@par
		It can be used only if a INTEGER-type Column exists which corresponds
		to the specified Row key.
	@par
		It determines whether to newly create or update a Row, based on the Row
		key and the state of the Container. If there is no corresponding Row in
		the Container, it determines to newly create a Row; otherwise, it
		updates a relevant Row.
		The Row key in the specified Row object is ignored.
	@par
		In the manual commit mode, the target Row is locked.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [in] rowObj
		A Row object representing the content of a Row to be newly created or
		updated
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no INTEGER-type Column exists which corresponds to the specified
			Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is included in the Row object
		- if @c NULL is specified to arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the fields other than Row key in the
			specified Row object, or even though the column corresponding to
			Row key exists and @c key is @c NULL, @c NULL is included in the
			field of Row key as well
	@see gsPutRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutRowByInteger(
		GSContainer *container, int32_t key, const void *rowObj,
		GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief LONG型のロウキーを指定して、ロウを新規作成または更新します。
	@par
		ロウキーに対応するLONG型のカラムが存在する場合のみ使用できます。
	@par
		ロウキーとコンテナの状態を基に、ロウを新規作成するか更新するかを決定します。
		この際、対応するロウがコンテナ内に存在しない場合は新規作成、
		存在する場合は更新します。ロウオブジェクト内のロウキーは無視されます。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [in] rowObj
		新規作成または更新するロウの内容と対応するロウオブジェクト
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するLONG型のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がロウオブジェクトに含まれていた場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、指定のロウオブジェクト内のロウキー以外のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合。もしくは、ロウキーに対応するカラムが
			存在し@c key が@c NULL であるにもかかわらず、ロウキーの フィールドに
			同様に@c NULL が含まれていた場合
	@see gsPutRow

	@EN
	@ingroup Group_GSContainer
	@brief Newly creates or update a Row by specifying the LONG type Row key.
	@par
		It can be used only if a LONG-type Column exists which corresponds
		to the specified Row key.
	@par
		It determines whether to newly create or update a Row, based on the Row
		key and the state of the Container. If there is no corresponding Row in
		the Container, it determines to newly create a Row; otherwise, it
		updates a relevant Row.
		The Row key in the specified Row object is ignored.
	@par
		In the manual commit mode, the target Row is locked.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [in] rowObj
		A Row object representing the content of a Row to be newly created or
		updated.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no LONG-type Column exists which corresponds to the specified
			Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is included in the Row object
		- if @c NULL is specified to arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the fields other than Row key in the
			specified Row object, or even though the column corresponding to
			Row key exists and @c key is @c NULL, @c NULL is included in the
			field of Row key as well
	@see gsPutRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutRowByLong(
		GSContainer *container, int64_t key, const void *rowObj,
		GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief TIMESTAMP型のロウキーを指定して、ロウを新規作成または更新します。
	@par
		ロウキーに対応するTIMESTAMP型のカラムが存在する場合のみ使用できます。
	@par
		ロウキーとコンテナの状態を基に、ロウを新規作成するか更新するかを決定します。
		この際、対応するロウがコンテナ内に存在しない場合は新規作成、
		存在する場合は更新します。ロウオブジェクト内のロウキーは無視されます。
	@par
		ただし、コンテナの種別ならびに設定によっては、制限が設けられています。
		指定のコンテナが時系列であり、圧縮オプションが設定されている場合、
		以下の操作のみを条件付きで行うことができます。
		- 新規作成
			- 最も新しい時刻を持つ既存ロウより新しい時刻のロウを指定した場合のみ
		- 既存ロウの内容の保持
			- 最も新しい時刻を持つ既存ロウの時刻が指定の時刻と一致する場合のみ
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [in] rowObj
		新規作成または更新するロウの内容と対応するロウオブジェクト
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するTIMESTAMP型のカラムが存在しない場合
		- 特定コンテナ固有の制限に反する操作を行った場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーまたはロウオブジェクトに含まれていた場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、指定のロウオブジェクト内のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合
	@see gsPutRow

	@EN
	@ingroup Group_GSContainer
	@brief Newly creates or update a Row by specifying the TIMESTAMP type Row
		key.
	@par
		It can be used only if a TIMESTAMP-type Column exists which corresponds
		to the specified Row key.
	@par
		It determines whether to newly create or update a Row, based on the Row
		key and the state of the Container. If there is no corresponding Row in
		the Container, it determines to newly create a Row; otherwise, it
		updates a relevant Row.
		The Row key in the specified Row object is ignored.
	@par
		However, there are some restrictions depending on the type of Container
		and its settings. It can be used only for the following operations
		conditionally if the specified Container is TimeSeries and its
		compression option is enabled.
		- Create New
			- Only if newer Row than the existing Row which has the most recent
				time is specified
		- Keep the contents of the existing Row
			- Only if the time of the existing Row which has the most recent
				time and the specified time matches
	@par
		In the manual commit mode, the target Row is locked.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [in] rowObj
		A Row object representing the content of a Row to be newly created or
		updated.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no TIMESTAMP-type Column exists which corresponds to the specified
			Row key
		- if its operation is contrary to the restrictions specific to a
			particular Container
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is set in the Row key or the Row object
		- if @c NULL is specified to arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the fields within the specified Row
			object
	@see gsPutRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutRowByTimestamp(
		GSContainer *container, GSTimestamp key, const void *rowObj,
		GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief STRING型のロウキーを指定して、ロウを新規作成または更新します。
	@par
		ロウキーに対応するSTRING型のカラムが存在する場合のみ使用できます。
	@par
		ロウキーとコンテナの状態を基に、ロウを新規作成するか更新するかを決定します。
		この際、対応するロウがコンテナ内に存在しない場合は新規作成、
		存在する場合は更新します。ロウオブジェクト内のロウキーは無視されます。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@attention
		指定の@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [in] rowObj
		新規作成または更新するロウの内容と対応するロウオブジェクト
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するSTRING型のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーまたはロウオブジェクトに含まれていた場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、指定のロウオブジェクト内のロウキー以外のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合
	@see gsPutRow

	@EN
	@ingroup Group_GSContainer
	@brief Newly creates or update a Row by specifying the STRING type Row key.
	@par
		It can be used only if a STRING-type Column exists which corresponds
		to the specified Row key.
	@par
		It determines whether to newly create or update a Row, based on the Row
		key and the state of the Container. If there is no corresponding Row in
		the Container, it determines to newly create a Row; otherwise, it
		updates a relevant Row.
		The Row key in the specified Row object is ignored.
	@par
		In the manual commit mode, the target Row is locked.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [in] rowObj
		A Row object representing the content of a Row to be newly created or
		updated.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no STRING-type Column exists which corresponds to the specified
			Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is set in the Row key or the Row object
		- if @c NULL is specified to arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the fields other than Row key in the
			specified Row object
	@see gsPutRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsPutRowByString(
		GSContainer *container, const GSChar *key, const void *rowObj,
		GSBool *exists);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsRemoveRowByInteger(
		GSContainer *container, int32_t key, GSBool *exists));
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsRemoveRowByLong(
		GSContainer *container, int64_t key, GSBool *exists));
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsRemoveRowByTimestamp(
		GSContainer *container, GSTimestamp key, GSBool *exists));
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsRemoveRowByString(
		GSContainer *container, const GSChar *key, GSBool *exists));
#endif

/*!
	@JP
	@ingroup Group_GSContainer
	@brief INTEGER型のロウキーに対応するロウを削除します。
	@par
		ロウキーに対応するINTEGER型のカラムが存在する場合のみ使用できます。
		対応するロウが存在しない場合は何も変更しません。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するINTEGER型のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
	@see gsDeleteRow

	@EN
	@ingroup Group_GSContainer
	@brief Deletes a Row corresponding to the INTEGER-type Row key.
	@par
		It can be used only if a INTEGER-type Column exists which corresponds
		to the specified Row key. If no corresponding Row exists, nothing is
		changed.
	@par
		In the manual commit mode, the target Row is locked.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no INTEGER-type Column exists which corresponds to the specified
			Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
	@see gsDeleteRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDeleteRowByInteger(
		GSContainer *container, int32_t key, GSBool *exists);


/*!
	@JP
	@ingroup Group_GSContainer
	@brief LONG型のロウキーに対応するロウを削除します。
	@par
		ロウキーに対応するLONG型のカラムが存在する場合のみ使用できます。
		対応するロウが存在しない場合は何も変更しません。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するLONG型のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
	@see gsDeleteRow

	@EN
	@ingroup Group_GSContainer
	@brief Deletes a Row corresponding to the LONG-type Row key.
	@par
		It can be used only if a LONG-type Column exists which corresponds
		to the specified Row key. If no corresponding Row exists, nothing is
		changed.
	@par
		In the manual commit mode, the target Row is locked.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no LONG-type Column exists which corresponds to the specified
			Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
	@see gsDeleteRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDeleteRowByLong(
		GSContainer *container, int64_t key, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief TIMESTAMP型のロウキーに対応するロウを削除します。
	@par
		ロウキーに対応するTIMESTAMP型のカラムが存在する場合のみ使用できます。
		対応するロウが存在しない場合は何も変更しません。
	@par
		ただし、コンテナの種別ならびに設定によっては、制限が設けられています。
		圧縮オプションが設定された状態の時系列に対しては使用できません。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するTIMESTAMP型のカラムが存在しない場合
		- 特定コンテナ固有の制限に反する操作を行った場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして指定された場合
	@see gsDeleteRow

	@EN
	@ingroup Group_GSContainer
	@brief Deletes a Row corresponding to the TIMESTAMP-type Row key.
	@par
		It can be used only if a TIMESTAMP-type Column exists which corresponds
		to the specified Row key. If no corresponding Row exists, nothing is
		changed.
	@par
		However, there are some restrictions depending on the type of Container
		and its settings. It cannot be used to the TimeSeries whose compression
		option is enabled.
	@par
		In the manual commit mode, the target Row is locked.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no TIMESTAMP-type Column exists which corresponds to the specified
			Row key
		- if its operation is contrary to the restrictions specific to a
			particular Container
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
	@see gsDeleteRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDeleteRowByTimestamp(
		GSContainer *container, GSTimestamp key, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSContainer
	@brief STRING型のロウキーに対応するロウを削除します。
	@par
		ロウキーに対応するSTRING型のカラムが存在する場合のみ使用できます。
		対応するロウが存在しない場合は何も変更しません。
	@par
		手動コミットモードの場合、対象のロウはロックされます。
	@param [in] container
		処理対象の@ref GSContainer
	@param [in] key
		処理対象のロウキー
	@param [out] exists
		処理対象のロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ロウキーに対応するSTRING型のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がキーとして指定された場合
	@see gsDeleteRow

	@EN
	@ingroup Group_GSContainer
	@brief Deletes a Row corresponding to the STRING-type Row key.
	@par
		It can be used only if a STRING-type Column exists which corresponds
		to the specified Row key. If no corresponding Row exists, nothing is
		changed.
	@par
		In the manual commit mode, the target Row is locked.
	@param [in] container
		@ref GSContainer to be processed
	@param [in] key
		Row key to be processed
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the target Row exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no STRING-type Column exists which corresponds to the specified
			Row key
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the Row key
	@see gsDeleteRow

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDeleteRowByString(
		GSContainer *container, const GSChar *key, GSBool *exists);

//
// Collection API
//

/*!
	@JP
	@ingroup Group_GSCollection
	@brief 指定した空間範囲条件に合致するロウ集合を求めるための、クエリを作成します。
	@par
		@ref gsFetch を通じてロウ集合を求める際、更新用ロックのオプションを
		有効にすることもできます。
	@param [in] collection
		処理対象の@ref GSCollection
	@param [in] column
		比較対象の空間型カラムの名前
	@param [in] geometry
		比較対象として与える空間構造
	@param [in] geometryOp
		比較方法
	@param [out] query
		@ref GSQuery インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナの種別がコレクションではない場合
		- 対応する名前のカラムが存在しない場合
		- @c geometryOp 以外の引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSCollection
	@brief Creates a query to obtain a set of Rows which are matched to
		specified geometry range conditions.
	@par
		When obtaining a set of Rows using @ref gsFetch, the option of locking
		for update can be enabled.
	@param [in] collection
		@ref GSCollection to be processed
	@param [in] column
		A name of the geometry type column to be compared
	@param [in] geometry
		Geometry structure to be compared
	@param [in] geometryOp
		Comparison method
	@param [out] query
		the pointer to a pointer variable to store @ref GSQuery instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specified Container type is not Collection
		- if no Column has the specified name
		- if @c NULL is specified to arguments except @c geometryOp

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsQueryByGeometry(
		GSCollection *collection, const GSChar *column, const GSChar *geometry,
		GSGeometryOperator geometryOp, GSQuery **query);

/*!
	@JP
	@ingroup Group_GSCollection
	@brief 除外範囲付きの空間範囲条件に合致するロウ集合を求めるための、
		クエリを作成します。
	@par
		@c geometryIntersection と交差し、かつ、@c geometryDisjoint と
		交差しないカラム値を持つロウ集合を取得します。交差判定の条件は、
		@ref GS_GEOMETRY_OPERATOR_INTERSECT と同一です。
	@par
		@ref gsFetch を通じてロウ集合を求める際、更新用ロックのオプションを
		有効にすることもできます。
	@param [in] collection
		処理対象の@ref GSCollection
	@param [in] column
		比較対象の空間型カラムの名前
	@param [in] geometryIntersection
		カラム上の値と交差する範囲を示す空間構造
	@param [in] geometryDisjoint
		上の値と交差しない範囲を示す空間構造
	@param [out] query
		@ref GSQuery インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナの種別がコレクションではない場合
		- 対応する名前のカラムが存在しない場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSCollection
	@brief Creates a query to obtain a set of Rows which are matched to
		specified geometry range conditions with exclusion range.
	@par
		Obtains a set of Rows which has the column values that intersect with
		@c geometryIntersection and do not intersect with @c geometryDisjoint.
		Conditions of the intersection determination is the same as the
		@ref GS_GEOMETRY_OPERATOR_INTERSECT.
	@par
		When obtaining a set of Rows using @ref gsFetch, the option of locking
		for update can be enabled.
	@param [in] collection
		@ref GSCollection to be processed
	@param [in] column
		A name of the geometry type column to be compared
	@param [in] geometryIntersection
		Geometry structure indicating a range that intersects with the value on
		the column
	@param [in] geometryDisjoint
		Geometry structure indicating a range that does not intersect with the
		values on the column
	@param [out] query
		the pointer to a pointer variable to store @ref GSQuery instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specified Container type is not Collection
		- if no Column has the specified name
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsQueryByGeometryWithDisjointCondition(
		GSCollection *collection, const GSChar *column,
		const GSChar *geometryIntersection, const GSChar *geometryDisjoint,
		GSQuery **query);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSResult GS_API_CALL gsQueryByGeometryWithExclusion(
		GSCollection *collection, const GSChar *column,
		const GSChar *geometryIntersection, const GSChar *geometryDisjoint,
		GSQuery **query));
#endif

//
// TimeSeries API
//

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 開始・終了時刻を指定して、ロウ集合またはその特定のカラムに対し
		集計演算を行います。
	@par
		@c column は@c aggregation 次第で無視されることがあります。
		演算対象には、開始・終了時刻と合致する境界上のロウも含まれます。
		終了時刻より新しい開始時刻を指定した場合、すべてのロウが対象外となります。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] start
		開始時刻
	@param [in] end
		終了時刻
	@param [in] column
		集計対象のカラム名。合計演算のように、特定のカラムを対象としない場合は@c NULL
	@param [in] aggregation
		集計方法
	@param [out] aggregationResult
		集計結果を@ref GSAggregationResult として格納するためのポインタ変数への
		ポインタ値。対象時系列の内容と集計方法によっては、@c NULL が設定されることも
		あります。詳細は@ref GSAggregation の定義を参照してください。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナの種別が時系列ではない場合
		- 指定の演算方法で許可されていない型のカラムを指定した場合
		- 対応する名前のカラムが存在しない場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- ポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimeSeries
	@brief Performs an aggregation operation on a Row set or its specific
		Columns, based on the specified start and end times.
	@par
		The parameter @c column might be ignored depending on the parameter
		@c aggregation. The boundary Rows whose timestamps are identical with
		the start or end time are included in the range of operation. If the
		specified start time is later than the end time, all Rows are excluded
		from the range.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] start
		Start time
	@param [in] end
		End time
	@param [in] column
		The name of a target Column. Specify @c NULL if the specified
		aggregation method does not target a specific Column.
	@param [in] aggregation
		An aggregation method
	@param [out] aggregationResult
		The pointer to a pointer variable to store the aggregation result as
		@ref GSAggregationResult. @c NULL may be set depending on the contents
		and the aggregation method of the target TimeSeries. Refer to the
		definition of @ref GSAggregation for more detail.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specified Container type is not TimeSeries
		- if the type of the specified Column is not supported by the specified
			aggregation method.
		- if no Column has the specified name
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if @c NULL is specified to pointer type arguments

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAggregateTimeSeries(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		const GSChar *column, GSAggregation aggregation,
		GSAggregationResult **aggregationResult);

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief GridDB上の現在時刻をロウキーとして、ロウを新規作成または更新します。
	@par
		GridDB上の現在時刻に相当するTIMESTAMP値をロウキーとする点を除き、
		@ref gsPutRow と同様に振る舞います。
		指定のロウオブジェクト内のロウキーは無視されます。
	@par
		圧縮オプションが設定された状態の時系列に対しては、
		GridDB上の現在時刻より新しい時刻のロウが存在しない場合のみ使用できます。
		最も新しい時刻を持つ既存ロウの時刻が現在時刻と一致する場合、
		何も変更は行わず既存ロウの内容を保持します。
	@par
		手動コミットモードの場合、対象のロウがロックされます。
		また、内部格納単位が同一の他のロウもロックされます。
	@attention
		指定の@ref GSTimeSeries にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] rowObj
		新規作成または更新するロウの内容と対応するロウオブジェクト
	@param [out] exists
		GridDB上の現在時刻と一致するロウが存在したかどうかを格納するための、
		ブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定の時系列について圧縮オプションが設定されており、
			現在時刻より新しい時刻のロウが存在した場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値がロウオブジェクトに含まれていた場合
		- @c exists 以外の引数に@c NULL が指定された場合。
			また、指定のロウオブジェクトについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合
	@see gsPutRow
	@see GSTimeSeriesPropertiesTag::compressionMethod

	@EN
	@ingroup Group_GSTimeSeries
	@brief Newly creates or updates a Row with a Row key of the current time on
		GridDB.
	@par
		It behaves in the same way as @ref gsPutRow, except that it sets as a
		Row key the TIMESTAMP value equivalent to the current time on GridDB.
		The Row key in the specified Row object is ignored.
	@par
		For the TimeSeries whose compression option is enabled, it can be used
		only if there is no newer Row than the current time on GridDB.
		If the time of the newest Row matches the current time, nothing changes
		and the contents of existed Row are kept.
	@par
		In the manual commit mode, the target Row is locked. Other Rows in the
		same internal storage unit are also locked.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSTimeSeries and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] rowObj
		A Row object representing the content of a Row to be newly created or
		updated.
	@param [out] exists
		The pointer to a BOOL-type variable to store the value which can be used
		to identify whether the Row that matches the current time on GridDB is
		existed or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the compression option is enabled on the specified TimeSeries,
			and if newer Row than current time exists
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is included in the Row object
		- if @c NULL is specified to arguments except @c exists,
			or if @c NULL is included in the pointer to the data whose size is
			variable-length like string in the specified Row object
	@see gsPutRow
	@see GSTimeSeriesPropertiesTag::compressionMethod

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAppendTimeSeriesRow(
		GSTimeSeries *timeSeries, const void *rowObj, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 指定の時刻を基準として、関係する1つのロウを取得します。
	@attention
		指定の@ref GSTimeSeries にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSTimeSeries
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] base
		基準となる時刻
	@param [in] timeOp
		取得方法
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		取得対象のロウが存在しない場合、ロウオブジェクトの内容は何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [out] exists
		条件に一致するロウが存在したかどうかを格納するためのブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値が基準時刻として指定された場合
		- @c exists 以外のポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimeSeries
	@brief Returns one Row related with the specified time.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSTimeSeries and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSTimeSeries.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] base
		A base time
	@param [in] timeOp
		An acquisition method
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Nothing will be changed in the contents of Row object if the target Row
		does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [out] exists
		The pointer to a BOOL-type variable to store the value which can be used
		to identify whether the Row which meets the criteria exists or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the base time
		- if @c NULL is specified to pointer type arguments except @c exists

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowByBaseTime(
		GSTimeSeries *timeSeries, GSTimestamp base, GSTimeOperator timeOp,
		void *rowObj, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 指定時刻に相当するロウオブジェクトについて、線形補間などを行い生成します。
	@par
		一致する時系列ロウの指定のカラムの値、もしくは、前後時刻の
		ロウの指定カラムの値を線形補間して得られた値を基にロウオブジェクトを生成します。
		前後時刻のロウの少なくともいずれか、もしくは、一致するロウが存在しない場合は
		生成されません。
	@par
		補間対象として指定できるカラムの型は、数値型のみです。
		指定のカラムならびにロウキー以外のフィールドには、指定時刻と同一または
		より前の時刻のロウのうち、最も新しい時刻を持つロウのフィールドの値が
		設定されます。
	@attention
		指定の@ref GSTimeSeries にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSTimeSeries
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] base
		基準となる時刻
	@param [in] column
		線形補間対象のカラム
	@param [out] rowObj
		生成結果を格納するためのロウオブジェクト。
		生成のために必要とするロウが存在しない場合、ロウオブジェクトの内容は
		何も変更されません。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@param [out] exists
		生成のために必要とするロウが存在したかどうかを格納するための、
		ブール型変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_FALSE が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 対応する名前のカラムが存在しない場合。また、サポートされていない型の
			カラムが指定された場合
		- この処理またはトランザクションのタイムアウト、
			指定のコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- サポート範囲外の値が基準時刻として指定された場合
		- @c exists 以外のポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimeSeries
	@brief Performs linear interpolation to a Row object corresponding to the
		specified time.
	@par
		It creates a Row object, based on the value of the specified Column of
		the TimeSeries Row identical with the base time or the value obtained
		by linearly interpolating the values of the specified Columns of
		adjacent Rows around the base time. If there is no Row with the same
		timestamp as the base time nor no Row with an earlier or later
		timestamp, no Row object is created.
	@par
		Only Columns of numeric type can be interpolated. The field values of
		the Row whose timestamp is identical with the specified timestamp or
		the latest among those with earlier timestamps are set on the specified
		Column and the fields other than a Row key.
	@attention
		The behavior is undefined if the type of Row object bound to the
		specified @ref GSTimeSeries and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSTimeSeries.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] base
		A base time
	@param [in] column
		A target column for linear interpolation
	@param [out] rowObj
		The Row object to store the Row created by linear interpolation.
		Nothing will be changed in the contents of Row object if the Row
		required for linear interpolation does not exist.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@param [out] exists
		the pointer to a BOOL-type variable to store the value which can be used
		to identify whether the Row required for Row object generation is
		existed or not.
		@ref GS_FALSE is stored if non-@ref GS_RESULT_OK is returned as the
		execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if no Column has the specified name; or if an unsupported type of
			Column is specified
		- if a timeout occurs during this operation or the transaction,
			a specified Container is deleted, its schema is changed or
			a connection failure occurs
		- if an unsupported value is specified as the base time
		- if @c NULL is specified to pointer type arguments except @c exists

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsInterpolateTimeSeriesRow(
		GSTimeSeries *timeSeries, GSTimestamp base, const GSChar *column,
		void *rowObj, GSBool *exists);

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 開始時刻・終了時刻を指定して、特定範囲のロウ集合を求めるクエリを作成します。
	@par
		取得対象には、開始・終了時刻と合致する境界上のロウも含まれます。
		終了時刻より新しい開始時刻を指定した場合、すべてのロウが対象外となります。
		要求するロウ集合は昇順、すなわち古い時刻から新しい時刻の順となります。
	@par
		@ref gsFetch を通じてロウ集合を求める際、更新用ロックのオプションを
		有効にすることもできます。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] start
		開始時刻
	@param [in] end
		終了時刻
	@param [out] query
		@ref GSQuery インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が設定されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナの種別が時系列ではない場合
		- ポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimeSeries
	@brief Creates a query to obtain a set of Rows within a specific range
		between the specified start and end times.
	@par
		The boundary Rows whose timestamps are identical with the start or end
		time are included in the range of fetch. If the specified start time is
		later than the end time, all Rows are excluded from the range. The Rows
		in a result set are sorted in ascending order, i.e., from an earlier
		timestamp to a later timestamp.
	@par
		The option of locking for update can be enabled when obtaining a set of
		Rows using @ref gsFetch.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] start
		Start time
	@param [in] end
		End time
	@param [out] query
		the pointer to a pointer variable to store @ref GSQuery instance.
		@c NULL is set to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specified Container type is not TimeSeries
		- if @c NULL is specified to pointer type arguments

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsQueryByTimeSeriesRange(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		GSQuery **query);

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 開始時刻・終了時刻・順序を指定して、特定範囲のロウ集合を求める
		クエリを作成します。
	@par
		取得対象には、開始・終了時刻と合致する境界上のロウも含まれます。
		終了時刻より新しい開始時刻を指定した場合、すべてのロウが対象外となります。
	@par
		@ref gsFetch を通じてロウ集合を求める際、更新用ロックのオプションを
		有効にすることもできます。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] start
		開始時刻または@c NULL 。@c NULL の場合、指定の時系列上の最も古いロウの
		時刻が開始時刻として指定されたものとみなします。
	@param [in] end
		終了時刻または@c NULL 。@c NULL の場合、指定の時系列上の最も新しいロウの
		時刻が終了時刻として指定されたものとみなします。
	@param [in] order
		取得するロウ集合の時刻順序。
		@ref GS_ORDER_ASCENDING の場合は古い時刻から新しい時刻の順、
		@ref GS_ORDER_DESCENDING の場合は新しい時刻から古い時刻の順となります。
	@param [out] query
		@ref GSQuery インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が設定されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナの種別が時系列ではない場合
		- @c start 、@c end 以外のポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimeSeries
	@brief Creates a query to obtain a set of Rows sorted in the specified order
		within a specific range between the specified start and end times.
	@par
		The boundary Rows whose timestamps are identical with the start or end
		time are included in the range of fetch. If the specified start time is
		later than the end time, all Rows are excluded from the range.
	@par
		The option of locking for update can be enabled when obtaining a set of
		Rows using @ref gsFetch.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] start
		start time or @c NULL. A @c NULL value indicates the timestamp of the
		oldest Row in this TimeSeries is specified.
	@param [in] end
		end time or @c NULL. A @c NULL value indicates the timestamp of the
		newest Row in this TimeSeries is specified.
	@param [in] order
		the time order of Rows in a result set. @ref GS_ORDER_ASCENDING
		indicates the order from older to newer, and @ref GS_ORDER_DESCENDING
		indicates the order from newer to older.
	@param [out] query
		the pointer to a pointer variable to store @ref GSQuery instance.
		@c NULL is set to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specified Container type is not TimeSeries
		- if @c NULL is specified to pointer type arguments except @c start and
			@c end

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsQueryByTimeSeriesOrderedRange(
		GSTimeSeries *timeSeries,
		const GSTimestamp *start, const GSTimestamp *end,
		GSQueryOrder order, GSQuery **query);

/*!
	@JP
	@ingroup Group_GSTimeSeries
	@brief 特定範囲のロウ集合をサンプリングするクエリを作成します。
	@par
		サンプリング対象の時刻は、開始時刻に対し非負整数倍のサンプリング
		間隔を加えた時刻のうち、終了時刻と同じかそれ以前のもののみです。
		終了時刻より新しい開始時刻を指定した場合、すべてのロウが対象外となります。
	@par
		作成したクエリを実行すると、各サンプリング位置の時刻と一致するロウが
		存在する場合は該当ロウの値を、存在しない場合は@c columnSet と
		@c mode 引数の指定に従い補間された値を使用しロウ集合を生成します。
		個別の補間方法については、@ref GSInterpolationMode の定義を
		参照してください。
	@par
		補間のために参照する必要のあるロウが存在しない場合、
		該当するサンプリング時刻のロウは生成されず、該当箇所の数だけ
		結果件数が減少します。
		サンプリング間隔をより短く設定すると、補間方法次第では
		異なるサンプリング時刻であっても同一のロウの内容が使用される
		可能性が高まります。
	@par
		@ref gsFetch を通じてロウ集合を求める際、更新用ロックのオプションを
		有効にすることはできません。
	@param [in] timeSeries
		処理対象の@ref GSTimeSeries
	@param [in] start
		開始時刻
	@param [in] end
		終了時刻
	@param [in] columnSet
		@c mode に基づき特定の補間処理を適用するカラムの名前の集合。
		文字列ポインタの配列より構成されます。
		空集合の場合は、適用対象のカラムを何も指定しないことを示します。
		@c NULL の場合は、空集合を指定した場合と同様です。
	@param [in] columnCount
		@c mode に基づき特定の補間処理を適用するカラムの個数
	@param [in] mode
		補間方法
	@param [in] interval
		サンプリング間隔。@c 0 および負の値は指定できません
	@param [in] intervalUnit
		サンプリング間隔の時間単位。@ref GS_TIME_UNIT_YEAR 、
		@ref GS_TIME_UNIT_MONTH は指定できません
	@param [out] query
		@ref GSQuery インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のコンテナの種別が時系列ではない場合
		- @c columnSet 以外のポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimeSeries
	@brief Creates a query to take a sampling of Rows within a specific range.
	@par
		Each sampling time point is defined by adding a sampling interval
		multiplied by a non-negative integer to the start time, excluding the
		time points later than the end time. If the specified start time is
		later than the end time, all Rows are excluded from the range.
	@par
		If executed, the query creates a set of Rows, using the values of the
		Rows whose timestamps are identical with each sampling time point, or
		the interpolated values according to the @c columnSet and @c mode
		if such a Row is not found. For specific interpolation methods, see the
		description of @ref GSInterpolationMode.
	@par
		If there is no Rows to be referenced for interpolation at a specific
		sampling time point, a corresponding Row is not generated, and thus the
		number of results returned is reduced by the number of such time points.
		A shorter sampling interval increases the likelihood that identical Row
		field values will be used even at different sampling time points,
		depending on the interpolation method.
	@par
		The option of locking for update cannot be enabled when obtaining a set
		of Rows using @ref gsFetch.
	@param [in] timeSeries
		@ref GSTimeSeries to be processed
	@param [in] start
		Start time
	@param [in] end
		End time
	@param [in] columnSet
		a set of names of the Columns to be interpolated according to @c mode.
		It consists of the array of string pointer. An empty set indicates no
		specification of target Columns. A @c NULL value indicates the same as
		an empty set
	@param [in] columnCount
		a number of Columns to be interpolated according to @c mode
	@param [in] mode
		an interpolation method
	@param [in] interval
		a sampling interval. @c 0 or a negative value cannot be specified
	@param [in] intervalUnit
		the time unit of the sampling interval. @ref GS_TIME_UNIT_YEAR and
		@ref GS_TIME_UNIT_MONTH cannot be specified
	@param [out] query
		the pointer to a pointer variable to store @ref GSQuery instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if specified Container type is not TimeSeries
		- if @c NULL is specified to pointer type arguments except @c columnSet

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsQueryByTimeSeriesSampling(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		const GSChar *const *columnSet, size_t columnCount,
		GSInterpolationMode mode, int32_t interval, GSTimeUnit intervalUnit,
		GSQuery **query);

#if GS_DEPRECATED_FUNC_ENABLED
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(GSResult GS_API_CALL
		gsCompatibleFunc_QueryByTimeSeriesSampling1(
		GSTimeSeries *timeSeries, GSTimestamp start, GSTimestamp end,
		const GSChar *const *columnSet,
		int32_t interval, GSTimeUnit intervalUnit,
		GSQuery **query));
#endif

#if GS_INTERNAL_DEFINITION_VISIBLE
#if GS_COMPATIBILITY_TIME_SERIES_SAMPLING_BETA_0_1
#define gsQueryByTimeSeriesSampling( \
				timeSeries, start, end, columnSet, interval, intervalUnit, query) \
		gsCompatibleFunc_QueryByTimeSeriesSampling1( \
				timeSeries, start, end, columnSet, interval, intervalUnit, query)
#else
#define gsQueryByTimeSeriesSampling( \
				timeSeries, start, end, columnSet, columnCount, \
				mode, interval, intervalUnit, query) \
		gsQueryByTimeSeriesSampling( \
				timeSeries, start, end, columnSet, columnCount, \
				mode, interval, intervalUnit, query)
#endif
#endif	// not GS_INTERNAL_DEFINITION_VISIBLE

//
// Row API
//

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定の@ref GSRow インスタンスを解放します。
	@param [in,out] row
		クローズ対象の@ref GSRow インスタンスを指しているポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSRow インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Releases a specified @ref GSRow instance.
	@param [in,out] row
		the pointer to a pointer variable that refers to @ref GSRow
		instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSRow instance which was
		already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseRow(GSRow **row);

#if GS_COMPATIBILITY_SUPPORT_3_5

#if GS_INTERNAL_DEFINITION_VISIBLE
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchemaV3_3(
		GSRow *row, GSContainerInfo *schemaInfo);
#endif

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のロウに対応するスキーマを取得します。
	@par
		ロウキーの有無を含むカラムレイアウトにする情報のみが設定された
		@ref GSContainerInfo が求まります。
		コンテナ名、コンテナ種別、索引設定、時系列構成オプションなどその他の
		コンテナ情報は含まれません。
	@attention
		カラム情報の列などの可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用します。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [out] schemaInfo
		スキーマ情報を格納するための@ref GSContainerInfo へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@ref GS_CONTAINER_INFO_INITIALIZER
		と同一の内容の初期値が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the schema corresponding to the specified Row.
	@par
		It returns @ref GSContainerInfo in which only the Column layout
		information including the existence of any Row key is set, and the
		Container name, the Container type, index settings, and the TimeSeries
		configuration options are not included.
	@attention
		In order to store the variable-length data such as the column of column
		information, it uses a temporary memory area which is managed by the
		specified @ref GSGridStore instance corresponding to the specified
		@ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [out] schemaInfo
		The pointer value to @ref GSContainerInfo for storing schema information.
		If @ref GS_RESULT_OK is not returned as the execution result, the initial
		value which is same as @ref GS_CONTAINER_INFO_INITIALIZER is stored.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsGetRowSchema(
		GSRow *row, GSContainerInfo *schemaInfo) {
	return gsGetRowSchemaV3_3(row, schemaInfo);
}

#elif GS_COMPATIBILITY_SUPPORT_2_1

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchemaV2_1(
		GSRow *row, GSContainerInfo *schemaInfo);

GS_STATIC_HEADER_FUNC_SPECIFIER GSResult gsGetRowSchema(
		GSRow *row, GSContainerInfo *schemaInfo) {
	return gsGetRowSchemaV2_1(row, schemaInfo);
}

#else

GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSchema(
		GSRow *row, GSContainerInfo *schemaInfo);

#endif	// not GS_COMPATIBILITY_SUPPORT_2_1

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のフィールドに値を設定します。
	@attention
		対象フィールドの値とその型との対応が一致しない場合、この処理の動作は
		未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値。
		@c type として@ref GS_TYPE_NULL が指定された場合は、指定の内容が
		参照されることはありません。ただし、@c NULL 以外のポインタ値を指定する
		必要があります
	@param [in] type
		対象フィールドの値の型
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- NOT NULL制約の設定されたカラムに対して、フィールド値の型として
			@ref GS_TYPE_NULL が指定された場合
		- フィールドの値の構成要素のポインタ値として@c NULL が含まれていた場合
		- フィールドの値がカラムの型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the value to the specified field.
	@attention
		The behavior is undefined if the value of the target field and
		its corresponding type does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
		If @ref GS_TYPE_NULL is specified as the @c type, the specified content will
		not be referenced. However, you need to specify a pointer value other than @c NULL.
	@param [in] type
		the value type of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- If @ref GS_TYPE_NULL is specified as the field value type for the column
			with the NOT NULL constraint
		- if @c NULL is included as a pointer value in the element of field
			value
		- if the type of the specified field does not match the type of the
			Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldGeneral(
		GSRow *row, int32_t column, const GSValue *fieldValue,
		GSType type);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のフィールドの値とその型を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] type
		対象フィールドの値の型を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the value and type in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] type
		the pointer to a variable to store the type of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldGeneral(
		GSRow *row, int32_t column, GSValue *fieldValue, GSType *type);

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief 指定のフィールドにNULLを設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- NOT NULL制約の設定されたカラムが指定された場合
	@since 3.5

	@EN
	@brief Sets NULL to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if Column with NOT NULL constraint is specified
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldNull(
		GSRow *row, int32_t column);

/*!
	@JP
	@brief 指定のフィールドにNULLが設定されているかどうかを返します。
	@par
		NOT NULL制約の設定されたカラムが指定された場合、常に@ref GS_FALSE
		を返します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] nullValue
		NULLが設定されているかどうか受け取る変数へのポインタ値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 3.5

	@EN
	@brief Returns whether the specified field is set to NULL.
	@par
		Whenever a Column with a NOT NULL constraint is specified,
		it always returns @ref GS_FALSE.
@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] nullValue
		Whenever a Column with a NOT NULL constraint is specified,
		it always returns @ref GS_FALSE.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if Column with NOT NULL constraint is specified
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldNull(
		GSRow *row, int32_t column, GSBool *nullValue);

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSTRING型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the STRING-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByString(
		GSRow *row, int32_t column, const GSChar *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSTRING型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the STRING-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsString(
		GSRow *row, int32_t column, const GSChar **fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBOOL型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the BOOL-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByBool(
		GSRow *row, int32_t column, GSBool fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBOOL型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the BOOL-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsBool(
		GSRow *row, int32_t column, GSBool *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBYTE型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the BYTE-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByByte(
		GSRow *row, int32_t column, int8_t fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBYTE型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the BYTE-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsByte(
		GSRow *row, int32_t column, int8_t *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSHORT型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the SHORT-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByShort(
		GSRow *row, int32_t column, int16_t fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSHORT型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the SHORT-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsShort(
		GSRow *row, int32_t column, int16_t *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のINTEGER型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the INTEGER-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByInteger(
		GSRow *row, int32_t column, int32_t fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のINTEGER型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the INTEGER-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsInteger(
		GSRow *row, int32_t column, int32_t *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のLONG型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the LONG-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByLong(
		GSRow *row, int32_t column, int64_t fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のLONG型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the LONG-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsLong(
		GSRow *row, int32_t column, int64_t *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のFLOAT型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the FLOAT-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByFloat(
		GSRow *row, int32_t column, float fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のFLOAT型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the FLOAT-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsFloat(
		GSRow *row, int32_t column, float *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のDOUBLE型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the DOUBLE-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByDouble(
		GSRow *row, int32_t column, double fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のDOUBLE型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the DOUBLE-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsDouble(
		GSRow *row, int32_t column, double *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のTIMESTAMP型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the TIMESTAMP-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByTimestamp(
		GSRow *row, int32_t column, GSTimestamp fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のTIMESTAMP型フィールドの値を取得します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the TIMESTAMP-type value in the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsTimestamp(
		GSRow *row, int32_t column, GSTimestamp *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のGEOMETRY型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the GEOMETRY-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByGeometry(
		GSRow *row, int32_t column, const GSChar *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のGEOMETRY型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the GEOMETRY-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsGeometry(
		GSRow *row, int32_t column, const GSChar **fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBLOB型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the BLOB-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByBlob(
		GSRow *row, int32_t column, const GSBlob *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBLOB型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the BLOB-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsBlob(
		GSRow *row, int32_t column, GSBlob *fieldValue);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSTRING配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
		- 配列要素に@c NULL が含まれる場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the STRING-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
		- if @c NULL is existed in the array element(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByStringArray(
		GSRow *row, int32_t column, const GSChar *const *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSTRING配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the STRING-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsStringArray(
		GSRow *row, int32_t column, const GSChar *const **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBOOL配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the BOOL-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByBoolArray(
		GSRow *row, int32_t column, const GSBool *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBOOL配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the BOOL-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsBoolArray(
		GSRow *row, int32_t column, const GSBool **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBYTE配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the BYTE-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByByteArray(
		GSRow *row, int32_t column, const int8_t *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のBYTE配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the BYTE-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsByteArray(
		GSRow *row, int32_t column, const int8_t **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSHORT配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the SHORT-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByShortArray(
		GSRow *row, int32_t column, const int16_t *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のSHORT配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the SHORT-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsShortArray(
		GSRow *row, int32_t column, const int16_t **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のINTEGER配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the INTEGER-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByIntegerArray(
		GSRow *row, int32_t column, const int32_t *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のINTEGER配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the INTEGER-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsIntegerArray(
		GSRow *row, int32_t column, const int32_t **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のLONG配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the LONG-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByLongArray(
		GSRow *row, int32_t column, const int64_t *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のLONG配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the LONG-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsLongArray(
		GSRow *row, int32_t column, const int64_t **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のFLOAT配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the FLOAT-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByFloatArray(
		GSRow *row, int32_t column, const float *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のFLOAT配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the FLOAT-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsFloatArray(
		GSRow *row, int32_t column, const float **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のDOUBLE配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the DOUBLE-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByDoubleArray(
		GSRow *row, int32_t column, const double *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のDOUBLE配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the DOUBLE-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsDoubleArray(
		GSRow *row, int32_t column, const double **fieldValue,
		size_t *size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のTIMESTAMP配列型フィールドに値を設定します。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [in] fieldValue
		対象フィールドの値
	@param [in] size
		対象フィールドの値の配列要素数
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
		- 指定のカラム番号の型と一致しない場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Sets the TIMESTAMP-array-type value to the specified field.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [in] fieldValue
		the value of the target field
	@param [in] size
		the number of array elements in the target field
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
		- if there is a type mismatch between the type of specified value and
			the type of specified Column
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetRowFieldByTimestampArray(
		GSRow *row, int32_t column, const GSTimestamp *fieldValue,
		size_t size);

/*!
	@JP
	@ingroup Group_GSRow
	@brief 指定のTIMESTAMP配列型フィールドの値を取得します。
	@attention
		フィールド値に含まれる可変長データを格納するために、指定の@ref GSRow
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] row
		処理対象の@ref GSRow
	@param [in] column
		対象フィールドのカラム番号。@c 0 以上かつカラム数未満の値
	@param [out] fieldValue
		対象フィールドの値を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		対象フィールドの値の配列要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のカラム番号が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRow
	@brief Returns the TIMESTAMP-array-type value in the specified field.
	@attention
		In order to store the variable-length data included in field value, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRow.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] row
		@ref GSRow to be processed
	@param [in] column
		the Column number of the target field, from @c 0 to number of Columns
		minus one.
	@param [out] fieldValue
		the pointer to a variable to store the value of the target field.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		target field.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if the specified Column number is out of range
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowFieldAsTimestampArray(
		GSRow *row, int32_t column, const GSTimestamp **fieldValue,
		size_t *size);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

//
// Query API
//

/*!
	@JP
	@ingroup Group_GSQuery
	@brief 指定の@ref GSQuery インスタンスを解放します。
	@param [in,out] query
		クローズ対象の@ref GSQuery インスタンスを指しているポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSQuery インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。

	@EN
	@ingroup Group_GSQuery
	@brief Releases a specified @ref GSQuery instance.
	@param [in,out] query
		the pointer to a pointer variable that refers to @ref GSQuery
		instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSQuery instance which was
		already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseQuery(GSQuery **query);

/*!
	@JP
	@ingroup Group_GSQuery
	@brief オプションを指定して指定のクエリを実行し、実行結果に対応するロウ集合を取得します。
	@par
		@c forUpdate に@ref GS_TRUE が指定された場合、取得対象のロウ
		すべてをロックします。ロックすると、対応するトランザクションが有効である間、
		他のトランザクションからの対象ロウに対する変更操作が阻止されます。
		対応するコンテナの自動コミットモードが無効の場合のみ、指定できます。
	@par
		新たなロウ集合を取得すると、指定のクエリについて直近に実行した結果の
		@ref GSRowSet を介するロウ操作はできなくなります。
	@par
		一度に大量のロウを取得しようとした場合、GridDBノードが管理する
		通信バッファのサイズの上限に到達し、失敗することがあります。
		上限サイズについては、GridDBテクニカルリファレンスを参照してください。
	@param [in] query
		処理対象の@ref GSQuery
	@param [in] forUpdate
		更新用ロックを要求するかどうか
	@param [out] rowSet
		@ref GSRowSet インスタンスを格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 対応するコレクションの自動コミットモード有効であるにもかかわらず、
			@c forUpdate に@ref GS_TRUE が指定された場合
		- ロックできないクエリであるにもかかわらず、@c forUpdate に@ref GS_TRUE
			が指定された場合。具体的なロックの可否は、指定のクエリを作成する機能の各種定義を
			参照してください。
		- 正しくないパラメータ・構文・命令を含むクエリを実行しようとした場合。
			具体的な制約は、指定のクエリを作成する機能の各種定義を参照してください。
		- この処理または関連するトランザクションのタイムアウト、
			対応するコンテナの削除もしくはスキーマ変更、接続障害が発生した場合、
			または対応する@ref GSContainer のクローズ後に呼び出された場合
		- ポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSQuery
	@brief Executes a specified query with the specified option and returns a
		set of Rows as an execution result.
	@par
		It locks all target Rows if @ref GS_TRUE is specified as @c forUpdate.
		If the target Rows are locked, update operations on the Rows by any
		other transactions are blocked while a relevant transaction is active.
		@ref GS_TRUE can be specified only if the auto commit mode is disabled
		on a relevant Container.
	@par
		When new set of Rows are obtained, any Row operation via @ref GSRowSet
		as the last result of specified query is prohibited.
	@par
		If the system tries to acquire a large number of Rows all at once, the
		upper limit of the communication buffer size managed by the GridDB node
		may be reached, possibly resulting in a failure.
		Refer to the GridDB Technical Reference for the upper limit
		size.
	@param [in] query
		@ref GSQuery to be processed
	@param [in] forUpdate
		indicates whether it requests a lock for update or not
	@param [out] rowSet
		the pointer to a pointer variable to store @ref GSRowSet instance.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @ref GS_TRUE is specified to @c forUpdate although the auto commit
			mode is enabled on a relevant Container
		- if @ref GS_TRUE is specified to @c forUpdate for a query which cannot
			acquire a lock. For the availability of a lock, see the descriptions
			of the functions to create a specified query
		- if the target query contains any wrong parameter, syntax, or
			directive. For detailed restrictions, see the descriptions of the
			functions to create a specified query
		- if a timeout occurs during this operation or related transaction,
			the relevant Container is deleted, its schema is changed,
			a connection failure occurs, or this functions is called after the
			relevant @ref GSContainer is closed
		- if @c NULL is specified to pointer type arguments

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsFetch(
		GSQuery *query, GSBool forUpdate, GSRowSet **rowSet);

/*!
	@JP
	@ingroup Group_GSQuery
	@brief 結果取得に関するオプションを設定します。
	@par
		設定可能なオプション項目と値の定義については、@ref GSFetchOptionTag を
		参照してください。
	@attention
		@c valueType と@c value の型との対応が正しくない場合、
		この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] query
		処理対象の@ref GSQuery
	@param [in] fetchOption
		オプション項目
	@param [in] value
		オプションの値。指定可能な型は、@c valueType によって次のように異なります。
		@c valueType | @c value の型
		-|-
		INTEGER | int32_t*
		LONG | int64_t*
		BOOL | @ref GSBool*
	@param [in] valueType
		オプションの値の型
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- オプション項目固有の制約に違反した場合
		- ポインタ型引数に@c NULL が指定された場合
		- 対応する@ref GSContainer のクローズ後に呼び出された場合

	@EN
	@ingroup Group_GSQuery
	@brief Sets an fetch option for a result acquisition.
	@par
		Refer @ref GSFetchOptionTag for the definitions of available options
		and values.
	@attention
		The behavior is undefined if the @c valueType and the type of @c value
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] query
		@ref GSQuery to be processed
	@param [in] fetchOption
		an option item
	@param [in] value
		an option value. A type of value depends on the @c valueType as follows.
		@c valueType | Type of @c value
		-|-
		INTEGER | int32_t*
		LONG | int64_t*
		BOOL | @ref GSBool*
	@param [in] valueType
		a type of option value
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if there is any violation to the option specific restrictions
		- if @c NULL is specified to pointer type arguments
		- if this function is called after the corresponding @ref GSContainer
			is closed

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetFetchOption(
		GSQuery *query, GSFetchOption fetchOption,
		const void *value, GSType valueType);

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSQuery
	@brief 直近に実行した結果の@ref GSRowSet を取得します。
	@par
		一度取得すると、以降新たに指定のクエリを実行するまで@ref GSRowSet
		が取得できなくなります。
	@par
		@ref GS_FETCH_PARTIAL_EXECUTION が有効に設定されていた場合、
		クエリ実行処理の続きを行う場合があります。
	@param [in] query
		処理対象の@ref GSQuery
	@param [out] rowSet
		直近に実行した結果の@ref GSRowSet インスタンスを格納するための
		ポインタ変数へのポインタ値。取得済みの場合、もしくは、一度もクエリを
		実行したことのない場合は@c NULL が設定されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- この処理または関連するトランザクションのタイムアウト、
			対応するコンテナの削除もしくはスキーマ変更、接続障害が発生した場合、
			または@ref GSContainer のクローズ後に呼び出された場合
		- ポインタ型引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSQuery
	@brief Returns @ref GSRowSet as the latest result.
	@par
		Once @ref GSRowSet is returned, it cannot be obtained until the new
		query is executed.
	@par
		When @ref GS_FETCH_PARTIAL_EXECUTION has been set
		to be effective, the continuation of the query execution may
		be executed in this method.
	@param [in] query
		@ref GSQuery to be processed
	@param [out] rowSet
		the pointer to a pointer variable to store @ref GSRowSet instance as
		the latest result. @c NULL is set if already acquired, or if there is
		no query execution.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if this function is called after the corresponding @ref GSContainer
			is closed
		- if @c NULL is specified to pointer type arguments
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetRowSet(
		GSQuery *query, GSRowSet **rowSet);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

//
// RowSet API
//

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief 指定の@ref GSRowSet インスタンスを解放します。
	@param [in,out] rowSet
		クローズ対象の@ref GSRowSet インスタンスを指しているポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSRowSet インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。

	@EN
	@ingroup Group_GSRowSet
	@brief Releases a specified @ref GSRowSet instance.
	@param [in,out] rowSet
		the pointer to a pointer variable that refers to @ref GSRowSet
		instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSRowSet instance which was
		already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseRowSet(GSRowSet **rowSet);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief 現在のカーソル位置のロウを削除します。
	@par
		ロックを有効にして取得した@ref GSRowSet に対してのみ使用できます。
		また、@ref gsDeleteRow と同様、コンテナの種別ならびに設定によっては、
		さらに制限が設けられています。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のロウ集合の種別が@ref GS_ROW_SET_CONTAINER_ROWS 以外の場合
		- 対象位置のロウが存在しない場合
		- ロックを有効にせずに取得した@ref GSRowSet に対して呼び出された場合
		- この処理または関連するトランザクションのタイムアウト、
			対応するコンテナの削除もしくはスキーマ変更、接続障害が発生した場合、
			または対応するコンテナのクローズ後に呼び出された場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSRowSet
	@brief Deletes the Row at the current cursor position.
	@par
		It can be used only for @ref GSRowSet obtained with locking enabled.
		Like @ref gsDeleteRow, further limitations are placed depending on the
		type and settings of a Container.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the type of the specified Row set is not
			@ref GS_ROW_SET_CONTAINER_ROWS
		- if there is no Row at the cursor position
		- if this function is called to @ref GSRowSet without enabling the lock
		- if a timeout occurs during this operation or related transaction,
			a specified Container is deleted, its schema is changed,
			a connection failure occurs, or this functions is called after the
			relevant Container is closed
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsDeleteCurrentRow(
		GSRowSet *rowSet);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief ロウ集合内の後続のロウにカーソル移動し、移動後の位置にあるロウオブジェクトを
		取得します。
		@par
		@ref GS_FETCH_PARTIAL_EXECUTION が有効に設定されていた場合、
		クエリ実行処理の続きを行う場合があります。
	@attention
		対応する@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@attention
		文字列や配列などの可変長のデータを格納するために、指定の@ref GSRowSet
		と対応する@ref GSGridStore インスタンス上で管理される、一時的なメモリ領域を
		使用します。この領域は、対応する@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@param [out] rowObj
		取得対象のロウの内容を格納するためのロウオブジェクト。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		ロウオブジェクトのフィールドのうち一部またはすべてが変更されることがあります。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のロウ集合の種別が@ref GS_ROW_SET_CONTAINER_ROWS 以外の場合
		- 対象位置のロウが存在しない場合
		- 引数に@c NULL が指定された場合
		- この処理または関連するトランザクションのタイムアウト、
			対応するコンテナの削除もしくはスキーマ変更、接続障害が発生した場合
		- 対応する@ref GSContainer のクローズ後に呼び出された場合

	@EN
	@ingroup Group_GSRowSet
	@brief Moves the cursor to the next Row in a Row set and returns the Row
		object at the moved position.
	@par
		When @ref GS_FETCH_PARTIAL_EXECUTION has been set
		to be effective, the continuation of the query execution may
		be executed in this method.
	@attention
		The behavior is undefined if the type of Row object bound to
		corresponding @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@attention
		In order to store the variable-length data such as string or array, it
		uses a temporary memory area which is managed by the specified
		@ref GSGridStore instance corresponding to the specified
		@ref GSRowSet.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@param [out] rowObj
		The Row object to store the contents of target Row to be obtained.
		Some or all fields in the Row object may be changed if
		non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the type of the specified Row set is not
			@ref GS_ROW_SET_CONTAINER_ROWS
		- if there is no Row at the cursor position
		- if @c NULL is specified in the argument(s)
		- if this function is called after the corresponding @ref GSContainer
			is closed

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetNextRow(
		GSRowSet *rowSet, void *rowObj);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief ロウ集合内の後続のロウにカーソル移動し、移動後の位置にある集計結果を
		取得します。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@param [out] aggregationResult
		集計結果を@ref GSAggregationResult として格納するためのポインタ変数への
		ポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、対応するポインタ変数に@c NULL が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のロウ集合の種別が@ref GS_ROW_SET_AGGREGATION_RESULT 以外の場合
		- 対象位置の集計結果が存在しない場合
		- 引数に@c NULL が指定された場合
		- 対応する@ref GSContainer のクローズ後に呼び出された場合

	@EN
	@ingroup Group_GSRowSet
	@brief Moves the cursor to the next Row in a Row set and returns the
		aggregation result at the moved position.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@param [out] aggregationResult
		the pointer to a pointer variable to store the aggregation result as
		@ref GSAggregationResult.
		@c NULL is stored to corresponding pointer variable if pointer is not
		@c NULL and non-@ref GS_RESULT_OK is returned as the execution result.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the type of the specified Row set is not
			@ref GS_ROW_SET_AGGREGATION_RESULT
		- if there is no aggregation result at the cursor position
		- if @c NULL is specified in the argument(s)
		- if this function is called after the corresponding @ref GSContainer
			is closed

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetNextAggregation(
		GSRowSet *rowSet, GSAggregationResult **aggregationResult);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief ロウ集合内の後続のロウにカーソル移動し、移動後の位置にある
		クエリ解析結果エントリを取得します。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@param [out] queryAnalysis
		クエリ解析結果エントリの内容を格納するための@ref GSQueryAnalysisEntry 。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、このポインタ値が
		@c NULL 以外の値であれば、@ref GS_QUERY_ANALYSIS_ENTRY_INITIALIZER
		と同一の内容の初期値が格納されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のロウ集合の種別が@ref GS_ROW_SET_QUERY_ANALYSIS 以外の場合
		- 対象位置のエントリが存在しない場合
		- 引数に@c NULL が指定された場合
		- 対応する@ref GSContainer のクローズ後に呼び出された場合

	@EN
	@ingroup Group_GSRowSet
	@brief Moves the cursor to the next Row in a Row set and returns the query
		analysis result entry at the moved position.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@param [out] queryAnalysis
		The pointer value to @ref GSQueryAnalysisEntry for storing the contents
		of query analysis result entry.
		If @ref GS_RESULT_OK is not returned as the execution result and this
		is not @c NULL pointer, the initial value which is same as
		@ref GS_QUERY_ANALYSIS_ENTRY_INITIALIZER is stored.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the type of the specified Row set is not
			@ref GS_ROW_SET_QUERY_ANALYSIS
		- if there is no entry at the cursor position
		- if @c NULL is specified in the argument(s)
		- if this function is called after the corresponding @ref GSContainer
			is closed

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetNextQueryAnalysis(
		GSRowSet *rowSet, GSQueryAnalysisEntry *queryAnalysis);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief ロウ集合の種別を取得します。
	@par
		ロウ集合の種別に応じて、それぞれ次の取得機能が使用できます。
		ロウ集合の種別 | 使用できる取得機能
		-|-
		@ref GS_ROW_SET_CONTAINER_ROWS | @ref gsGetNextRow
		@ref GS_ROW_SET_AGGREGATION_RESULT | @ref gsGetNextAggregation
		@ref GS_ROW_SET_QUERY_ANALYSIS | @ref gsGetNextQueryAnalysis
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@return ロウ集合の種別。ただし、@c rowSet に@c NULL が指定された場合は@c -1

	@EN
	@ingroup Group_GSRowSet
	@brief Returns the type of Row set.
	@par
		Following acquisition functions can be used depending on the type of
		Row set.
		Type of Row set | Available acquisition function
		-|-
		@ref GS_ROW_SET_CONTAINER_ROWS | @ref gsGetNextRow
		@ref GS_ROW_SET_AGGREGATION_RESULT | @ref gsGetNextAggregation
		@ref GS_ROW_SET_QUERY_ANALYSIS | @ref gsGetNextQueryAnalysis
	@param [in] rowSet
		@ref GSRowSet to be processed
	@return The type of Row set. However, @c -1 is returned if @c NULL is
		specified to @c rowSet

	@ENDL
 */
GS_DLL_PUBLIC GSRowSetType GS_API_CALL gsGetRowSetType(GSRowSet *rowSet);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief ロウ集合のサイズ、すなわちロウ集合作成時点におけるロウの数を返します。
	@par
		@ref GS_FETCH_PARTIAL_EXECUTION が有効に設定されていた場合、
		クエリ実行処理の進行状況によらず、結果を求めることはできません。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@return ロウ集合のサイズ。ただし、@c rowSet に@c NULL が指定された場合、
		またはオプション設定の影響によりロウの数を取得できない場合は@c -1

	@EN
	@ingroup Group_GSRowSet
	@brief Returns the size of Row set, i.e. the number of Row when a Row set
		is created.
	@par
		When @ref GS_FETCH_PARTIAL_EXECUTION has been set to be effective,
		it is not possible to get the results despite the status of the query
		execution progress.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@return The size of Row set. However, @c -1 is returned if @c NULL is
		specified to @c rowSet

	@ENDL
 */
GS_DLL_PUBLIC int32_t GS_API_CALL gsGetRowSetSize(GSRowSet *rowSet);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief 現在のカーソル位置を基準として、ロウ集合内に後続のロウが存在するかどうかを
		返します。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@return 後続のロウが存在するかどうか。ただし、@c rowSet に@c NULL が指定された
		場合は@ref GS_FALSE

	@EN
	@ingroup Group_GSRowSet
	@brief Returns whether a Row set has at least one Row ahead of the current
		cursor position.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@return @ref GS_TRUE if a Row exists ahead of the current cursor position.
		However, @ref GS_FALSE is returned if @c NULL is specified to @c rowSet

	@ENDL
 */
GS_DLL_PUBLIC GSBool GS_API_CALL gsHasNextRow(GSRowSet *rowSet);

/*!
	@JP
	@ingroup Group_GSRowSet
	@brief 現在のカーソル位置のロウについて、指定のロウオブジェクトを使用してロウキー
		以外の値を更新します。
	@attention
		対応する@ref GSContainer にバインドされたロウオブジェクトの型と
		指定のロウオブジェクトの型とが一致しない場合、この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@par
		ロックを有効にして取得した@ref GSRowSet に対してのみ使用できます。
		また、@ref gsPutRow と同様、コンテナの種別ならびに設定によっては、
		さらに制限が設けられています。
	@param [in] rowSet
		処理対象の@ref GSRowSet
	@param [in] rowObj
		更新するロウの内容と対応するロウオブジェクト。ロウキーの内容は無視されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 指定のロウ集合の種別が@ref GS_ROW_SET_CONTAINER_ROWS 以外の場合
		- 対象位置のロウが存在しない場合
		- ロックを有効にせずに取得した@ref GSRowSet に対して呼び出された場合
		- この処理または関連するトランザクションのタイムアウト、
			対応するコンテナの削除もしくはスキーマ変更、接続障害が発生した場合、
			または対応するコンテナのクローズ後に呼び出された場合。
			また、指定のロウオブジェクト内のロウキー以外のフィールドについて、
			文字列など可変長サイズのデータへのポインタ値に
			@c NULL が含まれていた場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSRowSet
	@brief Updates the values except a Row key of the Row at the cursor
		position, using the specified Row object.
	@attention
		The behavior is undefined if the type of Row object bound to
		corresponding @ref GSContainer and the type of specified Row object
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@par
		It can be used only for @ref GSRowSet obtained with locking enabled.
		Like @ref gsPutRow, further limitations are placed depending on the
		type and settings of a Container.
	@param [in] rowSet
		@ref GSRowSet to be processed
	@param [in] rowObj
		A Row object representing the content of a Row to be updated.
		The contents of Row key are ignored.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if the type of the specified Row set is not
			@ref GS_ROW_SET_CONTAINER_ROWS
		- if there is no Row at the cursor position
		- if this function is called to @ref GSRowSet without enabling the lock
		- if a timeout occurs during this operation or related transaction,
			a specified Container is deleted, its schema is changed,
			a connection failure occurs, or this functions is called after the
			relevant Container is closed, or if @c NULL is included in the
			pointer to the data whose size is variable-length like string in
			the fields other than Row key in the specified Row object
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsUpdateCurrentRow(
		GSRowSet *rowSet, const void *rowObj);

//
// AggregationResult API
//

/*!
	@JP
	@ingroup Group_GSAggregationResult
	@brief 指定の@ref GSAggregationResult インスタンスを解放します。
	@param [in,out] aggregationResult
		クローズ対象の@ref GSAggregationResult インスタンスを指しているポインタ変数への
		ポインタ値。クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSAggregationResult インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。

	@EN
	@ingroup Group_GSAggregationResult
	@brief Releases a specified @ref GSAggregationResult instance.
	@param [in,out] aggregationResult
		the pointer to a pointer variable that refers to
		@ref GSAggregationResult instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSAggregationResult instance which
		was already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseAggregationResult(
		GSAggregationResult **aggregationResult);

/*!
	@JP
	@ingroup Group_GSAggregationResult
	@brief 集計結果を指定の型の値として取得します。
	@par
		取り出しできる型は、指定の@c aggregationResult が保持している
		値の型によって、次のように決まります。
		取り出しできる値の型 | 保持している値の型
		-|-
		LONG型(@ref GS_TYPE_LONG) | 数値型
		DOUBLE型(@ref GS_TYPE_DOUBLE) | 数値型
		TIMESTAMP型(@ref GS_TYPE_TIMESTAMP) | TIMESTAMP型
	@par
		また、@c value として指定できる型は、@c valueType によって
		次のように決まります。
		@c valueType | @c value の型
		-|-
		LONG型(@ref GS_TYPE_LONG) | int64_t*
		DOUBLE型(@ref GS_TYPE_DOUBLE) | double*
		TIMESTAMP型(@ref GS_TYPE_TIMESTAMP) | @ref GSTimestamp*
	@attention
		@c valueType と@c value の型との対応が正しくない場合、
		この処理の動作は未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] aggregationResult
		処理対象の@ref GSAggregationResult
	@param [out] value
		取り出す値を格納するための変数へのポインタ値。
		@c aggregationResult が@c NULL の場合、また、@c valueType が
		@c value として指定できる型のいずれとも対応しない場合は、
		何も格納しません。
		@c aggregationResult が保持している値の型と照らし合わせて、@c valueType
		が取り出しできない型であった場合、初期値として@c 0 を格納します。
	@param [in] valueType
		取り出す値の型
	@return 指定の@c aggregationResult が保持している値を取り出しできたかどうか。
		次の場合、@ref GS_FALSE を返します。
		- @c valueType として取り出しできない型が指定された場合
		- ポインタ型引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSAggregationResult
	@brief Returns the aggregation result as the value with specified type.
	@par
		The type which can be retrieved is determined depending on the type of
		the value which is hold by the specified @c aggregationResult as
		follows.
		Type of the retrievable value | Type of the held value
		-|-
		LONG type (@ref GS_TYPE_LONG) | Numeric type
		DOUBLE type (@ref GS_TYPE_DOUBLE) | Numeric type
		TIMESTAMP type (@ref GS_TYPE_TIMESTAMP) | TIMESTAMP type
	@par
		In addition, the type which can be specified as @c value is determined
		depending on @c valueType as follows.
		@c valueType | Type of @c value
		-|-
		LONG type (@ref GS_TYPE_LONG) | int64_t*
		DOUBLE type (@ref GS_TYPE_DOUBLE) | double*
		TIMESTAMP type (@ref GS_TYPE_TIMESTAMP) | @ref GSTimestamp*
	@attention
		The behavior is undefined if the @c valueType and the type of @c value
		does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] aggregationResult
		@ref GSAggregationResult to be processed
	@param [out] value
		The pointer to a variable to store the retrieved value.
		If @c aggregationResult is @c NULL and @c valueType does not correspond
		to any of the type that can be specified as @c value, no value is
		stored.
		If the type of @c valueType is not retrievable by comparing to the type
		of value which is hold in @c aggregationResult, @c 0 is stored as
		initial value.
	@param [in] valueType
		The type of retrieved value
	@return Indicates whether the value held by specified @c aggregationResult
		can be retrieved or not.
		It returns @ref GS_FALSE in the following cases.
		- if the type that can not be picked out as @c valueType is specified
		- if @c NULL is specified to pointer type arguments

	@ENDL
 */
GS_DLL_PUBLIC GSBool GS_API_CALL gsGetAggregationValue(
		GSAggregationResult *aggregationResult, void *value,
		GSType valueType);

#if GS_COMPATIBILITY_SUPPORT_3_5

/*!
	@JP
	@brief 数値型の集計値をLONG型(@c int64_t)として取得します。
	@par
		数値型以外の値を保持している場合、@c assigned 引数には@ref GS_FALSE
		が格納されます。LONG型以外の数値を保持している場合、LONG型に変換した
		ものが格納されます。
	@param [in] aggregationResult
		取得対象の@ref GSAggregationResult
	@param [out] value
		集計値を格納するための変数へのポインタ値
	@param [out] assigned
		期待の型の値を取得できたかどうかを格納するための変数へのポインタ値。
		@c NULL が指定された場合、取得できたかどうかの情報は格納されません
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- @c assigned 以外の引数に@c NULL が指定された場合
	@since 3.5

	@EN
	@brief Gets the aggregate value of numeric type as LONG (@c int64_t).
	@par
		If a non-numeric value is saved, the @c assigned argument contains
		@ref GS_FALSE. If a numeric value other than LONG is held、a converted
		LONG type will be stored.
	@param [in] aggregationResult
		Target acquisition of @ref GSAggregationResult
	@param [out] value
		The pointer value to the variable for storing the aggregate value
	@param [out] assigned
		A pointer value to a variable for storing whether or not the expected
		type value can be acquired. When @c NULL is specified, the information
		on whether or not it was acquired is not stored.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- If @c NULL is specified as an argument other than "assigned"
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetAggregationValueAsLong(
		GSAggregationResult *aggregationResult, int64_t *value,
		GSBool *assigned);

/*!
	@JP
	@brief 数値型の集計値をDOUBLE型(@c double)として取得します。
	@par
		数値型以外の値を保持している場合、@c assigned 引数には@ref GS_FALSE
		が格納されます。DOUBLE型以外の数値を保持している場合、DOUBLE型に変換した
		ものが格納されます。
	@param [in] aggregationResult
		取得対象の@ref GSAggregationResult
	@param [out] value
		集計値を格納するための変数へのポインタ値
	@param [out] assigned
		期待の型の値を取得できたかどうかを格納するための変数へのポインタ値。
		@c NULL が指定された場合、取得できたかどうかの情報は格納されません
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- @c assigned 以外の引数に@c NULL が指定された場合
	@since 3.5

	@EN
	@brief Gets the aggregate value of numeric type as DOUBLE (@c double).
	@par
		If a non-numeric value is saved, the @c assigned argument contains
		@ref GS_FALSE. If a numeric value other than DOUBLE is held、a converted
		DOUBLE type will be stored.
	@param [in] aggregationResult
		Target acquisition of @ref GSAggregationResult
	@param [out] value
		The pointer value to the variable for storing the aggregate value
	@param [out] assigned
		A pointer value to a variable for storing whether or not the expected
		type value can be acquired. When @c NULL is specified, the information
		on whether or not it was acquired is not stored.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- If @c NULL is specified as an argument other than "assigned"
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetAggregationValueAsDouble(
		GSAggregationResult *aggregationResult, double *value,
		GSBool *assigned);

/*!
	@JP
	@brief 時刻型の集計値をTIMESTAMP型(@ref GSTimestamp)で取得します。
	@par
		TIMESTAMP型以外の値を保持している場合、@c assigned 引数には
		@ref GS_FALSE が格納されます。
	@param [in] aggregationResult
		取得対象の@ref GSAggregationResult
	@param [out] value
		集計値を格納するための変数へのポインタ値
	@param [out] assigned
		期待の型の値を取得できたかどうかを格納するための変数へのポインタ値。
		@c NULL が指定された場合、取得できたかどうかの情報は格納されません
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を
		返します。
		- @c assigned 以外の引数に@c NULL が指定された場合
	@since 3.5

	@EN
	@brief Gets the aggregate value of numeric type as TIMESTAMP (@c GSTimestamp).
	@par
		If a non TIMESTAMP value is saved, the @c assigned argument contains
		@ref GS_FALSE.
	@param [in] aggregationResult
		Target acquisition of @ref GSAggregationResult
	@param [out] value
		The pointer value to the variable for storing the aggregate value
	@param [out] assigned
		A pointer value to a variable for storing whether or not the expected
		type value can be acquired. When @c NULL is specified, the information
		on whether or not it was acquired is not stored.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- If @c NULL is specified as an argument other than "assigned"
	@since 3.5

	@ENDL
*/
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetAggregationValueAsTimestamp(
		GSAggregationResult *aggregationResult, GSTimestamp *value,
		GSBool *assigned);

#endif	// GS_COMPATIBILITY_SUPPORT_3_5

//
// RowKeyPredicate API
//

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 指定の@ref GSRowKeyPredicate インスタンスを解放します。
	@param [in,out] predicate
		クローズ対象の@ref GSRowKeyPredicate インスタンスを指している
		ポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSRowKeyPredicate インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Releases a specified @ref GSRowKeyPredicate instance.
	@param [in,out] predicate
		the pointer to a pointer variable that refers to @ref GSRowKeyPredicate
		instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSRowKeyPredicate instance which was
		already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsCloseRowKeyPredicate(
		GSRowKeyPredicate **predicate);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 合致条件の評価対象とするロウキーの型を取得します。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] keyType
		合致条件の評価対象とするロウキーの型を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the type of Row key used as a search condition.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] keyType
		the pointer to a variable to store the type of Row key used as a search
		condition.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateKeyType(
		GSRowKeyPredicate *predicate, GSType *keyType);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] startKey
		開始位置とするロウキーの値を格納するための変数へのポインタ値。
		開始位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the Row key at the starting position of the
		range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] startKey
		the pointer to a variable to store the value of the Row key at the
		starting position.
		@c NULL is stored if starting position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateStartKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue **startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするSTRING型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] startKey
		開始位置とするロウキーの値を格納するための変数へのポインタ値。
		開始位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the STRING-type Row key at the starting
		position of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] startKey
		the pointer to a variable to store the value of the Row key at the
		starting position.
		@c NULL is stored if starting position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateStartKeyAsString(
		GSRowKeyPredicate *predicate, const GSChar **startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするINTEGER型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] startKey
		開始位置とするロウキーの値を格納するための変数へのポインタ値。
		開始位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the INTEGER-type Row key at the starting
		position of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] startKey
		the pointer to a variable to store the value of the Row key at the
		starting position.
		@c NULL is stored if starting position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateStartKeyAsInteger(
		GSRowKeyPredicate *predicate, const int32_t **startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするLONG型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] startKey
		開始位置とするロウキーの値を格納するための変数へのポインタ値。
		開始位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the LONG-type Row key at the starting
		position of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] startKey
		the pointer to a variable to store the value of the Row key at the
		starting position.
		@c NULL is stored if starting position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateStartKeyAsLong(
		GSRowKeyPredicate *predicate, const int64_t **startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするTIMESTAMP型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] startKey
		開始位置とするロウキーの値を格納するための変数へのポインタ値。
		開始位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the TIMESTAMP-type Row key at the starting
		position of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] startKey
		the pointer to a variable to store the value of the Row key at the
		starting position.
		@c NULL is stored if starting position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateStartKeyAsTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp **startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] finishKey
		終了位置とするロウキーの値を格納するための変数へのポインタ値。
		終了位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the Row key at the end position of the
		range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] finishKey
		the pointer to a variable to store the value of the Row key at the
		end position.
		@c NULL is stored if end position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateFinishKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue **finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするSTRING型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] finishKey
		終了位置とするロウキーの値を格納するための変数へのポインタ値。
		終了位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the STRING-type Row key at the end position
		of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] finishKey
		the pointer to a variable to store the value of the Row key at the
		end position.
		@c NULL is stored if end position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateFinishKeyAsString(
		GSRowKeyPredicate *predicate, const GSChar **finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするINTEGER型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] finishKey
		終了位置とするロウキーの値を格納するための変数へのポインタ値。
		終了位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the INTEGER-type Row key at the end position
		of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] finishKey
		the pointer to a variable to store the value of the Row key at the
		end position.
		@c NULL is stored if end position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateFinishKeyAsInteger(
		GSRowKeyPredicate *predicate, const int32_t **finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするLONG型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] finishKey
		終了位置とするロウキーの値を格納するための変数へのポインタ値。
		終了位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the LONG-type Row key at the end position
		of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] finishKey
		the pointer to a variable to store the value of the Row key at the
		end position.
		@c NULL is stored if end position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateFinishKeyAsLong(
		GSRowKeyPredicate *predicate, const int64_t **finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするTIMESTAMP型ロウキーの値を取得します。
	@attention
		値を格納する領域を確保するために、指定の@ref GSRowKeyPredicate
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] finishKey
		終了位置とするロウキーの値を格納するための変数へのポインタ値。
		終了位置が設定されていない場合は@c NULL が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns the value of the TIMESTAMP-type Row key at the end position
		of the range condition.
	@attention
		In order to allocate the area for storing the value, it might use a
		temporary memory area which is managed by @ref GSGridStore instance
		related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] finishKey
		the pointer to a variable to store the value of the Row key at the
		end position.
		@c NULL is stored if end position is not set.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateFinishKeyAsTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp **finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件を構成するロウキーの値の集合を取得します。
	@attention
		値ならびにその列を格納する領域を確保するために、指定の
		@ref GSRowKeyPredicate と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] keyList
		個別条件を構成するロウキーの値の集合を構成する配列のアドレスを
		格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		個別条件を構成するロウキーの値の集合の要素数を格納するための変数への
		ポインタ値。個別条件が設定されていない場合は@c 0 が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns a set of the values of the Row keys that configure the
		individual condition.
	@attention
		In order to allocate the area for storing the value and its column, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] keyList
		the pointer to a variable to store the address of array which contains
		a set of the values of the Row keys that configure the individual
		condition.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of elements in
		a set of the values of the Row keys that configure the individual
		condition.
		@c 0 is stored if no individual condition is set.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateDistinctKeysGeneral(
		GSRowKeyPredicate *predicate, const GSValue **keyList, size_t *size);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件を構成するSTRING型ロウキーの値の集合を取得します。
	@attention
		値ならびにその列を格納する領域を確保するために、指定の
		@ref GSRowKeyPredicate と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] keyList
		個別条件を構成するロウキーの値の集合を構成する配列のアドレスを
		格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		個別条件を構成するロウキーの値の集合の要素数を格納するための変数への
		ポインタ値。個別条件が設定されていない場合は@c 0 が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns a set of the values of the STRING-type Row keys that
		configure the individual condition.
	@attention
		In order to allocate the area for storing the value and its column, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] keyList
		the pointer to a variable to store the address of array which contains
		a set of the values of the Row keys that configure the individual
		condition.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of elements in
		a set of the values of the Row keys that configure the individual
		condition.
		@c 0 is stored if no individual condition is set.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateDistinctKeysAsString(
		GSRowKeyPredicate *predicate,
		const GSChar *const **keyList, size_t *size);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件を構成するINTEGER型ロウキーの値の集合を取得します。
	@attention
		値ならびにその列を格納する領域を確保するために、指定の
		@ref GSRowKeyPredicate と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] keyList
		個別条件を構成するロウキーの値の集合を構成する配列のアドレスを
		格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		個別条件を構成するロウキーの値の集合の要素数を格納するための変数への
		ポインタ値。個別条件が設定されていない場合は@c 0 が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns a set of the values of the INTEGER-type Row keys that
		configure the individual condition.
	@attention
		In order to allocate the area for storing the value and its column, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] keyList
		the pointer to a variable to store the address of array which contains
		a set of the values of the Row keys that configure the individual
		condition.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of elements in
		a set of the values of the Row keys that configure the individual
		condition.
		@c 0 is stored if no individual condition is set.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateDistinctKeysAsInteger(
		GSRowKeyPredicate *predicate, const int32_t **keyList, size_t *size);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件を構成するLONG型ロウキーの値の集合を取得します。
	@attention
		値ならびにその列を格納する領域を確保するために、指定の
		@ref GSRowKeyPredicate と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] keyList
		個別条件を構成するロウキーの値の集合を構成する配列のアドレスを
		格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		個別条件を構成するロウキーの値の集合の要素数を格納するための変数への
		ポインタ値。個別条件が設定されていない場合は@c 0 が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns a set of the values of the LONG-type Row keys that
		configure the individual condition.
	@attention
		In order to allocate the area for storing the value and its column, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] keyList
		the pointer to a variable to store the address of array which contains
		a set of the values of the Row keys that configure the individual
		condition.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of elements in
		a set of the values of the Row keys that configure the individual
		condition.
		@c 0 is stored if no individual condition is set.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateDistinctKeysAsLong(
		GSRowKeyPredicate *predicate, const int64_t **keyList, size_t *size);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件を構成するTIMESTAMP型ロウキーの値の集合を取得します。
	@attention
		値ならびにその列を格納する領域を確保するために、指定の
		@ref GSRowKeyPredicate と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [out] keyList
		個別条件を構成するロウキーの値の集合を構成する配列のアドレスを
		格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		個別条件を構成するロウキーの値の集合の要素数を格納するための変数への
		ポインタ値。個別条件が設定されていない場合は@c 0 が格納されます。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Returns a set of the values of the TIMESTAMP-type Row keys that
		configure the individual condition.
	@attention
		In order to allocate the area for storing the value and its column, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSRowKeyPredicate.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [out] keyList
		the pointer to a variable to store the address of array which contains
		a set of the values of the Row keys that configure the individual
		condition.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of elements in
		a set of the values of the Row keys that configure the individual
		condition.
		@c 0 is stored if no individual condition is set.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPredicateDistinctKeysAsTimestamp(
		GSRowKeyPredicate *predicate,
		const GSTimestamp **keyList, size_t *size);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするロウキーの値を設定します。
	@par
		設定された値より小さな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@attention
		指定ロウキーの値とその型との対応が一致しない場合、この処理の動作は
		未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] startKey
		開始位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@param [in] keyType
		開始位置とするロウキーの値の型
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the Row key as the start position of
		the range condition.
	@par
		The Row key which has smaller value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@attention
		The behavior is undefined if the value of the specified Row key and
		its corresponding type does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] startKey
		the value of the Row key as the start position. If @c NULL, the setting
		is cancelled
	@param [in] keyType
		the type of the Row key as the start position
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateStartKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue *startKey, GSType keyType);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするSTRING型ロウキーの値を設定します。
	@par
		設定された値より小さな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型では大小関係が定義されていないため、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] startKey
		開始位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the STRING-type Row key as the start position of
		the range conditions.
	@par
		The Row key which has smaller value than the specified value is
		considered to be unmatched.
	@par
		Since the magnitude relationship is not defined in STRING-type, it can
		be set as a condition but cannot be used in the actual judgment.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] startKey
		the value of the Row key as the start position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateStartKeyByString(
		GSRowKeyPredicate *predicate, const GSChar *startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするINTEGER型ロウキーの値を設定します。
	@par
		設定された値より小さな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] startKey
		開始位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the INTEGER-type Row key as the start position of
		the range conditions.
	@par
		The Row key which has smaller value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] startKey
		the value of the Row key as the start position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateStartKeyByInteger(
		GSRowKeyPredicate *predicate, const int32_t *startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするLONG型ロウキーの値を設定します。
	@par
		設定された値より小さな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] startKey
		開始位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the LONG-type Row key as the start position of
		the range conditions.
	@par
		The Row key which has smaller value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] startKey
		the value of the Row key as the start position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateStartKeyByLong(
		GSRowKeyPredicate *predicate, const int64_t *startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の開始位置とするTIMESTAMP型ロウキーの値を設定します。
	@par
		設定された値より小さな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] startKey
		開始位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the TIMESTAMP-type Row key as the start position of
		the range conditions.
	@par
		The Row key which has smaller value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] startKey
		the value of the Row key as the start position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateStartKeyByTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp *startKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするロウキーの値を設定します。
	@par
		設定された値より大きな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@attention
		指定ロウキーの値とその型との対応が一致しない場合、この処理の動作は
		未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] finishKey
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@param [in] keyType
		終了位置とするロウキーの値の型
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the Row key as the end position of
		the range condition.
	@par
		The Row key which has greater value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@attention
		The behavior is undefined if the value of the specified Row key and
		its corresponding type does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] finishKey
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@param [in] keyType
		the type of the Row key as the end position
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateFinishKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue *finishKey, GSType keyType);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするSTRING型ロウキーの値を設定します。
	@par
		設定された値より大きな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型では大小関係が定義されていないため、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] finishKey
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the STRING-type Row key as the end position of
		the range conditions.
	@par
		The Row key which has greater value than the specified value is
		considered to be unmatched.
	@par
		Since the magnitude relationship is not defined in STRING-type, it can
		be set as a condition but cannot be used in the actual judgment.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] finishKey
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateFinishKeyByString(
		GSRowKeyPredicate *predicate, const GSChar *finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするINTEGER型ロウキーの値を設定します。
	@par
		設定された値より大きな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] finishKey
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the INTEGER-type Row key as the end position of
		the range conditions.
	@par
		The Row key which has greater value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] finishKey
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateFinishKeyByInteger(
		GSRowKeyPredicate *predicate, const int32_t *finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするLONG型ロウキーの値を設定します。
	@par
		設定された値より大きな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] finishKey
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the LONG-type Row key as the end position of
		the range conditions.
	@par
		The Row key which has greater value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] finishKey
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateFinishKeyByLong(
		GSRowKeyPredicate *predicate, const int64_t *finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 範囲条件の終了位置とするTIMESTAMP型ロウキーの値を設定します。
	@par
		設定された値より大きな値のロウキーは合致しないものとみなされるようになります。
	@par
		STRING型のように大小関係の定義されていない型の場合、
		条件として設定はできるものの、実際の判定に用いることはできません。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] finishKey
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c predicate 引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Sets the value of the TIMESTAMP-type Row key as the end position of
		the range conditions.
	@par
		The Row key which has greater value than the specified value is
		considered to be unmatched.
	@par
		A type with an undefined magnitude relationship can be set as a
		condition but cannot be used in the actual judgment, e.g. STRING type.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] finishKey
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c predicate
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsSetPredicateFinishKeyByTimestamp(
		GSRowKeyPredicate *predicate, const GSTimestamp *finishKey);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件の要素の一つとするロウキーの値を追加します。
	@par
		追加された値と同一の値のロウキーは合致するものとみなされるようになります。
	@attention
		指定ロウキーの値とその型との対応が一致しない場合、この処理の動作は
		未定義です。
		アクセス違反により実行プロセスが異常終了する可能性があります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] key
		個別条件の要素の一つとするロウキーの値
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@param [in] keyType
		終了位置とするロウキーの値の型
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Adds the value of the Row key as one of the elements
		in the individual condition.
	@par
		The Row key which has the same value with the added value is
		considered to be matched.
	@attention
		The behavior is undefined if the value of the specified Row key and
		its corresponding type does not match.
		There is a possibility that the execution process is abnormally
		terminated by the access violation.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] key
		the value of Row key to be added as one of the elements in the
		individual condition.
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@param [in] keyType
		the type of the Row key as the end position
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAddPredicateKeyGeneral(
		GSRowKeyPredicate *predicate, const GSValue *key, GSType keyType);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件の要素の一つとするSTRING型ロウキーの値を追加します。
	@par
		追加された値と同一の値のロウキーは合致するものとみなされるようになります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] key
		個別条件の要素の一つとするロウキーの値
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Adds the value of the STRING-type Row key as one of the elements
		in the individual condition.
	@par
		The Row key which has the same value with the added value is
		considered to be matched.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] key
		the value of Row key to be added as one of the elements in the
		individual condition.
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAddPredicateKeyByString(
		GSRowKeyPredicate *predicate, const GSChar *key);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件の要素の一つとするINTEGER型ロウキーの値を追加します。
	@par
		追加された値と同一の値のロウキーは合致するものとみなされるようになります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] key
		個別条件の要素の一つとするロウキーの値
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Adds the value of the INTEGER-type Row key as one of the elements
		in the individual condition.
	@par
		The Row key which has the same value with the added value is
		considered to be matched.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] key
		the value of Row key to be added as one of the elements in the
		individual condition.
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAddPredicateKeyByInteger(
		GSRowKeyPredicate *predicate, int32_t key);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件の要素の一つとするLONG型ロウキーの値を追加します。
	@par
		追加された値と同一の値のロウキーは合致するものとみなされるようになります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] key
		個別条件の要素の一つとするロウキーの値
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Adds the value of the LONG-type Row key as one of the elements
		in the individual condition.
	@par
		The Row key which has the same value with the added value is
		considered to be matched.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] key
		the value of Row key to be added as one of the elements in the
		individual condition.
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAddPredicateKeyByLong(
		GSRowKeyPredicate *predicate, int64_t key);

/*!
	@JP
	@ingroup Group_GSRowKeyPredicate
	@brief 個別条件の要素の一つとするTIMESTAMP型ロウキーの値を追加します。
	@par
		追加された値と同一の値のロウキーは合致するものとみなされるようになります。
	@param [in] predicate
		処理対象の@ref GSRowKeyPredicate
	@param [in] key
		個別条件の要素の一つとするロウキーの値
		終了位置とするロウキーの値。@c NULL の場合、設定が解除されます
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 個別条件がすでに設定されていた場合
		- 期待した型が合致条件の評価対象とするロウキーの型と異なる場合
	@since 1.5

	@EN
	@ingroup Group_GSRowKeyPredicate
	@brief Adds the value of the TIMESTAMP-type Row key as one of the elements
		in the individual condition.
	@par
		The Row key which has the same value with the added value is
		considered to be matched.
	@param [in] predicate
		@ref GSRowKeyPredicate to be processed
	@param [in] key
		the value of Row key to be added as one of the elements in the
		individual condition.
		the value of the Row key as the end position. If @c NULL, the setting
		is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if an individual condition has already been set
		- if the expected type is different from the type of Row key to be
			evaluated in the match conditions
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAddPredicateKeyByTimestamp(
		GSRowKeyPredicate *predicate, GSTimestamp key);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

//
// PartitionController API
//

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定の@ref GSPartitionController インスタンスを解放します。
	@param [in,out] controller
		クローズ対象の@ref GSPartitionController インスタンスを指している
		ポインタ変数へのポインタ値。
		クローズすると、ポインタ変数に@c NULL が設定されます。
		以後、解放した@ref GSPartitionController インスタンスにアクセスしてはなりません。
		ポインタ値またはポインタ変数の値が@c NULL の場合は、クローズ処理を行いません。
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Releases a specified @ref GSPartitionController instance.
	@param [in,out] controller
		the pointer to a pointer variable that refers to
		@ref GSPartitionController instance to be closed.
		@c NULL is set to pointer variable when closed.
		It is prohibited to access the @ref GSPartitionController instance which
		was already released.
		Closing process is not executed if @c NULL is specified to this pointer
		or pointer variable.
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC void GS_API_CALL gsClosePartitionController(
		GSPartitionController **controller);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 対象とするGridDBクラスタのパーティション数を取得します。
	@par
		対象とするGridDBクラスタにおけるパーティション数設定の値を
		取得します。一度取得した結果はキャッシュされ、次にクラスタ障害・
		クラスタノード障害を検知するまで再びGridDBクラスタに問い合わせる
		ことはありません。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [out] partitionCount
		パーティション数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c -1 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get the number of partitions in the target GridDB cluster.
	@par
		Get the value of the number of partitions set in the target GridDB
		cluster. Results are cached once acquired and until the next cluster
		failure and cluster node failure is detected, no inquiry will be sent to
		the GridDB cluster again.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [out] partitionCount
		the pointer to a variable to store the number of partitions.
		@c -1 is stored if non-@ref GS_RESULT_OK is
		returned as the execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionCount(
		GSPartitionController *controller, int32_t *partitionCount);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定のパーティションに属するコンテナの総数を取得します。
	@par
		コンテナ数を求める際の計算量は、コンテナ数にはおおむね依存しません。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] partitionIndex
		パーティションインデックス。@c 0 以上パーティション数未満の値
	@param [out] containerCount
		コンテナ数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のパーティションインデックスが指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get the total number of containers belonging to a specified
		partition.
	@par
		The calculated quantity when determining the number of containers is
		generally not dependent on the number of containers.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] partitionIndex
		the partition index, from @c 0 to the number of partitions minus one
	@param [out] containerCount
		the pointer to a variable to store the number of Container.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if a specified partition index is out of range
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionContainerCount(
		GSPartitionController *controller, int32_t partitionIndex,
		int64_t *containerCount);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定のパーティションに所属するコンテナの名前の一覧を取得します。
	@par
		指定のパーティションについてコンテナの新規作成・構成変更・削除が
		行われたとしても、該当コンテナを除くとその前後で一覧の取得結果の順序が
		変わることはありません。それ以外の一覧の順序に関しては不定です。
		重複する名前が含まれることはありません。
	@par
		取得件数の上限が指定された場合、上限を超える場合、後方のものから
		切り捨てられます。指定条件に該当するものが存在しない場合、空のリストが
		求まります。
	@attention
		コンテナ名一覧を格納する領域を確保するために、指定の
		@ref GSPartitionController と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] partitionIndex
		パーティションインデックス。@c 0 以上パーティション数未満の値。
	@param [in] start
		取得範囲の開始位置。@c 0 以上の値
	@param [in] limit
		取得件数の上限。@c NULL の場合、上限なしとみなされる
	@param [out] nameList
		コンテナ名一覧の配列を格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		コンテナ名一覧の配列の要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c limit 以外のポインタ型引数に@c NULL が指定された場合
		- 範囲外のパーティションインデックスが指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get a list of the Container names belonging to a specified partition.
	@par
		For the specified partition, the sequence of the list of acquisition
		results before and after will not be changed when the relevant Container
		is excluded even if a Container is newly created, its composition
		changed or the Container is deleted. All other lists are compiled in no
		particular order. No duplicate names will be included.
	@par
		If the upper limit of the number of acquisition cases is specified, the
		cases will be cut off starting from the ones at the back if the upper
		limit is exceeded. If no relevant specified condition exists, a blank
		list is returned.
	@attention
		In order to allocate the area for storing the list of Container name, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSPartitionController.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] partitionIndex
		the partition index, from @c 0 to the number of partitions minus one
	@param [in] start
		the start position of the acquisition range. A value must be greater
		than or equal to @c 0
	@param [in] limit
		the upper limit of the number of cases acquired. If @c NULL, it is
		considered to be no upper limit
	@param [out] nameList
		the pointer to a pointer variable to store the array list of Container
		name.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		Container name list.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments except @c limit
		- if a specified partition index is out of range
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionContainerNames(
		GSPartitionController *controller, int32_t partitionIndex,
		int64_t start, const int64_t *limit,
		const GSChar *const **nameList, size_t *size);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定のパーティションに対応するノードのアドレス一覧を取得します。
	@par
		一覧の順序に関しては不定です。重複するアドレスが含まれることはありません。
	@attention
		アドレス一覧を格納する領域を確保するために、指定の
		@ref GSPartitionController と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] partitionIndex
		パーティションインデックス。@c 0 以上パーティション数未満の値。
	@param [out] addressList
		アドレスの文字列表現一覧の配列を格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		コンテナ名一覧の配列の要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のパーティションインデックスが指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get a list of the addresses of the nodes corresponding to a specified
		partition.
	@par
		The list will be compiled in no particular order. No duplicate address
		will be included.
	@attention
		In order to allocate the area for storing the list of address, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSPartitionController.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] partitionIndex
		the partition index, from @c 0 to the number of partitions minus one
	@param [out] addressList
		the pointer to a pointer variable to store the array list of string
		representation of the address.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		Container name list.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if a specified partition index is out of range
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionHosts(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar *const **addressList, size_t *size);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定のパーティションに対応するオーナノードのアドレスを取得します。
	@par
		オーナノードとは、@ref gsGetGridStore おける一貫性レベルとして
		@c "IMMEDIATE" を指定した場合に、常に選択されるノードのことです。
	@attention
		アドレスを格納する領域を確保するために、指定の@ref GSPartitionController
		と関係する@ref GSGridStore インスタンス上で管理される一時的なメモリ領域を
		使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] partitionIndex
		パーティションインデックス。@c 0 以上パーティション数未満の値。
	@param [out] address
		アドレスの文字列表現を格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のパーティションインデックスが指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get the address of the owner node corresponding to a specified
		partition.
	@par
		An owner node is a node that is always selected when @c "IMMEDIATE" is
		specified as a consistency level in @ref gsGetGridStore.
	@attention
		In order to allocate the area for storing the address, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSPartitionController.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] partitionIndex
		the partition index, from @c 0 to the number of partitions minus one
	@param [out] address
		the pointer to a pointer variable to store the string representation of
		the address.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if a specified partition index is out of range
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionOwnerHost(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar **address);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定のパーティションに対応するバックアップノードのアドレス一覧を取得します。
	@par
		オーナノードとは、@ref gsGetGridStore おける一貫性レベルとして
		@c "EVENTUAL" を指定した場合に、優先的に選択されるノードのことです。
	@par
		一覧の順序に関しては不定です。重複するアドレスが含まれることはありません。
	@attention
		アドレス一覧を格納する領域を確保するために、指定の
		@ref GSPartitionController と関係する@ref GSGridStore インスタンス上で
		管理される一時的なメモリ領域を使用する場合があります。
		この領域は、指定の@ref GSGridStore もしくはその関連リソースに対し、
		この関数もしくは同様に一時的なメモリ領域を使用する関数が再び実行されるまで
		有効です。 無効になった領域にアクセスしようとした場合の動作は未定義です。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] partitionIndex
		パーティションインデックス。@c 0 以上パーティション数未満の値。
	@param [out] addressList
		アドレスの文字列表現一覧の配列を格納するためのポインタ変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c NULL が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@param [out] size
		コンテナ名一覧の配列の要素数を格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c 0 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- ポインタ型引数に@c NULL が指定された場合
		- 範囲外のパーティションインデックスが指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get a list of the addresses of the backup nodes corresponding to a
		specified partition.
	@par
		A backup node is a node that is selected with a higher priority when
		@c "EVENTUAL" is specified as a consistency level in
		@ref gsGetGridStore.
	@par
		The list will be compiled in no particular order. No duplicate address
		will be included.
	@attention
		In order to allocate the area for storing the list of address, it
		might use a temporary memory area which is managed by @ref GSGridStore
		instance related to specified @ref GSPartitionController.
		This area is valid until this function or similar functions which use a
		temporary memory area managed by specified @ref GSGridStore instance or
		by related resources is executed again.
		The behavior is undefined when the area which has been invalidated is
		accessed.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] partitionIndex
		the partition index, from @c 0 to the number of partitions minus one
	@param [out] addressList
		the pointer to a pointer variable to store the array list of string
		representation of the address.
		@c NULL is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@param [out] size
		the pointer to a variable to store the number of array elements of the
		Container name list.
		@c 0 is stored if non-@ref GS_RESULT_OK is returned as the execution
		result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to pointer type arguments
		- if a specified partition index is out of range
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionBackupHosts(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar *const **addressList, size_t *size);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 優先的に選択されるホストのアドレスを設定します。
	@par
		バックアップノードへの接続など、可能な接続先が複数存在する場合に、
		設定されたアドレスが候補に含まれていれば常に選択されるようになります。
		それ以外の場合は設定が無視されます。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] partitionIndex
		パーティションインデックス。@c 0 以上パーティション数未満の値。
	@param [in] host
		優先的に選択されるホストのアドレス。IPアドレス(IPV4のみ)も可。
		@c NULL の場合、設定が解除される
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- @c controller 引数に@c NULL が指定された場合
		- 範囲外のパーティションインデックスが指定された場合
		- アドレスの名前解決に失敗した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Set the address of the host to be prioritized in the selection.
	@par
		If multiple possible destinations exist, e.g. connections to backup
		nodes, etc., the address set will always be selected if it is included
		in the candidate destination. The setting is ignored otherwise.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] partitionIndex
		the partition index, from @c 0 to the number of partitions minus one
	@param [in] host
		the address of the host to be prioritized in the selection. IP address
		(IPV4 only) is also available. For @c NULL, the setting is cancelled
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified to @c controller
		- if a specified partition index is out of range
		- if failed to resolve address of the host
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsAssignPartitionPreferableHost(
		GSPartitionController *controller, int32_t partitionIndex,
		const GSChar *host);

/*!
	@JP
	@ingroup Group_GSPartitionController
	@brief 指定のコンテナ名に対応するパーティションインデックスを取得します。
	@par
		一度GridDBクラスタが構築されると、コンテナの所属先のパーティションが
		変化することはなく、パーティションインデックスも一定となります。
		指定の名前に対応するコンテナが存在するかどうかは、結果に依存しません。
	@par
		パーティションインデックスの算出に必要とする情報はキャッシュされ、
		次にクラスタ障害・クラスタノード障害を検知するまで再びGridDBクラスタに
		問い合わせることはありません。
	@param [in] controller
		処理対象の@ref GSPartitionController
	@param [in] containerName
		コンテナ名
	@param [out] partitionIndex
		パーティションインデックスを格納するための変数へのポインタ値。
		実行結果として@ref GS_RESULT_OK 以外が返される場合、
		@c -1 が格納されます。
		ポインタ値が@c NULL の場合、この格納処理が省略されます。
	@return 実行結果のコード番号。次の場合、@ref GS_RESULT_OK 以外の値を返します。
		- 引数に@c NULL が指定された場合
		- コンテナ名として許可されない文字列が指定された場合
		- この処理のタイムアウト、接続障害が発生した場合
		- 対応する@ref GSGridStore のクローズ後に呼び出された場合
	@since 1.5

	@EN
	@ingroup Group_GSPartitionController
	@brief Get the partition index corresponding to the specified Container
		name.
	@par
		Once a GridDB cluster is constructed, there will not be any changes in
		the partitions of the destination that the Container belongs to and the
		partition index will also be fixed. Whether there is a Container
		corresponding to the specified name or not does not depend on the
		results.
	@par
		Information required in the computation of the partition index is cached
		and until the next cluster failure and cluster node failure is detected,
		no inquiry will be sent to the GridDB cluster again.
	@param [in] controller
		@ref GSPartitionController to be processed
	@param [in] containerName
		Container name
	@param [out] partitionIndex
		the pointer to a variable to store the partition index.
		@c -1 is stored if non-@ref GS_RESULT_OK is
		returned as the execution result.
		No value is stored if @c NULL is specified to this pointer.
	@return Return code of the execution result. It returns the value except
		@ref GS_RESULT_OK in the following cases.
		- if @c NULL is specified in the argument(s)
		- if a character string which is not allowed to use as a Container name
			is specified
		- if a timeout occurs during this operation, or a connection failure
			occurs
		- if this function is called after the corresponding @ref GSGridStore
			is closed
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetPartitionIndexOfContainer(
		GSPartitionController *controller, const GSChar *containerName,
		int32_t *partitionIndex);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5

//
// Timestamp API
//

/*!
	@JP
	@ingroup Group_GSTimestamp
	@brief 現在時刻を求めます。
	@return 現在時刻に相当する@ref GSTimestamp 。内部のシステムコールに
		失敗した場合、@c -1

	@EN
	@ingroup Group_GSTimestamp
	@brief Returns the current time.
	@return @ref GSTimestamp corresponding to the current time.
		@c -1 is returned if internal system call fails.

	@ENDL
 */
GS_DLL_PUBLIC GSTimestamp GS_API_CALL gsCurrentTime();

/*!
	@JP
	@ingroup Group_GSTimestamp
	@brief 時刻に一定の値を加算します。
	@par
		@c amount に負の値を指定することで、指定の時刻より前の時刻を求めることができます。
	@par
		現バージョンでは、算出の際に使用されるタイムゾーンはUTCです。
	@param [in] timestamp
		対象とする時刻
	@param [in] amount
		加算する値
	@param [in] timeUnit
		加算する値の単位
	@return 加算後の@ref GSTimestamp 。次の場合は@c -1
		- 年・月単位の加算において桁あふれを起こすなどし、内部のシステムコールに
			失敗した場合

	@EN
	@ingroup Group_GSTimestamp
	@brief Adds a specific value to the specified time.
	@par
		An earlier time than the specified time can be obtained by specifying a
		negative value as @c amount.
	@par
		The current version uses the UTC time zone for calculation.
	@param [in] timestamp
		the time to be processed
	@param [in] amount
		the value to be added
	@param [in] timeUnit
		the unit of value to be added
	@return @ref GSTimestamp after adding the specified time value. It returns
		@c -1 in the following cases.
		- if an internal system call fails by the overflow during this operation

	@ENDL
 */
GS_DLL_PUBLIC GSTimestamp GS_API_CALL gsAddTime(
		GSTimestamp timestamp, int32_t amount, GSTimeUnit timeUnit);

/*!
	@JP
	@ingroup Group_GSTimestamp
	@brief TQLのTIMESTAMP値表記に従い、時刻の文字列表現を求めます。
	@par
		現バージョンでは、変換の際に使用されるタイムゾーンはUTCです。
	@param [in] timestamp
		対象とする時刻
	@param [out] strBuf
		出力先の文字列バッファ。
		@c bufSize を超えない範囲で終端文字を含む文字列を出力します。
		@c bufSize が@c 1 以上であり、出力に必要とするサイズに満たない場合、
		終端文字をバッファ範囲内の最終位置に設定し、残りの領域に可能な限り
		文字列を出力します。
		@c strBuf が @c NULL または@c bufSize が@c 0 の場合、文字列は出力されません。
	@param [in] bufSize
		出力先の文字列バッファについての、終端文字を含んだバイト単位のサイズ
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の最低サイズ。
		ただし、内部のシステムコールに失敗した場合は空文字列のサイズに相当する@c 1 。
	@see GS_TIME_STRING_SIZE_MAX

	@EN
	@ingroup Group_GSTimestamp
	@brief Returns the string representing the specified time, according to the
		TIMESTAMP value notation of TQL.
	@par
		The current version uses the UTC time zone for conversion.
	@param [in] timestamp
		the time to be processed
	@param [out] strBuf
		the output string buffer.
		The string contains the termination character within a range that does
		not exceed the @c bufSize.
		If @c bufSize is greater than or equal to @c 1 and less than the size
		required to output, the termination character is set to the last
		position in the buffer range and the string is output as much as
		possible to the rest of the region.
		If @c strBuf is @c NULL or @c bufSize is @c 0, no string is output.
	@param [in] bufSize
		the size of output string buffer in bytes, including the termination
		character
	@return The minimum size of string buffer required for output in bytes,
		including the termination character.
		However, @c 1 which is equivalent to the size of the empty string is
		returned if an internal system call fails
	@see GS_TIME_STRING_SIZE_MAX

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatTime(
		GSTimestamp timestamp, GSChar *strBuf, size_t bufSize);

/*!
	@JP
	@ingroup Group_GSTimestamp
	@brief TQLのTIMESTAMP値表記に従い、指定の文字列に対応する
		@ref GSTimestamp 値を求めます。
	@par
		現バージョンでは、変換の際に使用されるタイムゾーンはUTCです。
	@param [in] str
		対象とする時刻の文字列表現
	@param [out] timestamp
		格納先の@ref GSTimestamp 変数へのポインタ値。
		戻り値が@ref GS_FALSE となる場合、このポインタ値が@c NULL ではない限り
		@c -1 が格納されます。
	@return @ref GSTimestamp 値への変換に成功し結果を格納できたかどうか。
		次の場合、@ref GS_FALSE を返します。
		- 時刻の文字列表記と一致しない文字列が指定された場合
		- 内部のシステムコールに失敗した場合
		- 引数に@c NULL が指定された場合

	@EN
	@ingroup Group_GSTimestamp
	@brief Returns the @ref GSTimestamp value corresponding to the specified
		string, according to the TIMESTAMP value notation of TQL.
	@par
		The current version uses the UTC time zone for conversion.
	@param [in] str
		the string representation of the time to be processed
	@param [out] timestamp
		the pointer to a @ref GSTimestamp variable to be stored.
		If return value is @ref GS_FALSE, @c -1 is stored unless the pointer
		is not @c NULL.
	@return Indicates whether the conversion to @ref GSTimestamp value is
		succeeded and its result are successfully stored or not.
		It returns @ref GS_FALSE in the following cases.
		- if the string which does not match to the string representation of
			time is specified
		- if an internal system call fails
		- if @c NULL is specified in the argument(s)

	@ENDL
 */
GS_DLL_PUBLIC GSBool GS_API_CALL gsParseTime(
		const GSChar *str, GSTimestamp *timestamp);

//
// Error handling API
//

/*!
	@JP
	@defgroup Group_ErrorHandling エラー処理

	@EN
	@defgroup Group_ErrorHandling Error handling

	@ENDL
 */

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラー情報のスタックサイズを取得します。
	@par
		エラー情報はスタック構造になっており、スタック番号の大きいものほど
		より直接的なエラー原因と対応します。
	@param [in] gsResource
		リソースのアドレス。ここでのリソースとは、@ref GSGridStoreFactory インスタンス、
		または、@ref GSGridStoreFactory を介して生成された、クローズ関数により
		破棄可能なリソースのことです。
		@c NULL が指定された場合、有効な結果を取得できません。
	@return スタックサイズ。該当する情報を取得できなかった場合、@c 0

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the stack size of last error information related to
		specified resource.
	@par
		Error information has become a stack structure.
		A large stack number corresponds to the more direct cause of the error.
	@param [in] gsResource
		The address to the resource. The resource means @ref GSGridStoreFactory
		instance or resource that is generated via @ref GSGridStoreFactory and
		can be destroyed by the close function.
		If @c NULL is specified, it is unable to get a valid result.
	@return the stack size. @c 0 is returned if corresponding information can
		not be obtained

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsGetErrorStackSize(
		void *gsResource);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーのエラーコードを取得します。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。@c 0 以上スタックサイズ未満の値を指定した場合のみ、
		有効な結果を取得できます。
	@return エラーコード。該当する情報を取得できなかった場合、
		@ref GS_RESULT_OK 以外の値

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the error code of last error related to specified resource.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. A valid result will be returned only if a
		value which has more than @c 0 and less than the stack size is
		specified.
	@return The error code. If it fails to get the appropriate information,
		a value except @ref GS_RESULT_OK is returned.

	@ENDL
 */
GS_DLL_PUBLIC GSResult GS_API_CALL gsGetErrorCode(
		void *gsResource, size_t stackIndex);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーのメッセージを取得します。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@param [out] strBuf
		エラーメッセージを格納する文字列バッファ。
		@c NULL の場合、有効な結果を取得できません。
		@c NULL ではなく、別の原因で有効な結果が取得できなかった場合、
		@c bufSize が正の値であれば空文字列を格納します。
	@param [in] bufSize
		エラーメッセージを格納する文字列バッファの終端文字を含む文字数。
		格納する文字列の終端文字を含む文字数の方が大きい場合、終端文字を
		除く後方の文字列を切り詰めて格納します。@c 0 の場合、文字列バッファに
		アクセスしません。
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の
		最低サイズ。該当する情報を取得できなかった場合、@c 0

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the message of last error related to specified resource.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@param [out] strBuf
		The string buffer to store the error message.
		If @c NULL is specified, it is unable to get a valid result.
		If this is not @c NULL and if it is unable to get a valid result in a
		different cause and if a positive value is specified to @c bufSize,
		then the empty string is stored.
	@param [in] bufSize
		The number of characters in the string buffer to store the error
		message including the termination character.
		If the number of characters including the termination character to be
		stored in string buffer is larger than this value, the backside string
		except the termination character will be truncated.
		If @c 0 is specified, there is no access to the string buffer.
	@return The minimum size of string buffer required for output in bytes,
		including the termination character. @c 0 is returned if corresponding
		information cannot be obtained.

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatErrorMessage(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーのメッセージの内部モジュールの
		エラー位置情報を取得します。
	@par
		設定によっては常に空文字列しか求まらない場合があります。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@param [out] strBuf
		エラー位置情報を格納する文字列バッファ。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@param [in] bufSize
		エラー位置情報を格納する文字列バッファの終端文字を含む文字数。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の
		最低サイズ。該当する情報を取得できなかった場合、@c 0

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the error location of the internal module to the message of
		last error related to specified resource.
	@par
		It might always return empty string depending on the settings.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@param [out] strBuf
		The string buffer to store the error location information. It is the
		same as the argument with the same name of @ref gsFormatErrorMessage.
	@param [in] bufSize
		The number of characters in the string buffer to store the error
		location information including the termination character.
		It is the same as the argument with the same name of
		@ref gsFormatErrorMessage.
	@return The minimum size of string buffer required for output in bytes,
		including the termination character. @c 0 is returned if corresponding
		information cannot be obtained.

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatErrorLocation(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize);

#if GS_COMPATIBILITY_SUPPORT_1_5

/*!
	@JP
	@ingroup Group_ErrorHandling

	@EN
	@ingroup Group_ErrorHandling

	@ENDL
 */

#if GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE
#if GS_DEPRECATED_FUNC_ENABLED
/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 一時的な障害を原因とし、一定の手続きにより回復できる可能性のある
		エラーに該当するエラーコードかどうかを判定します。
	@par
		現バージョンでは、非公開オプションである@ref GS_FETCH_SIZE を指定して
		取得した@ref GSRowSet に対し、@ref gsGetNextRow を実行した場合のみ
		発生しうるエラーに該当する場合のみ@ref GS_TRUE を返却します。
	@return 該当するエラーコードかどうか
	@since 1.5

	@EN
	@ingroup Group_ErrorHandling
	@brief This function can be used to determine whether the result of the
		requested process shows the error code corresponding to the error that
		could be restored by a certain procedure and caused by a temporary
		failure.
	@par
		In the current version, @ref GS_TRUE will be returned only if the
		result corresponds to the error that can be occurred by executing
		@ref gsGetNextRow to @ref GSRowSet acquired by specifying the private
		option @ref GS_FETCH_SIZE.
	@return Indicates whether the corresponding error code or not
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GS_DEPRECATED_FUNC(
		GSBool GS_API_CALL gsIsRecoverableError(GSResult result));
#endif
#endif

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 要求した処理が既定の時間内に終了しなかった場合に発生した
		エラーに該当するエラーコードかどうかを判定します。
	@return 該当するエラーコードかどうか
	@since 1.5

	@EN
	@ingroup Group_ErrorHandling
	@brief This function can be used to determine whether the result of the
		requested process shows the error code corresponding to the error that
		occurred when the requested process is not completed within a
		predetermined time.
	@return Indicates whether the corresponding error code or not
	@since 1.5

	@ENDL
 */
GS_DLL_PUBLIC GSBool GS_API_CALL gsIsTimeoutError(GSResult result);

#endif	// GS_COMPATIBILITY_SUPPORT_1_5


#if GS_COMPATIBILITY_SUPPORT_4_2
/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーのエラー名を取得します。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@param [out] strBuf
		エラー名を格納する文字列バッファ。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@param [in] bufSize
		エラー名を格納する文字列バッファの終端文字を含む文字数。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の
		最低サイズ。該当する情報を取得できなかった場合、@c 0
	@since 4.2

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the error name of last error related to specified resource.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@param [out] strBuf
		The string buffer to store the error name. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@param [in] bufSize
		The number of characters in the string buffer to store the error
		name including the termination character. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@return The minimum size of string buffer required for output in bytes,
		including the termination character. @c 0 is returned if corresponding
		information cannot be obtained.
	@since 4.2

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatErrorName(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーの説明内容を取得します。
	@par
		説明内容は、エラーメッセージのうち、エラー番号・エラー名を除いた部分に
		相当します。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@param [out] strBuf
		エラーの説明内容を格納する文字列バッファ。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@param [in] bufSize
		エラーの説明内容を格納する文字列バッファの終端文字を含む文字数。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の
		最低サイズ。該当する情報を取得できなかった場合、@c 0
	@since 4.2

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the description of last error related to specified resource.
	@par
		The description is equivalent to part of the error message which
		contains neither the error code nor the error name.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@param [out] strBuf
		The string buffer to store the error description. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@param [in] bufSize
		The number of characters in the string buffer to store the error
		description including the termination character. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@return The minimum size of string buffer required for output in bytes,
		including the termination character. @c 0 is returned if corresponding
		information cannot be obtained.
	@since 4.2

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatErrorDescription(
		void *gsResource, size_t stackIndex, GSChar *strBuf, size_t bufSize);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーに関するパラメータの個数を
		取得します。
	@par
		エラーに関する内容のうち、特定の情報についてはパラメータとして取り出す
		ことができます。各パラメータは、名前と値を持ちます。パラメータの個数に
		基づく各インデックス値を通じ、順不同にパラメータを列挙することが
		できます。取得できるパラメータについては、エラーを引き起こした操作に
		関する、個々のインタフェースまたは関連するインタフェースの定義を参照
		してください。
	@par
		取得できるパラメータの内容は、@ref gsFormatErrorMessage もしくは
		@ref gsFormatErrorDescription より求まる文字列にも原則として含まれます。
		一方、この文字列から特定の情報だけを一定の文字列解析規則で取り出せる
		とは限りません。特定のバージョンのある状況下では取り出せたとしても、別の
		条件では意図しない情報が求まるなどして取り出せない可能性があります。
		エラーに関するパラメータを個々に取得することで、インタフェースの定義で
		明記された一部の情報については、文字列解析を行わずに取り出せます。
	@par
		エラーに関するパラメータだけを記録し、メッセージ文字列などその他の
		エラー内容を記録しなかった場合、記録された内容からエラーの原因を
		特定することが困難となる可能性があります。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@return エラーに関するパラメータの個数。該当する情報を取得できなかった
		場合、@c 0
	@since 4.2

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the number of parameters of last error related to specified
		resource.
	@par
		Particular information about the error can be obtained as parameters.
		Each parameter consists of a name and a value. The parameters can be
		enumerated in an undefined order by using each index value based on
		the number of the parameters. For the parameters which can be obtained,
		see the definition of interfaces of which operation may occur the error
		or the definition of related interfaces.
	@par
		The content of the parameters is also included in the  string returned
		by @ref gsFormatErrorMessage or @ref gsFormatErrorDescription
		in principle. But by a fixed parsing rule, it may not be able to extract
		the particular information from this message. Even if the intended
		information can be extracted from a context in a version, for other
		conditions, unintended information may be acquired or nothing may
		be acquired. By obtaining the parameters, a part of information
		specified in the definition of the interfaces can be acquired without
		parsing.
	@par
		When recording only the content of the obtained parameters and
		not recording other error information such as the message
		text, it may become difficult to identify the reason for the error.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@return The number of parameters. @c 0 is returned if corresponding
		information cannot be obtained
	@since 4.2

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsGetErrorParameterCount(
		void *gsResource, size_t stackIndex);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーに関するパラメータの名前を取得します。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@param [in] parameterIndex
		エラーに関するパラメータ集合のインデックス。@c 0 以上、かつ、パラメータの個数
		未満の値を指定した場合のみ、有効な結果を取得できます。
	@param [out] strBuf
		パラメータ名を格納する文字列バッファ。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@param [in] bufSize
		パラメータ名を格納する文字列バッファの終端文字を含む文字数。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の
		最低サイズ。該当する情報を取得できなかった場合、@c 0
	@see gsGetErrorParameterCount
	@since 4.2

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the parameter name of last error related to specified
		resource.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@param [in] parameterIndex
		The index of set of parameters about the error. A valid result will be
		returned only if a value which has more than @c 0 and less than the
		number of the parameters is specified.
	@param [out] strBuf
		The string buffer to store the parameter name. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@param [in] bufSize
		The number of characters in the string buffer to store the parameter
		name including the termination character. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@return The minimum size of string buffer required for output in bytes,
		including the termination character. @c 0 is returned if corresponding
		information cannot be obtained.
	@see gsGetErrorParameterCount
	@since 4.2

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatErrorParameterName(
		void *gsResource, size_t stackIndex, size_t parameterIndex,
		GSChar *strBuf, size_t bufSize);

/*!
	@JP
	@ingroup Group_ErrorHandling
	@brief 指定のリソースに関する直前のエラーに関するパラメータの値を取得します。
	@param [in] gsResource
		リソースのアドレス。
		@ref gsGetErrorStackSize の同名の引数と同様です。
	@param [in] stackIndex
		エラースタックのインデックス。
		@ref gsGetErrorCode の同名の引数と同様です。
	@param [in] parameterIndex
		エラーに関するパラメータ集合のインデックス。
		@ref gsFormatErrorParameterName の同名の引数と同様です。
	@param [out] strBuf
		パラメータ値を格納する文字列バッファ。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@param [in] bufSize
		パラメータ値を格納する文字列バッファの終端文字を含む文字数。
		@ref gsFormatErrorMessage の同名の引数と同様です。
	@return 終端文字を含んだ、出力に必要とする文字列バッファのバイト単位の
		最低サイズ。該当する情報を取得できなかった場合、@c 0
	@see gsGetErrorParameterCount
	@since 4.2

	@EN
	@ingroup Group_ErrorHandling
	@brief Returns the parameter value of last error related to specified
		resource.
	@param [in] gsResource
		The address to the resource. It is the same as the argument with the
		same name of @ref gsGetErrorStackSize.
	@param [in] stackIndex
		The index of error stack. It is the same as the argument with the same
		name of @ref gsGetErrorCode.
	@param [in] parameterIndex
		The index of set of parameters about the error. It is the same as the
		argument with the same name of @ref gsFormatErrorParameterName.
	@param [out] strBuf
		The string buffer to store the parameter value. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@param [in] bufSize
		The number of characters in the string buffer to store the parameter
		value including the termination character. It is the same as the
		argument with the same name of @ref gsFormatErrorMessage.
	@return The minimum size of string buffer required for output in bytes,
		including the termination character. @c 0 is returned if corresponding
		information cannot be obtained.
	@see gsGetErrorParameterCount
	@since 4.2

	@ENDL
 */
GS_DLL_PUBLIC size_t GS_API_CALL gsFormatErrorParameterValue(
		void *gsResource, size_t stackIndex, size_t parameterIndex,
		GSChar *strBuf, size_t bufSize);
#endif	// GS_COMPATIBILITY_SUPPORT_4_2

//
// Experimental API
//

#if GS_EXPERIMENTAL_TOOL_ENABLED
typedef struct GSExperimentalRowIdTag {
	struct {
		GSCollection *container;
		int64_t transactionId;
		int64_t baseId;
	} internal;
} GSExperimentalRowId;

GS_DLL_PUBLIC GSResult GS_API_CALL gsExperimentalGetRowIdForUpdate(
		GSRowSet *rowSet, GSExperimentalRowId *rowId);
GS_DLL_PUBLIC GSResult GS_API_CALL gsExperimentalUpdateRowById(
		GSContainer *container, const GSExperimentalRowId *rowId,
		const void *rowObj);
GS_DLL_PUBLIC GSResult GS_API_CALL gsExperimentalDeleteRowById(
		GSContainer *container, const GSExperimentalRowId *rowId);
#endif

/*!
	@JP
	@brief ユーザ定義構造体とコンテナスキーマとの対応関係の定義を取得します。
	@par
		指定の定義名の@ref GS_STRUCT_BINDING の定義を参照できるようにする
		必要があります。
	@param type
		対応関係の定義名。
	@return 対応関係を示す@ref GSBinding*型の値
	@see GS_STRUCT_BINDING

	@EN
	@brief Returns the definition of the relationship between the user-defined
		structure and the Container schema.
	@par
		It is required to be able to refer the definition of
		@ref GS_STRUCT_BINDING of specified definition name.
	@param type
		the definition name of the correspondence
	@return the @ref GSBinding*-type value that indicates the correspondence relationship
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_GET_STRUCT_BINDING(type) \
	gsSetupStructBindingOf_##type()

/*!
	@JP
	@brief ユーザ定義構造体とコンテナスキーマとの対応関係を定義します。
	@par
		現バージョンでは、静的関数の定義に展開されます。
	@param type
		対応関係の定義名。関数名の一部として使用されます。
	@param entries
		構造体メンバとカラム定義との対応関係を示す以下の定義の列を、
		「,」で区切らず順に並べます。
		- @ref GS_STRUCT_BINDING_NAMED_ELEMENT
		- @ref GS_STRUCT_BINDING_NAMED_KEY
		- @ref GS_STRUCT_BINDING_NAMED_ARRAY
		- @ref GS_STRUCT_BINDING_ELEMENT
		- @ref GS_STRUCT_BINDING_KEY
		- @ref GS_STRUCT_BINDING_ARRAY
	@see GS_GET_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure and the
		Container schema.
	@par
		In the current version, it will be expanded in the definition of the
		static function.
	@param type
		the definition name of the correspondence.
		It is used as part of the function name
	@param entries
		the columns of the following definitions showing correspondence between
		the structure members and the column definition are arranged without
		separating by a ",".
		- @ref GS_STRUCT_BINDING_NAMED_ELEMENT
		- @ref GS_STRUCT_BINDING_NAMED_KEY
		- @ref GS_STRUCT_BINDING_NAMED_ARRAY
		- @ref GS_STRUCT_BINDING_ELEMENT
		- @ref GS_STRUCT_BINDING_KEY
		- @ref GS_STRUCT_BINDING_ARRAY
	@see GS_GET_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING(type, entries) \
	GS_STATIC_HEADER_FUNC_SPECIFIER const GSBinding* \
	GS_GET_STRUCT_BINDING(type) { \
		typedef type GSBindingType; \
		static GSBindingEntry assignedEntries[] = { \
			entries \
		}; \
		static GSBinding binding = { \
			assignedEntries, \
			sizeof(assignedEntries) / sizeof(*assignedEntries) \
		}; \
		return &binding; \
	}

#if GS_INTERNAL_DEFINITION_VISIBLE
#define GS_STRUCT_OFFSET_OF(member) \
	( \
		(uintptr_t) &((GSBindingType*) 0)->member - \
		(uintptr_t) ((GSBindingType*) 0) \
	)

#define GS_STRUCT_BINDING_CUSTOM_NAMED_ELEMENT( \
	name, member, memberType, options) \
	{ name, memberType, GS_STRUCT_OFFSET_OF(member), \
		(size_t) -1, options },

#define GS_STRUCT_BINDING_CUSTOM_ELEMENT(member, memberType, options) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ELEMENT( \
		#member, member, memberType, options)

#define GS_STRUCT_BINDING_CUSTOM_NAMED_ARRAY( \
	name, member, sizeMember, elementType, options) \
	{ name, elementType, GS_STRUCT_OFFSET_OF(member), \
		GS_STRUCT_OFFSET_OF(sizeMember), options },

#define GS_STRUCT_BINDING_CUSTOM_ARRAY( \
	member, sizeMember, elementType, options) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ARRAY( \
		#member, member, sizeMember, elementType, options)
#endif	// GS_INTERNAL_DEFINITION_VISIBLE

/*!
	@JP
	@brief カラム名を指定して、ユーザ定義構造体メンバと基本型カラムスキーマとの
		対応関係を定義します。
	@param name
		カラム名
	@param member
		構造体メンバの名前
	@param memberType
		基本型の名前
	@see GS_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure members
		and the base type column schema by specifying column name.
	@param name
		The column name
	@param member
		The name of the structure member
	@param memberType
		The name of the base type
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING_NAMED_ELEMENT(name, member, memberType) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ELEMENT( \
		name, member, memberType, 0)

/*!
	@JP
	@brief カラム名を指定して、ユーザ定義構造体メンバとロウキー付き
		基本型カラムスキーマとの対応関係を定義します。
	@param name
		カラム名
	@param member
		構造体メンバの名前
	@param memberType
		基本型の名前
	@see GS_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure members
		and the base type column schema with row key by specifying a column
		name.
	@param name
		The column name
	@param member
		The name of the structure member
	@param memberType
		The name of the base type
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING_NAMED_KEY(name, member, memberType) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ELEMENT( \
		name, member, memberType, GS_TYPE_OPTION_KEY)

/*!
	@JP
	@brief カラム名を指定して、ユーザ定義構造体メンバと配列型カラムスキーマとの
		対応関係を定義します。
	@param name
		カラム名
	@param member
		配列ポインタ変数に対応する構造体メンバの名前
	@param sizeMember
		配列サイズ変数に対応する構造体メンバの名前
	@param elementType
		配列型の要素型の名前
	@see GS_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure members
		and the array type column schema by specifying a column name.
	@param name
		The column name
	@param member
		The name of the structure member that corresponds to the array pointer
		variable
	@param sizeMember
		The name of the structure member that corresponds to the size of the
		array variable
	@param elementType
		The name of the element type of the array type
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING_NAMED_ARRAY( \
	name, member, sizeMember, elementType) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ARRAY( \
		name, member, sizeMember, elementType, 0)

/*!
	@JP
	@brief ユーザ定義構造体メンバと基本型カラムスキーマとの対応関係を定義します。
	@par
		構造体メンバの名前がそのままカラム名として使用されます。
	@param member
		構造体メンバの名前
	@param memberType
		基本型の名前
	@see GS_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure members
		and the base type column schema.
	@par
		The name of the structure member is used as the column name.
	@param member
		The name of the structure member
	@param memberType
		The name of the base type
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING_ELEMENT(member, memberType) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ELEMENT( \
		#member, member, memberType, 0)

/*!
	@JP
	@brief ユーザ定義構造体メンバとロウキー付き基本型カラムスキーマとの対応関係を
		定義します。
	@par
		構造体メンバの名前がそのままカラム名として使用されます。
	@param member
		構造体メンバの名前
	@param memberType
		基本型の名前
	@see GS_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure members
		and the base type column schema with row key.
	@par
		The name of the structure member is used as the column name.
	@param member
		The name of the structure member
	@param memberType
		The name of the base type
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING_KEY(member, memberType) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ELEMENT( \
		#member, member, memberType, GS_TYPE_OPTION_KEY)

/*!
	@JP
	@brief ユーザ定義構造体メンバと配列型カラムスキーマとの対応関係を定義します。
	@par
		構造体メンバの名前がそのままカラム名として使用されます。
	@param member
		配列ポインタ変数に対応する構造体メンバの名前
	@param sizeMember
		配列サイズ変数に対応する構造体メンバの名前
	@param elementType
		配列型の要素型の名前
	@see GS_STRUCT_BINDING

	@EN
	@brief Defines the relationship between the user-defined structure members
		and the array type column schema.
	@par
		The name of the structure member is used as the column name.
	@param member
		The name of the structure member that corresponds to the array pointer
		variable
	@param sizeMember
		The name of the structure member that corresponds to the size of the
		array variable
	@param elementType
		The name of the element type of the array type
	@see GS_STRUCT_BINDING

	@ENDL
 */
#define GS_STRUCT_BINDING_ARRAY(member, sizeMember, elementType) \
	GS_STRUCT_BINDING_CUSTOM_NAMED_ARRAY( \
		#member, member, sizeMember, elementType, 0)

#ifdef __cplusplus
}
#endif

#endif	// not GRIDSTORE_H_
