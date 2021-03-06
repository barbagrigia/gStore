/*=============================================================================
# Filename: Util.h
# Author: Bookug Lobert 
# Mail: 1181955272@qq.com
# Last Modified: 2015-10-16 10:43
# Description: 
1. firstly written by liyouhuan, modified by zengli
2. common macros, functions, classes, etc
# Notice: we only talk about sub-graph isomorphism in the essay, however, in
# this system, the homomorphism is supported.(which means that multiple variables
in the sparql query can point to the same node in data graph)
=============================================================================*/

#ifndef _UTIL_UTIL_H
#define _UTIL_UTIL_H

//basic macros and types are defined here, including common headers 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>
#include <locale.h>
#include <assert.h>
#include <libgen.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

//NOTICE:below are restricted to C++, C files should not include(maybe nested) this header!
#include <bitset>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <map>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <list>
#include <iterator>
#include <algorithm>
#include <functional>
#include <utility>

//NOTICE:below are libraries need to link
#include <pthread.h> 
#include <math.h>
#include <readline/readline.h>
#include <readline/history.h>

//if use pthread and lock
//#define THREAD_ON 1			
//if use stream module if result is too large than memory can hold
#define STREAM_ON 1			
//when used as C/S, if output query result in the server port: default not(you can see the result in the client)
//#define OUTPUT_QUERY_RESULT 1
#define SERVER_SEND_JSON 1
//if to use readline library for console(open by default)
#define READLINE_ON	1
//if to use multiple strategy for answering queries
#define MULTI_INDEX 1
//#define SO2P 1
//#define USE_GROUP_INSERT 1
//#define USE_GROUP_DELETE 1

//indicate that in debug mode
//#define DEBUG_JOIN      
//#define DEBUG_STREAM
//#define DEBUG_PRECISE 1		all information
//#define DEBUG_KVSTORE 1		//in KVstore
//#define DEBUG_VSTREE 1	//in Database 
//#define DEBUG_LRUCACHE 1
//#define DEBUG_DATABASE 1	//in Database
//
//

#ifdef DEBUG_PRECISE
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG_KVSTORE
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG_VSTREE
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG_DATABASE
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifdef DEBUG_JOIN
#ifndef DEBUG
#define DEBUG
#endif
#endif

#ifndef DEBUG
//#define DEBUG
#endif

#define xfree(x) free(x); x = NULL;

//NOTICE:include Util.h and below in each main function
//(the beginning position)
//#ifdef DEBUG
//	Util util;
//#endif

typedef unsigned(*HashFunction)(const char*);
//NOTICE:hash functions for int are not so many, so we represent int by a 4-byte stringinstead
//(not totally change int to string, which is costly)
//http://www.cppblog.com/aurain/archive/2010/07/06/119463.html
//http://blog.csdn.net/mycomputerxiaomei/article/details/7641221
//http://kb.cnblogs.com/page/189480/
//
//type for the triple num
//TODO:this should use unsigned (triple num may > 2500000000)
typedef int TNUM;
//type for entity/literal/predicate ID
typedef int ELPID;

//TODO:typedef several ID typesand new a ID module
//what is more, the str length and Block ID in kvstore
typedef unsigned PREDICATE_ID;
//TODO:encode entity from low to high, encode literal from high to low(finally select the mid of space as border)
typedef unsigned ENTITY_LITERAL_ID;
typedef unsigned NODE_ID;
//can use `man limits.h` to see more
#define INVALID UINT_MAX

/******** all static&universal constants and fucntions ********/
class Util
{
public:
	//static int triple_num;
	//static int pre_num;
	//static int entity_num;
	//static int literal_num;

	static const unsigned MB = 1048576;
	static const unsigned GB = 1073741824;
	static const int TRIPLE_NUM_MAX = 1000*1000*1000;
	static const char EDGE_IN = 'i';
	static const char EDGE_OUT= 'o';
	//In order to differentiate the sub-part and literal-part of object
	//let subid begin with 0, while literalid begins with LITERAL_FIRST_ID 
	//used in Database and Join
	static const int LITERAL_FIRST_ID = 1000*1000*1000;
	//initial transfer buffer size in Tree/ and Stream/
	static const unsigned TRANSFER_SIZE = 1 << 20;	//1M
	//NOTICE:the larger the faster, but need to care the memory usage(not use 1<<33, negative)
	//static const unsigned long long MAX_BUFFER_SIZE = 0xffffffff;		//max buffer size in Storage
	static const unsigned long long MAX_BUFFER_SIZE = 1 << 30;
	//static const unsigned long long MAX_BUFFER_SIZE = 0x1ffffffff;		//max buffer size in Storage
	//NOTICE:use smaller if saving space, use larger if to be faster
	//static const unsigned STORAGE_BLOCK_SIZE = 1 << 10;	//fixed size of disk-block in B+ tree storage
	static const unsigned STORAGE_BLOCK_SIZE = 1 << 12;	//fixed size of disk-block in B+ tree storage
	//max block num in kvstore storage, blockNum*blockSize for a B+ tree file should >= 256G
	//static const unsigned MAX_BLOCK_NUM = 1 << 26;
	//DEBUG:maybe the file size will over if the data is too large

	//type of B+ tree
	static const int SS_TREE = 0;
	static const int SI_TREE = 1;
	static const int II_TREE = 2;
	static const int IS_TREE = 3;

