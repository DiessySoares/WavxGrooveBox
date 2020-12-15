#include <stdio.h>
#include <stdlib.h>

/*------------------------------------------
0x12
-0,56765 0.86955
-0,56765 0.86955
-0,56765 0.86955
x12
0,343243 0,343234
0,343243 0,343234
0,343243 0,343234
x21
-0,56765 0.86955
-0,56765 0.86955
-0,56765 0.86955
------------------------------------------*/


#define KICK 	0xf000
#define SNARE 	0xf001
#define CLAP 	0xf002
#define HAT 	0xf003
#define RIDE 	0xf004	
#define PERC 	0xf005

#define ROM		"909.rom"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>



#define KICK_PATH ".\\perc\\"

#define atual PERC



/* This is just a sample code, modify it to meet your need */
int main(int argc, char **argv)
{
    DIR* FD;
    struct dirent* in_file;
    FILE    *common_file;
    FILE    *entry_file;
    char    buffer[BUFSIZ];
    
    char path[80];
    
    float value;

    /* Openiing common file for writing */
    common_file = fopen(ROM, "a");
    if (common_file == NULL)
    {
        fprintf(stderr, "Error : Failed to open common_file - %s\n", strerror(errno));

        return 1;
    }

    /* Scanning the in directory */
    if (NULL == (FD = opendir (KICK_PATH))) 
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        fclose(common_file);

        return 1;
    }
    fprintf(common_file, "%d,", atual);	// begin
    while ((in_file = readdir(FD))) 
    {
        /* On linux/Unix we don't want current and parent directories
         * On windows machine too, thanks Greg Hewgill
         */
        if (!strcmp (in_file->d_name, "."))
            continue;
        if (!strcmp (in_file->d_name, ".."))    
            continue;
        /* Open directory entry file for common operation */
        /* TODO : change permissions to meet your need! */
        
        
        strcpy(path, KICK_PATH);
        strcat(path, in_file->d_name);
        
        entry_file = fopen(path, "r");

        if (entry_file == NULL)
        {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            fclose(common_file);

            return 1;
        }

        /* Doing some struf with entry_file : */
        /* For example use fgets */
        
        while (fgets(buffer, BUFSIZ, entry_file) != NULL)
        {
            fscanf(entry_file, "%f", &value);
            fprintf(common_file, "%d,", (int)((float)value * SHRT_MAX));
        }
        fprintf(common_file, "%d,", 0xff88);	// EndStream	

        /* When you finish with the file, close it */
        fclose(entry_file);
    }

    /* Don't forget to close common file before leaving */
    fclose(common_file);

    return 0;
}
