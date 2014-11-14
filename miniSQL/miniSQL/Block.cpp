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

IndexBlock::IndexBlock(std::string IndexName,int block_id,int NodeType,int AttrType,int valuecharlen):IndexName(IndexName),Block(block_id),NodeType(NodeType),nowkey(0),AttrType(AttrType),split(0),valuecharlen(valuecharlen){
    init();
};


IndexBlock::~IndexBlock(){
    //要把东西写回buffer
    //然后clear
    clr();
}

IndexBlock_Buffer IndexBlock_Buffer::IndexBlock_To_IndexBlockBuffer(IndexBlock tmp){
    IndexBlock_Buffer ret;
    ret.block_id = tmp.block_id;
    ret.is_dirty = tmp.is_dirty;
    ret.is_pin = tmp.is_pin;
    ret.is_valid = tmp.is_valid;
    ret.head_size = tmp.head_size;
    for (int i=0;i<32;i++) ret.tablename[i] = tmp.tablename[i];
    
    ret.maxkey = tmp.maxkey;
    ret.type = tmp.AttrType;
    ret.valuecharlen = tmp.valuecharlen;
    ret.indexnamelen = (int)tmp.IndexName.length();
    int p=0;
    memcpy(ret.content+p, &tmp.NodeType, sizeof(int)); p += sizeof(int);
    memcpy(ret.content+p, &tmp.nowkey, sizeof(int)); p += sizeof(int);
    memcpy(ret.content+p, &tmp.maxkey, sizeof(int)); p += sizeof(int);
    memcpy(ret.content+p, &tmp.AttrType, sizeof(int)); p += sizeof(int);
    memcpy(ret.content+p, &tmp.split, sizeof(int)); p += sizeof(int);
    memcpy(ret.content+p, tmp.IndexName.c_str(), (int)tmp.IndexName.length()+1);
    p += (int)tmp.IndexName.length()+1;
    for (int i=0;i<ret.maxkey;i++){
        memcpy(ret.content+p, &tmp.slots[i].block_id, sizeof(int)); p += sizeof(int);
        memcpy(ret.content+p, &tmp.slots[i].offset, sizeof(int)); p += sizeof(int);
    }
    for (int i=0;i<ret.maxkey;i++){
        memcpy(ret.content+p, &tmp.key[i].type, sizeof(int)); p += sizeof(int);
        memcpy(ret.content+p, &tmp.key[i].intKey, sizeof(int)); p += sizeof(int);
        memcpy(ret.content+p, &tmp.key[i].floatKey, sizeof(float)); p += sizeof(float);
        memcpy(ret.content+p, &tmp.key[i].Valid, sizeof(bool)); p += sizeof(bool);
        memcpy(ret.content+p, tmp.key[i].charKey.c_str(), ret.valuecharlen+1);
        p += ret.valuecharlen+1;
    }
    return ret;
}

IndexBlock IndexBlock_Buffer::IndexBlockBuffer_To_IndexBlock(IndexBlock_Buffer tmp){
    IndexBlock ret;
    ret.block_id = tmp.block_id;
    ret.is_dirty = tmp.is_dirty;
    ret.is_pin = tmp.is_pin;
    ret.is_valid = tmp.is_valid;
    ret.head_size = tmp.head_size;
    for (int i=0;i<32;i++) ret.tablename[i] = tmp.tablename[i];
    
    int p=0;
    memcpy(&ret.NodeType, tmp.content+p, sizeof(int)); p += sizeof(int);
    memcpy(&ret.nowkey, tmp.content+p, sizeof(int)); p += sizeof(int);
    memcpy(&ret.maxkey, tmp.content+p, sizeof(int)); p += sizeof(int);
    memcpy(&ret.AttrType, tmp.content+p, sizeof(int)); p += sizeof(int);
    memcpy(&ret.split, tmp.content+p, sizeof(int)); p += sizeof(int);
    
    char ch[256];
    memcpy(ch, tmp.content+p, (int)tmp.indexnamelen+1);
    for (int i=0;i<tmp.indexnamelen;i++) ret.IndexName += ch[i];
    p += (int)tmp.indexnamelen+1;
    
    ret.key.resize(ret.maxkey);
    ret.slots.resize(ret.maxkey);
    ret.slots_child.resize(ret.maxkey);
    
    for (int i=0;i<ret.maxkey;i++){
        memcpy(&ret.slots[i].block_id, tmp.content+p, sizeof(int)); p += sizeof(int);
        memcpy(&ret.slots[i].offset, tmp.content+p, sizeof(int)); p += sizeof(int);
    }
    for (int i=0;i<ret.maxkey;i++){
        memcpy(&ret.key[i].type, tmp.content+p, sizeof(int)); p += sizeof(int);
        memcpy(&ret.key[i].intKey, tmp.content+p, sizeof(int)); p += sizeof(int);
        memcpy(&ret.key[i].floatKey, tmp.content+p, sizeof(float)); p += sizeof(float);
        memcpy(&ret.key[i].Valid, tmp.content+p, sizeof(bool)); p += sizeof(bool);
        
        char ch[256];
        memcpy(ch, tmp.content+p, (int)tmp.valuecharlen+1);
        for (int k=0;k<tmp.valuecharlen;k++) ret.key[i].charKey += ch[k];
        p += tmp.valuecharlen+1;
    }
    return ret;
}