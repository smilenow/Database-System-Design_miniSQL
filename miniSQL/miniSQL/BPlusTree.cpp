//
//  BPlusTree.cpp
//  miniSQL
//
//  Created by SmilENow on 10/14/14.
//  Copyright (c) 2014 Jiaquan Yin. All rights reserved.
//

#include "BPlusTree.h"

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

void IndexBlock::read(){ // buffer 怎么给我？
    
}


//------------------------------------------------------------------------------------------//

void BPlusTree::Create_BPlusTree(std::string IndexName,int IndexType,std::vector<Value> data,std::vector<slot> dataslot){
    int NewBlockID;
    // buffer 给我申请一个可用的block_id,记为NewBlockID
    root = new IndexBlock(IndexName,NewBlockID,_leaf_type,IndexType);
    for (int i=0;i<data.size();i++){
        std::pair<Value *,slot *> tmp = find(root, data[i]);
        if (tmp.first!=NULL && tmp.first->getValid()==true)
            printf("Key %s is duplicated\n",data[i].getKey().c_str());
        else if (tmp.first==NULL) root = insert(root,data[i],dataslot[i]);
        else tmp.first->setValid(true);
    }
}

std::pair<Value*,slot*> BPlusTree::find(IndexBlock *nownode, Value key){
    if (nownode == NULL) return std::make_pair<Value*, slot*>(NULL,NULL);
    if (nownode->NodeType == _leaf_type){
        for (int i=0;i<nownode->nowkey;i++)
            if (isEqual(nownode->key[i], key)) return std::make_pair(&nownode->key[i],&nownode->slots[i]);
        return std::make_pair<Value*, slot*>(NULL,NULL);
    }
    else {
        for (int i=1;i<nownode->nowkey;i++)
            if (isLess(key, nownode->key[i])) return find(nownode->slots_child[i-1],key);
        return find(nownode->slots_child[nownode->nowkey],key);
    }
    return std::make_pair<Value*, slot*>(NULL,NULL);;
}

IndexBlock* BPlusTree::findBigger(IndexBlock* nownode,Value key){
    if (nownode == NULL) return NULL;
    if (nownode->NodeType == _leaf_type) return nownode;
    for (int i=1;i<nownode->nowkey;i++)
        if (isLess(key, nownode->key[i])) return findBigger(nownode->slots_child[i-1], key);
    return findBigger(nownode->slots_child[nownode->nowkey], key);
}

slot BPlusTree::search(IndexBlock *nownode, Value key){
    if (nownode == NULL) return slot(-1,-1);
    if (nownode->NodeType == _leaf_type){
        for (int i=0;i<nownode->nowkey;i++)
            if (isEqual(nownode->key[i], key)) return nownode->slots[i];
        return slot(-1,-1);
    }
    else {
        for (int i=1;i<nownode->nowkey;i++)
            if (isLess(key, nownode->key[i])) return search(nownode->slots_child[i-1],key);
        return search(nownode->slots_child[nownode->nowkey],key);
    }
    return slot(-1,-1);
}

