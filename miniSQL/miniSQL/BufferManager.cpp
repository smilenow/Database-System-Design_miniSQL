//
//  buffermanager.cpp
//  miniSQL
//
//  Created by Kael on 10/18/14.
//  Copyright (c) 2014 Xinyuan Lu. All rights reserved.
//

// v2.0, re-rewrite and re-name interfaces, by Xinyuan Lu

// v3.0, modify how to write, also modify interfaces, according to
// other 3 managers, by Xinyuan Lu

// 每次修改都要置dirty、reference

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include "BufferManager.h"

bool BufferManager::pin_bit[Buffer_Capacity];
bool BufferManager::reference_bit[Buffer_Capacity];
int BufferManager::reference_bit_count=0;

BufferManager::BufferManager(){
	mkdir("~/dsd", 0777);
	mkdir("~/dsd/index", 0777);
	mkdir("~/dsd/data", 0777);
	mkdir("~/dsd/catalog", 0777);

	int fd;
	fd=open("~/dsd/catalog/indexcatalog", O_RDWR|O_CREAT, 0777);
	close(fd);
	fd=open("~/dsd/catalog/attrcatalog", O_RDWR|O_CREAT, 0777);
	close(fd);
	fd=open("~/dsd/catalog/tablecatalog", O_RDWR|O_CREAT, 0777);
	close(fd);

	block_number=0;
	reference_bit_count=0;

    memset(reference_bit, false, sizeof(reference_bit));
	memset(pin_bit, false, sizeof(pin_bit));
	// filename?
	LRU();
}

bool BufferManager::is_full(){
	if(block_number==Buffer_Capacity) return true;
	else return false;
}

// 得到block类型
int BufferManager::find_type(Block *block){
	if(dynamic_cast<RecordBlock*>(block))
		return DB;
	if(dynamic_cast<IndexBlock*>(block))
		return IB;
	if(dynamic_cast<IndexCatalogBlock*>(block))
		return ICB;
	if(dynamic_cast<TableCatalogBlock*>(block))
		return TCB;
	if(dynamic_cast<AttrCatalogBlock*>(block))
		return ACB;
	return 0;
}

void* BufferManager::clock_LRU(void *arg){
	while(1){
		if(pin_bit[reference_bit_count]==false)
			reference_bit[reference_bit_count]=0;
		for(int i=0; i<100000; i++);			// time-delay, should modify the argument in order to timing
		reference_bit_count++;
	}
    BufferManager* new_arg=(BufferManager*)arg;
    return (void *)new_arg;
}

void BufferManager::LRU(){
	pthread_t tid;
//	pthread_attr_t attr;
//	void * arg=0;

//	pthread_attr_init(&attr);
    pthread_create(&tid, NULL, BufferManager::clock_LRU, this);
}

// 下面三个创建各自目录下的文件，暂时没有用？
// void BufferManager::create_data(std::string tablename){
// 	char fullname[2*max_name_length];
// 	strcpy(fullname, "~/dsd/data/");
// 	strcat(fullname, tablename.c_str());
// 	creat(fullname, 0777);
// }

// void BufferManager::create_catalog(std::string tablename){
// 	char fullname[2*max_name_length];
// 	strcpy(fullname, "~/dsd/catalog/");
// 	strcat(fullname, tablename.c_str());
// 	creat(fullname, 0777);
// }

// void BufferManager::create_index(std::string tablename, std::string indexname){
// 	char fullname[3*max_name_length];
// 	strcpy(fullname, "~/dsd/catalog/");
// 	tablename=tablename+"$"+indexname;
// 	strcat(fullname, tablename.c_str());
// 	creat(fullname, 0777);
// }

void BufferManager::pin_block(int block_n){
	pin_bit[block_n]=true;
}

void BufferManager::unpin_block(int block_n){
	pin_bit[block_n]=false;
}

// 下面三个可以合并成一个，但尚不确定
// 这三个write会erase filename, clear is_dirty, 并且delete指针, 其他不动
bool BufferManager::write_recordblock(int block_n){
	int fd;
	char fullname[2*max_name_length];
    std::map<int, std::string>::iterator it;

	it=filename.find(block_n);
	strcpy(fullname, "~/dsd/data/");
	strcat(fullname, (it->second).c_str());
	buffer[block_n]->is_dirty=false;
	if((fd=open(fullname, O_RDWR|O_APPEND))<0)
		return false;
	if(write(fd, buffer[block_n], block_size)==-1)
		return false;
	if(close(fd)==-1)
		return false;
	delete buffer[block_n];
	filename.erase(it);
	return true;
}

