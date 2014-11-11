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

// v4.0, Add All types of block

#ifndef __BLOCK__
#define __BLOCK__

#include "Base.h"
#include <cstring>
#include <string>
#include <vector>

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
    virtual void calc_head_size(){}
    virtual ~Block(){}
};

//------------------------------------------------------------------------------------------//

// B+树节点的类 一个节点就是一个block
class IndexBlock: public Block{
public:
    std::vector<Value> key;     // 去除了节点头的 B+树节点内容
    std::vector<slot> slots;    // 节点与节点之间的slot,非叶子则是只有一个指向孩子的块bid,叶子则有指向bid和offset的信息
    // 标记每个slot的指针
    std::vector<IndexBlock* > slots_child;
    std::string IndexName;      // 索引的名字
    int NodeType;               // 判断叶子还是非叶子,叶子0非叶子1
    int nowkey;                 // 当前节点拥有的节点数
    int maxkey;                 // 当前节点应该拥有的节点数N
    int AttrType;               // 索引对应的类型
    int split;                  // 判断是否有分裂
public:
    IndexBlock():Block(),nowkey(0){ clr(); };
    IndexBlock(std::string IndexName,int block_id,int NodeType,int AttrType);
    
    // 析构的时候要把信息写回buffer
    ~IndexBlock();
    
    void init();                // 用于初始化
    
    // 对两个vetor进行清空
    void clr(){
        key.clear(); slots.clear();
        for (auto &i: slots_child) delete i;
        slots_child.clear();
    };
    
    // 对key slots这两个vector进行大小的初始化定义
    void init_key_slots(){
        key.resize(maxkey);
        slots.resize(maxkey);
        slots_child.resize(maxkey);
        for (auto &i: slots_child) i = NULL;
    }
    
    // 计算块头大小
    void calc_head_size(){
        head_size = sizeof(IndexBlock);
    };
    
    // 计算一个block最大可以存放的key值
    void calc_maxkey(){
        calc_head_size();
        maxkey = (block_size - head_size) / (sizeof(Value)+sizeof(slot)+sizeof(IndexBlock*));
    }
    
    // 获取最后一个slot
    IndexBlock* get_last_slot_child() const {
        return slots_child[slots_child.size()-1];
    }
    
    // 设置最后一个slot
    void set_last_slot(IndexBlock* nowslot_child){
        slots_child[slots_child.size()-1] = nowslot_child;
    }
};

//------------------------------------------------------------------------------------------//

const static int contentsize = block_size-sizeof(Block)-sizeof(int);
//table:name, attr#, pkontable
//char(64)+int+int*#+char(64)*#+int*#+char(64)
class TableCatalogBlock:public Block{
public:
    TableCatalogBlock():Block(){};
public:
    char content[contentsize];
    int nowcontentsize;
};
//tablen, attrn, attrp, attrt, attrindex
class AttrCatalogBlock:public Block{
public:
    AttrCatalogBlock():Block(){};
    
public:
    char content[contentsize];
    int nowcontentsize;
};
//index:table, attrname
//char(64)+char(64)+char(64)
class IndexCatalogBlock:public Block{
public:
    IndexCatalogBlock():Block(){};
public:
    char content[contentsize];
    int nowcontentsize;
};

//------------------------------------------------------------------------------------------//

static const int contentsize_recordmanager = block_size-sizeof(Block)-sizeof(int);

class RecordBlock:public Block{
public:
    RecordBlock():Block(){};
    RecordBlock(const char *Tablename):Block(Tablename){};
    RecordBlock(int block_id):Block(block_id){};
    RecordBlock(int block_id,const char *Tablename):Block(block_id,Tablename){};
public:
    char content[contentsize_recordmanager];
    int nowcontentsize;
};

//------------------------------------------------------------------------------------------//

#endif
