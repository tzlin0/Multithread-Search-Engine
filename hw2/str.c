#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 100
void printstr(char **str) {
    for (int i = 0; i < 4; i++) {
        printf("%s\n",str[i]);
    }
}
int main(int argc, char** argv) {

    char *line = NULL;
    size_t size;
    // type: man manaul
    // char* {"man manual"}
    // char* {"man", "manual"}
    // while (getline(&line, &size, stdin) != -1) {
    //     printf("%s",line);
    //     printf("%d",strlen(line));
    // }
    char buf[MAX];

    while (fgets(buf, MAX, stdin)) {
        printf("%s", buf);
        printf("%d",strlen(buf));
    }
    
	// char str[] = "strtok needs to be called several times to split a string";
    // char delim[] = " ";
    // int num = 0;
    // // copy the original str to count the words first
    // char * newstr = (char*) malloc(strlen(str)+1);
    // strcpy(newstr, str);
    // // set the length of the char[];
    
    // char *np = strtok(newstr,delim);
    // while (np != NULL) {
    //     num++;
    //     np = strtok(NULL,delim);
    // }
    // // printf("n: %d\n",num);
    // // set the query[]
    // char *query[num];
    // // printstr(newstr);
	// char *ptr = strtok(str, delim);
    // int i = 0;
	// while(ptr != NULL)
	// {
	// 	query[i++] = ptr;
	// 	ptr = strtok(NULL, delim);
	// }
    // printstr(query);
    


	return 0;
}