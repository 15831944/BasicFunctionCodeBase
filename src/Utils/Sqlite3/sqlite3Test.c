#include <stdio.h>
#include <stdlib.h>

#include "sqlite/sqlite3.h"

#include "Log/LogC.h"
#include "logUtils.h"
#include "Utils.h"



int ReadSqliteTest();
int WriteSqliteTest();
int DeleteSqliteTest();
int InsertSqliteTest();
int ModifySqliteTest();
int InquirySqliteCallBackTest();
int InquirySqliteNoCallBackTest();

int main(int argc, char **argv)
{
    log_utils_init();


    utilsLogInfo("========================1111======================================================--------------=\n");
    InquirySqliteCallBackTest();

    utilsLogVerbose("=======================22222=======================================================--------------=\n");
    InquirySqliteNoCallBackTest();


    utilsLogDebug("======================33333========================================================--------------=\n");

    return 0;
}






/**
 *
 *  typedef int (*sqlite3_callback)(void*,int,char**, char**);
 *  int sqlite3_exec(sqlite3*, const char *sql, sqlite3_callback, void *,  char **errmsg );
 *  这就是执行一条 sql 语句的函数。
第1个参数不再说了，是前面open函数得到的指针。说了是关键数据结构。
第2个参数const char *sql 是一条 sql 语句，以/0结尾。
第3个参数sqlite3_callback 是回调，当这条语句执行之后，sqlite3会去调用你提供的这个函数。（什么是回调函数，自己找别的资料学习）
第4个参数void * 是你所提供的指针，你可以传递任何一个指针参数到这里，这个参数最终会传到回调函数里面，如果不需要传递指针给回调函数，可以填NULL。等下我们再看回调函数的写法，以及这个参数的使用。
第5个参数char ** errmsg 是错误信息。注意是指针的指针。sqlite3里面有很多固定的错误信息。执行 sqlite3_exec 之后，执行失败时可以查阅这个指针（直接 printf(“%s/n”,errmsg)）得到一串字符串信息，这串信息告诉你错在什么地方。sqlite3_exec函数通过修改你传入的指针的指针，把你提供的指针指向错误提示信息，这样sqlite3_exec函数外面就可以通过这个 char*得到具体错误提示。
说明：通常，sqlite3_callback 和它后面的 void * 这两个位置都可以填 NULL。填NULL表示你不需要回调。比如你做insert 操作，做 delete 操作，就没有必要使用回调。而当你做 select 时，就要使用回调，因为 sqlite3 把数据查出来，得通过回调告诉你查出了什么数据。
 *
 **/


//sqlite3的回调函数
// sqlite 每查到一条记录，就调用一次这个回调
/**
 *  Name::  LoadMyInfo
 *  Param:: para:  类型 int* ,是你在 sqlite3_exec 里传入的 int* 参数,
 *                  通过para参数，可传一些特殊的指针（比如类指针、结构指针），然后强制转换成对应的类型；然后操作这些数据；
 *          n_column:  类型 int，是这一条记录有多少个字段 (即这条记录有多少列)；
 *          column_value:  类型 char**，是个关键值，查出来的数据都保存在这个字段里，
 *                       它实际上是个1维数组（不要以为是2维数组），每一个元素都是一个 char* 值，是一个字段内容（用字符串来表示，以/0结尾）；
 *          column_name:  类型 char**，跟 column_value是对应的，表示这个字段的字段名称；
 *
 *
 **/
int LoadMyInfo( void *para, int n_column, char **column_value, char **column_name )
{
    int i;
    printf( "---------------------记录包含 %d 个字段 \n", n_column );

    for ( i = 0 ; i < n_column; i ++ ) {
        printf( "字段名:%s  > 字段值:%s \n",  column_name[i], column_value[i] );
    }
    printf( "---------------------------------------------\n" );

    return 0;
}

