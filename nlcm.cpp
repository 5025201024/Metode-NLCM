#include <bits/stdc++.h>
using namespace std;

char inputName[100];
int row, col, matrix[100][100], s[100];
int TC[100] = {}, CS[100] = {}, allocated[100][100], sumRow[100] = {};
int status[100];
int flag = 0;

void minimumCell(FILE *inFile){
	int i, j, minimum;
	for(j = 0; j < col; j++){
		minimum = 0;
		for (i = 1; i < row; i++){
			if(matrix[minimum][j] > matrix[i][j]) minimum = i;
			else if(matrix[minimum][j] == matrix[i][j]) {
				if(TC[i] > TC[minimum]) minimum = i;
			}
		}
		fscanf(inFile,"%d", &allocated[minimum][j]);
	}
	
	for (i = 0; i < row; i++){
		for (j = 0; j < col; j++)
			sumRow[i] += allocated[i][j];
	}
}

int updateStatus(int *satisfied){
	//1 = Satisfied
	//2 = needSupply
	//3 = Excess Row
	int i, j, nNeedSupply = 0, nExcess = 0;
	for (i = 0; i < row; i++){
		if (sumRow[i] == s[i]) {
			if(status[i] != 1) (*satisfied)--;
			status[i] = 1;
		} else if (sumRow[i] < s[i]) {
			status[i] = 2;
			nNeedSupply = 1;
		}
		else if (sumRow[i] > s[i]) {
			status[i] = 3;
			nExcess = 1;
		}
	}
	if (!nNeedSupply || !nExcess) return 0;
	return 1;
}

void findFLC(int *flcRow, int *slcRow, int *donationCol){
	int i, j, k, needSup[100] = {}, nNeedSup = 0;
	for (i = 0; i < row; i++){
		if(status[i] == 2) {	//2 = needSupply
			needSup[nNeedSup] = i;
			nNeedSup++;
		}
	}

	int temp = 0, smallestIndex = 0, minDiff[col], nextDiff[col], diffDon[col], diffRec[col], nextDiffDon[col], nextDiffRec[col];

	//Cari local-best donation
	for(j = 0; j < col; j++){
		minDiff[j] = INT_MAX;
		nextDiff[j] = INT_MAX;
		
		//mencari semua kandidat donasi
		for(i = 0; i < row; i++){
			if(status[i] == 3 && allocated[i][j]){		//3 = Excess Row
				for(k = 0; k < nNeedSup; k++){
					temp = matrix[i][j] - matrix[ needSup[k] ][j];
					if (temp < 0) temp *= -1;
					//Find local smallest	
					if(minDiff[j] > temp) {
						nextDiff[j] = minDiff[j];
						nextDiffDon[j] = diffDon[j];
						nextDiffRec[j] = diffRec[j];
						minDiff[j] = temp;
						diffDon[j] = i;
						diffRec[j] = needSup[k];

					}
					else if (nextDiff[j] > temp){
						nextDiff[j] = temp;
						nextDiffDon[j] = i;
						nextDiffRec[j] = needSup[k];
						
					}
				}
			}
		}
		
		//mencari global smallest dan check flag
		if(minDiff[smallestIndex] > minDiff[j] || smallestIndex == j) {
			smallestIndex = j;
			flag = 0;
		}
		// bandingkan nextDiff 
		else if(minDiff[smallestIndex] == minDiff[j] && nNeedSup > 1) flag = 1;
		else if(minDiff[smallestIndex] == minDiff[j]) flag = 2;
	}
	
	//donasi paling optimal
	if(flag == 0) {
		*flcRow = diffDon[smallestIndex];
		*slcRow = diffRec[smallestIndex];
		*donationCol = smallestIndex;
	} 
	else if(flag == 1) {
		// int donCapability1, donCapability2, maxDon1, maxDon2, priority1, priority2;
		int demandSmallestIndex, demandJ;
		for (j = smallestIndex + 1; j < col; j++){
			if(minDiff[smallestIndex] == minDiff[j]){
				
				//bandingkan jumlah donasi
				if(nextDiff[smallestIndex] < nextDiff[j]) {
					*flcRow = diffDon[smallestIndex];
					*slcRow = diffRec[smallestIndex];
					*donationCol = smallestIndex;
				} else if(nextDiff[smallestIndex] > nextDiff[j]){
					*flcRow = diffDon[j];
					*slcRow = diffRec[j];
					*donationCol = j;
					smallestIndex = j;
				} 
				
				//jika nextDiff sama
				else{
					//tentukan maxDon * cell
					demandSmallestIndex = allocated[diffDon[smallestIndex]][smallestIndex];
					demandJ = allocated[diffDon[j]][j];
					if(demandSmallestIndex <= demandJ) {
						*flcRow = diffDon[smallestIndex];
						*slcRow = diffRec[smallestIndex];
						*donationCol = smallestIndex;
					}
					else{
						*flcRow = diffDon[j];
						*slcRow = diffRec[j];
						*donationCol = j;
						smallestIndex = j;
					}
					//kalo sama, ambil index terkecil
				}
			}
		}
	}
	//flag 2
	else {
		int demandSmallestIndex = allocated[diffDon[smallestIndex]][smallestIndex];
		int demandJ = allocated[diffDon[j]][j];
		if(demandSmallestIndex >= demandJ) {
			*flcRow = diffDon[smallestIndex];
			*slcRow = diffRec[smallestIndex];
			*donationCol = smallestIndex;
		}
		else{
			*flcRow = diffDon[j];
			*slcRow = diffRec[j];
			*donationCol = j;
			smallestIndex = j;
		}
	}

}

