#include "Data_structure.h"
using namespace std;




int main() {

	db_metadata functions;

	//Resources
	string name;
	int db_size;
	int block_size;
	int menu_option;
	int x = 1;

	do {
		system("CLS");
		cout << endl;
		cout << "1) Create Database" << endl;
		cout << "2) Open Database" << endl;
		cout << "3) Drop Database" << endl;
		cout << "Enter option here:";
		cin >> menu_option;

		if (menu_option == 1) {
			cout << "--------------Create Database-----------" << endl;
			cout << "Enter Name: ";
			cin >> name;
			cout << "Enter database size in mb: ";
			cin >> db_size;
			cout << "Enter Block size in bytes (512-8000): ";
			cin >> block_size;

			//Create db_metadata structure object
			db_metadata dbm(db_size, block_size);

			//cout<<(int)n_block.remaining_size;
			functions.create_database(name, dbm);
			do {
				system("CLS");
				cout << "------------------------MENU------------------------" << endl;
				cout << "1) Create Table" << endl << endl;
				cout << "2) Drop Table" << endl << endl;
				cout << "3) Insert record" << endl << endl;
				cout << "4) Delete record" << endl << endl;
				cout << "5) Update record" << endl << endl;
				cout << "6) Select" << endl << endl;
				cout << "7) Exit" << endl << endl;
				cout << "Enter option here:";
				cin >> menu_option;
				switch (menu_option)
				{
				case 1:
					system("CLS");
					functions.create_table(dbm);
					system("pause");
					break;
				case 2:
					system("CLS");
					functions.drop_table(dbm);
					break;
				case 3:
					system("CLS");
					functions.insert_records_part1(dbm);
					system("pause");
					break;
				case 4:
					system("CLS");
					functions.delete_record(dbm);
					system("pause");
					break;
				case 5:
					system("CLS");
					functions.update_record(dbm);

					system("pause");
					break;


				case 6:
					system("CLS");
					functions.select(dbm);
					system("pause");
					break;

				case 7:
					cout << "---Exit---" << endl;
					return 0;
					break;
				}


			} while (true);

		}
		else if (menu_option == 2) {

			db_metadata dbm;

			cout << "--------------Open Database-----------" << endl;
			cout << "Enter Name: ";
			cin >> name;
			functions.open_database(name);
			dbm = functions.fill_dbm(name, dbm);
			do {
				system("CLS");
				cout << "------------------------MENU------------------------" << endl;
				cout << "1) Create Table" << endl << endl;
				cout << "2) Drop Table" << endl << endl;
				cout << "3) Insert record" << endl << endl;
				cout << "4) Delete record" << endl << endl;
				cout << "5) Update record" << endl << endl;
				cout << "6) Select " << endl << endl;
				cout << "7) Exit" << endl << endl;
				cout << "Enter option here:";
				cin >> menu_option;
				switch (menu_option)
				{
				case 1:
					system("CLS");
					functions.create_table(dbm);


					break;
				case 2:
					system("CLS");
					functions.drop_table(dbm);
					break;
				case 3:
					system("CLS");
					functions.insert_records_part1(dbm);
					break;
				case 4:
					system("CLS");
					functions.delete_record(dbm);
					system("pause");
					break;
				case 5:
					system("CLS");
					functions.update_record(dbm);

					system("pause");
					break;


				case 6:
					system("CLS");
					functions.select(dbm);
					system("pause");
					break;

				case 7:
					cout << "---Exit---" << endl;
					return 0;
					break;
				}
			} while (true);
		}
		else if (menu_option == 3) {
			functions.drop_database();

		}
		else {
			cout << "Enter a available option" << endl;

		}



		system("pause");

	}while (true);


};