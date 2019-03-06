#include "Data_structure.h"
#include <stdio.h>
#pragma warning (disable:4996);

string db_name;//path global
char* remaining_size; //Remaining size of block 
int  remainingsize_length;//length of the Remaining size of block char

//CONSTRUCTORS------------------------------------------------------------------
db_metadata::db_metadata()
{
}
block::block()
{
}
table::table()
{
}
table::table(char[20])
{
}
db_metadata::db_metadata(int db_size_, int block_size_)
{
	this->cant_blocks = db_size_ * 1024 / block_size_;
	this->db_size = db_size_ * 1024;
	this->block_size = block_size_;

}
records_metadata::records_metadata()
{
}


//HELPING FUNCTIONS---------------------------------------------------------------------

db_metadata db_metadata::fill_dbm(string path, db_metadata dbm) {
	ifstream file_reader(path, ios::binary);
	file_reader.read(reinterpret_cast<char*>(&dbm), sizeof(db_metadata));
	return dbm;
}

char* get_remaining_size(db_metadata dbm) {
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return false;
	}
	int rsize_length = dbm.block_size - 20;
	char*r_size;
	db_file.ignore(12);
	r_size = new char[rsize_length];
	db_file.read(reinterpret_cast<char*>(&r_size), sizeof(char)*rsize_length);

	return r_size;
}

void db_metadata::open_database(string path) {
	db_name = path;

}

int db_metadata::get_freeblock_index(db_metadata dbm) {//Returns index of the block wich has no tables in it (Used while creating tables)
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);

	if (!db_file) {
		cout << "Problem with Binary file";
		return 0;
	}
	//Objects
	block block_reader;

	//Move to the start of block number 3 (Form 3 and on, Tables are created)
	db_file.seekg(0 | ios::beg);
	db_file.seekg(dbm.block_size * 2);

	//Travel the blocks 
	for (int i = 0; i < (dbm.cant_blocks - 2); i++)
	{
		//Ignore the space in iwch the tables and records are saved
		db_file.ignore(remainingsize_length);

		//Read the struct of block after the space available for tables. It contains(int,int)
		db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));

		//If there is no table written on the block
		if (block_reader.is_empty == true) {
			//Return the index of that block
			return (((int)db_file.tellg() / dbm.block_size));
		}
	}
	return 0;
}

int db_metadata::get_table_index(db_metadata dbm, char table_name_[20]) {//Given a table name, find the index of it

	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return 0;
	}

	//Objects
	sys_table_metadata sysmeta;
	sys_table_field sysfield;

	//Move to second block. The second block contains the System table Master
	db_file.seekg(dbm.block_size);
	db_file.read(reinterpret_cast<char*>(&sysmeta), sizeof(sys_table_metadata));

	//make a vector of system table fields
	vector<sys_table_field>sys_vector;

	//Since the system tables metadata contains the number of tables in it, we can travel easily through it
	for (int i = 0; i < sysmeta.table_count; i++)
	{
		//Fill the vector with fields from system table
		sys_table_field sysfield;
		db_file.read(reinterpret_cast<char*>(&sysfield), sizeof(sys_table_field));
		sys_vector.push_back(sysfield);
	}
	//Loop though the vetor until the table name is found
	for (int i = 0; i < sys_vector.size(); i++)
	{
		if (strcmp(sys_vector[i].table_name, table_name_) == 0) {
			//cout  << endl;
			//cout << "Table found" << endl;
			return sys_vector[i].index;

		}
	}
	cout << "Table not found" << endl;
	return false;

};


//LIST FUNCTIONS---------------------------------------------------------------------

void db_metadata::list_systables(db_metadata dbm) {
	system("CLS");
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}
	sys_table_metadata sysmeta;
	sys_table_field sysfield;

	//move to second block to fill systable with new created table
	db_file.seekg(dbm.block_size);
	db_file.ignore(20);
	db_file.read(reinterpret_cast<char*>(&sysmeta.table_count), sizeof(int));

	vector<sys_table_field>sys_vector;
	for (int i = 0; i < sysmeta.table_count; i++)
	{
		sys_table_field sysfield;
		db_file.read(reinterpret_cast<char*>(&sysfield), sizeof(sys_table_field));
		sys_vector.push_back(sysfield);
	}

	cout << "---------TABLES IN FILE--------" << endl;
	for (int i = 0; i < sys_vector.size(); i++)
	{
		cout << "Table name: "; cout << sys_vector[i].table_name; cout << " Index: ";
		cout << sys_vector[i].index; cout << endl;

	}


}

void db_metadata::select(db_metadata dbm) {
	system("CLS");
	int option = 0;
	char tabl_name[20];
	cout << "1)Show all tables" << endl;
	cout << "2)Show all columns from a table" << endl;
	cout << "3)Show specific column records from a table" << endl;
	cout << "4)Show all records from table" << endl;
	cout << "Enter option here:";
	cin >> option; cout << endl;
	if (option == 1) {
		system("CLS");
		list_systables(dbm);
	}
	if (option == 2) {
		system("CLS");
		list_systables(dbm);
		cout << "Enter table name: ";
		cin >> tabl_name;
		int index = get_table_index(dbm, tabl_name);
		int full_index = (index - dbm.block_size);
		show_table_columns(full_index);
	}
	if (option == 3) {
		select_specific_column(dbm);
		system("pause");
		system("CLS");

	}
	if (option == 4) {
		system("CLS");
		list_systables(dbm);
		cout << endl;
		cout << "Enter table name: ";
		cin >> tabl_name;
		cout << endl;
		int index = get_table_index(dbm, tabl_name);
		int full_index = (index - dbm.block_size);
		show_all_records(full_index, dbm, 0);

	}

}

