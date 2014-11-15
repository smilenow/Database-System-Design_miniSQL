//
//  buffermanager.cpp
//  miniSQL
//
//  Created by SmilENow on 10/18/14.
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

// 创建文件夹、catalog的三个文件，初始化，调用LRU()
BufferManager::BufferManager(){
//    // debug结束之后必须要修改！！！！！！！
//    rmdir("/Users/SmilENow/dsd");
	mkdir("/Users/SmilENow/dsd", 0777);
//    printf("%d\n", errno);
	mkdir("/Users/SmilENow/dsd/index", 0777);
	mkdir("/Users/SmilENow/dsd/data", 0777);
	mkdir("/Users/SmilENow/dsd/catalog", 0777);

	int fd;
	fd=open("/Users/SmilENow/dsd/catalog/indexcatalogblock", O_RDWR|O_CREAT, 0777);
	close(fd);
	fd=open("/Users/SmilENow/dsd/catalog/attrcatalogblock", O_RDWR|O_CREAT, 0777);
	close(fd);
	fd=open("/Users/SmilENow/dsd/catalog/tablecatalogblock", O_RDWR|O_CREAT, 0777);
	close(fd);
//	assert(0);
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
	if(dynamic_cast<IndexBlock_Buffer*>(block))
		return IB;
	if(dynamic_cast<IndexCatalogBlock*>(block))
		return ICB;
	if(dynamic_cast<TableCatalogBlock*>(block))
		return TCB;
	if(dynamic_cast<AttrCatalogBlock*>(block))
		return ACB;
	return 0;
}

// 创建thread不断更新reference_bit，如果被锁住不更新
void* BufferManager::clock_LRU(void *arg){
	while(1){
		if(pin_bit[reference_bit_count]==false)
			reference_bit[reference_bit_count]=0;
		for(int i=0; i<10000; i++);			// time-delay, should modify the argument in order to timing
		reference_bit_count++;
        reference_bit_count=reference_bit_count%(Buffer_Capacity-1);
	}
//    BufferManager* new_arg=(BufferManager*)arg;
//    return (void *)new_arg;
}

// 创建线程
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
// 	strcpy(fullname, "/Users/SmilENow/dsd/data/");
// 	strcat(fullname, tablename.c_str());
// 	creat(fullname, 0777);
// }

// void BufferManager::create_catalog(std::string tablename){
// 	char fullname[2*max_name_length];
// 	strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
// 	strcat(fullname, tablename.c_str());
// 	creat(fullname, 0777);
// }

// void BufferManager::create_index(std::string tablename, std::string indexname){
// 	char fullname[3*max_name_length];
// 	strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
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

// 下面三个根据buffer中的位置block_n写回不同的block
// 下面三个可以合并成一个，但尚不确定
// 这三个write会erase filename, clear is_dirty, 并且delete指针, 其他不动
// 修改block_number--
bool BufferManager::write_recordblock(int block_n){
	int fd;
	char fullname[2*max_name_length];
    std::map<int, std::string>::iterator it;

	it=filename.find(block_n);
	strcpy(fullname, "/Users/SmilENow/dsd/data/");
	strcat(fullname, (it->second).c_str());
	buffer[block_n]->is_dirty=false;
    if((fd=open(fullname, O_RDWR))<0){
        close(fd);
		return false;
    }
    lseek(fd, buffer[block_n]->block_id*block_size, SEEK_SET);
    if(write(fd, buffer[block_n], block_size)==-1){
        close(fd);
		return false;
    }
	if(close(fd)==-1)
		return false;
	delete buffer[block_n];
    buffer[block_n]=NULL;
    block_number--;
	filename.erase(it);
    pin_bit[block_n]=false;
    reference_bit[block_n]=false;
//    printf("write_record\n");
	return true;
}

bool BufferManager::write_catalogblock(int block_n){
	int fd;
	char fullname[2*max_name_length];
    std::map<int, std::string>::iterator it;

	it=filename.find(block_n);
	strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
	strcat(fullname, (it->second).c_str());
	buffer[block_n]->is_dirty=false;
    if((fd=open(fullname, O_RDWR))<0){
        close(fd);
		return false;
    }
    lseek(fd, buffer[block_n]->block_id*block_size, SEEK_SET);
    if(write(fd, buffer[block_n], block_size)==-1){
        close(fd);
        return false;
    }
	if(close(fd)==-1)
		return false;
	delete buffer[block_n];
    buffer[block_n]=NULL;
    block_number--;
	filename.erase(it);
    pin_bit[block_n]=false;
    reference_bit[block_n]=false;
//    printf("write_catalog\n");
	return true;
}

