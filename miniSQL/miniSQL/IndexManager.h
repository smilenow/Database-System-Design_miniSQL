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
#include "BPlusTree.h"

class IndexManager{
public:
    BPlusTree CurrentBPT;
    std::string IndexName;
    
public:
    IndexManager():IndexName(""){};
    IndexManager(std::string IndexName):IndexName(IndexName){};
    virtual ~IndexManager(){ if (this->IndexName != "" ) CurrentBPT.store_BPlusTree(); };
    
    void CreateIndex(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot,int valuecharlen);
    void DropIndex(std::string IndexName);
    void _delete(std::string IndexName,Value key);
    void _insert(std::string IndexName,Value key, slot keyslot);
    slot select(std::string IndexName,Value key);
    std::vector<slot> _FindSmall(std::string IndexName,Value key);
    std::vector<slot> _FindSmallEqual(std::string IndexName,Value key);
    std::vector<slot> _FindBigger(std::string IndexName,Value key);
    std::vector<slot> _FindBiggerEqual(std::string IndexName,Value key);
    std::vector<slot> _FindNotEqual(std::string IndexName,Value key);
};

#endif /* defined(__miniSQL__IndexManager__) */
