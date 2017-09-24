#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define CHUNK 1024 /* Le 1024 bytes por vez */
#define ADDRESS_SIZE 500

int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat);

int main(int argc, char *argv[]) {
	char address[ADDRESS_SIZE], temp_address[ADDRESS_SIZE];
    struct stat fileStat;
    int aux = 0;

    if(argc != 3){
    	fprintf(stderr, "\nUso: ./lab7 \"Caminho_web\" \"Recurso\"\n\n");
    	return 1;
    }

    //concatena as 2 entradas
    strcpy(address, "\0");
  	strcat(address, argv[1]);
  	if(address[strlen(address)-1] != '/')		//se o endereco nao termina em '/'
    	strcat(address, "/");				//insere '/' no final do endereco
    strcat(address, argv[2]);
    printf("Entrada: %s\n",address);
    

    //verifica se o arquivo existe, e associa ele a fileStat
    if(stat(address,&fileStat) < 0){
    		fprintf(stderr, "404 - Not Found\n");
        return 1;
    }
		
  	//verifica se o recurso eh um diretorio
    if( S_ISDIR(fileStat.st_mode) ) {
    	//se ele possui permissao de execucao (acesso)
		if( fileStat.st_mode & S_IXUSR ){

			//concatena address com index
    		strcpy(temp_address, "\0");
  			strcat(temp_address, address);
  			if(address[strlen(address)-1] != '/')		//se o endereco nao termina em '/'
    			strcat(temp_address, "/");				//insere '/' no final do endereco
    		strcat(temp_address, "index.html");

    		//verifica se o index.html existe, e associa ele a fileStat
    		if(stat(temp_address,&fileStat) >= 0){
    			if( escreveArquivo(temp_address, fileStat) ) return 0;
    			aux = 1; //index nao tem permissao de leitura
    		}

    		//concatena address com welcome
    		strcpy(temp_address, "\0");
  			strcat(temp_address, address);
  			if(address[strlen(address)-1] != '/')		//se o endereco nao termina em '/'
    			strcat(temp_address, "/");				//insere '/' no final do endereco
    		strcat(temp_address, "welcome.html");

    		//verifica se o welcome.html existe, e associa ele a fileStat
    		if(stat(temp_address,&fileStat) >= 0){
    			if( escreveArquivo(temp_address, fileStat) ) return 0;
    				fprintf(stderr, "403 - Forbidden\n");
					return 1;
    			}

    		if(aux){
    			fprintf(stderr, "403 - Forbidden\n");
				return 1;
    		}

    		//nenhum dos arquivos existem
    		fprintf(stderr, "404 - Not Found\n");
        	return 1;
    	}
		//se ele nao possui permissao de execucao (acesso)
		else{
			fprintf(stderr, "403 - Forbidden\n");
			return 1;
		}
    }
    //se o recurso eh um arquivo
    else {      
		return escreveArquivo(address, fileStat);     
    }
  
  	/*
    printf("Information for %s\n",address);
    printf("---------------------------\n");
    printf("File Size: \t\t%ld bytes\n",fileStat.st_size);
    printf("Number of Links: \t%lu\n",fileStat.st_nlink);
    printf("File inode: \t\t%lu\n",fileStat.st_ino);

    printf("File Permissions: \t");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");

    printf("The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");
	*/
    return 1;
}

/*
retorno:
0 = nao ha permissao de leitura
1 = o arquivo foi impresso corretamente
*/
int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat){
	int file, size;
	char  buf[CHUNK];
	//verifica permissao de leitura
	if( !(fileStat.st_mode & S_IRUSR) ){
        fprintf(stderr, "403 - Forbidden\n");		return 0;
	}

	//tenta abrir o arquivo
	if ((file = open(address, O_RDONLY, 0600)) < 0){
		fprintf(stderr, "Erro na abertura de %s (tem permissao de leitura).\n", address);
		exit(0);
	}

	//le e escreve o arquivo na saida padrao
	while( (size =  read(file, buf, sizeof(buf))) > 0 ){
		if(write( 1, buf, size) < 0){
			fprintf(stderr, "Erro na escrita de %s.\n", address);
			exit(0);
		}
	}
	return 1;
}
