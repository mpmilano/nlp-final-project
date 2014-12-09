#include "stdlib.h"
#include "stdio.h"
#include "string.h"
    
namespace CurrentMemUsage {

auto parseLine(char* line){
	auto i = strlen(line);
	while (*line < '0' || *line > '9') line++;
	line[i-3] = '\0';
	i = atoi(line);
	return i;
}

//14000000    

long long getValue(){ //Note: this value is in KB!
	FILE* file = fopen("/proc/self/status", "r");
	long long result = -1;
	char line[128];
    

	while (fgets(line, 128, file) != NULL){
		if (strncmp(line, "VmRSS:", 6) == 0){
			result = parseLine(line);
			break;
		}
	}
	fclose(file);
	return result;
}

}