void db_metadata::show_table_columns(int index) {//Tengo que darle el index restado del blocksize
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Objects
	table table_object2;

	//Create a varibale wich stores int,char and doubles sizes 
	int reg_fields_move = 0;//Move this times per reigstry

							//Enter block index
	db_file.seekg(index);

	//Fill table object with data from file
	db_file.read(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
	db_file.read(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));

	for (int i = 0; i < table_object2.vector_size; i++)
	{
		field field_object;
		records_field reg_f;

		db_file.read(reinterpret_cast<char*>(&field_object), sizeof(field));
		table_object2.fields.push_back(field_object);
	}

	//Print whats inside table object
	cout << "Table: ";
	cout << table_object2.table_name;
	cout << "  Number of objects: ";
	cout << table_object2.vector_size << endl;
	cout << "---------------Printing all columns---------------------" << endl;


	for (int i = 0; i < table_object2.vector_size; i++)
	{

		if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
			cout << "Column name: ";
			cout << table_object2.fields[i].field_name; cout << " ";
			cout << table_object2.fields[i].field_type; cout << endl;

		}
		else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {
			cout << "Column name: ";
			cout << table_object2.fields[i].field_name; cout << " ";
			cout << table_object2.fields[i].field_type; cout << endl;
		}

		else {
			cout << "Column name: ";

			cout << table_object2.fields[i].field_name;  cout << " ";
			cout << "Char(";
			cout << table_object2.fields[i].field_type; cout << ")"; cout << endl;
		}
	}

}

void db_metadata::show_all_records(int index, db_metadata dbm, int recursive_stop) {//le mando index - block size
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Objects
	table table_object2;
	int where = (int)db_file.tellg();
	//Create a varibale wich stores int,char and doubles sizes 
	int reg_fields_move = 0;//Move this times per reigstry

							//Enter block index
	db_file.seekg(index);
	where = (int)db_file.tellg();
	//Fill table object with data from file
	db_file.read(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
	db_file.read(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));

	for (int i = 0; i < table_object2.vector_size; i++)
	{
		field field_object;
		records_field reg_f;

		db_file.read(reinterpret_cast<char*>(&field_object), sizeof(field));
		table_object2.fields.push_back(field_object);
		where = (int)db_file.tellg();
		//Fill how many bytes will be necessary to move)-------------------------------------------------
		if (strcmp(field_object.field_type, "int") == 0) {
			reg_fields_move = reg_fields_move + 4;
		}
		else if (strcmp(field_object.field_type, "double") == 0) {
			reg_fields_move = reg_fields_move + 8;
		}
		else {
			reg_fields_move = reg_fields_move + atoi(field_object.field_type);
		}
		int n = (int)db_file.tellg();
		//Insert(beta)-------------------------------------------------
	}

	//Print whats inside table object
	if (recursive_stop == 0) {
		cout << "Table: ";
		cout << table_object2.table_name;
		cout << endl;
		cout << "---------------Showing records---------------------" << endl;
	}



	int integer_field;
	double dbl_field;
	char* varchar;

	//INSERT(BETA)-------------------------------------------------------
	int n = (int)db_file.tellg();
	records_metadata regdata;

	db_file.read(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));

	n = (int)db_file.tellg();
	n = (int)db_file.tellg();
	//If there is a records
	if (strcmp(regdata.records_separator, "records") == 0) {

		if (recursive_stop == 0) {
			cout << "Column name: ";
			for (int i = 0; i < table_object2.vector_size; i++)
			{
				cout << table_object2.fields[i].field_name; cout << "   ";
			}
		}
		cout << endl;
		for (int i = 0; i < regdata.records_count; i++)
		{
			for (int i = 0; i < table_object2.vector_size; i++)
			{
				n = (int)db_file.tellg();
				if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
					n = (int)db_file.tellg();
					db_file.read(reinterpret_cast<char*>(&integer_field), sizeof(int));
					n = (int)db_file.tellg();
					cout << ""; cout << integer_field; cout << "   ";
				}
				else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {
					db_file.read(reinterpret_cast<char*>(&dbl_field), sizeof(double));
					cout << ""; cout << dbl_field; cout << "   ";
				}
				else {
					varchar = new char[atoi(table_object2.fields[i].field_type)];
					db_file.read(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
					cout << ""; cout << varchar; cout << "   ";
				}

			}

			cout << endl;

		}


	}
	/*int tbl_index = get_table_index(dbm, table_object2.table_name);
	db_file.seekg(tbl_index - 8);
	block block_reader;
	db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));
	db_file.close();
	if (block_reader.next > 1 && recursive_stop == 0) {
		show_all_records((block_reader.next - dbm.block_size), dbm, 1);
	}
	return;*/
	where = (int)db_file.tellg();
	int tbl_index = index + dbm.block_size;
	db_file.seekg(tbl_index - 8);
	block block_reader;
	db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));
	where = (int)db_file.tellg();
	if (block_reader.next > 1) {
		show_all_records((block_reader.next - dbm.block_size), dbm, 1);
	}
	return;




}



