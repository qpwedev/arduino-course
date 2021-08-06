#include <stdio.h>
#include <cmath>

// prints values on the screen
void teploty(int list[], int size, int mini){
	int last = 1;
	for (int i=0;i<size;i++){
		if (list[i] == -999){
			int lis[] {last};
			teploty(lis, 1, mini);
			continue;
			}
		else if (list[i] < 0){
			for (int k=0;k<abs(mini)-abs(list[i]);k++){
				printf(" ");
			}
		}
		else{
			for (int k=0;k<abs(mini);k++){
				printf(" ");
			}
		}
		printf("*");
		last = list[i];
		for (int j=0;j<abs(list[i]);j++){
			printf("*");
		}
		printf("\n");
	}
}

// finds the minimal value in array, except -999
int minimal(int list[], int size){
	int mini = 2147483647;
	for (int i=0;i<size;i++){
			if ((list[i]<mini)&(list[i]!=-999)){
				mini = list[i];
			}
		}
	return mini;
}

int main() {
	constexpr int no_value = -999;
    int temperatures[] { 10, 12, no_value, no_value, 20, 14, 6, -1, -5, 0, no_value, 1, -3 };
	int size = sizeof(temperatures)/sizeof(temperatures[0]);
	int mini = minimal(temperatures, size);
	teploty(temperatures, size, mini);
}