bool BufferManager::write_catalogblock(int block_n){
	int fd;
	char fullname[2*max_name_length];
    std::map<int, std::string>::iterator it;

	it=filename.find(block_n);
	strcpy(fullname, "~/dsd/catalog/");
	strcat(fullname, (it->second).c_str());
	buffer[block_n]->is_dirty=false;
	if((fd=open(fullname, O_RDWR|O_APPEND))<0)
		return false;
	if(write(fd, buffer[block_n], block_size)==-1)
		return false;
	if(close(fd)==-1)
		return false;
	delete buffer[block_n];
	filename.erase(it);
	return true;
}

bool BufferManager::write_indexblock(int block_n){
	int fd;
	char fullname[2*max_name_length];
    std::map<int, std::string>::iterator it;

	it=filename.find(block_n);
	strcpy(fullname, "~/dsd/index/");
	strcat(fullname, (it->second).c_str());
	buffer[block_n]->is_dirty=false;
	if((fd=open(fullname, O_RDWR|O_APPEND))<0)
		return false;
	if(write(fd, buffer[block_n], block_size)==-1)
		return false;
	if(close(fd)==-1)
		return false;
	delete buffer[block_n];
	filename.erase(it);
	return true;
}

// 根据LRU写回一个block，修改block_number--
int BufferManager::write_block(){
	while(1){
		if(reference_bit[reference_bit_count]==false && pin_bit[reference_bit_count]==false){
			if(buffer[reference_bit_count]->is_dirty){
				int type=0;
				Block *block;
				block=buffer[reference_bit_count];
				type=find_type(block);
				
				switch(type){
					case DB:
					if(write_recordblock(reference_bit_count)==false)
						assert(0);
					break;
					case IB:
					if(write_indexblock(reference_bit_count)==false)
						assert(0);
					break;
					default:
					if(write_catalogblock(reference_bit_count)==false)
						assert(0);
					break;
				}
			}
			block_number--;
			return reference_bit_count;
		}
		else
			for(int i=0; i<100000; i++);
	}
}

// 获取一个空的block_n可供使用，必要的时候会写回block
int BufferManager::get_available_block(){
	if(!is_full()){
		// time-wasting!!!!
		// need improvement
		for(int i=0; i<Buffer_Capacity; i++){
			if(buffer[i]==NULL){
				return i;
			}
		}
	}

	return write_block();
}

// 写回所有的block，析构用
void BufferManager::write_all(){
	for(int i=0; i<Buffer_Capacity; i++){
		if(buffer[i]->is_dirty){
			int type=-1;
			Block *block;
			block=buffer[i];
			type=find_type(block);

			switch(type){
				case DB:
				if(write_recordblock(i)==false)
					assert(0);
				break;
				case IB:
				if(write_indexblock(i)==false)
					assert(0);
				break;
				default:
				if(write_catalogblock(i)==false)
					assert(0);
			}
		}
	}
	block_number=0;
}

// 根据上层指定的block指针写回去一个block，会删除buffer里面的该block
// 置两个bit
bool BufferManager::storeBlock(std::string tablename, Block *block){
    int i;
	for(i=0; i<Buffer_Capacity; i++){
		if(buffer[i]==block) break;
	}
	if(i==Buffer_Capacity) assert(0);
	int type=find_type(buffer[i]);
	switch(type){
		case DB:
		if(write_recordblock(i)==0) assert(0);
		break;
		case IB:
		if(write_indexblock(i)==0) assert(0);
		break;
		default:
		if(write_catalogblock(i)==0) assert(0);
		break;
	}
	reference_bit[i]=false;
	pin_bit[i]=false;
	return true;
}

// get the number of blocks in a table/index
// if the file doens't exist, will create one
int BufferManager::get_block_number(int type, std::string fname){
	char filename[3*max_name_length];
	int fd;
	off_t offset;

	switch(type){
		case DB:
		strcpy(filename, "~/dsd/data/");
		strcat(filename, fname.c_str());
		break;
		case IB:
		strcpy(filename, "~/dsd/index/");
		strcat(filename, fname.c_str());
		break;
		default:
		strcpy(filename, "~/dsd/catalog/");
		strcat(filename, fname.c_str());
	}
	if((fd=open(filename, O_RDWR|O_APPEND|O_CREAT, 777))<0)
		assert(0);
	offset=lseek(fd, 0, SEEK_END);
	if(offset==0) return 0;
	// ??
	return (int)ceil((float)offset/block_size);
	assert(0);
}

