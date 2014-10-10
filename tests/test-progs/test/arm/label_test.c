#define SET_LABEL_L 0x80000000
#define SET_LABEL_H 0x80000001

#include <stdio.h>
#include <stdlib.h>

int main() {
	int i;
	int * buffer;
	buffer = (int *) malloc(sizeof(int)*1000);
	
	int *set_label_l = (int *)SET_LABEL_L;
	int *set_label_h = (int *)SET_LABEL_H;
	
	i = *set_label_h;
	
	for (i = 0; i < 1000; i++) buffer[i] = i;
	
	printf("finish setting H label!\n");

	i = *set_label_l;
	
	for (i = 0; i < 1000; i++) buffer[i] = -i;
	
	printf("finish setting L label!\n");
	
	return 0;
}