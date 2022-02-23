//AAS - Aplicacao para o agente de seguranca
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
#include <signal.h>

#define BUF_SIZE 1024

//memset(buffer,0,sizeof(buffer)); limpa/zera os buffers antes de cada utilização

void erro(char *msg);
void menu(int fd);

int main(int argc, char *argv[])
{

	int pid = fork();

	if (pid == 0)
	{ //processo ALARME  socket UDP

		int udpSocket, nBytes;
		char buffer[1024];

		struct sockaddr_in serverAddr, clientAddr;
		struct sockaddr_storage serverStorage;

		socklen_t addr_size, client_addr_size;

		/*Create UDP socket*/
		udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

		/*Configure settings in address struct*/
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(7893); //porto 7892
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

		/*Bind socket with address struct*/
		bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

		/*Initialize size variable to be used later on*/
		addr_size = sizeof serverStorage;
		while (1)
		{

			

			recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);

			printf("%s ESTÁ EM APUROS!!!\n", buffer);
			

			
		}
	}

	else
	{

		char endServer[100], buffer[BUF_SIZE], login[BUF_SIZE], buffer1[BUF_SIZE];
		int fd, client, nread;
		struct sockaddr_in addr, client_addr;
		int client_addr_size;
		struct hostent *hostPtr;

		if (argc != 1)
		{
			printf("Não coloque argumentos \n");
			exit(-1);
		}

		argv[1] = "localhost";
		strcpy(endServer, argv[1]);
		if ((hostPtr = gethostbyname(endServer)) == 0)
		{
			printf("Couldn t get host address.\n");
			exit(-1);
		}

		argv[2] = "9000";
		bzero((void *)&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
		addr.sin_port = htons((short)atoi(argv[2]));

		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			erro("socket");
		if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
			erro("Connect");


		
		system("clear");//clear na console
		char verificacao[BUF_SIZE] = "1";
		char username[BUF_SIZE];
		write(fd, verificacao, 1 + strlen(verificacao));
		// login ou sign up
		memset(buffer, 0, sizeof(buffer));
		read(fd, buffer, BUF_SIZE - 1);
		printf("%s\n", buffer);
		//escolhe 1 ou 2 e envia
		memset(login, 0, sizeof(login));
		scanf("%s", login);
		write(fd, login, 1 + strlen(login));
		if (strcmp(login, "1") == 0)
		{
			//fazer login
			system("clear");
			printf("\nPara fazer login precisamos de algumas informacoes...\n");
			printf("Username: \n");
			memset(buffer, 0, sizeof(buffer));
			scanf("%s", buffer);
			write(fd, buffer, 1 + strlen(buffer));
			while (1)
			{
				//envia username

				memset(buffer, 0, sizeof(buffer));
				read(fd, buffer, BUF_SIZE - 1);
				if (strcmp(buffer, "existe") == 0)
				{
					//o utilizador existe e sai do ciclo
					break;
				}
				else
				{
					system("clear");
					printf("\nO utilizador indicado nao existe por favor volte a tentar: \n");
					memset(buffer, 0, sizeof(buffer));
					scanf("%s", buffer);
					write(fd, buffer, 1 + strlen(buffer));
				}
			}
			// se chega aqui o utilizador existe logo vamos ver se esta validado
			memset(buffer, 0, sizeof(buffer));
			read(fd, buffer, BUF_SIZE - 1);
			//printf(" validacao %s \n",buffer);
			if (strcmp(buffer, "validado") == 0)
			{
				memset(buffer1, 0, sizeof(buffer1));
				read(fd, buffer1, BUF_SIZE - 1);
				system("clear");
				printf("\nPassword: \n");
				memset(buffer, 0, sizeof(buffer));
				scanf("%s", buffer);
				while (strcmp(buffer, buffer1) != 0)
				{
					system("clear");
					printf("\nPassword incorreta, volte a repetir: \n");
					memset(buffer, 0, sizeof(buffer));
					scanf("%s", buffer);
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
		else if (strcmp(login, "2") == 0)
		{
			char password[BUF_SIZE], password1[BUF_SIZE];
			//fazer registo de novo agente de seguran�a
			system("clear");
			printf("\nPara procedermos ao registo precisamos de algumas informacoes...\n");
			printf("Username: ");
			memset(buffer, 0, sizeof(buffer));
			scanf("%s", buffer);
			write(fd, buffer, 1 + strlen(buffer));
			while (1)
			{
				//envia username

				memset(buffer, 0, sizeof(buffer));
				read(fd, buffer, BUF_SIZE - 1);
				if (strcmp(buffer, "nexiste") == 0)
				{
					//o utilizador nao existe e sai do ciclo
					break;
				}
				else
				{
					system("clear");
					printf("\nO utilizador indicado ja existe por favor volte a tentar: \n");
					memset(buffer, 0, sizeof(buffer));
					scanf("%s", buffer);
					write(fd, buffer, 1 + strlen(buffer));
				}
			}
			system("clear");
			printf("\nPor ultimo indique uma password: \n");
			scanf("%s", password);
			printf("Repita a password:	\n");
			scanf("%s", password1);
			printf("\n");
			while (strcmp(password, password1) != 0)
			{
				system("clear");
				printf("As passwords nao coincidem por favor volte a inserir:	 \n");
				scanf("%s", password);
				printf("Repita a password:	\n");
				scanf("%s", password1);
			}
			write(fd, password, 1 + strlen(password));
			memset(buffer, 0, sizeof(buffer));
			read(fd, buffer, BUF_SIZE - 1);
			printf("\n%s \n", buffer);
		}
		else if (strcmp(login, "3") == 0)
		{
			fflush(stdout);
			close(fd);
		}
		else if(strcmp(login, "9") == 0){

			system("clear");
			printf("Login - Permite-lhe aceder a sua conta atraves das suas credenciais \n Criar conta - Caso ainda nao tenha conta pode criar a sua\n inserindo um username e password, a sua conta ficara pendente de validacao por parte do admin de sistema.\n Sair - Fecha a aplicacao. \n\n");
			sleep(15000);
			return(0);
		}
		fflush(stdout);
		close(fd);
	}
}

void menu(int fd)
{
	char op1[BUF_SIZE], existe[BUF_SIZE], password[BUF_SIZE], password1[BUF_SIZE];
	memset(op1, 0, sizeof(op1));
	char buffer[BUF_SIZE];
	printf("Menu principal...\n");
	printf("1 - Modificar conta\n2 - Consultar Ocorrencias\n9 - Help\n");
	scanf("%s", op1);
	write(fd, op1, 1 + strlen(op1));
	if(strcmp(op1,"9")==0){system("clear");		printf("Modificar conta permite alterar o seu username e password \n Consultar ocorrencias permite ver o registo de ocorrencias por parte dos profissionais de saude e tambem aplicar filtros a esse registo\n\n");}
	if (strcmp(op1, "1") == 0)
	{
		printf("\n1- Alterar username\n2- Alterar password\n3- Apagar conta\n4- Voltar ao menu principal");
		memset(op1, 0, sizeof(op1));
		scanf("%s", op1);
		write(fd, op1, 1 + strlen(op1));
		if (strcmp(op1, "1") == 0)
		{
			char user[BUF_SIZE];
			printf("Indique o novo username:	");
			memset(user, 0, sizeof(user));
			scanf("%s", user);
			write(fd, user, 1 + strlen(user));
			while (1)
			{
				memset(existe, 0, sizeof(existe));
				read(fd, existe, BUF_SIZE - 1);
				if (strcmp(existe, "existe") == 0)
				{
					printf("O novo username indicado ja existe por favor indique outro:		");
					memset(user, 0, sizeof(user));
					scanf("%s", user);
					write(fd, user, 1 + strlen(user));
				}
				else
				{
					memset(password, 0, sizeof(password));
					read(fd, password, BUF_SIZE - 1);
					printf("Para confirmar a altera�ao indique a password:	");
					scanf("%s", password1);
					while (strcmp(password, password1) != 0)
					{
						printf("Password incoreta, volte a indicar:		");
						scanf("%s", password1);
					}
					write(fd, user, 1 + strlen(user));
					break;
				}
			}
		}
		else if (strcmp(op1, "2") == 0)
		{
			printf("Indique nova password	");
			memset(password1, 0, sizeof(password1));
			scanf("%s", password1);
			write(fd, password1, 1 + strlen(password1));
		}
		else if (strcmp(op1, "3") == 0)
		{
			memset(password, 0, sizeof(password));
			read(fd, password, BUF_SIZE - 1);
			printf("Para confirmar a altera�ao indique a password:	");
			scanf("%s", password1);
			while (strcmp(password, password1) != 0)
			{
				printf("Password incoreta, volte a indicar:		");
				scanf("%s", password1);
			}
			write(fd, "1", 2);
		}
		else if (strcmp(op1, "4") == 0)
		{
			//volta ao menu anterior e envia mensagem ao servidor para ele saber
		}
	}
	else if (strcmp(op1, "2") == 0)
	{
		memset(buffer, 0, sizeof(buffer)); // mensagem de login efetuado e menu de listar ocorrencias
		read(fd, buffer, BUF_SIZE - 1);
		printf("%s\n", buffer);

		char opc[BUF_SIZE]; // envia a escolha efetuada
		memset(opc, 0, sizeof(opc));
		scanf("%s", opc);
		write(fd, opc, 1 + strlen(opc));

		if (strcmp(opc, "1") == 0)
		{ //escolheu opção 1 - Listar todas as ocorrências

			memset(buffer, 0, sizeof(buffer)); // recebe lista
			read(fd, buffer, BUF_SIZE - 1);
			printf("%s\n", buffer);

			fflush(stdout);
			close(fd);
		}

		else if (strcmp(opc, "2") == 0)
		{ //escolheu opção 2 - Pesquisar por local

			char local[BUF_SIZE];
			printf("Local: \n");
			memset(local, 0, sizeof(local));
			scanf("%s", local);
			write(fd, local, 1 + strlen(local));

			memset(buffer, 0, sizeof(buffer)); // recebe lista
			read(fd, buffer, BUF_SIZE - 1);
			printf("%s\n", buffer);
		}
		else if (strcmp(opc, "3") == 0)
		{ // escolheu opção 3 - Pesquisar por nome

			char nome[BUF_SIZE];
			printf("Nome: \n");
			memset(nome, 0, sizeof(nome));
			scanf("%s", nome);
			write(fd, nome, 1 + strlen(nome));

			memset(buffer, 0, sizeof(buffer));
			read(fd, buffer, BUF_SIZE - 1);
			printf("%s\n", buffer);
		}

		else if (strcmp(opc, "4") == 0)
		{

			char data[BUF_SIZE];
			printf("Data: (dd/mm/aaaa) \n");
			memset(data, 0, sizeof(data));
			scanf("%s", data);
			write(fd, data, 1 + strlen(data));

			memset(buffer, 0, sizeof(buffer));
			read(fd, buffer, BUF_SIZE - 1);
			printf("%s\n", buffer);
		}
	}
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}
