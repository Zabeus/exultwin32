#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void rebuild(void)
{
	unsigned int c;
	char s[10];
	char filename[18];
	char *pos;
	FILE *fi=fopen("index","r"),*fi2,*fo=fopen("usecode","wb");
	if (fi==NULL)
		{
			printf("Can't open index file.\n");
			exit(0);
		}
	if (fo==NULL)
		{
			printf("Can't create usecode file.\n");
			exit(0);
		}
	while (!feof(fi)) {
		fgets(s,10,fi);
		strcpy(filename, s);
		pos = strchr(filename, '\n');
		if (pos) *pos = '\0';
		strcat(filename, ".uco");
		if (!feof(fi))
			{
				s[strlen(s)-1]=0;
				printf("Writing function: %s... ",s);
				if ((fi2=fopen(filename,"rb"))==NULL)
					{
						printf("Can't open file %s\n", filename);
						exit(0);
					}
				while(!feof(fi2))
					{
						c=fgetc(fi2);
						if (!feof(fi2))
							fputc(c,fo);
					}
				fclose(fi2);
				printf("done\n");
			}
	}
	exit(0);
}

int main(int argc,char *argv[])
{
	unsigned short fn, fnc;
	unsigned short temp;
	int fs,fsc;
	unsigned int i,put=0;
	int number;
	int extended;
	char s[10];
	char filename[18];
	FILE *fi,*fo,*fo2;
	printf("Wody's Rip v0.005\nCopyright (c) 1999 Wody Dragon (a.k.a. Wouter Dijkslag)\n");
	if (argc<2||(!strcasecmp(argv[1],"put")&&argc!=3))
		{
			printf("Syntax: rip <number>\tGets function <number> out of usecode (and"
				   " index)\n\trip all\t\tGets all functions out of usecode (and index)\n"
				   "\trip glue\tRecreate usecode file (needs all functions)\n"
				   "\trip index\tOnly get index\n\trip put <nr>\tInserts function"
				   " <nr> into the usecode file\n");
			exit(0);
		}
	if (!strcasecmp(argv[1],"all"))
		number=-1;
	else if (!strcasecmp(argv[1],"glue"))
		rebuild(); // note: this doesn't return
	else if (!strcasecmp(argv[1],"index"))
		number=-2;
	else if (!strcasecmp(argv[1],"put"))
	{
		sscanf(argv[2],"%x",&number);
		put=1;
	}
	else
		sscanf(argv[1],"%x",&number);

	if ((fi=fopen("usecode","rb+"))==NULL)
	{
		printf("Can't open usecode file.\n");
		exit(0);
	}
	if ((fo2=fopen("index","w"))==NULL)
	{
		printf("Can't create index file.\n");
		exit(0);
	}
	while (1) {
		if (fread(&fn,2,1,fi)!=1) break;
		if (fn == 0xFFFF) {
			extended = 1;
			fread(&fn,2,1,fi);
			fread(&fs,4,1,fi);
		} else {
			extended = 0;
			fread(&temp,2,1,fi);
			fs = temp;
		}

		if (number==-1||number==-2||number==fn)
		{
			sprintf(s,"%04X",fn);
			strcpy(filename, s);
			strcat(filename, ".uco");
			fprintf(fo2,"%s\n",s);
		}
		if (number==-1||number==fn)
		{
			if (!put)
			{
				printf("Writing function: %s... ",s);
				if ((fo=fopen(filename,"wb"))==NULL)
				{
					printf("Can't open file %s\n", filename);
					exit(0);
				}

				if (extended) {
					temp = 0xFFFF;
					fwrite(&temp,2,1,fo);
					fwrite(&fn,2,1,fo);
					fwrite(&fs,4,1,fo);
				} else {
					fwrite(&fn,2,1,fo);
					temp = fs;
					fwrite(&temp,2,1,fo);
				}

				for (i=0;i<fs;i++)
					fputc(fgetc(fi),fo);
				fclose(fo);
				printf("done\n");
			}
			else
			{
				printf("Reading function: %s... ",s);
				if ((fo=fopen(filename,"rb"))==NULL)
				{
					printf("Can't open file %s\n", filename);
					exit(0);
				}
				fread(&fnc,2,1,fo);
				if (fnc == 0xFFFF) {
					if (extended == 0) {
						printf("Wrong header (u7) in object\n");
						exit(0);
					}
					fread(&fnc,2,1,fo);
					fread(&fsc,4,1,fo);
				} else {
					if (extended == 1) {
						printf("Wrong header (extended) in object\n");
						exit(0);
					}
					fread(&temp,2,1,fo);
					fsc = temp;
				}
				if (fnc!=fn)
				{
					printf("Wrong function in object\n");
					exit(0);
				}
				if (fsc!=fs)
				{
					printf("Wrong size in object\n");
					exit(0);
				}
				fseek(fi,ftell(fi),SEEK_SET);	/* These two fseeks force my */
				for (i=0;i<fs;i++)		/* Borland C++ 5.02 to read */
					fputc(fgetc(fo),fi);	/* write. Without them they */
				fclose(fo);			/* don't work as I think */
				fseek(fi,ftell(fi),SEEK_SET);	/* they should (the writing */
				printf("done\n");		/* doesn't work) */
			}
		}
		else						/* Skip function */
			fseek(fi,fs,SEEK_CUR);
	}
	fclose(fi);
	printf("All done\n");
	return 0;
}