// load block from disk, 必要时会写回
// buffer为它开辟block的时候会载入之前的信息bit，更新三个is_* bit
// block_number++
// 有冗余，需要优化
void BufferManager::load_block(int block_n, int type, std::string tablename, int bid){
	int fd;
	char fullname[3*max_name_length];
	off_t offset=bid*block_size;

	switch(type){
		case DB:
		strcpy(fullname, "~/dsd/data/");
		strcat(fullname, (tablename).c_str());
		break;
		case IB:
//		tablename=tablename+"$"+indexname;
		strcpy(fullname, "~/dsd/index/");
		strcat(fullname, (tablename).c_str());
		break;
		default:
		strcpy(fullname, "~/dsd/catalog/");
		strcat(fullname, (tablename).c_str());
		break;
	}

	if((fd=open(fullname, O_RDWR))<0)
        assert(0);

	switch(type){
		// 这里真的有必要这样写吗？
		case DB:
		buffer[block_n]=new RecordBlock();
		break;
		case IB:
		buffer[block_n]=new IndexBlock();
		break;
		case TCB:
		buffer[block_n]=new TableCatalogBlock();
		break;
		case ICB:
		buffer[block_n]=new IndexCatalogBlock();
		break;
		case ACB:
		buffer[block_n]=new AttrCatalogBlock();
		break;
	}
	lseek(fd, offset, SEEK_SET);
	read(fd, buffer[block_n], block_size);

	block_number++;
	buffer[block_n]->is_valid=true;
	buffer[block_n]->is_pin=false;
	buffer[block_n]->is_dirty=false;
	return;
}

// 返回后需要dynamic_cast
// 从disk/buffer拿回来一个block, 如果是从disk拿回来的，CB会pin
// 置reference bit, pin bit
// 加上block_id, 
// 更新filename[]
Block* BufferManager::getBlock(int type, std::string tablename, int bid){
	// first find in buffer
	// 可以优化
	if(type==DB){
		for(int i; i<Buffer_Capacity; i++){
			if(find_type(buffer[i])==DB){
                std::map<int, std::string>::iterator it;
				it=filename.find(i);
				if(tablename==it->second && buffer[i]->block_id==bid){
					reference_bit[i]=true;
					//is_dirty????????
					return buffer[i];
				}
			}
		}
	}
	else if(type==IB){
//		tablename=tablename+"$"+indexname;
		for(int i; i<Buffer_Capacity; i++){
			if(find_type(buffer[i])==IB){
                std::map<int, std::string>::iterator it;
				it=filename.find(i);
				if(tablename==it->second && buffer[i]->block_id==bid){
					reference_bit[i]=true;
					return buffer[i];
				}
			}
		}
	}
	else{
		for(int i; i<Buffer_Capacity; i++){
			if(find_type(buffer[i])==TCB || find_type(buffer[i])==ICB || find_type(buffer[i])==ACB){
				std::map<int, std::string>::iterator it;
				it=filename.find(i);
				if(tablename==it->second && buffer[i]->block_id==bid){
					reference_bit[i]=true;
					return buffer[i];
				}
			}
		}
	}
	// from disk
	int block_n;
	block_n=get_available_block();
	if(type==DB){
		load_block(block_n, type, tablename, bid);
	}
	else if(type==TCB || type==ICB || type==ACB){
		load_block(block_n, type, tablename, bid);
		pin_block(block_n);
	}
	else if(type==IB){
		// pin?
		load_block(block_n, type, tablename, bid);
	}
	filename.insert(std::pair<int,std::string>(block_n, tablename));
	return buffer[block_n];
}

// 多加一个new block for index
// 同getBlock/loadBlock
Block* BufferManager::newBlock(int type, std::string tablename){
	int bid;
//	if(type==IB) tablename=tablename+"$"+indexname;
	bid=get_block_number(type, tablename);
	int block_n=get_available_block();

	switch(type){
		// 这里真的有必要这样写吗？
		case DB:
		buffer[block_n]=new RecordBlock();
		break;
		case IB:
		buffer[block_n]=new IndexBlock();
		break;
		case TCB:
		buffer[block_n]=new TableCatalogBlock();
		pin_bit[block_n]=true;
		break;
		case ICB:
		buffer[block_n]=new IndexCatalogBlock();
		pin_bit[block_n]=true;
		break;
		case ACB:
		buffer[block_n]=new AttrCatalogBlock();
		pin_bit[block_n]=true;
		break;
	}
	strcpy(buffer[block_n]->tablename, tablename.c_str());
	buffer[block_n]->block_id=bid;
	buffer[block_n]->is_valid=true;
	buffer[block_n]->is_pin=false;
	buffer[block_n]->is_dirty=false;
	// head_size不用我处理吧？
	reference_bit[block_n]=true;
	block_number++;
	filename.insert(std::pair<int,std::string>(block_n, tablename));
	return buffer[block_n];
} 

