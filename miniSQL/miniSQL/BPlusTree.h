//
//  BPlusTree.h
//  miniSQL
//
//  Created by SmilENow on 10/14/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#ifndef __miniSQL__BPlusTree__
#define __miniSQL__BPlusTree__

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include "Base.h"
#include "Block.h"

#define _string_type    1
#define _int_type       0
#define _float_type     -1

#define _leaf_type 0
#define _inner_type 1


//存放值的类
class Value{
private:                    // 用type来区别是哪种类型,由于没有写多态,所以开了三种类型来选择
    int type;               // 1 string 0 int -1 float
    std::string charKey;
    int intKey;
    float floatKey;

public:
    // 构造
    Value(int type):type(type){};
    Value(int type,std::string k):type(type),charKey(k){};
    Value(int type,int k):type(type),intKey(k){};
    Value(int type,float k):type(type),floatKey(k){};
    
    //获取类型属性和相对应的值
    int getType() const { return type; };
    int getIntKey() const { return intKey; };
    float getFloatKey() const { return floatKey; };
    std::string getCharKey() const { return charKey; };
    
    //设置对应的值
    void setKey(int k) { intKey = k; };
    void setKey(float k) { floatKey = k; };
    void setKey(std::string k) { charKey = k; };
    
    //具体函数看.cpp,获取key的值,用string输出
    std::string getKey();
    void resetKey();
};

struct slot{
    int block_id;
    int offset;
    slot():block_id(-1),offset(-1){};
    slot(int bid,int offset):block_id(bid),offset(offset){};
    void reset(){ block_id=-1,offset=-1; }
};

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
    IndexBlock():nowkey(0){ clr(); };
    IndexBlock(std::string IndexName,int NodeType);
    IndexBlock(std::string IndexName,int NodeType,int AttrType);
    //这两个tag是因为Ctor不能接受同样的parameter来构造，和上述的会混淆
    IndexBlock(std::string IndexName,int block_id,int NodeType,int tag);
    IndexBlock(std::string IndexName,int block_id,int NodeType,int AttrType,int tag);
    
    // 析构的时候要把信息写回buffer
    ~IndexBlock();
    
    void read();                // 把块的内容读出来
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

class BPlusTree{
public:
    BPlusTree(){};
    void Create_BPlusTree(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot);
    
    // Value类的比较函数
    bool isLess(const Value&,const Value&);
    bool isLessEqual(const Value&, const Value&);
    bool isEqual(const Value&,const Value&);
    
    // 查找对应的slot
    bool find(IndexBlock* nownode,Value key);
    // 等值查询，返回一个slot
    slot search(IndexBlock* nownode,Value key);
    // 插入
    IndexBlock* insert(IndexBlock* nownode, Value key,slot keyslot);
    // 把叶子连接在一起
    void Link_Leaf();
    void _delete(IndexBlock* nownode,Value key);
    
public:
    IndexBlock *root;
};

#endif /* defined(__miniSQL__BPlusTree__) */
