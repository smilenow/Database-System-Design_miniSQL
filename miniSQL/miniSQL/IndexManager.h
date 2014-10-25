//
//  IndexManager.h
//  miniSQL
//
//  Created by SmilENow on 10/25/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#ifndef __miniSQL__IndexManager__
#define __miniSQL__IndexManager__

#include <stdio.h>
#include <unordered_map>
#include "BPlusTree.h"

class IndexManager{
public:
    std::unordered_map<std::string,BPlusTree*> BPlusTreeGroup;
    
public:
    IndexManager();
    virtual ~IndexManager(){};
    
    void CreatIndex(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot);
    void DropIndex(std::string IndexName);
    void _delete(std::string IndexName,Value key);
    void _insert(std::string IndexName,Value key, slot keyslot);
    slot select(std::string IndexName,Value key);
    std::vector<slot> _FindSmall(std::string IndexName,Value key);
    std::vector<slot> _FindSmallEqual(std::string IndexName,Value key);
    std::vector<slot> _FindBigger(std::string IndexName,Value key);
    std::vector<slot> _FindBiggerEqual(std::string IndexName,Value key);
};

#endif /* defined(__miniSQL__IndexManager__) */