//CORE FUNCTIONS---------------------------------------------------------------------
void db_metadata::create_database(string path, db_metadata dbm)
{
	system("CLS");
	db_name = path;
	//Create database file with given path
	ofstream database_file(db_name, ios::out | ios_base::binary);

	if (!database_file) {
		cout << "Problem with Binary file";
		return;
	}

	database_file.seekp(0 | ios::beg);
	for (int i = 0; i < dbm.cant_blocks; i++)
	{
		block block_data;
		block_data.is_empty = true;

		//Write dataabse metadata into file including the first block
		if (i == 0) {
			block_data.is_empty = false;
			remainingsize_length = dbm.block_size - 20;
			remaining_size = new char[remainingsize_length];

			database_file.write(reinterpret_cast<char*>(&dbm), sizeof(db_metadata));
			database_file.write(&remaining_size[0], sizeof(char)*remainingsize_length);
			database_file.write(reinterpret_cast<char*>(&block_data), sizeof(block));
		}
		//Make second block full because the system table goes here
		if (i == 1) {
			block_data.is_empty = false;
			remainingsize_length = dbm.block_size - 8;
			remaining_size = new char[remainingsize_length];
			database_file.write(&remaining_size[0], sizeof(char)*remainingsize_length);
			database_file.write(reinterpret_cast<char*>(&block_data), sizeof(block));

		}

		//Write the rest of the blocks into file
		if (i > 1) {
			remainingsize_length = dbm.block_size - 8;
			remaining_size = new char[remainingsize_length];
			database_file.write(&remaining_size[0], sizeof(char)*remainingsize_length);
			database_file.write(reinterpret_cast<char*>(&block_data), sizeof(block));

		}


	}
	//Write system table
	sys_table_metadata sysmeta;

	strcpy(sysmeta.table_name, "sql_master");
	sysmeta.table_count = 0;
	database_file.seekp(dbm.block_size);
	database_file.write(reinterpret_cast<char*>(&sysmeta), sizeof(sys_table_metadata));
	return;

}

void db_metadata::drop_database() {

	string path;

	cout << "Enter name of file to delete: ";
	cin >> path;
	if (remove(path.c_str()) != 0)
		cout << "Remove operation failed" << endl;
	else
		cout << path << " has been removed." << endl;

}

void db_metadata::create_table(db_metadata dbm)
{
	system("CLS");
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Resources
	char table_name_[20];

	//Objects
	block block_reader;
	table table_object;
	block_reader.is_empty = false;

	//Index to jump to the position in which there is free space
	//Multiply the index with the blocksize to obtain the position in bytes
	int free_block = get_freeblock_index(dbm)*dbm.block_size;
	int verify = get_freeblock_index(dbm);
	if (verify == false) {
		cout << "There is no more space available" << endl;
		return;
	}

	//The pointer is at the end of the block, so subtract 4 bytes to fill the "is_empty" integer;
	db_file.seekg((free_block - 4));
	db_file.write(reinterpret_cast<char*>(&block_reader.is_empty), sizeof(block_reader.is_empty));

	//Create the table
	cout << "Enter Table name: ";
	cin >> table_name_; cout << endl;

	strcpy(table_object.table_name, table_name_);

	//Create fields
	while (true) {

		//Objects
		field field_object;


		//resources
		int option = 0;
		char field_name_[10];
		char field_type_[10];


		cout << "->Add field name" << endl;
		cout << "Enter field name here: ";
		cin >> field_name_; cout << endl;
		strcpy(field_object.field_name, field_name_);

		cout << "->Add field type (int,char(1-4000),double)" << endl;
		cout << "Enter type here: ";
		cin >> field_type_; cout << endl;
		strcpy(field_object.field_type, field_type_);

		cout << "1) Add more or  2) Finish" << endl;
		cout << "Enter option here: ";
		cin >> option; cout << endl;

		table_object.fields.push_back(field_object);
		system("CLS");
		if (option == 2)
			break;
	}
	//Return to the start position of the block
	db_file.seekg((free_block - dbm.block_size));

	//Give vector size
	table_object.vector_size = table_object.fields.size();

	//Write the table name in file
	db_file.write(reinterpret_cast<char*>(&table_object.table_name), sizeof(char) * 20);
	db_file.write(reinterpret_cast<char*>(&table_object.vector_size), sizeof(int));

	//Write the fields from vector to file
	if (!table_object.fields.empty())
	{
		db_file.write(reinterpret_cast<char*>(&table_object.fields[0]), table_object.fields.size() * 20);
	}


	//Create system table field---------
	sys_table_metadata sysmeta;
	sys_table_field sysfield;

	//move to second block to fill systable with new created table
	db_file.seekg(dbm.block_size);
	db_file.read(reinterpret_cast<char*>(&sysmeta), sizeof(sys_table_metadata));
	int n = sysmeta.table_count;
	//changing
	db_file.seekg((int)db_file.tellg() - sizeof(sysmeta));
	sysmeta.table_count = sysmeta.table_count + 1;
	db_file.write(reinterpret_cast<char*>(&sysmeta), sizeof(sysmeta));

	if (sysmeta.table_count == 1)
	{
		sysfield.index = free_block;
		strcpy(sysfield.table_name, table_name_);
		db_file.write(reinterpret_cast<char*>(&sysfield), sizeof(sys_table_field));
	}
	else {
		db_file.ignore((sysmeta.table_count - 1) * 24);
		sysfield.index = free_block;
		strcpy(sysfield.table_name, table_name_);
		db_file.write(reinterpret_cast<char*>(&sysfield), sizeof(sys_table_field));


	}

	db_file.close();


	return;

}