	static int memUsedPercentage();
	static int memoryLeft();
	static int compare(const char* _str1, unsigned _len1, const char* _str2, unsigned _len2); //QUERY(how to use default args)
	static int string2int(std::string s);
	static std::string int2string(long n);
	//string2str: s.c_str()
	//str2string: string(str)
	static int compIIpair(int _a1, int _b1, int _a2, int _b2);
	static std::string showtime();
	static int cmp_int(const void* _i1, const void* _i2);
	static void sort(int*& _id_list, int _list_len);
	static int bsearch_int_uporder(int _key, const int* _array,int _array_num);
	static bool bsearch_preid_uporder(int _preid, int* _pair_idlist, int _list_len);
	static int bsearch_vec_uporder(int _key, const std::vector<int>* _vec);
	static std::string result_id_str(std::vector<int*>& _v, int _var_num);
	static bool dir_exist(const std::string _dir);
	static bool create_dir(const std:: string _dir);
	static long get_cur_time();
	static bool save_to_file(const char*, const std::string _content);
	static bool isValidPort(std::string);
	static bool isValidIP(std::string);
	static std::string getTimeString();
	static std::string node2string(const char* _raw_str);

	static bool is_literal_ele(int);
	static int removeDuplicate(int*, int);
	static std::string getQueryFromFile(const char* _file_path); 
	static std::string getSystemOutput(std::string cmd);
	static std::string getExactPath(const char* path);
	static std::string getItemsFromDir(std::string path);
	static void logging(std::string _str);

	// Below are some useful hash functions for string
	static unsigned simpleHash(const char *_str);
	static unsigned APHash(const char *_str);
	static unsigned BKDRHash(const char *_str);
	static unsigned DJBHash(const char *_str);
	static unsigned ELFHash(const char *_str);
	static unsigned DEKHash(const char* _str);
	static unsigned BPHash(const char* _str);
	static unsigned FNVHash(const char* _str);
	static unsigned HFLPHash(const char* _str);
	static unsigned HFHash(const char* _str);
	static unsigned JSHash(const char *_str);
	static unsigned PJWHash(const char *_str);
	static unsigned RSHash(const char *_str);
	static unsigned SDBMHash(const char *_str);
	static unsigned StrHash(const char* _str);
	static unsigned TianlHash(const char* _str);

	static const unsigned HashNum = 16;
	static HashFunction hash[];

	static double logarithm(double _a, double _b);
	static void intersect(int*& _id_list, int& _id_list_len, const int* _list1, int _len1, const int* _list2, int _len2);

	static char* l_trim(char *szOutput, const char *szInput);
	static char* r_trim(char *szOutput, const char *szInput);
	static char* a_trim(char *szOutput, const char * szInput);

	//NOTICE: this function must be called at the beginning of executing!
	Util();
	~Util();
	static std::string profile;
	static bool configure();  //read init.conf and set the parameters for this system
	static bool config_setting();
	static bool config_advanced();
	static bool config_debug();
	//static bool gStore_mode;
	static std::map<std::string, std::string> global_config;
	//static std::string db_home;
	
	//sort functions for qsort
	static int _spo_cmp(const void* _a, const void* _b);
	static int _ops_cmp(const void* _a, const void* _b);
	static int _pso_cmp(const void* _a, const void* _b);

	static std::string tmp_path;
	// this are for debugging
	//to build logs-system, each class: print() in time 
	static std::string debug_path;
	static FILE* debug_kvstore;				
	static FILE* debug_database;
	static FILE* debug_vstree;


private:
	static bool isValidIPV4(std::string);
	static bool isValidIPV6(std::string);
};

class BlockInfo
{
public:
	unsigned num;			
	BlockInfo* next;
	BlockInfo()
	{
		num = 0;
		next = NULL;
	}
	BlockInfo(unsigned _num)
	{
		num = _num;
		next = NULL;
	}
	BlockInfo(unsigned _num, BlockInfo* _bp)
	{
		num = _num;
		next = _bp;
	}
};

//BETTER+TODO:if considering frequent insert/delete, there maybe too many empty positions, too wasteful!
//A method is to divide as groups, set the base for each, not conflict
//Reproducing the array if needed!
class Buffer
{
public:
	unsigned size;
	std::string* buffer;
	
	Buffer(unsigned _size)
	{
		this->size = _size;
		this->buffer = new std::string[this->size];
		//for(unsigned i = 0; i < this->size; ++i)
		//{
			//this->buffer[i] = "";
		//}
	}
	
	bool set(unsigned _pos, const std::string& _ele)
	{
		if(_pos >= this->size)
		{
			return false;
		}
		//BETTER:check if this position is used, and decide abort or update?
		this->buffer[_pos] = _ele;
		return true;
	}

	bool del(unsigned _pos)
	{
		if(_pos >= this->size)
		{
			return false;
		}
		this->buffer[_pos] = "";
		return true;
	}

	std::string& get(unsigned _pos) const
	{
		return this->buffer[_pos];
	}

	~Buffer()
	{
		//for(unsigned i = 0; i < size; ++i)
		//{
			//delete[] buffer[i];
		//}
		delete[] buffer;
	}
};

#endif //_UTIL_UTIL_H