// 这三个要注意indexname问题
std::vector<IndexBlock> BufferManager::load_tree(std::string indexname){
	std::vector<IndexBlock> tree;
	int n=get_block_number(IB, indexname);
	char fullname[2*max_name_length];
	int fd;
	// int offset;

	strcpy(fullname, "~/dsd/index/");
	strcat(fullname, indexname.c_str());
	fd=open(fullname, O_RDONLY);
	IndexBlock* i=new IndexBlock();	// 可能要用指针？
	for(int j=0; j<n; i++){
		read(fd, (void*)i, block_size);
		tree.push_back(*i);
		delete i;
	}
	close(fd);
	return tree;
}

bool BufferManager::store_tree(std::string indexname, std::vector<IndexBlock>& tree){
	int n=get_block_number(IB, indexname);
	char fullname[2*max_name_length];
	int fd;
	// int offset;

	strcpy(fullname, "~/dsd/index/");
	strcat(fullname, indexname.c_str());
	fd=open(fullname, O_WRONLY);
	// IndexBlock* ib;	// 可能要用指针？
	for(std::vector<IndexBlock>::iterator j=tree.begin(); j!=tree.end(); j++){
		IndexBlock* ib=new IndexBlock();
		write(fd, (void*)ib, block_size);
        delete ib;
	}
	close(fd);
	return true;
	// delete tree?
}

bool BufferManager::delete_tree(std::string indexname){
	char fullname[2*max_name_length];
	strcpy(fullname, "~/dsd/index/");
	strcat(fullname, indexname.c_str());
	return remove(fullname);
	// 析构B+树
}

int BufferManager::getTableCatalogNumbers(){
    return get_block_number(TCB, "tablecatalogblock");
}
int BufferManager::getAttrCatalogNumbers(){
    return get_block_number(ACB, "attrcatalogblock");
}
int BufferManager::getIndexCatalogNumbers(){
    return get_block_number(ICB, "indexcatalogblock");
}
TableCatalogBlock BufferManager::getTableCatalogBlocks(int block_id){
    return *(dynamic_cast<TableCatalogBlock *>(getBlock(TCB, "tablecatalogblock", block_id)));
}
AttrCatalogBlock BufferManager::getAttrCatalogBlocks(int block_id){
    return *(dynamic_cast<AttrCatalogBlock *>(getBlock(ACB, "attrcatalogblock", block_id)));
}
IndexCatalogBlock BufferManager::getIndexCatalogBlocks(int block_id){
    return *(dynamic_cast<IndexCatalogBlock *>(getBlock(ICB, "indexcatalogblock", block_id)));
}
void BufferManager::storeTableCatalogBlocks(int block_id, TableCatalogBlock& nowblock){
    storeBlock("tablecatalogblock", &nowblock);
}
void BufferManager::storeAttrCatalogBlocks(int block_id, AttrCatalogBlock& nowblock){
    storeBlock("attrcatalogblock", &nowblock);
}
void BufferManager::storeIndexCatalogBlocks(int block_id, IndexCatalogBlock& nowblock){
    storeBlock("indexcatalogblock", &nowblock);
}
TableCatalogBlock BufferManager::newTableCatalogBlocks(int block_id){
    return *(dynamic_cast<TableCatalogBlock *>(newBlock(TCB, "tablecatalogblock")));
}
AttrCatalogBlock BufferManager::newAttrCatalogBlocks(int block_id){
    return *(dynamic_cast<AttrCatalogBlock *>(newBlock(ACB, "attrcatalogblock")));
}
IndexCatalogBlock BufferManager::newIndexCatalogBlocks(int block_id){
    return *(dynamic_cast<IndexCatalogBlock *>(newBlock(ICB, "indexcatalogblock")));
}

// 要修改的bit：reference, pin, is_dirty, block_number,