void db_metadata::insert_records_part1(db_metadata dbm) {
	char table_name_[20];
	list_systables(dbm);
	cout << "Enter table name: ";
	cin >> table_name_; cout << endl;
	int index = get_table_index(dbm, table_name_);
	int full_index = (index - dbm.block_size);
	insert_records_part2(full_index, dbm);

}

void db_metadata::insert_records_part2(int index, db_metadata dbm) {//Given the table FULL index (it means the bytes traveled until the point where it starts)

	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Objects
	table table_object2;
	int where = (int)db_file.tellg();
	//Create a varibale wich stores int,char and doubles sizes 
	int reg_fields_move = 0;//Move this times per record

							//Enter block index
	db_file.seekg(index);

	//Fill table object with data from file
	db_file.read(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
	db_file.read(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));
	where = (int)db_file.tellg();
	for (int i = 0; i < table_object2.vector_size; i++)
	{
		field field_object;
		records_field reg_f;

		db_file.read(reinterpret_cast<char*>(&field_object), sizeof(field));
		table_object2.fields.push_back(field_object);

		//Fill how many bytes will be necessary to move)-------------------------------------------------
		if (strcmp(field_object.field_type, "int") == 0) {
			reg_fields_move = reg_fields_move + 4;
		}
		else if (strcmp(field_object.field_type, "double") == 0) {
			reg_fields_move = reg_fields_move + 8;
		}
		else {
			reg_fields_move = reg_fields_move + atoi(field_object.field_type);
		}

		//Insert(beta)-------------------------------------------------
	}

	//Print whats inside table object
	cout << "Table: ";
	cout << table_object2.table_name;
	cout << "  Number of vector fields: ";
	cout << table_object2.vector_size << endl;
	cout << "---------------Insert records---------------------" << endl;

	where = (int)db_file.tellg();
	//INSERT(BETA)-------------------------------------------------------
	records_metadata regdata;
	int n = (int)db_file.tellg();
	int pos_now = (int)db_file.tellg();//pos befor regdata
	db_file.read(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));

	//if has next pointer--------------------
	//Current block index
	int right_now_pos = db_file.tellg();
	int has_next = 0;
	int this_block_index = index + dbm.block_size;//this index is the full one
	//Move to next pointer

	db_file.seekg(this_block_index - 8);
	db_file.read(reinterpret_cast<char*>(&has_next), sizeof(int));

	n = (int)db_file.tellg();

	int integer_field = 0;
	double dbl_field;
	char* varchar;


	//If there is a record and has no pointer and needs new block
	if (strcmp(regdata.records_separator, "records") == 0 && has_next < 1) {
		//Verify if there is space in block;
		int r_size = dbm.block_size - 8;
		db_file.seekg(right_now_pos);//niu
		if (regdata.records_count == 1) {
			db_file.seekg((int)db_file.tellg()+(regdata.records_count *reg_fields_move));
			where = (int)db_file.tellg();
		}
		else {
			db_file.seekg((int)db_file.tellg()+((regdata.records_count - 1)*reg_fields_move));
		}

		//If the block is full
		where = (int)db_file.tellg();
		//db_file.seekg(right_now_pos);
		if ((int)db_file.tellg() + reg_fields_move > ((index + dbm.block_size) - 8)) {//RESOLVE ESTOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
			where = (int)db_file.tellg();
			cout << "The block is full, the data will be stored in another block, if availabe" << endl;
			system("pause");
			cout << endl;
			cout << "Checking if there is a free block available" << endl;

			//obtain free block
			int f_block = get_freeblock_index(dbm);
			//If there is no free block
			if (f_block == 0) {
				cout << "NO MORE SPACE AVAILABLE" << endl;
				return;
			}
			else {//If there is a free block
				//Current block index
				cout << "Free block found" << endl;
				cout << endl;
				int this_block_index = index + dbm.block_size;//this index is the full one
				int new_block_full_index = f_block * dbm.block_size;
				//Move to next pointer
				db_file.seekg(this_block_index - 8);

				db_file.write(reinterpret_cast<char*>(&new_block_full_index), sizeof(int));
				where = (int)db_file.tellg();
				//Move to new block
				db_file.seekg((f_block*dbm.block_size) - 4);//Im at the last pos of the new block
				block block_reader;
				//Set new block space to used
				block_reader.is_empty = false;
				db_file.write(reinterpret_cast<char*>(&block_reader.is_empty), sizeof(int));
				int num = (int)db_file.tellg();
				//Go to the start of the new block
				db_file.seekg(new_block_full_index - dbm.block_size);
				//Write the table name and vector size from old block into the new block
				db_file.write(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
				db_file.write(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));
				int num2 = (int)db_file.tellg();
				//Write the fields from vector of old block into the new block
				if (!table_object2.fields.empty())
				{
					db_file.write(reinterpret_cast<char*>(&table_object2.fields[0]), table_object2.fields.size() * 20);
				}

				//Write records into block
				strcpy(regdata.records_separator, "records");
				regdata.records_count = 1;
				db_file.write(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));

				for (int i = 0; i < table_object2.vector_size; i++)
				{


					if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
						cout << "Column name: ";
						cout << table_object2.fields[i].field_name; cout << " ";
						cout << table_object2.fields[i].field_type; cout << endl;


						cout << "Enter record: ";
						cin >> integer_field; cout << endl;
						db_file.write(reinterpret_cast<char*>(&integer_field), sizeof(int));
					}
					else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {
						cout << "Column name: ";
						cout << table_object2.fields[i].field_name; cout << " ";
						cout << table_object2.fields[i].field_type; cout << endl;

						cout << "Enter record: ";
						cin >> dbl_field; cout << endl;
						db_file.write(reinterpret_cast<char*>(&dbl_field), sizeof(double));
					}

					else {
						cout << "Column name: ";
						cout << table_object2.fields[i].field_name; cout << " ";
						cout << " Char "; cout << table_object2.fields[i].field_type; cout << endl;


						varchar = new char[atoi(table_object2.fields[i].field_type)];
						cout << "Enter record: ";
						cin >> varchar;
						int n =(int)db_file.tellg();
						n = (int)db_file.tellg();
						db_file.write(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
					}
					int num3 = (int)db_file.tellg();
				}

			}

		}
		else {//If the block is not full and dont newwd to create new block
			n = (int)db_file.tellg();
			db_file.seekg(pos_now);//pos now is the position before reading record metadata
			n = (int)db_file.tellg();
			strcpy(regdata.records_separator, "records");
			regdata.records_count = regdata.records_count + 1;
			db_file.write(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));
			n = (int)db_file.tellg();
			if (regdata.records_count == 1) {
				db_file.seekg((int)db_file.tellg()+regdata.records_count *reg_fields_move);
				where = (int)db_file.tellg();
			}
			else {
				db_file.seekg((int)db_file.tellg()+(regdata.records_count - 1)*reg_fields_move);
			}

			for (int i = 0; i < table_object2.vector_size; i++)
			{
				if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
					db_file.read(reinterpret_cast<char*>(&integer_field), sizeof(int));
					cout << "Column name: ";
					cout << table_object2.fields[i].field_name; cout << " ";
					cout << table_object2.fields[i].field_type; cout << endl;


					cout << "Enter record: ";
					cin >> integer_field; cout << endl;
					db_file.seekg((int)db_file.tellg() - sizeof(int));
					db_file.write(reinterpret_cast<char*>(&integer_field), sizeof(int));
				}
				else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {
					db_file.read(reinterpret_cast<char*>(&dbl_field), sizeof(double));
					cout << "Column name: ";
					cout << table_object2.fields[i].field_name; cout << " ";
					cout << table_object2.fields[i].field_type; cout << endl;

					cout << "Enter record: ";
					cin >> dbl_field; cout << endl;
					db_file.seekg((int)db_file.tellg() - sizeof(double));
					db_file.write(reinterpret_cast<char*>(&dbl_field), sizeof(double));
				}

				else {
					int n = (int)db_file.tellg();
					varchar = new char[atoi(table_object2.fields[i].field_type)];
					db_file.read(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
					cout << "Column name: ";
					cout << table_object2.fields[i].field_name; cout << " ";
					cout << " Char "; cout << table_object2.fields[i].field_type; cout << endl;

					n=(int)db_file.tellg();
					varchar = new char[atoi(table_object2.fields[i].field_type)];
					cout << "Enter record: ";
					cin >> varchar;
					db_file.seekg((int)db_file.tellg() - sizeof(char)*atoi(table_object2.fields[i].field_type));
				
					n=(int)db_file.tellg();
					n = (int)db_file.tellg();
					db_file.write(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
				}




			}
		}


	}

	else if (strcmp(regdata.records_separator, "records") == 0 && has_next > 1) {//has a pointer to other block
		insert_records_part2(has_next - dbm.block_size, dbm);


	}
	//If there are no records
	else {
		n = (int)db_file.tellg();
		db_file.seekg(pos_now);//pos now is the position before reading record metadata
		n = (int)db_file.tellg();
		strcpy(regdata.records_separator, "records");
		regdata.records_count = 1;
		db_file.write(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));
		n = (int)db_file.tellg();

		for (int i = 0; i < table_object2.vector_size; i++)
		{
			if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
				cout << "Column name: ";
				cout << table_object2.fields[i].field_name; cout << " ";
				cout << table_object2.fields[i].field_type; cout << endl;


				cout << "Enter record: ";
				cin >> integer_field; cout << endl;
				db_file.write(reinterpret_cast<char*>(&integer_field), sizeof(int));
			}
			else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {
				cout << "Column name: ";
				cout << table_object2.fields[i].field_name; cout << " ";
				cout << table_object2.fields[i].field_type; cout << endl;

				cout << "Enter record: ";
				cin >> dbl_field; cout << endl;
				db_file.write(reinterpret_cast<char*>(&dbl_field), sizeof(double));
			}

			else {
				cout << "Column name: ";
				cout << table_object2.fields[i].field_name; cout << " ";
				cout << " Char "; cout << table_object2.fields[i].field_type; cout << endl;


				varchar = new char[atoi(table_object2.fields[i].field_type)];
				cout << "Enter record: ";
				cin >> varchar;
				db_file.write(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
			}




		}

	}


}

