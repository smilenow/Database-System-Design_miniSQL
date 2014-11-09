//
//  Block.cpp
//  miniSQL
//
//  Created by SmilENow on 11/9/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "Block.h"

//------------------------------------------------------------------------------------------//

void IndexBlock::init(){
    clr();
    calc_maxkey();
    init_key_slots();
};

IndexBlock::IndexBlock(std::string IndexName,int NodeType):Block(),IndexName(IndexName),NodeType(NodeType),nowkey(0),split(0){ init(); };

IndexBlock::IndexBlock(std::string IndexName,int NodeType,int AttrType):Block(),IndexName(IndexName),NodeType(NodeType),nowkey(0),AttrType(AttrType),split(0){
    init();
};

IndexBlock::IndexBlock(std::string IndexName,int block_id,int NodeType,int tag):IndexName(IndexName),Block(block_id),NodeType(NodeType),nowkey(0),split(0){
    init();
};

IndexBlock::IndexBlock(std::string IndexName,int block_id,int NodeType,int AttrType,int tag):IndexName(IndexName),Block(block_id),NodeType(NodeType),nowkey(0),AttrType(AttrType),split(0){
    init();
};


IndexBlock::~IndexBlock(){
    //要把东西写回buffer
    //然后clear
    clr();
}