int InquirySqliteCallBackTest()
{
    utilsLogDebug("=====InquirySqliteCallBackTest===--------------=\n");
    sqlite3 *db;
    int result;
    char *errmsg = NULL;

    result = sqlite3_open( "./sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        utilsLogError("sqlite3_open return value[%d].\n", result);
        return -1;
    }

    //数据库操作代码
    //创建一个测试表，表名叫 MyTable_1，有2个字段： ID 和 name。其中ID是一个自动增加的类型，以后insert时可以不去指定这个字段，它会自己从0开始增加
    result = sqlite3_exec( db, "CREATE TABLE MyTable_1(ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME VARCHAR(32));", NULL, NULL, &errmsg );
    if (result != SQLITE_OK ) {
        utilsLogWarning( "创建表失败，错误码:%d，错误原因:%s\n", result, errmsg );
    }

    //插入一些记录
    //result = sqlite3_exec( db, "insert into MyTable_1( name ) values ( ‘走路’ )", 0, 0, &errmsg );
    result = sqlite3_exec( db, "INSERT INTO MyTable_1( name ) VALUES('走路');", 0, 0, &errmsg );
    if (result != SQLITE_OK ) {
        utilsLogWarning( "插入记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
    }

    result = sqlite3_exec( db, "insert into MyTable_1( name ) values ( '骑单车' )", 0, 0, &errmsg );
    if (result != SQLITE_OK ) {
        utilsLogWarning( "插入记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
    }

    result = sqlite3_exec( db, "insert into MyTable_1( name ) values ( '坐汽车' )", 0, 0, &errmsg );
    if (result != SQLITE_OK ) {
        utilsLogWarning( "插入记录失败，错误码:%d，错误原因:%s \n", result, errmsg );
    }

    //开始查询数据库
    result = sqlite3_exec( db, "select * from MyTable_1", LoadMyInfo, NULL, &errmsg );

    //关闭数据库
    sqlite3_close( db );

    return 0;
}


//不使用回调查询数据库
int InquirySqliteNoCallBackTest()
{
    utilsLogDebug("=====InquirySqliteNoCallBackTest===--------------=\n");
    sqlite3 *db;
    int result;
    char *errmsg = NULL;
    char **dbResult; //是 char ** 类型，两个*号
    int nRow, nColumn;
    int i , j;
    int index;

    result = sqlite3_open( "./sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        utilsLogError("sqlite3_open return value[%d].\n", result);
        return -1;
    }

    //数据库操作代码
    //假设前面已经创建了 MyTable_1 表
    //开始查询，传入的 dbResult 已经是 char **，这里又加了一个 & 取地址符，传递进去的就成了 char ***
    result = sqlite3_get_table( db, "select * from MyTable_1", &dbResult, &nRow, &nColumn, &errmsg );
    if ( SQLITE_OK == result ) {
        //查询成功
        index = nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数据
        utilsLogInfo( "查到%d条记录. \n", nRow );

        for (  i = 0; i < nRow ; i++ ) {
            utilsLogInfo( "第 %d 条记录. \n", i + 1 );
            for ( j = 0 ; j < nColumn; j++ ) {
                utilsLogInfo( "字段名:%s  ß> 字段值:%s. \n",  dbResult[j], dbResult [index] );
                ++index; // dbResult 的字段值是连续的，从第0索引到第 nColumn - 1索引都是字段名称，从第 nColumn 索引开始，后面都是字段值，它把一个二维的表（传统的行列表示法）用一个扁平的形式来表示
            }
            utilsLogInfo( "--------------------------------\n" );
        }
    }

    //到这里，不论数据库查询是否成功，都释放 char** 查询结果，使用 sqlite 提供的功能来释放
    sqlite3_free_table( dbResult );

    //关闭数据库
    sqlite3_close( db );



    return 0;
}






int ReadSqliteTest()
{
    utilsLogInfo("=====ReadSqliteTest===--------------=\n");
    sqlite3 *db = NULL;  //声明sqlite关键结构指针
    int result;

    //打开数据库
    //需要传入 db 这个指针的指针，因为 sqlite3_open 函数要为这个指针分配内存，还要让db指针指向这个内存区
    result = sqlite3_open( "sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        return -1;
    }
    //数据库操作代码
    //…

    //数据库打开成功
    //关闭数据库
    sqlite3_close( db );

    return 0;
}


int WriteSqliteTest()
{
    utilsLogVerbose("=====WriteSqliteTest===--------------=\n");
    sqlite3 *db = NULL;  //声明sqlite关键结构指针
    int result;

    //打开数据库
    //需要传入 db 这个指针的指针，因为 sqlite3_open 函数要为这个指针分配内存，还要让db指针指向这个内存区
    result = sqlite3_open( "sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        return -1;
    }
    //数据库操作代码
    //…

    //数据库打开成功
    //关闭数据库
    sqlite3_close( db );

    return 0;
}


int DeleteSqliteTest()
{
    utilsLogDebug("=====DeleteSqliteTest===--------------=\n");
    sqlite3 *db = NULL;  //声明sqlite关键结构指针
    int result;

    //打开数据库
    //需要传入 db 这个指针的指针，因为 sqlite3_open 函数要为这个指针分配内存，还要让db指针指向这个内存区
    result = sqlite3_open( "sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        return -1;
    }
    //数据库操作代码
    //…

    //数据库打开成功
    //关闭数据库
    sqlite3_close( db );
    return 0;
}


int InsertSqliteTest()
{
    utilsLogDebug("=====InsertSqliteTest===--------------=\n");
    sqlite3 *db = NULL;  //声明sqlite关键结构指针
    int result;

    //打开数据库
    //需要传入 db 这个指针的指针，因为 sqlite3_open 函数要为这个指针分配内存，还要让db指针指向这个内存区
    result = sqlite3_open( "sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        return -1;
    }
    //数据库操作代码
    //…

    //数据库打开成功
    //关闭数据库
    sqlite3_close( db );

    return 0;
}

int ModifySqliteTest()
{
    utilsLogDebug("=====ModifySqliteTest===--------------=\n");
    sqlite3 *db = NULL;  //声明sqlite关键结构指针
    int result;

    //打开数据库
    //需要传入 db 这个指针的指针，因为 sqlite3_open 函数要为这个指针分配内存，还要让db指针指向这个内存区
    result = sqlite3_open( "sqlite3_test_database.db", &db );
    if ( result != SQLITE_OK ) {
        //数据库打开失败
        return -1;
    }
    //数据库操作代码
    //…

    //数据库打开成功
    //关闭数据库
    sqlite3_close( db );

    return 0;
}













