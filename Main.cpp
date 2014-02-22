#include <iostream>
#include <string>
#include "SkipList.hpp"

using namespace std;
using namespace cs540;

int random_int()
{
	return rand() % 50000;
}

int main()
{

	srand(time(NULL));
	SkipList<int, int, 8> mySL;

	for(int i = 0; i < 100; i++)
	{
		int idx = random_int();
		pair<const int, int> val_to_insert(idx, i*3);
		mySL.insert(val_to_insert);
	}
	cout << "List size: " << mySL.size() << endl;

	mySL.clear();
	cout << "List size: " << mySL.size() << endl;

	for(int i = 0; i < 100; i++)
	{
		int idx = random_int();
		pair<const int, int> val_to_insert(idx, i+4);
		mySL.insert(val_to_insert);
	}
	cout << "List size: " << mySL.size() << endl;

	while(mySL.size() > 0)
	{
		mySL.erase(mySL.begin());
		cout << mySL.size() << endl;
	}
	cout << "List size: " << mySL.size() << endl;

	for(int i = 0; i < 100; i++)
	{
		int idx = random_int();
		pair<const int, int> val_to_insert(idx, idx);
		mySL.insert(val_to_insert);
	}
	cout << "List size: " << mySL.size() << endl;

	return 0;
}
