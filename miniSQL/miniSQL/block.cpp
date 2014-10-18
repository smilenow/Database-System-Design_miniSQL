//
//  block.cpp
//  miniSQL
//
//  Created by SmilENow on 10/13/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

/*
 
 v1.1 from Xinyuan Lu
 What's new in v1.1：
 1. 增加一个指示该block是否被锁住的变量is_block初始化
 2. 实现锁住、解锁方法
 
 */

#include "block.h"

Block::Block(){
    TableName[0] = 0;
    NextOffset = 0;
    ContentSize = 0;
    Is_Valid = true;
    Is_Index = false;
    Is_Blocked = false;
    
    Offset = 0;
    Is_Dirty = true;
    memset(Content,0,sizeof(Content));
};

Block::Block(bool index){
    TableName[0] = 0;
    NextOffset = 0;
    ContentSize = 0;
    Is_Valid = false;
    Is_Index = index;
    Is_Blocked = false;
    
    Offset = 0;
    Is_Dirty = true;
};

void Block::dirty(){
    Is_Dirty = true;
}

void Block::block(){
    Is_Blocked = true;
}

void Block::unblock(){
    Is_Blocked = false;
}

char* Block::getContent(){
    return Content;
}

int Block::getContentSize(){
    return ContentSize;
}

void Block::debug(bool requireContent){
    printf("[Block]%s[%d]\n",TableName,Offset);
    if (Is_Valid) printf("Valid\n");
    else printf("NOT Valid\n");
    if (Is_Index) printf("Index\n");
    else printf("NOT Index\n");
    if (Is_Dirty) printf("Dirty\n");
    else printf("NOT Dirty\n");
    printf("Next=%d\n",NextOffset);
    printf("Size=%d\n",ContentSize);
    if (requireContent) printf("%s\n",Content);
}