void db_metadata::drop_table(db_metadata dbm) {

	char table_name_[20];
	list_systables(dbm);
	cout << "Enter table name: ";
	cin >> table_name_; cout << endl;
	int table_index = ((get_table_index(dbm, table_name_)));
	drop_table_helper(table_index, dbm, table_name_, 0);

}
void db_metadata::drop_table_helper(int table_index, db_metadata dbm, char table_name_[20], int recursive_verify) {
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);

	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}
	block block_reader;
	db_file.seekg((table_index - 8));
	db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));
	if (block_reader.next > 1) {
		drop_table_helper((block_reader.next), dbm, table_name_, 1);
	}


	char*r_size = new char[dbm.block_size - 8];
	db_file.seekg((int)db_file.tellg() - dbm.block_size);
	db_file.write(reinterpret_cast<char*>(&r_size[0]), sizeof(char)*dbm.block_size - 8);
	block block_reader2;
	block_reader2.is_empty = true;
	db_file.write(reinterpret_cast<char*>(&block_reader2), sizeof(block));

	if (recursive_verify == 0) {
		////
		sys_table_metadata sysmeta;
		sys_table_field sysfield;

		//move to second block to fill systable with new created table
		db_file.seekg(dbm.block_size);
		db_file.ignore(20);
		db_file.read(reinterpret_cast<char*>(&sysmeta.table_count), sizeof(int));

		vector<sys_table_field>sys_vector;
		for (int i = 0; i < sysmeta.table_count; i++)
		{//Delete from system table
			sys_table_field sysfield;
			db_file.read(reinterpret_cast<char*>(&sysfield), sizeof(sys_table_field));
			if (strcmp(sysfield.table_name, table_name_) == 0 && i != sysmeta.table_count) {//If table is found
				db_file.seekg((int)db_file.tellg() - sizeof(sysfield));
				strcpy(sysfield.table_name, "Deleted");
				sysfield.index = -1;
				db_file.write(reinterpret_cast<char*>(&sysfield), sizeof(sys_table_field));
				return;
			}
		}
	}




}

