//
//  block.h
//  miniSQL
//
//  Created by Kael on 10/18/14.
//  Copyright (c) 2014 Xinyuan Lu. All rights reserved.
//

// block_v2.0.h, 根据讨论结果重新编写
// 现在的block类只是一个抽象类
// 只提供了最基础的需要共享的几个成员变量
// 具体实现参见各block子类
// 建议子类命名：DataBlock, IndexBlock, CatalogBlock

// v2.1, thank Jiaquan Yin for his suggestion

// v3.0, Add two Ctor with block_id , By. Jiaquan Yin

#ifndef __BLOCK__
#define __BLOCK__

#include <cstring>

#define block_size 4096

class Block
{
public:
	char tablename[32];
	int block_id;		// 该block在它所在的文件中是第几个
	bool is_valid;		// valid/garbage
	bool is_pin;		// 是否被锁住
	bool is_dirty;		// 是否被修改过
	int head_size;		// head的大小

public:
	Block(): block_id(0), is_valid(true), is_pin(false), is_dirty(false), head_size(0){
		memset(tablename, 0, sizeof(tablename));
	}
	Block(const char *Tablename):block_id(0), is_valid(true), is_pin(false), is_dirty(false), head_size(0){
		strcpy(tablename, Tablename);
	}
    Block(int block_id):block_id(block_id),is_valid(true),is_pin(false),is_dirty(false),head_size(0){
        memset(tablename, 0, sizeof(tablename));
    }
    Block(int block_id,const char *Tablename):block_id(block_id),is_valid(true),is_pin(false),is_dirty(false),head_size(0){
		strcpy(tablename, Tablename);
    }

	int get_id()const{return block_id;}
	void set_id(const int &bid){block_id=bid;}
	
	bool get_valid()const{return is_valid;}
	void set_valid(const bool &valid){is_valid=valid;}
	
	bool get_pin()const{return is_pin;}
	void set_pin(const bool &pin){is_pin=pin;}
	
	bool get_dirty()const{return is_dirty;}
	void set_dirty(const bool &dirty){is_dirty=dirty;}
	virtual void calc_head_size()=0;
	virtual ~Block()=0;
};

#endif
