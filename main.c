#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

#include "minicas.h"

MatObject mats[256];
VariableObject vars[256];
int cur_mat = 0;
int cur_var = 0;

int main (int argc, char* argv[])
{
	srand48(time(NULL));
	
	int fd, dupSTDIN, i;
	struct stat buffer;
		
	if (argc > 1)
	{
		stat(argv[1], &buffer);
		if ((buffer.st_mode & S_IFMT)==S_IFREG)
		{
			fd = open(argv[1], O_RDONLY);
			dupSTDIN = dup(0);
			dup2(fd, 0);
			
			LaunchInterpreter();
	
			close(fd);
			close(0);
			dup(dupSTDIN);
			close(dupSTDIN);
		}
		else
		{
			printf("\t%s : non regular file\n", argv[1]);
			return 1;
		}
	}
	else LaunchInterpreter();
	
	for (i=0; i<cur_var; i++) free(vars[i]);
	for (i=0; i<cur_mat; i++)
	{
		deleteMatrix(mats[i]->mat);
		free(mats[i]);
	}
	
	return 0;
}
