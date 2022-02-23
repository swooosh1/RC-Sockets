//AGS - Aplicação do Gestor do Sistema 
//Client TCP 

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>

#define BUF_SIZE 1024

//memset(buffer,0,sizeof(buffer)); limpa/zera os buffers antes de cada utilização

void erro(char *msg);
void menu(int fd);

int main(int argc, char *argv[]) {
  char endServer[100], buffer[BUF_SIZE], buffer1[BUF_SIZE];
  int fd, client, nread;
  struct sockaddr_in addr, client_addr;
  int client_addr_size;
  struct hostent *hostPtr;

  if (argc != 1) {
    printf("Não coloque argumentos \n");
    exit(-1);
  }

	argv[1] = "localhost";
  strcpy(endServer, argv[1]);
  if ((hostPtr = gethostbyname(endServer)) == 0) {
    printf("Couldn t get host address.\n");
    exit(-1);
  }

  argv[2] = "9001";
  bzero((void *) &addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
  addr.sin_port = htons((short) atoi(argv[2]));
    

  if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	erro("socket");
  if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
	erro("Connect");

  system("clear"); //clear na console
  char verificacao[BUF_SIZE]="2";
  write(fd, verificacao,1+strlen(verificacao));
// login ou sign up
  memset(buffer,0,sizeof(buffer));
  read(fd, buffer, BUF_SIZE - 1);		
  printf("%s\n", buffer);
// envia a informa�ao do que quer fazer (entrar, registar, sair)
  memset(buffer,0,sizeof(buffer));
  scanf("%s",buffer);
  write(fd,buffer,1+strlen(buffer));
  	if(strcmp(buffer,"1")==0)
  	{
  		//fazer login 
  		system("clear");
  		printf("\nPara fazer login precisamos de algumas informacoes...\n");
  		printf("Username: ");
  		memset(buffer,0,sizeof(buffer));
  		scanf("%s",buffer);
  		write(fd,buffer,1+strlen(buffer));
  		while(1)
		{
			//envia username

  			memset(buffer,0,sizeof(buffer));
  			read(fd, buffer, BUF_SIZE - 1);
  			if (strcmp(buffer,"existe")==0)
  			{
  				//o utilizador existe e sai do ciclo
				break;
			}
			else
			{
				system("clear");
				printf("\nO utilizador indicado nao existe por favor volte a tentar: ");
				memset(buffer,0,sizeof(buffer));
  				scanf("%s",buffer);
  				write(fd,buffer,1+strlen(buffer));
			}
  			
		}
		// se chega aqui o utilizador existe logo vamos ver se esta validado
		memset(buffer,0,sizeof(buffer));
  		read(fd, buffer, BUF_SIZE - 1);	
		//printf(" validacao %s \n",buffer);	
  		if(strcmp(buffer,"validado")==0)
  		{
  			memset(buffer1,0,sizeof(buffer1));
  			read(fd, buffer1, BUF_SIZE - 1);
  			system("clear");
  			printf("\nPassword: ");
			memset(buffer,0,sizeof(buffer));
  			scanf("%s",buffer);
  			while(strcmp(buffer,buffer1)!=0)
  			{
  				system("clear");
  				printf("\nPassword incorreta, volte a repetir: ");
  				memset(buffer,0,sizeof(buffer));
  				scanf("%s",buffer);
		    }
		    system("clear");
			printf("\nLogin Efectuado com SUCESSO!!!\n\n");
			menu(fd);
		}
		else
		{
			printf("\nO utilizador existe mas nao se encontra validado.\n");
		}
  	}
 	else if(strcmp(buffer,"2")==0)
  	{
  		char password[BUF_SIZE], password1[BUF_SIZE];
  		//fazer registo de novo Gestor do sistema
  		system("clear");
  		printf("\nPara procedermos ao registo precisamos de algumas informacoes...\n");
  		printf("Username: ");
		memset(buffer,0,sizeof(buffer));
  		scanf("%s",buffer);
  		write(fd,buffer,1+strlen(buffer));
  		while(1)
		{
			//envia username

  			memset(buffer,0,sizeof(buffer));
  			read(fd, buffer, BUF_SIZE - 1);
  			if (strcmp(buffer,"nexiste")==0)
  			{
  				//o utilizador nao existe e sai do ciclo
				break;
			}
			else
			{
				system("clear");
				printf("\nO utilizador indicado ja existe por favor volte a tentar: ");
				memset(buffer,0,sizeof(buffer));
  				scanf("%s",buffer);
  				write(fd,buffer,1+strlen(buffer));
			}
  			
		}
		system("clear");
		printf("\nPor ultimo indique uma password: ");
		scanf("%s",password);
		printf("Repita a password:	");
		scanf("%s",password1);
		printf("\n");
		while (strcmp(password,password1)!=0)
		{
			system("clear");
			printf("As passwords nao coincidem por favor volte a inserir:	 ");
			scanf("%s",password);
			printf("Repita a password:	");
			scanf("%s",password1);
		}
		write(fd,password,1+strlen(password));
		memset(buffer,0,sizeof(buffer));
  		read(fd, buffer, BUF_SIZE - 1);	
		printf("\n%s \n",buffer);
  	}
  	
  	
  	else
  	{
  		fflush(stdout);
  		close(fd);
  	}
  
  
    


  fflush(stdout);
  close(fd);
}

void menu(int fd)
{
	char op[BUF_SIZE];
	printf("Menu principal...\n1- Ver utilizadores\n2- Autorizar utilizadores\n ");
	scanf("%s", op);
	write(fd,op,1+strlen(op));
	if (strcmp(op,"1")==0)
	{
		char utilizadores[BUF_SIZE];
		memset(utilizadores,0,sizeof(utilizadores));
		read(fd,utilizadores,BUF_SIZE-1);
		printf("Utilizadores: \n %s\n",utilizadores);
		
	}
	else if(strcmp(op,"2")==0)
	{
		char user[BUF_SIZE];
		printf("\nIndique o utilizador a que pretende autorizar: 	");
		memset(user,0,sizeof(user));
		scanf("%s",user);
		write(fd, user, 1+strlen(user));
		
		
		
	}
	exit(-1);
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}
