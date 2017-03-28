/*=============================================================================
# Filename: VList.h
# Author: Bookug Lobert 
# Mail: zengli-bookug@pku.edu.cn
# Last Modified: 2017-03-27 15:40
# Description: 
=============================================================================*/

#ifndef _KVSTORE_IVTREE_STORAGE_VLIST_H
#define _KVSTORE_IVTREE_STORAGE_VLIST_H

#include "../../../Util/Util.h"
#include "../../../Util/Bstr.h"

//TODO: not keep long list in memory, read each time
//but when can you free the long list(kvstore should release it after parsing)
//
//CONSIDER: if to keep long list in memory, should adjust the bstr in memory:
//unsigned: 0  char*: an object (if in memory, if modified, length, content, block num)
//when reading a long list in a node, generate the object first, and the object will tell you whether 
//the list is in mmeory or not

//BETTER: use two kind of blocks in two files, like 1M and 1G (split the block num into two parts)

class VList
{
public:
	//NOTICE:the border is 10^6, but the block is larger, 1M
	static const unsigned LENGTH_BORDER = 1000000;
	static const unsigned BLOCK_SIZE = 1 << 20;	//fixed size of disk-block
	static const unsigned MAX_BLOCK_NUM = 1 << 16;		//max block-num
	//below two constants: must can be exactly divided by 8
	static const unsigned SET_BLOCK_NUM = 1 << 8;		//initial blocks num
	static const unsigned SET_BLOCK_INC = SET_BLOCK_NUM;	//base of blocks-num inc
	static const unsigned SuperNum = MAX_BLOCK_NUM / (8 * BLOCK_SIZE) + 1;

private:
	unsigned long long max_buffer_size;
	unsigned cur_block_num;
	std::string filepath;
	BlockInfo* freelist;
	//very long value list are stored in a separate file(with large block)
	//
	//NOTICE: according to the summary result, 90% value lists are just below 100 bytes
	//<10%: 5000000~100M bytes
	FILE* valfp;

	//NOTICE: freemem's type is long long here, due to large memory in server.
	//However, needmem in handler() and request() is ok to be int/unsigned.
	//Because the bstr' size is controlled, so is the node.
	unsigned long long freemem;  		//free memory to use, non-negative
	//unsigned long long time;			//QUERY(achieving an old-swap startegy?)
	long Address(unsigned _blocknum) const;
	unsigned Blocknum(long address) const;
	unsigned AllocBlock();
	void FreeBlock(unsigned _blocknum);
	void ReadAlign(unsigned* _next);
	void WriteAlign(unsigned* _next, bool& _SpecialBlock);

public:
	VList();
	VList(std::string& _filepath, unsigned long long _buffer_size);//create a fixed-size file or open an existence
	bool readBstr(Bstr* _bp, unsigned* _next);
	bool writeBstr(const Bstr* _bp, unsigned* _curnum, bool& _SpecialBlock);
	bool readValue(unsigned _block_num);
	bool writeValue(const Bstr* _bp);
	~VList();
};

#endif
