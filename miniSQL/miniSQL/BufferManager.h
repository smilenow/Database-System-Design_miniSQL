//
//  buffermanager.h
//  miniSQL
//
//  Created by Kael on 10/18/14.
//  Copyright (c) 2014 Xinyuan Lu. All rights reserved.
//

#ifndef __BUFFERMANAGER__
#define __BUFFERMANAGER__

#include "Block.h"
#include "Base.h"
#include <string>
#include <vector>
#include <map>

// #include "DataBlock.h"
// #include "IndexBlock.h"
// #include "IndexCatalogBlock.h"
// #include "TableCatalogBlock.h"
// #include "AttributeCatalogBlock.h"

#define Buffer_Capacity 32768

#define DB 1	// DataBlock
#define IB 2	// IndexBlock
#define ICB 3	// IndexCatalogBlock
#define TCB 4	// TableCatalogBlock
#define ACB 5	// AttributeCatalogBlock

#define max_name_length 128		// suppose the maximum of all the names is 256

class BufferManager{
public:
	BufferManager();			// 同时也创建三个文件夹
	~BufferManager(){
		write_all();
	}

	Block* newBlock(int type, std::string tablename, std::string indexname="");
	// todo: catalog_block分开
	// 读block可不分开，但是必须要指明是哪个manager
	// 返回后需要dynamic_cast
	Block* getBlock(int type, std::string tablename, int bid, std::string indexname="");
	// storeBlock第一个参数可以去掉，只给出block就可以
	bool storeBlock(std::string tablename, Block *block);
	// 为了防止产生歧义，getblocknumber使用fname(file)做变量名
	// 事实上只有index的fname相对特殊
	int get_block_number(int type, std::string fname);

	std::vector<IndexBlock> load_tree(std::string indexname);
	bool store_tree(std::string indexname, std::vector<IndexBlock>& tree);
	bool delete_tree(std::string indexname);

public:
	bool write_datablock(int block_n);
	bool write_catalogblock(int block_n);
	bool write_indexblock(int block_n);

	int write_block();
	void write_all();
	// void create_data(std::string tablename);
	// void create_catalog(std::string tablename);
	// void create_index(std::string tablename, std::string indexname);
	bool is_full();
	void clock_LRU(void *arg);
	void LRU();
	void pin_block(int block_n);
	void unpin_block(int block_n);
	int find_type(Block* block);
	void load_block(int block_n, int type, std::string tablename, int bid, std::string indexname="");
	int get_available_block();
public:
	int block_number;
	bool reference_bit[Buffer_Capacity];

	// if the pin
	// but if explicitly required, it should write inspite the pin_bit
	bool pin_bit[Buffer_Capacity];

	int reference_bit_count;
	
	std::map<int, std::string> filename;
	Block* buffer[Buffer_Capacity];
};

#endif