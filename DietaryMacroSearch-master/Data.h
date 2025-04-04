#pragma once
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <regex>
using namespace std;

class Data {
	ifstream file;
	vector<string> macros; // name of each macro (corresponds accrodingly to float vector's index)
    // nutritien name, protein, fat, etc
	map<string, vector<float>> ingredients; // name of ingredient, vector of amount of macro
    // ingredient name : nutrient value
public:
	Data() {}
	Data(string filename);
	void organizeData();
	vector<pair<string, float>> heapSort(string chosenMacro, string noMacro);
	vector<pair<string, float>> mergeSort(string chosenMacro, string noMacro);
	void mergeSortHelper(vector<pair<string, float>>& ingredientChosenMacroPairsVec, int left, int right);
	void merge(vector<pair<string, float>>& ingredientChosenMacroPairsVec, int left, int mid, int right);
	const vector<string>& getMacros() const;
};

Data::Data(string filename) {
	file.open(filename, ios::in);
	if (!file.is_open()) {
		cout << "File not open!" << endl;
	}
}

void Data::organizeData() {
	string temp, ingredientCode, ingredientDescrip, nutrientCode;
	string nutrientDescrip, nutrientVal, nutrientValSource, fdcID;
	string derivationCode, sfAddModYear, foundationYearAcq;
	regex letters = regex("[A-Za-z]");

	//reads first 3 lines
	getline(file, temp);
    
	getline(file, temp);
    
	getline(file, temp);
    
    
	//reads all ingredients & macro vals into map
	// for (int i = 0; i < 122330; i++) {
    for (int i = 0; i < 1000; i++) {
        
		getline(file, ingredientCode, ',');
        
		getline(file, ingredientDescrip, ',');
        
		getline(file, nutrientCode, ',');
       
		while (regex_search(nutrientCode, letters)) {
			ingredientDescrip += nutrientCode;
            
			getline(file, nutrientCode, ',');

		}
        
		getline(file, nutrientDescrip, ',');
		getline(file, nutrientVal, ',');
		while (regex_search(nutrientVal, letters)) {
			nutrientDescrip += nutrientVal;
			getline(file, nutrientVal, ',');
		}
		getline(file, nutrientValSource, ',');
		getline(file, fdcID, ',');
		getline(file, derivationCode, ',');
		getline(file, sfAddModYear, ',');
		getline(file, foundationYearAcq);

		ingredientDescrip = ingredientDescrip.substr(1, ingredientDescrip.length() - 2);
        // remove the quotes
        
		nutrientDescrip = nutrientDescrip.substr(1, nutrientDescrip.length() - 2);
		nutrientVal = nutrientVal.substr(1, nutrientVal.length() - 2);
        
		//65 distrinct macros considered for each ingredient
		if (macros.size() < 65) {
            
			macros.push_back(nutrientDescrip);
		}
		ingredients[ingredientDescrip].push_back(stof(nutrientVal));
	}
}

const vector<string>& Data::getMacros() const {
	return macros;
}

vector<pair<string, float>> Data::heapSort(string chosenMacro, string noMacro = "None") {
	string* heap{ new string[1882] };
	int size = 0;
	int indexBuild = 0;
	int parent = floor((indexBuild - 1) / 2.0);
	int indexMacro = find(macros.begin(), macros.end(), chosenMacro) - macros.begin();
    
	//build and insert into heap
	for (auto i : ingredients) {
		indexBuild = size;
		heap[indexBuild] = i.first;
		parent = floor((indexBuild - 1) / 2.0);

		while (parent > -1 && ingredients[heap[indexBuild]][indexMacro] > ingredients[heap[parent]][indexMacro]) {
			swap(heap[parent], heap[indexBuild]);
			indexBuild = parent;
			parent = floor((indexBuild - 1) / 2.0);
		}
		size++;
	}

	vector<pair<string, float>> result;
	int indexExtract = 0;
	int left = (2 * indexExtract) + 1;
	int right = (2 * indexExtract) + 2;
	int indexNoMacro;
	noMacro == "None" ? indexNoMacro = -1 : indexNoMacro = find(macros.begin(), macros.end(), noMacro) - macros.begin();

	//extract max from heap
	while (size > -1) {
		if (noMacro == "None" || ingredients[heap[0]][indexNoMacro] == 0.00) {
			result.push_back(make_pair(heap[0], ingredients[heap[0]][indexMacro]));
		}
		if (size == 0) { break; }

		indexExtract = 0, left = 1, right = 2;
		heap[indexExtract] = heap[size - 1];
		size--;

		while (right < size && (ingredients[heap[indexExtract]][indexMacro] < ingredients[heap[left]][indexMacro] || ingredients[heap[indexExtract]][indexMacro] < ingredients[heap[right]][indexMacro])) {
			if (ingredients[heap[left]][indexMacro] > ingredients[heap[right]][indexMacro]) {
				swap(heap[left], heap[indexExtract]);
				indexExtract = left;
			}
			else {
				swap(heap[right], heap[indexExtract]);
				indexExtract = right;
			}
			left = (2 * indexExtract) + 1;
			right = (2 * indexExtract) + 2;
		}
		if (right == size && ingredients[heap[indexExtract]][indexMacro] < ingredients[heap[left]][indexMacro]) {
			swap(heap[left], heap[indexExtract]);
		}
	}
	return result;
}