IndexBlock* BPlusTree::insert(IndexBlock* nownode, Value key,slot keyslot){
    // 判断叶子节点
    if (nownode->NodeType == _leaf_type){
        if (nownode->nowkey < nownode->maxkey-2){
            int i;
            for (i=nownode->nowkey; i>=0; i--)
                if (isLess(key,nownode->key[i])){
                    nownode->key[i+1] = nownode->key[i];
                    nownode->slots[i+1] = nownode->slots[i];
                }
                else break;
            nownode->key[i+1] = key;
            nownode->slots[i+1]=keyslot;
            nownode->nowkey++;
            nownode->split = 0;
            return nownode;
        } else {
            // 分裂，申请一个新的block
            int i;
            for (i=nownode->nowkey; i>=0; i--)
                if (isLess(key,nownode->key[i])){
                    nownode->key[i+1] = nownode->key[i];
                    nownode->slots[i+1] = nownode->slots[i];
                }
                else break;
            nownode->key[i+1] = key;
            nownode->slots[i+1]=keyslot;
            
            int NewBID_TMP;
            // buffer 给我申请一个可用的block_id
            
            IndexBlock* tmp = new IndexBlock(nownode->IndexName,NewBID_TMP,_leaf_type,nownode->AttrType);
            int k = (nownode->maxkey%2==0)?0:1;
            for (int j=0;j<nownode->maxkey/2;j++){
                tmp->key[j] = nownode->key[nownode->maxkey/2+j+k];
                tmp->slots[j] = nownode->slots[nownode->maxkey/2+j+k];
                nownode->key[nownode->maxkey/2+j+k].resetKey();
                nownode->slots[nownode->maxkey/2+j+k].reset();
            }
            tmp->nowkey = nownode->maxkey/2;
            nownode->nowkey -= nownode->maxkey/2;
            
            int NewBID_TMP_FA;
            // buffer 给我申请一个可用的block_id
            IndexBlock* tmpfa = new IndexBlock(nownode->IndexName,NewBID_TMP_FA,_inner_type,nownode->AttrType);
            tmpfa->nowkey = 2;
            
            tmpfa->slots_child[0]=nownode;
            tmpfa->slots_child[1]=tmpfa;
            tmpfa->key[0] = nownode->key[0];
            tmpfa->key[1] = tmp->key[0];
            slot s1(nownode->block_id,-1), s2(tmp->block_id,-1);
            tmpfa->slots[0] = s1;
            tmpfa->slots[1] = s2;
            
            tmpfa->split=1;
            return tmpfa;
        }
    }
    // 判断内部节点
    else {
        for (int i=1;i<=nownode->nowkey+1;i++){
            if (isLess(key, nownode->key[i]) || (i==(nownode->nowkey+1))){ // 找个合适的内部节点插入
                if (nownode->slots_child[i-1]->nowkey < nownode->maxkey-2 ){ // 可以有合适的位置插入
                    nownode->slots_child[i-1] = insert(nownode->slots_child[i-1], key, keyslot);
                    nownode->key[i-1] = nownode->slots_child[i-1]->key[0]; // 更新
                    slot s1(nownode->slots_child[i-1]->block_id,-1);
                    nownode->slots[i-1] = s1;
                    nownode->split = 0;
                    return nownode;
                }
                else {
                    IndexBlock * tmp = insert(nownode->slots_child[i-1], key, keyslot);
                    if (tmp->split == 0){ // 插入不引起分裂
                        nownode->slots_child[i-1] = tmp;
                        // 更新内部节点的node
                        for (int j=0;j<nownode->nowkey;j++){
                            nownode->key[j] = nownode->slots_child[j]->key[0];
                            slot s1(nownode->slots_child[j]->block_id,-1);
                            nownode->slots[j] = s1;
                        }
                        nownode->split = 0;
                        return nownode;
                    }
                    if (nownode->nowkey <= nownode->maxkey-2){
                        nownode->nowkey++;
                        for (int j=nownode->nowkey-1; j>=i; j--){
                            nownode->key[j+1] = nownode->key[j];
                            nownode->slots[j+1] = nownode->slots[j];
                            nownode->slots_child[j+1] = nownode->slots_child[j];
                        }
                        nownode->slots_child[i-1] = tmp->slots_child[0];
                        nownode->slots_child[i] = tmp->slots_child[1];
                        for (int j=0;j<nownode->nowkey;j++){
                            nownode->key[j] = nownode->slots_child[j]->key[0];
                            slot s1(nownode->slots_child[j]->block_id,-1);
                            nownode->slots[j] = s1;
                        }
                        if (nownode->nowkey != nownode->maxkey-1){
                            nownode->split = 0;
                            return nownode;
                        }
                        else {
                            int NewBID_TMP_t1,NewBID_TMP_t2;
                            // buffer 给我申请一个可用的block_id
                            IndexBlock * t1 = new IndexBlock(nownode->IndexName,NewBID_TMP_t1,_inner_type,nownode->AttrType);
                            IndexBlock * t2 = new IndexBlock(nownode->IndexName,NewBID_TMP_t2,_inner_type,nownode->AttrType);
                            nownode->NodeType=_inner_type;
                            int j,k;
                            for (j=0;j<nownode->maxkey/2;j++){
                                t1->key[j] = nownode->key[j];
                                t1->slots_child[j] = nownode->slots_child[j];
                                t1->slots[j] = nownode->slots[j];
                            }
                            t1->nowkey = j;
                            for (k=j;k<=nownode->maxkey-1;k++){
                                t2->key[k-j] = nownode->key[k];
                                t2->slots[k-j] = nownode->slots[k];
                                t2->slots_child[k-j] = nownode->slots_child[k];
                            }
                            t2->nowkey = k-j;
                            for (int pi=0;pi<nownode->maxkey;pi++){
                                nownode->slots[pi].reset();
                                nownode->key[pi].resetKey();
                            }
                            nownode->nowkey = 2;
                            nownode->slots_child[0] = t1;
                            nownode->slots_child[1] = t2;
                            nownode->key[0]=t1->key[0];
                            nownode->key[1]=t2->key[1];
                            slot s1(t1->block_id,-1), s2(t2->block_id,-1);
                            nownode->slots[0] = s1;
                            nownode->slots[1] = s2;
                            nownode->split = 1;
                            return nownode;
                        }
                    }
                }
            }
        }
        return nownode;
    }
}

