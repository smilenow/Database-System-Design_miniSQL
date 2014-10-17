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
        default: return 0;
    }
}

//------------------------------------------------------------------------------------------//

// 从buffer里面读入,需要和buffermanager商量给出来的是什么
void Node::read(){
    for (auto &i:nowTable.AttrList)
        if (i.name == IndexName){
            switch (i.datatype) {
                case -1:
                    AttrType = _float_type;
                    break;
                case 0:
                    AttrType = _int_type;
                    break;
                case 1:
                    AttrType = _string_type;
                    break;
                default:
                    break;
            }
            break;
    }
}

Node::Node(std::string IndexName,Table nowTable,int n):IndexName(IndexName),nowTable(nowTable),ptrNum(n){
    block = _buffermanager->newIndexBlock(IndexName);
    offset = block.Offset;
}

Node::Node(std::string IndexName,Table nowTable,int n,int offset):IndexName(IndexName),nowTable(nowTable),ptrNum(n),offset(offset){
    block = _buffermanager->newIndexBlock(IndexName,offset);
    read();
}

Node::Node(std::string IndexName,Table nowTable,int n,int offset,int tag):IndexName(IndexName),nowTable(nowTable),ptrNum(n),offset(offset){
    block = _buffermanager->newIndexBlock(IndexName,offset);
}

Node::~Node(){ // 这里面要和buffermanager商量好怎么写入数据，先空着
    
}

void Node::update(){
    cnt = 0;
    for (int i=0;i<info.size();i+=2) cnt++;
}



//------------------------------------------------------------------------------------------//


