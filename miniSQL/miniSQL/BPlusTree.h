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
#include "base.h"
#include "block.h"
#include "BufferManager.h"

#define _string_type    0
#define _int_type       1
#define _float_type     2

#define _leaf_type 0
#define _inner_type 1

extern BufferManager *_buffermanager;

//存放值的类
class Value{
private:                    // 用type来区别是哪种类型,由于没有写多态,所以开了三种类型来选择
    int type;               // 0 string 1 int 2 float
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
};

// B+树节点的类
class Node{
private:
    std::vector<Value> info;    // 去除了节点头的 B+树节点内容
    std::string IndexName;      // 索引名称
    int NodeType;               // 判断叶子还是非叶子,叶子0非叶子1
    int cnt;                    // 当前节点拥有的节点数
    int ptrNum;                 // 当前节点应该拥有的节点数N
    int AttrType;               // 索引对应的类型
    int offset;                 // 偏移量
    Block block;                // 块
    Table nowTable;             // 当前表
    void read();                // 把块的内容读出来
public:
    Node(std::string IndexName,Table nowTable,int n);
    Node(std::string IndexName,Table nowTable,int n,int offset);
    Node(std::string IndexName,Table nowTable,int n,int offset,int tag); // tag用于区分和第二个有读入的构造函数的区别,在实际构造的时候不起作用
    virtual ~Node();    // 析构的时候要把数据写入之后才能销毁数据
    int getOffset() const { return offset; };
    
    //设置 去除了节点头的 B+树节点内容
    void set(std::vector<Value> T){ info.clear(); info = T; update(); }
    
    //更新该节点当前指针数
    void update();
};

class BPlusTree{
private:
    int cnt;                // 节点指针数
    int type;               // 键值类型
    std::string IndexName;  // 索引名称
    Table nowTalbe;         // 当前表
    int root;               // 根节点
    
    class _Parent{
    public:
        int NodePtr,ParentPtr;  // 当前节点的对应的父节点
    };
    std::vector<_Parent> ParentMap; // 当前父节点列表
    
    bool Less(const Value &k1, const Value &k2); // 小于比较
    bool Equal(const Value &k1, const Value &k2); // 等于比较
    bool LessEqual(const Value &k1, const Value &k2); // 小于等于比较
    
    // 插入到叶子
    void insertToLeaf(Node node,Value key,int pointer);
    // 插入到非叶子 传进去的是要更新的父节点的子节点
    void insertToNonLeaf(Node node,Value key,int pointer);
    // 查找叶子节点,如果没有找到返回-1,如果找到了就返回叶子在叶子节点的整个偏移量
    int findLeaf(Value key);
    // 用ParentMap来查找当前节点ptr的父节点
    int findParent(int ptr);
    
public:
    // 构造？
    BPlusTree(int type);
    
    // 建立B+树
    void CreateBPlusTree(Table nowTable, std::string IndexName, int type);
    
    // 载入B+树
    void loadBPlusTree(std::string IndexName);
    
    // 插入节点
    void _insert(Value key,int pointer);
    
    // 在B+树中找到相对应的key值,并返回key值绑定的偏移量(在总文件中的偏移量)
    int find(Value key);
    
    // 删除节点
    void _delete(Value key);
    
};

#endif /* defined(__miniSQL__BPlusTree__) */