bool BufferManager::write_indexblock(int block_n){
	int fd;
	char fullname[2*max_name_length];
    std::map<int, std::string>::iterator it;

	it=filename.find(block_n);
	strcpy(fullname, "/Users/SmilENow/dsd/index/");
	strcat(fullname, (it->second).c_str());
	buffer[block_n]->is_dirty=false;
    if((fd=open(fullname, O_RDWR))<0){
        close(fd);
		return false;
    }
    lseek(fd, buffer[block_n]->block_id*block_size, SEEK_SET);
    if(write(fd, buffer[block_n], block_size)==-1){
        close(fd);
        return false;
    }
	if(close(fd)==-1)
		return false;
	delete buffer[block_n];
    buffer[block_n]=NULL;
    block_number--;
	filename.erase(it);
    pin_bit[block_n]=false;
    reference_bit[block_n]=false;
//    printf("write_index_buffer\n");
	return true;
}

// 根据LRU写回一个block
int BufferManager::write_block(){
	while(1){
        int n=reference_bit_count;
		if(reference_bit[n]==false && pin_bit[n]==false){
			if(buffer[n]->is_dirty){
				int type=0;
				Block *block;
				block=buffer[n];
				type=find_type(block);
				
				switch(type){
					case DB:
					if(write_recordblock(n)==false)
						assert(0);
					break;
					case IB:
					if(write_indexblock(n)==false)
						assert(0);
					break;
					default:
					if(write_catalogblock(n)==false)
						assert(0);
					break;
				}
			}
			return n;
		}
        else{
            // 检查所有是不是被pin，正好同时实现一定的时间延迟，延迟之后再尝试write
            bool all_pin=true;
            for(int i=0; i<Buffer_Capacity; i++){
                if(!pin_bit[i]){ all_pin=false; break;}
            }
            if(all_pin==true){
                int n=reference_bit_count;
                int type=0;
                Block *block;
                block=buffer[n];
                type=find_type(block);
                
                switch(type){
                    case DB:
                        if(write_recordblock(n)==false) assert(0); break;
                    case IB:
                        if(write_indexblock(n)==false) assert(0); break;
                    default:
                        if(write_catalogblock(n)==false) assert(0); break;
                }
                return n;
            }
//			for(int i=0; i<10000; i++);
        }
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
		if(buffer[i]!=NULL && buffer[i]->is_dirty){
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
int BufferManager::get_block_number(int type, std::string fname){
	char filename[3*max_name_length];
	int fd;
	off_t offset;

	switch(type){
		case DB:
		strcpy(filename, "/Users/SmilENow/dsd/data/");
		strcat(filename, fname.c_str());
		break;
		case IB:
		strcpy(filename, "/Users/SmilENow/dsd/index/");
		strcat(filename, fname.c_str());
		break;
		default:
		strcpy(filename, "/Users/SmilENow/dsd/catalog/");
		strcat(filename, fname.c_str());
	}
    if((fd=open(filename, O_RDWR))<0){
    	close(fd); //????
    	return 0;
        // printf("------%d\n", errno);
		// assert(0);
    }
	offset=lseek(fd, 0, SEEK_END);
    close(fd);
	if(offset==0) return 0;
	// ??
	return (int)ceil((float)offset/block_size);
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
		strcpy(fullname, "/Users/SmilENow/dsd/data/");
		strcat(fullname, (tablename).c_str());
		break;
		case IB:
//		tablename=tablename+"$"+indexname;
		strcpy(fullname, "/Users/SmilENow/dsd/index/");
		strcat(fullname, (tablename).c_str());
		break;
		default:
		strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
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
		buffer[block_n]=new IndexBlock_Buffer();
		break;
		case TCB:
		buffer[block_n]=new TableCatalogBlock();
//        buffer[block_n]=new Block();
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
    close(fd);
	block_number++;
	buffer[block_n]->is_valid=true;
    pin_bit[block_n]=false;
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
		for(int i=0; i<Buffer_Capacity; i++){
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
//		for(int i=0; i<Buffer_Capacity; i++){
//			if(find_type(buffer[i])==IB){
//                std::map<int, std::string>::iterator it;
//				it=filename.find(i);
//				if(tablename==it->second && buffer[i]->block_id==bid){
//					reference_bit[i]=true;
//					return buffer[i];
//				}
//			}
//		}
	}
	else{
		for(int i=0; i<Buffer_Capacity; i++){
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
Block* BufferManager::newBlock(int type, std::string tablename, int NodeType, int AttrType, int valuecharlen){
//	if(type==IB) tablename=tablename+"$"+indexname;
	int block_n=get_available_block();
    char fullname[3*max_name_length];
    int bid=get_block_number(type, tablename);

	switch(type){
		// 这里真的有必要这样写吗？
		case DB:
		buffer[block_n]=new RecordBlock();
            pin_bit[block_n]=false;
            strcpy(fullname, "/Users/SmilENow/dsd/data/");
            strcat(fullname, (tablename).c_str());
		break;
		case IB:
		buffer[block_n]=new IndexBlock_Buffer();
            pin_bit[block_n]=false;
            strcpy(fullname, "/Users/SmilENow/dsd/index/");
            strcat(fullname, (tablename).c_str());
            break;
		case TCB:
		buffer[block_n]=new TableCatalogBlock();
		pin_bit[block_n]=true;
            strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
            strcat(fullname, (tablename).c_str());
		break;
		case ICB:
		buffer[block_n]=new IndexCatalogBlock();
		pin_bit[block_n]=true;
            strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
            strcat(fullname, (tablename).c_str());
            break;
		case ACB:
		buffer[block_n]=new AttrCatalogBlock();
		pin_bit[block_n]=true;
            strcpy(fullname, "/Users/SmilENow/dsd/catalog/");
            strcat(fullname, (tablename).c_str());
		break;
	}
	strcpy(buffer[block_n]->tablename, tablename.c_str());
	buffer[block_n]->block_id=bid;
	buffer[block_n]->is_valid=true;
	buffer[block_n]->is_pin=false;
	buffer[block_n]->is_dirty=false;

    int fd=open(fullname, O_RDWR|O_APPEND|O_CREAT, 0777);
    // char a[block_size]={'a', };
    long t=write(fd, buffer[block_n], block_size);
    // printf("%d\n", errno);
    close(fd);
	// head_size不用我处理吧？
	reference_bit[block_n]=true;
	block_number++;
	filename.insert(std::pair<int,std::string>(block_n, tablename));
    if(type==IB){
        IndexBlock *ib=new IndexBlock(tablename, bid, NodeType, AttrType, valuecharlen);
        write_indexblock(block_n);
        reference_bit[block_n]=false;
        pin_bit[block_n]=false;
        return ib;
    }
	return buffer[block_n];
} 

// 这三个要注意indexname问题
std::vector<IndexBlock> BufferManager::load_tree(std::string indexname){
	std::vector<IndexBlock> tree;
	int n=get_block_number(IB, indexname);
    if(n>1){
        
    }
    char fullname[2*max_name_length];
    strcpy(fullname, "/Users/SmilENow/dsd/index/");
    strcat(fullname, indexname.c_str());
    int fd=open(fullname, O_RDONLY);
    if(fd<0) assert(0);
    
    Block* tempb=new IndexBlock_Buffer();
    for(int j=0; j<n; j++){
        read(fd, tempb, block_size);
        IndexBlock tempib=
            IndexBlock_Buffer::IndexBlockBuffer_To_IndexBlock(*dynamic_cast<IndexBlock_Buffer *>(tempb));
        tree.push_back(tempib);
//        printf("load_index\n");
//        storeBlock(indexname, tempb);
	}
    close(fd);
    delete tempb;
    return tree;
}

bool BufferManager::store_tree(std::string indexname, std::vector<IndexBlock> tree){
//	get_block_number(IB, indexname);
	char fullname[2*max_name_length];
	int fd;
	// int offset;

	strcpy(fullname, "/Users/SmilENow/dsd/index/");
	strcat(fullname, indexname.c_str());
    remove(fullname);
	fd=open(fullname, O_WRONLY|O_CREAT, 0777);
    // lseek?
    IndexBlock *ib;
	for(std::vector<IndexBlock>::iterator j=tree.begin(); j!=tree.end(); j++){
        ib=&(*j);
        IndexBlock_Buffer tempib=IndexBlock_Buffer::IndexBlock_To_IndexBlockBuffer(*ib);
		write(fd, &tempib, block_size);
//        printf("write_index\n");
	}
	close(fd);
	return true;
	// delete tree?
}

bool BufferManager::delete_tree(std::string indexname){
	char fullname[2*max_name_length];
	strcpy(fullname, "/Users/SmilENow/dsd/index/");
	strcat(fullname, indexname.c_str());
	return remove(fullname);
	// 析构B+树
}

void BufferManager::delete_blocks(std::string tablename){
    for(int i=0; i<Buffer_Capacity; i++){
        if(buffer[i]!=NULL && strcmp(buffer[i]->tablename, tablename.c_str())==0){
            std::map<int, std::string >::iterator it;
            it=filename.find(i);
            
            delete buffer[i];
            buffer[i]=NULL;
            block_number--;
            filename.erase(it);
            pin_bit[i]=false;
            reference_bit[i]=false;
        }
    }
}

int BufferManager::getTableCatalogBlocksNumber(){
    return get_block_number(TCB, "tablecatalogblock");
}
int BufferManager::getAttrCatalogBlocksNumber(){
    return get_block_number(ACB, "attrcatalogblock");
}
int BufferManager::getIndexCatalogBlocksNumber(){
    return get_block_number(ICB, "indexcatalogblock");
}
TableCatalogBlock* BufferManager::getTableCatalogBlocks(int block_id){
    Block *b=getBlock(TCB, "tablecatalogblock", block_id);
    if(dynamic_cast<TableCatalogBlock *>(b)==NULL) assert(0);
    TableCatalogBlock *tcb=dynamic_cast<TableCatalogBlock *>(b);
    return tcb;
}
AttrCatalogBlock* BufferManager::getAttrCatalogBlocks(int block_id){
    Block *b=getBlock(ACB, "attrcatalogblock", block_id);
    AttrCatalogBlock *acb=dynamic_cast<AttrCatalogBlock *>(b);
    return acb;
}
IndexCatalogBlock* BufferManager::getIndexCatalogBlocks(int block_id){
    Block *b=getBlock(ICB, "indexcatalogblock", block_id);
    IndexCatalogBlock *icb=dynamic_cast<IndexCatalogBlock *>(b);
    return icb;
}
void BufferManager::storeTableCatalogBlocks(int block_id, TableCatalogBlock* nowblock){
    nowblock->block_id=block_id;
    storeBlock("tablecatalogblock", nowblock);
}
void BufferManager::storeAttrCatalogBlocks(int block_id, AttrCatalogBlock* nowblock){
    nowblock->block_id=block_id;
    storeBlock("attrcatalogblock", nowblock);
}
void BufferManager::storeIndexCatalogBlocks(int block_id, IndexCatalogBlock* nowblock){
    nowblock->block_id=block_id;
    storeBlock("indexcatalogblock", nowblock);
}
TableCatalogBlock* BufferManager::newTableCatalogBlocks(){
//    return (dynamic_cast<TableCatalogBlock *>(newBlock(TCB, "tablecatalogblock")));
    Block *b=newBlock(TCB, "tablecatalogblock");
    if(dynamic_cast<TableCatalogBlock *>(b)==NULL) assert(0);
    TableCatalogBlock *tcb=dynamic_cast<TableCatalogBlock *>(b);
    return tcb;
}
AttrCatalogBlock* BufferManager::newAttrCatalogBlocks(){
//    return (dynamic_cast<AttrCatalogBlock *>(newBlock(ACB, "attrcatalogblock")));
    Block *b=newBlock(ACB, "attrcatalogblock");
    AttrCatalogBlock *acb=dynamic_cast<AttrCatalogBlock *>(b);
    return acb;
}
IndexCatalogBlock* BufferManager::newIndexCatalogBlocks(){
//    return (dynamic_cast<IndexCatalogBlock *>(newBlock(ICB, "indexcatalogblock")));
    Block *b=newBlock(ICB, "indexcatalogblock");
    IndexCatalogBlock *icb=dynamic_cast<IndexCatalogBlock *>(b);
    return icb;
}

// 要修改的bit：reference, pin, is_dirty, block_number,