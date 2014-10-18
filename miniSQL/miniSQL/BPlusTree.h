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
};

struct slot{
    int block_id;
    int offset;
};

// B+树节点的类 一个节点就是一个block
class IndexBlock: public Block{
private:
    std::vector<Value> key;     // 去除了节点头的 B+树节点内容
    std::vector<slot> slots;    // 节点与节点之间的slot,非叶子则是只有一个指向孩子的块bid,叶子则有指向bid和offset的信息
    int NodeType;               // 判断叶子还是非叶子,叶子0非叶子1
    int nowkey;                 // 当前节点拥有的节点数
    int maxkey;                 // 当前节点应该拥有的节点数N
    int AttrType;               // 索引对应的类型
    void read();                // 把块的内容读出来
public:
    IndexBlock():nowkey(0){ clr(); };
    IndexBlock(int NodeType):NodeType(NodeType),nowkey(0){ clr(); calc_maxkey(); };
    IndexBlock(int NodeType,int AttrType):NodeType(NodeType),nowkey(0),AttrType(AttrType){ clr(); calc_maxkey(); };
    ~IndexBlock(){ clr(); };
    // 对两个vetor进行清空
    void clr(){
        key.clear(); slots.clear();
    };
    // 计算块头大小
    void calc_head_size(){
        head_size = sizeof(Block) + 24*2 /* 两个vector */ + 4*sizeof(int);
    };
    // 计算一个block最大可以存放的key值
    void calc_maxkey(){
        calc_head_size();
        maxkey = (block_size - head_size - sizeof(slot)) / (sizeof(Value)+sizeof(slot));
    }
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