void db_metadata::delete_record(db_metadata dbm) {
	system("CLS");
	char tabl_name[20];
	list_systables(dbm);
	cout << "Enter table name: ";
	cin >> tabl_name;
	int index = get_table_index(dbm, tabl_name);
	int full_index = (index - dbm.block_size);
	char chosen_column[10];
	show_table_columns(index - dbm.block_size);
	cout << endl;
	cout << "Enter the column where the record is located" << endl;
	cin >> chosen_column; cout << endl;
	delete_record_helper(full_index, dbm, chosen_column);
}

void db_metadata::delete_record_helper(int index, db_metadata dbm, char chosen_column[10]) {
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Objects
	table table_object2;

	//Create a varibale wich stores int,char and doubles sizes 
	int reg_fields_move = 0;//Move this times per reigstry

							//Enter block index
	db_file.seekg(index);

	//Fill table object with data from file
	db_file.read(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
	db_file.read(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));

	for (int i = 0; i < table_object2.vector_size; i++)
	{
		field field_object;
		records_field reg_f;

		db_file.read(reinterpret_cast<char*>(&field_object), sizeof(field));
		table_object2.fields.push_back(field_object);

		//Fill how many bytes will be necessary to move)-------------------------------------------------
		if (strcmp(field_object.field_type, "int") == 0) {
			reg_fields_move = reg_fields_move + 4;
		}
		else if (strcmp(field_object.field_type, "double") == 0) {
			reg_fields_move = reg_fields_move + 8;
		}
		else {
			reg_fields_move = reg_fields_move + atoi(field_object.field_type);
		}
		int n = (int)db_file.tellg();
		//Insert(beta)-------------------------------------------------
	}

	//Print whats inside table object



	//INSERT(BETA)-------------------------------------------------------
	int n = (int)db_file.tellg();
	records_metadata regdata;

	db_file.read(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));

	n = (int)db_file.tellg();
	int integer_field;
	double dbl_field;
	char* varchar;

	int option = 0;
	if (strcmp(regdata.records_separator, "records") == 0) {
		cout << endl;
		for (int i = 0; i < regdata.records_count; i++)
		{

			for (int i = 0; i < table_object2.vector_size; i++)
			{
				
				system("CLS");
				if (strcmp(chosen_column, table_object2.fields[i].field_name) == 0) {
					if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
						db_file.read(reinterpret_cast<char*>(&integer_field), sizeof(int));
						cout << "Record found : "; cout << integer_field; cout << " "; cout << table_object2.fields[i].field_type; cout << endl;

						cout << "Delete this record?  1)yes 2)no" << endl;
						cin >> option;
						if (option == 1) {
							db_file.seekg((int)db_file.tellg() - sizeof(int));
							int integer_field2=000;
							db_file.write(reinterpret_cast<char*>(&integer_field2), sizeof(int));
							cout << "Deleted" << endl;
						}


					}
					else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {

						db_file.read(reinterpret_cast<char*>(&dbl_field), sizeof(double));
						cout << "Record: "; cout << dbl_field; cout << " "; cout << table_object2.fields[i].field_type;  cout << endl;

						cout << "Delete this record?  1)yes 2)no" << endl;
						cin >> option;
						if (option == 1) {
							db_file.seekg((int)db_file.tellg() - sizeof(double));
							double dbl_field2=000;
							db_file.write(reinterpret_cast<char*>(&dbl_field2), sizeof(double));
							cout << "Deleted" << endl;
						}
					}

					else {
						varchar = new char[atoi(table_object2.fields[i].field_type)];
						db_file.read(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
						cout << "Record: "; cout << varchar; cout << " Char("; cout << table_object2.fields[i].field_type; cout << ")"; cout << endl;

						cout << "Delete this record?  1)yes 2)no" << endl;
						cin >> option;
						if (option == 1) {
							db_file.seekg((int)db_file.tellg() - sizeof(char)*atoi(table_object2.fields[i].field_type));
							char* varchar2 = new char[atoi(table_object2.fields[i].field_type)];
							strcpy(varchar2, "000");
							db_file.write(reinterpret_cast<char*>(&varchar2[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
							cout << "Deleted" << endl;
						}
					}


				}

				cout << endl;

			}

		}

	}
	int tbl_index = index + dbm.block_size;
	db_file.seekg(tbl_index - 8);
	block block_reader;
	db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));
	db_file.close();
	if (block_reader.next > 1) {
		delete_record_helper((block_reader.next - dbm.block_size), dbm, chosen_column);
	}
	return;



}

