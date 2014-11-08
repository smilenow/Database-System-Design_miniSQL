//
//  IndexManager.cpp
//  miniSQL
//
//  Created by SmilENow on 10/25/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "IndexManager.h"

void IndexManager::CreateIndex(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot){
    CurrentBPT.Create_BPlusTree(IndexName, IndexType, data, dataslot);
    CurrentBPT.store_BPlusTree();
}

void IndexManager::DropIndex(std::string IndexName){
    CurrentBPT.delete_whole_tree(CurrentBPT.root);
}

slot IndexManager::select(std::string IndexName,Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    return CurrentBPT.search(CurrentBPT.root, key);
}

void IndexManager::_delete(std::string IndexName,Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    CurrentBPT._delete(CurrentBPT.root, key);
    CurrentBPT.store_BPlusTree();
}

void IndexManager::_insert(std::string IndexName,Value key, slot keyslot){
    CurrentBPT.load_BPlusTree(IndexName);
    CurrentBPT._insert(key, keyslot);
    CurrentBPT.store_BPlusTree();
}

std::vector<slot> IndexManager::_FindSmall(std::string IndexName,Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    return CurrentBPT.Smaller(key);
}

std::vector<slot> IndexManager::_FindSmallEqual(std::string IndexName,Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    return CurrentBPT.SmallerEqual(key);
}

std::vector<slot> IndexManager::_FindBigger(std::string IndexName,Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    return CurrentBPT.Bigger(key);
}

std::vector<slot> IndexManager::_FindBiggerEqual(std::string IndexName,Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    return CurrentBPT.BiggerEqual(key);
}

std::vector<slot> IndexManager::_FindNotEqual(std::string IndexName, Value key){
    CurrentBPT.load_BPlusTree(IndexName);
    return CurrentBPT.NotEqual(key);
}