void donate (int *flcRow, int *slcRow, int *donationCol, int childRow, int parentRow){
	//tentukan kapabilitas donatur
	int donCapability = allocated[*flcRow][*donationCol];
	printf("Kapabilitas donatur = %d\n", donCapability);
	
	//tentukan kapasitas donatur berdasarkan prioritas satisfied
	////cari prioritas satisfied
	int supDon = s[ *flcRow ];
	int supRec = s[ *slcRow ];

	int cosFLC = matrix[*flcRow][*donationCol];
	int cosSLC = matrix[*slcRow][*donationCol];

	int priority;
	if(cosFLC == cosSLC) {
		if(sumRow[*flcRow] > allocated[*flcRow][*donationCol]) {
			priority = *flcRow;
		}
		else {
			priority = *slcRow;
		}
	}
	else if(supDon > supRec) 							//prioritas donatur satisfied
		priority = *slcRow;
	else if (supDon != s[childRow]) 					//prioritas receiver satisfied
		priority = *slcRow;
	else if (supRec == s[parentRow])		//satisfied index terkecil
		priority = *slcRow;
	else 
		priority = *flcRow;
		
	////cari kapasitas donatur
	int maxDon = s[ priority ] - sumRow[ priority ];
	if(maxDon < 0) maxDon *= -1;
	printf("Kapasitas = %d\n", maxDon);
	
	//tentukan jumlah donasi
	if(donCapability < maxDon) maxDon = donCapability;
	
	//check satisfied receiver
	int maxRec = s[ *slcRow ] - sumRow[ *slcRow ];
	printf("Jumlah donasi		= %d\n", maxDon);
	
	//lakukan donasi
	allocated[*flcRow][*donationCol] -= maxDon;
	sumRow[*flcRow] -= maxDon;
	allocated[*slcRow][*donationCol] += maxDon;
	sumRow[*slcRow] += maxDon;
}

void countFTC(){
	int FTC = 0, i, j;
	for(i = 0; i < row; i++)
		for(j = 0; j < col; j++)
			FTC += (matrix[i][j]*allocated[i][j]);

	printf("FTC = %d\n", FTC);
}

void check() {
	int i, j;
	char stat[][20]={"SATISFIED", "NEED SUPPLY", "EXCESS ROW"};

	for(j = 0; j < col; j++) printf("\t");
	printf("    supply\tTC\tCS\tTOTAL ALLOC\tSTATUS\n");
	for(j = 0; j < col; j++) printf("\t");
	printf("    ---------------------------------------------------------\n");
	for(i = 0; i < row; i++){
		for(j = 0; j < col; j++){
			printf("%d [%d]\t", matrix[i][j], allocated[i][j]);
		}
		printf("||  %d\t\t%d\t\t%s\n\n", s[i], sumRow[i], stat[status[i] - 1]);
	}
	printf("\n\n\n");
}

void compute() {
	FILE *inFile  = fopen(inputName, "r"); // read only 
	FILE *outFile = fopen("output.txt", "w"); // write only
	if (inFile == NULL) {   
    	printf("Error! Could not open file\n");
    	return;
	}
	
	//# Store data from input file
	//Row column
	fscanf(inFile,"%d %d", &row, &col);
	
	//Store matriks D and S, compute TC, and set supply to 0
	int i, j;
	for (i = 0; i < row; i++){
		TC[i] = 0;
		for(j = 0; j < col; j++){
			fscanf(inFile,"%d", &matrix[i][j]);
			allocated[i][j] = 0;
			TC[i] += matrix[i][j];
		}
		status[i] = 0;
		sumRow[i] = 0;
	}

	//S dan CS = TC*S
	for (i = 0; i < row; i++) {
		fscanf(inFile,"%d", &s[i]);
		CS[i] = s[i] * TC[i];
	}

	// mark minimal and maximal dari array s
	int childRow = 0, parentRow = 0;
	for(i = 1; i < row; i++){
		if(s[i] < s[childRow]) childRow = i;
		if(s[i] > s[parentRow]) parentRow = i;
	}
	
	//D
	minimumCell(inFile);

	int satisfied = row, flcRow, slcRow, donationCol;
	if(!updateStatus(&satisfied)) {
		printf("Soal Salah\n");
		return;
	}
	check();
	while(satisfied){
		flcRow = -1; slcRow = -1;
		findFLC(&flcRow, &slcRow, &donationCol);	
		donate(&flcRow, &slcRow, &donationCol, childRow, parentRow);
		updateStatus(&satisfied);
		check();
	}
	
	countFTC();	
}

void custom() {
	printf("Name = ");
	scanf("%s", inputName);
}

main (){
	int option = 0;
	printf("What is your input-file's name?\n1 = Default (input.txt)\n2 = Custom\n3 = Exit\n");
	printf("Option = ");
	scanf("%d", &option); if(option == 3) return 0;
	do{
		if(option == 1) {strcpy(inputName, "input.txt"); compute();}
		else if (option == 2) {custom(); compute();}
		else if (option != 3) printf("Not Valid\n");
		printf("Option = ");
		scanf("%d", &option); if(option == 3);
	} while(option != 3);
	printf("Program Stop\n\n");
	return 0;
}