void db_metadata::update_record(db_metadata dbm) {
	system("CLS");
	char tabl_name[20];
	list_systables(dbm);
	cout << "Enter table name: ";
	cin >> tabl_name;
	int index = get_table_index(dbm, tabl_name);
	int full_index = (index - dbm.block_size);
	char chosen_column[10];
	show_table_columns(index - dbm.block_size);
	cout << endl;
	cout << "Enter the column you want to update" << endl;
	cin >> chosen_column; cout << endl;
	update_record_helper(full_index, dbm, chosen_column);
}

void db_metadata::update_record_helper(int index, db_metadata dbm, char chosen_column[10]) {
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Objects
	table table_object2;

	//Create a varibale wich stores int,char and doubles sizes 
	int reg_fields_move = 0;//Move this times per reigstry

							//Enter block index
	db_file.seekg(index);

	//Fill table object with data from file
	db_file.read(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
	db_file.read(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));

	for (int i = 0; i < table_object2.vector_size; i++)
	{
		field field_object;
		records_field reg_f;

		db_file.read(reinterpret_cast<char*>(&field_object), sizeof(field));
		table_object2.fields.push_back(field_object);

		//Fill how many bytes will be necessary to move)-------------------------------------------------
		if (strcmp(field_object.field_type, "int") == 0) {
			reg_fields_move = reg_fields_move + 4;
		}
		else if (strcmp(field_object.field_type, "double") == 0) {
			reg_fields_move = reg_fields_move + 8;
		}
		else {
			reg_fields_move = reg_fields_move + atoi(field_object.field_type);
		}
		int n = (int)db_file.tellg();
		//Insert(beta)-------------------------------------------------
	}

	//Print whats inside table object

	int integer_field;
	double dbl_field;
	char* varchar;


	//INSERT(BETA)-------------------------------------------------------
	int n = (int)db_file.tellg();
	records_metadata regdata;

	db_file.read(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));

	n = (int)db_file.tellg();


	int option = 0;
	if (strcmp(regdata.records_separator, "records") == 0) {
		cout << endl;
		for (int i = 0; i < regdata.records_count; i++)
		{

			for (int i = 0; i < table_object2.vector_size; i++)
			{
				system("CLS");
				if (strcmp(chosen_column, table_object2.fields[i].field_name) == 0) {
					if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
						db_file.read(reinterpret_cast<char*>(&integer_field), sizeof(int));
						cout << "Record found : "; cout << integer_field; cout << " "; cout << table_object2.fields[i].field_type; cout << endl;

						cout << "Update this record?  1)yes 2)no" << endl;
						cin >> option;
						if (option == 1) {
							db_file.seekg((int)db_file.tellg() - sizeof(int));
							cout << "Enter new record: ";
							cin >> integer_field; cout << endl;
							db_file.write(reinterpret_cast<char*>(&integer_field), sizeof(int));
							cout << "Updated" << endl;
						}


					}
					else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {

						db_file.read(reinterpret_cast<char*>(&dbl_field), sizeof(double));
						cout << "Record: "; cout << dbl_field; cout << " "; cout << table_object2.fields[i].field_type;  cout << endl;

						cout << "Update this record?  1)yes 2)no" << endl;
						cin >> option;
						if (option == 1) {
							db_file.seekg((int)db_file.tellg() - sizeof(double));
							cout << "Enter new record: ";
							cin >> dbl_field; cout << endl;
							db_file.write(reinterpret_cast<char*>(&dbl_field), sizeof(double));
							cout << "Updated" << endl;
						}
					}

					else {
						varchar = new char[atoi(table_object2.fields[i].field_type)];
						db_file.read(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
						cout << "Record: "; cout << varchar; cout << " Char("; cout << table_object2.fields[i].field_type; cout << ")"; cout << endl;

						cout << "Update this record?  1)yes 2)no" << endl;
						cin >> option;
						if (option == 1) {
							db_file.seekg((int)db_file.tellg() - sizeof(char)*atoi(table_object2.fields[i].field_type));
							cout << "Enter new record: ";
							cin >> varchar; cout << endl;
							db_file.write(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
							cout << "Updated" << endl;
						}
					}


				}

				cout << endl;

			}

		}

	}
	int tbl_index = index + dbm.block_size;
	db_file.seekg(tbl_index - 8);
	block block_reader;
	db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));
	db_file.close();
	if (block_reader.next > 1) {
		update_record_helper((block_reader.next - dbm.block_size), dbm, chosen_column);
	}
	return;


}