// Sorts the ingredients with merge sort based on how much of each ingredient is wanted (and which one is unwanted)
vector<pair<string, float>> Data::mergeSort(string chosenMacro, string noMacro = "None") {
	int indexMacro = find(macros.begin(), macros.end(), chosenMacro) - macros.begin(); // finds index of the chosen macro
	vector<pair<string, float>> ingredientChosenMacroPairsVec; // vector of pairs that contain each ingredient and the amount of the chosen macro that they contain

	int indexUnwantedMacro;
	noMacro == "None" ? indexUnwantedMacro = -1 : indexUnwantedMacro = find(macros.begin(), macros.end(), noMacro) - macros.begin(); // finds the index of the unwanted ingredient, if there is one chosen

	// iterates through all the ingredients and assigns a pair of the ingredient name and how much of the chosen macro there is contained, and pushes this pair to a vector
	for (const auto& i : ingredients) {
		float macroValue = ingredients[i.first][indexMacro];
		if (indexUnwantedMacro != -1) { // If there is an unwanted macro in an ingredient, then if only pushes that ingredient DOES NOT contain the unwanted macro
			float unwantedMacroValue = ingredients[i.first][indexUnwantedMacro];
			if (unwantedMacroValue == 0.f) {
				ingredientChosenMacroPairsVec.push_back({ i.first, macroValue });
			}
		}
		else {
			ingredientChosenMacroPairsVec.push_back({ i.first, macroValue });
		}
	}

	int left = 0;
	int right = ingredientChosenMacroPairsVec.size() - 1;

	// Calls the actual sorting mergesort algorithm 
	mergeSortHelper(ingredientChosenMacroPairsVec, left, right);
	return ingredientChosenMacroPairsVec;
}

// Mergesort code (with recursion). NOTE: BASED OFF THE PROFESSOR'S MERGE SORT CODE FROM THE MODULE 6 SLIDES
void Data::mergeSortHelper(vector<pair<string, float>>& ingredientChosenMacroPairsVec, int left, int right) {

	// Check if the cur segment of the vector > 1 element
	if (left < right) {
		int mid = left + (right - left) / 2; // Splits the vector into 2 at "mid" index
		mergeSortHelper(ingredientChosenMacroPairsVec, left, mid); // recursivily mergesorts the left subvector
		mergeSortHelper(ingredientChosenMacroPairsVec, mid + 1, right); // recursivily mergesorts the right sub vector
		merge(ingredientChosenMacroPairsVec, left, mid, right); //now the subvectors are sorted so we merge them
	}
}

// code for merging the subvectors  NOTE: BASED OFF THE PROFESSOR'S MERGE SORT CODE FROM THE MODULE 6 SLIDES
void Data::merge(vector<pair<string, float>>& ingredientChosenMacroPairsVec, int left, int mid, int right) {

	int len1 = mid - left + 1;
	int len2 = right - mid;

	// create sub vectors
	vector<pair<string, float>> leftSubvector(len1);
	vector<pair<string, float>> rightSubvector(len2);

	// transfer the data to the subvectors
	for (int i = 0; i < len1; i++) {
		leftSubvector[i] = ingredientChosenMacroPairsVec[left + i];
	}
	for (int j = 0; j < len2; j++) {
		rightSubvector[j] = ingredientChosenMacroPairsVec[mid + 1 + j];
	}

	int i = 0;
	int j = 0;
	int k = left;

	// merging the subvecs back into ingredientChosenMacroPairsVec
	while (i < len1 && j < len2) {
		if (leftSubvector[i].second >= rightSubvector[j].second) {
			ingredientChosenMacroPairsVec[k] = leftSubvector[i];
			i++;
		}
		else {
			ingredientChosenMacroPairsVec[k] = rightSubvector[j];
			j++;
		}
		k++;
	}
	// copy any remaining values from left subvector to the merged vector
	while (i < len1) {
		ingredientChosenMacroPairsVec[k] = leftSubvector[i];
		i++;
		k++;
	}
	// copy any remaining values  right subvector to the merged vector
	while (j < len2) {
		ingredientChosenMacroPairsVec[k] = rightSubvector[j];
		j++;
		k++;
	}
}
