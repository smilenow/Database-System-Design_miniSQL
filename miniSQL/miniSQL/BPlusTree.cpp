//
//  BPlusTree.cpp
//  miniSQL
//
//  Created by SmilENow on 10/14/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "BPlusTree.h"

std::string Value::getKey(){
    switch(type){
        case _string_type:return charKey;
        case _int_type:{
            std::stringstream ss;
            ss << intKey;
            std::string tmp;
            ss >> tmp;
            return tmp;
        }
        case _float_type:{
            std::stringstream ss;
            ss << floatKey;
            std::string tmp;
            ss >> tmp;
            return tmp;
        }
        default: return "";
    }
}

//------------------------------------------------------------------------------------------//

void IndexBlock::init(){
    clr();
    calc_maxkey();
    init_key_slots();
};

IndexBlock::IndexBlock(std::string IndexName,int NodeType):IndexName(IndexName),NodeType(NodeType),nowkey(0){ init(); };

IndexBlock::IndexBlock(std::string IndexName,int NodeType,int AttrType):IndexName(IndexName),NodeType(NodeType),nowkey(0),AttrType(AttrType){
    init();
};

IndexBlock::IndexBlock(std::string IndexName,int block_id,int NodeType,int tag):IndexName(IndexName),Block(block_id),NodeType(NodeType),nowkey(0){
    init();
};

IndexBlock::IndexBlock(std::string IndexName,int block_id,int NodeType,int AttrType,int tag):IndexName(IndexName),Block(block_id),NodeType(NodeType),nowkey(0),AttrType(AttrType){
    init();
};


IndexBlock::~IndexBlock(){
    //要把东西写回buffer
    //然后clear
    clr();
}

void IndexBlock::read(){ // buffer 怎么给我？
    
}


//------------------------------------------------------------------------------------------//

void BPlusTree::Create_BPlusTree(std::string IndexName,int IndexType){
    int NewBlockID;
    // buffer 给我申请一个可用的block_id,记为NewBlockID
    root = new IndexBlock(IndexName,0,IndexType);
}

bool BPlusTree::isLess(const Value & k1, const Value & k2){
    switch (k1.getType()) {
        case _int_type:
            return (k1.getIntKey() < k2.getIntKey());
            break;
        case _float_type:
            return (k1.getFloatKey() < k2.getFloatKey());
            break;
        case _string_type:
            return (k1.getCharKey() < k2.getCharKey());
            break;
        default:
            break;
    }
    return false;
};

bool BPlusTree::isLessEqual(const Value & k1, const Value & k2){
    switch (k1.getType()) {
        case _int_type:
            return (k1.getIntKey() <= k2.getIntKey());
            break;
        case _float_type:
            return (k1.getFloatKey() <= k2.getFloatKey());
            break;
        case _string_type:
            return (k1.getCharKey() <= k2.getCharKey());
            break;
        default:
            break;
    }
    return false;
};

bool BPlusTree::isEqual(const Value & k1, const Value & k2){
    switch (k1.getType()) {
        case _int_type:
            return (k1.getIntKey() == k2.getIntKey());
            break;
        case _float_type:
            return (k1.getFloatKey() == k2.getFloatKey());
            break;
        case _string_type:
            return (k1.getCharKey() == k2.getCharKey());
            break;
        default:
            break;
    }
    return false;
}