void BPlusTree::Link_Leaf(){
    std::vector<IndexBlock* > link;
    std::queue<IndexBlock* > q;
    IndexBlock* cur;
    q.push(root);
    while (!q.empty()){
        cur = q.front();
        q.pop();
        if (cur->NodeType == _leaf_type){
            link.push_back(cur);
            continue;
        }
        for (int i=0;i<=cur->nowkey;i++) q.push(cur->slots_child[i]);
    }
    for (int i=1;i<link.size();i++){
        link[i-1]->set_last_slot(link[i]);
    }
    link[link.size()-1]->set_last_slot(NULL);
}

void BPlusTree::_insert(Value key,slot keyslot){
    std::pair<Value*, slot*> tmp = find(root, key);
    if (tmp.first==NULL){ root = insert(root,key,keyslot); return ; }
    if (tmp.first->getValid() == true)
        printf("Key %s is duplicated\n",key.getKey().c_str());
    else if (tmp.first->getValid() == false && compare(*tmp.second, keyslot)) tmp.first->setValid(true);
    else tmp.second->block_id=keyslot.block_id, tmp.second->offset=keyslot.offset, tmp.first->setValid(true);
}

void BPlusTree::_delete(IndexBlock* nownode,Value key){
    std::pair<Value*, slot*> tmp = find(root,key);
    if (tmp.first==NULL) return;
    tmp.first->setValid(false);
}

void BPlusTree::delete_whole_tree(IndexBlock *nownode) {
    if (nownode == NULL) return;
    for (int i=0;i<nownode->maxkey;i++){
        delete_whole_tree(nownode->slots_child[i]);
    }
    delete nownode;
}

std::vector<slot> BPlusTree::Smaller(Value key){
    IndexBlock* Leaf = root;
    while (Leaf->NodeType == _inner_type)
        Leaf = Leaf->slots_child[0];
    std::vector<slot> answer;
    answer.resize(0);
    while (Leaf != NULL){
        for (int i=0;i<Leaf->nowkey;i++)
            if (isLess(key, Leaf->key[i])) answer.push_back(Leaf->slots[i]);
            else return answer;
        Leaf = Leaf->get_last_slot_child();
    }
    return answer;
}

std::vector<slot> BPlusTree::SmallerEqual(Value key){
    IndexBlock* Leaf = root;
    while (Leaf->NodeType == _inner_type)
        Leaf = Leaf->slots_child[0];
    std::vector<slot> answer;
    answer.resize(0);
    while (Leaf != NULL){
        for (int i=0;i<Leaf->nowkey;i++)
            if (isLessEqual(key, Leaf->key[i])) answer.push_back(Leaf->slots[i]);
            else return answer;
        Leaf = Leaf->get_last_slot_child();
    }
    return answer;
}

std::vector<slot> BPlusTree::Bigger(Value key){
    IndexBlock* Leaf = findBigger(root, key);
    std::vector<slot> answer;
    answer.resize(0);
    while (Leaf!=NULL){
        for (int i=0;i<Leaf->nowkey;i++)
            if (isLess(Leaf->key[i], key)) answer.push_back(Leaf->slots[i]);
        Leaf = Leaf->get_last_slot_child();
    }
    return answer;
}

std::vector<slot> BPlusTree::BiggerEqual(Value key){
    IndexBlock* Leaf = findBigger(root, key);
    std::vector<slot> answer;
    answer.resize(0);
    while (Leaf!=NULL){
        for (int i=0;i<Leaf->nowkey;i++)
            if (isLessEqual(Leaf->key[i], key)) answer.push_back(Leaf->slots[i]);
        Leaf = Leaf->get_last_slot_child();
    }
    return answer;
}


//------------------------------------------------------------------------------------------//

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

bool BPlusTree::compare(const slot & s1, const slot & s2){
    return (s1.block_id==s2.block_id && s1.offset==s2.offset);
}
