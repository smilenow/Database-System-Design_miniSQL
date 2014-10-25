//
//  IndexManager.cpp
//  miniSQL
//
//  Created by SmilENow on 10/25/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "IndexManager.h"

void IndexManager::CreatIndex(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot){
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s is duplicated\n",IndexName.c_str());
        return;
    }
    BPlusTree *nowTree = new BPlusTree();
    nowTree->Create_BPlusTree(IndexName, IndexType, data, dataslot);
    BPlusTreeGroup.insert(std::make_pair(IndexName, nowTree));
}

void IndexManager::DropIndex(std::string IndexName){
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return;
    }
    BPlusTreeGroup[IndexName]->delete_whole_tree(BPlusTreeGroup[IndexName]->root);
    BPlusTreeGroup.erase(BPlusTreeGroup.find(IndexName));
}

slot IndexManager::select(std::string IndexName,Value key){
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return slot(-1,-1);
    }
    return BPlusTreeGroup[IndexName]->search(BPlusTreeGroup[IndexName]->root, key);
}

void IndexManager::_delete(std::string IndexName,Value key){
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return ;
    }
    BPlusTreeGroup[IndexName]->_delete(BPlusTreeGroup[IndexName]->root, key);
    return ;
}

void IndexManager::_insert(std::string IndexName,Value key, slot keyslot){
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return ;
    }
    BPlusTreeGroup[IndexName]->_insert(key, keyslot);
}

std::vector<slot> IndexManager::_FindSmall(std::string IndexName,Value key){
    std::vector<slot> empty;
    empty.resize(0);
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return empty;
    }
    return BPlusTreeGroup[IndexName]->Smaller(key);
}

std::vector<slot> IndexManager::_FindSmallEqual(std::string IndexName,Value key){
    std::vector<slot> empty;
    empty.resize(0);
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return empty;
    }
    return BPlusTreeGroup[IndexName]->SmallerEqual(key);
}

std::vector<slot> IndexManager::_FindBigger(std::string IndexName,Value key){
    std::vector<slot> empty;
    empty.resize(0);
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return empty;
    }
    return BPlusTreeGroup[IndexName]->Bigger(key);
}

std::vector<slot> IndexManager::_FindBiggerEqual(std::string IndexName,Value key){
    std::vector<slot> empty;
    empty.resize(0);
    if (BPlusTreeGroup.count(IndexName)>0){
        printf("%s doesn't exist in the IndexTable",IndexName.c_str());
        return empty;
    }
    return BPlusTreeGroup[IndexName]->BiggerEqual(key);
}
