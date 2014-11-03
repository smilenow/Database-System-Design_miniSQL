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

extern class DataBlock;
extern class IndexBlock;
extern class CatalogBlock;

// #include "DataBlock.h"
// #include "IndexBlock.h"
// #include "CatalogBlock.h"

#define Buffer_Capacity 4096

#define DB 1	// DataBlock
#define IB 2	// IndexBlock
#define ICB 3	// IndexCatalogBlock
#define TCB 4	// TableCatalogBlock
#define ACB 5	// AttributeCatalogBlock

#define max_name_length 256		// suppose the maximum of all the names is 256

class BufferManager{
public:
	BufferManager();			// 同时也创建三个文件夹
	~BufferManager(){
		write_all();
	}

	Block* newBlock(int type, std::string tablename);
	// todo: catalog_block分开
	// 读block可不分开，但是必须要指明是哪个manager
	Block* getBlock(int type, std::string tablename, int bid);
	bool storeBlock(Block& block);
	int get_block_number(int type, std::string tablename);

	std::vector<IndexBlock> load_tree(std::string indexname);
	bool store_tree(std::string indexname, std::vector<IndexBlock>& tree);
	bool delete_tree(std::string indexname);

public:
	bool write_datablock(int block_n);
	bool write_catalogblock(int block_n);
	bool write_indexblock(int block_n);
	// todo: store(int bid, Block& block);

	int write_block();
	void write_all();
	void create_data(std::string tablename);
	void create_catalog(std::string tablename);
	void create_index(std::string tablename);
	bool is_full();
	void clock_LRU(void *arg);
	void LRU();
	void pin_block(int block_n);
	void unpin_block(int block_n);
	int find_type(Block* block);
	void load_block(int block_id, int type, std::string tablename, int bid);
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