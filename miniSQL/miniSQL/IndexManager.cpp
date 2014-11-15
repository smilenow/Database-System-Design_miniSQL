//
//  IndexManager.cpp
//  miniSQL
//
//  Created by SmilENow on 10/25/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "IndexManager.h"

void IndexManager::CreateIndex(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot,int valuecharlen){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        this->IndexName = IndexName;
        CurrentBPT.Create_BPlusTree(IndexName, IndexType, data, dataslot,valuecharlen);
    }
}

void IndexManager::DropIndex(std::string IndexName){
//    CurrentBPT.delete_whole_tree(CurrentBPT.root);
    this->IndexName = "";
    buffermanager->delete_tree(IndexName);
}

slot IndexManager::select(std::string IndexName,Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    return CurrentBPT.search(CurrentBPT.root, key);
}

void IndexManager::_delete(std::string IndexName,Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT._delete(CurrentBPT.root, key);
}

void IndexManager::_insert(std::string IndexName,Value key, slot keyslot){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT._insert(key, keyslot);
}

std::vector<slot> IndexManager::_FindSmall(std::string IndexName,Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT.Link_Leaf();
    return CurrentBPT.Smaller(key);
}

std::vector<slot> IndexManager::_FindSmallEqual(std::string IndexName,Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT.Link_Leaf();
    return CurrentBPT.SmallerEqual(key);
}

std::vector<slot> IndexManager::_FindBigger(std::string IndexName,Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT.Link_Leaf();
    return CurrentBPT.Bigger(key);
}

std::vector<slot> IndexManager::_FindBiggerEqual(std::string IndexName,Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT.Link_Leaf();
    return CurrentBPT.BiggerEqual(key);
}

std::vector<slot> IndexManager::_FindNotEqual(std::string IndexName, Value key){
    if (this->IndexName != IndexName){
        if (this->IndexName != "" ) CurrentBPT.store_BPlusTree();
        CurrentBPT.load_BPlusTree(IndexName);
        this->IndexName = IndexName;
    }
    CurrentBPT.Link_Leaf();
    return CurrentBPT.NotEqual(key);
}