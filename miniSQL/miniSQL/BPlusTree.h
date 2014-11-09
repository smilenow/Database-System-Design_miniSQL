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
#include "BufferManager.h"

#define _string_type    1
#define _int_type       0
#define _float_type     -1

#define _leaf_type 0
#define _inner_type 1

extern BufferManager *buffermanager;

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
    IndexBlock(std::string IndexName,int NodeType);
    IndexBlock(std::string IndexName,int NodeType,int AttrType);
    //这两个tag是因为Ctor不能接受同样的parameter来构造，和上述的会混淆
    IndexBlock(std::string IndexName,int block_id,int NodeType,int tag);
    IndexBlock(std::string IndexName,int block_id,int NodeType,int AttrType,int tag);
    
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

class BPlusTree{
public:
    BPlusTree():root(NULL){};
    void Create_BPlusTree(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot);
    //
    
    // buffer怎么给？
    void rebuild(IndexBlock *nownode);
    void load_BPlusTree(std::string IndexName);
    void FindBPlusTreeAllNode(IndexBlock *nownode);
    void store_BPlusTree();
    //
    
    // Value类的比较函数
    bool isLess(const Value&,const Value&);
    bool isLessEqual(const Value&, const Value&);
    bool isEqual(const Value&,const Value&);
    bool compare(const slot&, const slot&);
    
    // 查找对应的slot
    std::pair<Value *,slot *> find(IndexBlock* nownode,Value key);
    // 查找对应的IndexBlock,用于范围查询的>和>=
    IndexBlock* findBigger(IndexBlock* nownode,Value key);
    
    // 等值查询，返回一个slot
    slot search(IndexBlock* nownode,Value key); // ==
    
    // 范围查询
    std::vector<slot> Smaller(Value key);       // <
    std::vector<slot> SmallerEqual(Value key);  // <=
    std::vector<slot> Bigger(Value key);        // >
    std::vector<slot> BiggerEqual(Value key);   // >=
    std::vector<slot> NotEqual(Value key);      // !=
    
    // 插入
    IndexBlock* insert(IndexBlock* nownode, Value key,slot keyslot);
    void _insert(Value key,slot keyslot);
    
    // 把叶子连接在一起
    void Link_Leaf();
    
    // 删除
    void _delete(IndexBlock* nownode,Value key);
    
    // 删除整棵树
    void delete_whole_tree(IndexBlock* nownode);
    
public:
    IndexBlock *root;
    std::vector<IndexBlock> AllNode;
};

#endif /* defined(__miniSQL__BPlusTree__) */
