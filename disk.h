#ifndef DISK_H
void get_directory();
char* find_files(char* filespec);
filestruct* get_first_sector(char* filename);
int get_next_sector(filestruct* fs);

#endif