void db_metadata::select_specific_column(db_metadata dbm) {
	system("CLS");
	char tabl_name[20];
	list_systables(dbm);
	cout << "Enter table name: ";
	cin >> tabl_name;
	int index = get_table_index(dbm, tabl_name);
	int full_index = (index - dbm.block_size);
	char chosen_column[10];
	show_table_columns(full_index);
	cout << endl;
	cout << "Enter the column you want to select" << endl;
	cin >> chosen_column; cout << endl;
	select_specific_column_helper(full_index, dbm, chosen_column);

}
void db_metadata::select_specific_column_helper(int index, db_metadata dbm, char chosen_column[10]) {
	fstream db_file(db_name, ios_base::in | ios_base::out | ios_base::binary);
	if (!db_file) {
		cout << "Problem with Binary file";
		return;
	}

	//Objects
	table table_object2;

	//Create a varibale wich stores int,char and doubles sizes 
	int reg_fields_move = 0;//Move this times per reigstry

							//Enter block index
	db_file.seekg(index);

	//Fill table object with data from file
	db_file.read(reinterpret_cast<char*>(&table_object2.table_name), sizeof(char) * 20);
	db_file.read(reinterpret_cast<char*>(&table_object2.vector_size), sizeof(int));

	for (int i = 0; i < table_object2.vector_size; i++)
	{
		field field_object;
		records_field reg_f;

		db_file.read(reinterpret_cast<char*>(&field_object), sizeof(field));
		table_object2.fields.push_back(field_object);

		//Fill how many bytes will be necessary to move)-------------------------------------------------
		if (strcmp(field_object.field_type, "int") == 0) {
			reg_fields_move = reg_fields_move + 4;
		}
		else if (strcmp(field_object.field_type, "double") == 0) {
			reg_fields_move = reg_fields_move + 8;
		}
		else {
			reg_fields_move = reg_fields_move + atoi(field_object.field_type);
		}
		int n = (int)db_file.tellg();
		//Insert(beta)-------------------------------------------------
	}

	//Print whats inside table object

	int integer_field;
	double dbl_field;
	char* varchar;


	//INSERT(BETA)-------------------------------------------------------
	int n = (int)db_file.tellg();
	records_metadata regdata;

	db_file.read(reinterpret_cast<char*>(&regdata), sizeof(records_metadata));

	n = (int)db_file.tellg();


	int option = 0;
	if (strcmp(regdata.records_separator, "records") == 0) {
		cout << endl;
		cout<<"Column name: ";cout << chosen_column << endl;
		cout<<">Records"<< endl;
		for (int i = 0; i < regdata.records_count; i++)
		{

			for (int i = 0; i < table_object2.vector_size; i++)
			{
				
				
					if (strcmp(table_object2.fields[i].field_type, "int") == 0) {
						
							db_file.read(reinterpret_cast<char*>(&integer_field), sizeof(int));
							if (strcmp(chosen_column, table_object2.fields[i].field_name) == 0) {
								cout << integer_field; cout << endl;
							}
						
					}
					else if (strcmp(table_object2.fields[i].field_type, "double") == 0) {
					
							db_file.read(reinterpret_cast<char*>(&dbl_field), sizeof(double));
							if (strcmp(chosen_column, table_object2.fields[i].field_name) == 0) {
								cout << integer_field; cout << endl;
							}
						
					}

					else {

						varchar = new char[atoi(table_object2.fields[i].field_type)];
						db_file.read(reinterpret_cast<char*>(&varchar[0]), sizeof(char)*atoi(table_object2.fields[i].field_type));
						if (strcmp(chosen_column, table_object2.fields[i].field_name) == 0) {
							cout << varchar; cout << endl;
						}


					}


				

				

			}

		}
		int tbl_index = index + dbm.block_size;
		db_file.seekg(tbl_index - 8);
		block block_reader;
		db_file.read(reinterpret_cast<char*>(&block_reader), sizeof(block));
		db_file.close();
		if (block_reader.next > 1) {
			update_record_helper((block_reader.next - dbm.block_size), dbm, chosen_column);
		}
		return;

	}



}