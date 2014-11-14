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

class BPlusTree{
public:
    BPlusTree():root(NULL){};
    void Create_BPlusTree(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot,int valuecharlen);
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
    std::vector<IndexBlock> AllNode_save;
    std::vector<IndexBlock> AllNode_load;
};

#endif /* defined(__miniSQL__BPlusTree__) */
