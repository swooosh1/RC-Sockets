// AC - Aplicacao Central
// Servidor

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define SERVER_PORT_1 9000 //porta para servidor AGS
#define SERVER_PORT_2 9001 //porta para o servidor AAS
#define BUF_SIZE 1024

void process_client_1(int fd);
void process_client_2(int fd);
void erro(char *msg);

FILE *fptr;

struct rg
{

	char username[BUF_SIZE];
	char password[BUF_SIZE];
	char tipo[BUF_SIZE];
	char validacao[BUF_SIZE];
};
void menuAS(int fd, char *user1, char *password);
void menuAGS(int fd);

struct ocorrencia
{

	char data[BUF_SIZE];
	char hora[BUF_SIZE];
	char local[BUF_SIZE];
	char tipo[BUF_SIZE];
	char nome[BUF_SIZE];
};

int main()
{

	system("clear"); //clear na console
	printf("servidor a correr .....\n\n");

	int pid = fork();

	if (pid == 0)
	{ //processo filho - servidor UDP

		printf("Servidor 1 \n");
		int udpSocket, nBytes;
		char buffer[1024];
		char sucesso[BUF_SIZE] = "Ocorrência adicionada! \n";
		char leitura[BUF_SIZE];
		struct rg f;

		struct sockaddr_in serverAddr, clientAddr;
		struct sockaddr_storage serverStorage;

		socklen_t addr_size, client_addr_size;
		int i;

		/*Create UDP socket*/
		udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

		/*Configure settings in address struct*/
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(7891);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

		/*Bind socket with address struct*/
		bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

		/*Initialize size variable to be used later on*/
		addr_size = sizeof serverStorage;
		while (1)
		{

			//inicializa o programa
			memset(buffer, 0, sizeof(buffer));
			recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);
			//printf("chegou aqui ;%s;\n",buffer);
			if (strcmp(buffer, "1") == 0)
			{ //fazer login do profissional de saude
				char linha[BUF_SIZE];
				struct rg f;
				int c = 0;
				//printf("aqui\n\n");
				while (1)
				{
					//printf("\npassa aqui\n");
					fptr = fopen("utilizadores.txt", "r");
					if (fptr == NULL)
					{
						fputs("Error at opening File utilizadores \n", stderr);
						exit(1);
					}
					memset(leitura, 0, sizeof(leitura));
					recvfrom(udpSocket, leitura, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);
					//printf("recebeu este username %s\n",leitura);
					while (fgets(linha, sizeof(linha), fptr))
					{
						memset(f.username, 0, sizeof(f.username));
						memset(f.password, 0, sizeof(f.password));
						memset(f.tipo, 0, sizeof(f.tipo));
						memset(f.validacao, 0, sizeof(f.validacao));
						int i = 0;
						int b = 0;
						int a = 0;
						for (i = 0; i < (strlen(linha) - 1); i++)
						{
							if (linha[i] == '\n')
								break;
							else if (linha[i] == ';')
							{
								a = a + 1;
							}
							else if (a == 0)
							{
								f.username[i] = linha[i];
							}
							else if (a == 1)
							{
								f.password[b] = linha[i];
								b = b + 1;
							}
							else if (a == 2)
							{
								f.tipo[0] = linha[i];
							}
							else if (a == 3)
							{
								f.validacao[0] = linha[i];
							}
						}
						// ja temos a informa��o toda da linha
						if (strcmp(f.username, leitura) == 0 && strcmp(f.tipo, "3") == 0)
						{
							c = 1;
							break;
						}
					}
					if (c == 1)
					{
						//escreve ao servidor que existe e fecha o while
						char existe[BUF_SIZE] = "existe";
						sendto(udpSocket, existe, strlen(existe) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
						fclose(fptr);
						break;
					}
					else
					{
						// escreve ao servidor que nao existe e pede para voltar a pedir o username
						char existe[BUF_SIZE] = "nexiste";
						sendto(udpSocket, existe, strlen(existe) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
					}
					fclose(fptr);
				}

				// se chegou aqui � pq indicou um utilizador que existe
				if (strcmp(f.validacao, "2") == 0)
				{
					char valido[BUF_SIZE] = "validado";
					sendto(udpSocket, valido, strlen(valido) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
					sendto(udpSocket, f.password, strlen(f.password) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);

					nBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);

					fptr = fopen("ocorrencias.txt", "a");

					fprintf(fptr, "%s\n", buffer);
					fclose(fptr);

					sendto(udpSocket, sucesso, nBytes, 0, (struct sockaddr *)&serverStorage, addr_size);
				}
				else
				{
					char valido[BUF_SIZE] = "nvalidado";
					sendto(udpSocket, valido, strlen(valido) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
				}
			}
			else if (strcmp(buffer, "2") == 0)
			//para fazer o registo do agente
			{
				char linha[BUF_SIZE];
				struct rg f;

				while (1)
				{
					int c = 0;
					fptr = fopen("utilizadores.txt", "r");
					if (fptr == NULL)
					{
						fputs("Error at opening File gestores \n", stderr);
						exit(1);
					}
					memset(leitura, 0, sizeof(leitura));
					recvfrom(udpSocket, leitura, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);
					//printf("recebeu este username %s\n",leitura);
					while (fgets(linha, sizeof(linha), fptr))
					{
						int i = 0;
						memset(f.username, 0, sizeof(f.username));
						for (i = 0; i < (strlen(linha) - 1); i++)
						{
							if (linha[i] == ';')
								break;
							else
							{
								f.username[i] = linha[i];
							}
						}
						// ja temos a informa��o do username da linha
						if (strcmp(f.username, leitura) == 0)
						{
							c = 1;
							break;
						}
					}
					if (c == 1)
					{
						//escreve ao servidor que existe e pede para voltar a pedir o username
						char existe[BUF_SIZE] = "existe";
						sendto(udpSocket, existe, strlen(existe) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
					}
					else
					{
						// escreve ao servidor que nao existe e fecha o while
						char existe[BUF_SIZE] = "nexiste";
						sendto(udpSocket, existe, strlen(existe) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
						fclose(fptr);
						break;
					}
					fclose(fptr);
				}
				//se chegou aqui e pq indicou um utilizador que nao existe
				memset(f.username, 0, sizeof(0));
				strcat(f.username, leitura);
				memset(f.password, 0, sizeof(0));
				recvfrom(udpSocket, f.password, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);
				fptr = fopen("utilizadores.txt", "a");
				if (fptr == NULL)
				{
					fputs("Error at opening File agentes", stderr);
					exit(1);
				}
				fprintf(fptr, "%s;%s;3;1;\n", f.username, f.password);
				fclose(fptr);
				char sucesso[BUF_SIZE] = "Registo efectuado com SUCESSO!!!";
				sendto(udpSocket, sucesso, strlen(sucesso) + 1, 0, (struct sockaddr *)&serverStorage, addr_size);
			}
			else if (strcmp(buffer, "3") == 0)
			{
				printf("saiu \n");
			}

			//nBytes = recvfrom(udpSocket,buffer,1024,0,(struct sockaddr *)&serverStorage, &addr_size);

			//fptr = fopen("ocorrencias.txt","a");

			//fprintf(fptr,"%s\n",buffer);
			//fclose(fptr);

			//sendto(udpSocket,sucesso,nBytes,0,(struct sockaddr *)&serverStorage,addr_size);
		}
	}

	else
	{ // processo  - servidores TCP

		int pid1 = fork();
		if (pid1 == 0)
		{ /// agente de seguran�a

			printf("Servidor 2 \n");

			int fd, client;
			struct sockaddr_in addr, client_addr;
			int client_addr_size;

			bzero((void *)&addr, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(SERVER_PORT_1);

			if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				erro("na funcao socket");
			if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
				erro("na funcao bind 2");
			if (listen(fd, 5) < 0)
				erro("na funcao listen");

			int nclientes = 0;

			while (1)
			{
				client_addr_size = sizeof(client_addr);
				client = accept(fd, (struct sockaddr *)&client_addr, &client_addr_size);
				nclientes++;
				if (client > 0)
				{

					if (fork() == 0)
					{

						close(fd);
						process_client_1(client);
						exit(0);
					}
					close(client);
				}
			}
		}
		else
		{ //gestor do sistema

			printf("Servidor 3 \n");

			int fd, client;
			struct sockaddr_in addr, client_addr;
			int client_addr_size;

			bzero((void *)&addr, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_ANY);
			addr.sin_port = htons(SERVER_PORT_2);

			if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				erro("na funcao socket");
			if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
				erro("na funcao bind 2");
			if (listen(fd, 5) < 0)
				erro("na funcao listen");

			int nclientes = 0;



			int fd2[2]; // cria pipe entre processos

			if (pipe(fd2) == -1)
			{

				printf("PIPE ERROR \n");
			}

			int pid2 = fork();
			if (pid2 == 0)
			{ // processo alarme UDP - APS RECEBE


				printf("Servidor 4 \n");


				close(fd2[0]);

				int udpSocket, nBytes;
				char buffer[1024];

				struct sockaddr_in serverAddr, clientAddr;
				struct sockaddr_storage serverStorage;

				socklen_t addr_size, client_addr_size;

				/*Create UDP socket*/
				udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

				/*Configure settings in address struct*/
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(7892); //porto 7892
				serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
				memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

				/*Bind socket with address struct*/
				bind(udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

				/*Initialize size variable to be used later on*/
				addr_size = sizeof serverStorage;
				while (1)
				{

					

					recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&serverStorage, &addr_size);

					printf("Recebeu %s \n", buffer);
					write(fd2[1], buffer, sizeof(buffer));

					close(fd2[1]);
				}
			} 
			else 
			{ // processo alarme UDP - AAS ENVIA

				printf("Servidor 5 \n");

				close(fd2[1]);

				int clientSocket, portNum, nBytes;
				char buffer[1024], buffer1[1024];
				struct sockaddr_in serverAddr;
				socklen_t addr_size;

				/*Create UDP socket*/
				clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

				/*Configure settings in address struct*/
				serverAddr.sin_family = AF_INET;
				serverAddr.sin_port = htons(7893);
				serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
				memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

				/*Initialize size variable to be used later on*/
				addr_size = sizeof serverAddr;


				read(fd2[0], buffer, sizeof(buffer));
				
				close(fd2[0]);

				printf("ALARME : %s \n", buffer);
				sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size); //envia mensagem de alarme

				
			}
			
			while (1)
			{
				client_addr_size = sizeof(client_addr);
				client = accept(fd, (struct sockaddr *)&client_addr, &client_addr_size);
				nclientes++;
				if (client > 0)
				{

					if (fork() == 0)
					{

						close(fd);
						process_client_2(client);
						exit(0);
					}
					close(client);
				}
			}
		}
	}

	fflush(stdout);

	return 0;
}

void process_client_1(int client_fd)
{
	char verificacao[BUF_SIZE];
	char login[BUF_SIZE];
	memset(verificacao, 0, sizeof(verificacao));
	read(client_fd, verificacao, BUF_SIZE - 1);
	if (strcmp(verificacao, "1") == 0) // agente de seguran�a
	{
		char mensagem[BUF_SIZE] = "Bem-vindo a aplicacao de agente de seguranca! \n\n1- Entrar\n2- Criar conta\n3- Sair\n 9 - Help \nDas opcoes escolha (1 ou 2 ou 3).\n";
		char leitura[BUF_SIZE];
		write(client_fd, mensagem, 1 + strlen(mensagem));
		memset(login, 0, sizeof(login));
		read(client_fd, login, BUF_SIZE - 1);
		if (strcmp(login, "1") == 0)
		// fazer login do agente de seguran�a
		{

			// recebe username

			char linha[BUF_SIZE];
			struct rg f;
			int c = 0;
			while (1)
			{
				fptr = fopen("utilizadores.txt", "r");
				if (fptr == NULL)
				{
					fputs("Error at opening File gestores \n", stderr);
					exit(1);
				}
				memset(leitura, 0, sizeof(leitura));
				read(client_fd, leitura, BUF_SIZE - 1);
				//printf("recebeu este username %s\n",leitura);
				while (fgets(linha, sizeof(linha), fptr))
				{
					memset(f.username, 0, sizeof(f.username));
					memset(f.password, 0, sizeof(f.password));
					memset(f.tipo, 0, sizeof(f.tipo));
					memset(f.validacao, 0, sizeof(f.validacao));
					int i = 0;
					int b = 0;
					int a = 0;
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == '\n')
							break;
						else if (linha[i] == ';')
						{
							a = a + 1;
						}
						else if (a == 0)
						{
							f.username[i] = linha[i];
						}
						else if (a == 1)
						{
							f.password[b] = linha[i];
							b = b + 1;
						}
						else if (a == 2)
						{
							f.tipo[0] = linha[i];
						}
						else if (a == 3)
						{
							f.validacao[0] = linha[i];
						}
					}
					// ja temos a informa��o toda da linha
					if (strcmp(f.username, leitura) == 0 && strcmp(f.tipo, "1") == 0)
					{
						c = 1;
						break;
					}
				}
				if (c == 1)
				{
					//escreve ao servidor que existe e fecha o while
					char existe[BUF_SIZE] = "existe";
					write(client_fd, existe, BUF_SIZE - 1);
					fclose(fptr);
					break;
				}
				else
				{
					// escreve ao servidor que nao existe e pede para voltar a pedir o username
					char existe[BUF_SIZE] = "nexiste";
					write(client_fd, existe, BUF_SIZE - 1);
				}
				fclose(fptr);
			}

			// se chegou aqui � pq indicou um utilizador que existe
			if (strcmp(f.validacao, "2") == 0)
			{
				char valido[BUF_SIZE] = "validado";
				write(client_fd, valido, BUF_SIZE - 1);
				write(client_fd, f.password, BUF_SIZE - 1);
				menuAS(client_fd, f.username, f.password);
			}
			else
			{
				char valido[BUF_SIZE] = "nvalidado";
				write(client_fd, valido, BUF_SIZE - 1);
			}
		}
		else if (strcmp(login, "2") == 0)
		//para fazer o registo do agente
		{
			char linha[BUF_SIZE];
			struct rg f;

			while (1)
			{
				int c = 0;
				fptr = fopen("utilizadores.txt", "r");
				if (fptr == NULL)
				{
					fputs("Error at opening File gestores \n", stderr);
					exit(1);
				}
				memset(leitura, 0, sizeof(leitura));
				read(client_fd, leitura, BUF_SIZE - 1);
				//printf("recebeu este username %s\n",leitura);
				while (fgets(linha, sizeof(linha), fptr))
				{
					int i = 0;
					memset(f.username, 0, sizeof(f.username));
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == ';')
							break;
						else
						{
							f.username[i] = linha[i];
						}
					}
					// ja temos a informa��o do username da linha
					if (strcmp(f.username, leitura) == 0)
					{
						c = 1;
						break;
					}
				}
				if (c == 1)
				{
					//escreve ao servidor que existe e pede para voltar a pedir o username
					char existe[BUF_SIZE] = "existe";
					write(client_fd, existe, BUF_SIZE - 1);
				}
				else
				{
					// escreve ao servidor que nao existe e fecha o while
					char existe[BUF_SIZE] = "nexiste";
					write(client_fd, existe, BUF_SIZE - 1);
					fclose(fptr);
					break;
				}
				fclose(fptr);
			}
			//se chegou aqui e pq indicou um utilizador que nao existe
			memset(f.username, 0, sizeof(0));
			strcat(f.username, leitura);
			memset(f.password, 0, sizeof(0));
			read(client_fd, f.password, BUF_SIZE - 1);
			fptr = fopen("utilizadores.txt", "a");
			if (fptr == NULL)
			{
				fputs("Error at opening File agentes", stderr);
				exit(1);
			}
			fprintf(fptr, "%s;%s;1;1;\n", f.username, f.password);
			fclose(fptr);
			char sucesso[BUF_SIZE] = "Registo efectuado com SUCESSO!!!";
			write(client_fd, sucesso, BUF_SIZE - 1);
		}
		else if (strcmp(login, "3"))
		{
			close(client_fd);
		}
		else
		{
			close(client_fd);
		}
	}
	close(client_fd);
}

// processos de gestor do sistema
void process_client_2(int client_fd)
{
	char verificacao[BUF_SIZE];
	char login[BUF_SIZE];
	memset(verificacao, 0, sizeof(verificacao));
	read(client_fd, verificacao, BUF_SIZE - 1);
	if (strcmp(verificacao, "2") == 0) // gestor
	{
		char admin[BUF_SIZE] = "Bem-vindo a aplicacao de Gestor de Sistema! \n\n1- Entrar\n2- Criar conta\n3- Sair\nDas opcoes escolha (1 ou 2 ou 3).\n";
		char leitura[BUF_SIZE];
		write(client_fd, admin, 1 + strlen(admin));
		memset(leitura, 0, sizeof(leitura));
		read(client_fd, leitura, BUF_SIZE - 1);
		if (strcmp(leitura, "1") == 0)
		{
			// fazer login do Gestor do sistema
			// recebe username

			char linha[BUF_SIZE];
			struct rg f;
			int c = 0;
			while (1)
			{
				fptr = fopen("utilizadores.txt", "r");
				if (fptr == NULL)
				{
					fputs("Error at opening File gestores \n", stderr);
					exit(1);
				}
				memset(leitura, 0, sizeof(leitura));
				read(client_fd, leitura, BUF_SIZE - 1);
				//printf("recebeu este username %s\n",leitura);
				while (fgets(linha, sizeof(linha), fptr))
				{
					memset(f.username, 0, sizeof(f.username));
					memset(f.password, 0, sizeof(f.password));
					memset(f.tipo, 0, sizeof(f.tipo));
					memset(f.validacao, 0, sizeof(f.validacao));
					int i = 0;
					int b = 0;
					int a = 0;
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == '\n')
							break;
						else if (linha[i] == ';')
						{
							a = a + 1;
						}
						else if (a == 0)
						{
							f.username[i] = linha[i];
						}
						else if (a == 1)
						{
							f.password[b] = linha[i];
							b = b + 1;
						}
						else if (a == 2)
						{
							f.tipo[0] = linha[i];
						}
						else if (a == 3)
						{
							f.validacao[0] = linha[i];
						}
					}
					// ja temos a informa��o toda da linha
					if (strcmp(f.username, leitura) == 0 && strcmp(f.tipo, "2") == 0)
					{
						c = 1;
						break;
					}
				}
				if (c == 1)
				{
					//escreve ao servidor que existe e fecha o while
					char existe[BUF_SIZE] = "existe";
					write(client_fd, existe, BUF_SIZE - 1);
					fclose(fptr);
					break;
				}
				else
				{
					// escreve ao servidor que nao existe e pede para voltar a pedir o username
					char existe[BUF_SIZE] = "nexiste";
					write(client_fd, existe, BUF_SIZE - 1);
				}
				fclose(fptr);
			}
			// se chegou aqui � pq indicou um utilizador que existe
			if (strcmp(f.validacao, "2") == 0)
			{
				char valido[BUF_SIZE] = "validado";
				write(client_fd, valido, BUF_SIZE - 1);
				write(client_fd, f.password, BUF_SIZE - 1);
				menuAGS(client_fd);
			}
			else
			{
				char valido[BUF_SIZE] = "nvalidado";
				write(client_fd, valido, BUF_SIZE - 1);
			}
		}
		else if (strcmp(leitura, "2") == 0)
		{
			// fazer registo do Gestor do sistema
			char linha[BUF_SIZE];
			struct rg f;

			while (1)
			{
				int c = 0;
				fptr = fopen("utilizadores.txt", "r");
				if (fptr == NULL)
				{
					fputs("Error at opening File gestores \n", stderr);
					exit(1);
				}
				memset(leitura, 0, sizeof(leitura));
				read(client_fd, leitura, BUF_SIZE - 1);
				//printf("recebeu este username %s\n",leitura);
				while (fgets(linha, sizeof(linha), fptr))
				{
					int i = 0;
					memset(f.username, 0, sizeof(f.username));
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == ';')
							break;
						else
						{
							f.username[i] = linha[i];
						}
					}
					// ja temos a informa��o do username da linha
					if (strcmp(f.username, leitura) == 0)
					{
						c = 1;
						break;
					}
				}
				if (c == 1)
				{
					//escreve ao servidor que existe e pede para voltar a pedir o username
					char existe[BUF_SIZE] = "existe";
					write(client_fd, existe, BUF_SIZE - 1);
				}
				else
				{
					// escreve ao servidor que nao existe e fecha o while
					char existe[BUF_SIZE] = "nexiste";
					write(client_fd, existe, BUF_SIZE - 1);
					fclose(fptr);
					break;
				}
				fclose(fptr);
			}
			//se chegou aqui e pq indicou um utilizador que nao existe
			memset(f.username, 0, sizeof(0));
			strcat(f.username, leitura);
			memset(f.password, 0, sizeof(0));
			read(client_fd, f.password, BUF_SIZE - 1);
			fptr = fopen("utilizadores.txt", "a");
			if (fptr == NULL)
			{
				fputs("Error at opening File agentes", stderr);
				exit(1);
			}
			fprintf(fptr, "%s;%s;2;1;\n", f.username, f.password);
			fclose(fptr);
			char sucesso[BUF_SIZE] = "Registo efectuado com SUCESSO!!!";
			write(client_fd, sucesso, BUF_SIZE - 1);
		}

		else if (strcmp(leitura, "3") == 0)
		{
			//sair do tcp
			close(client_fd);
		}
	}

	close(client_fd);
}

void erro(char *msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}

void menuAS(int client_fd, char *user1, char *password)
{
	char op1[BUF_SIZE], leitura[BUF_SIZE], linha[BUF_SIZE];
	struct rg f, g;
	printf("\nFez login e chegou aqui ...%s\n", user1);
	read(client_fd, op1, BUF_SIZE - 1);
	// faz a leitura se quer modificar conta ou ver ocorrencias
	//1- modificar conta
	if (strcmp(op1, "1") == 0)
	{
		//recebe informacao do que quer alterar (1username, 2password, 3apagar conta, 4voltar ao menu principal)
		memset(op1, 0, sizeof(op1));
		read(client_fd, op1, BUF_SIZE - 1);
		if (strcmp(op1, "1") == 0)
		{
			while (1)
			{
				int c = 0;
				fptr = fopen("utilizadores.txt", "r");
				if (fptr == NULL)
				{
					fputs("Error at opening File gestores \n", stderr);
					exit(1);
				}
				memset(leitura, 0, sizeof(leitura));
				// recebe o nome username
				read(client_fd, leitura, BUF_SIZE - 1);
				//printf("recebeu este username %s\n",leitura);
				while (fgets(linha, sizeof(linha), fptr))
				{
					memset(f.username, 0, sizeof(f.username));
					memset(f.password, 0, sizeof(f.password));
					memset(f.tipo, 0, sizeof(f.tipo));
					memset(f.validacao, 0, sizeof(f.validacao));
					int i = 0;
					int b = 0;
					int a = 0;
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == '\n')
							break;
						else if (linha[i] == ';')
						{
							a = a + 1;
						}
						else if (a == 0)
						{
							f.username[i] = linha[i];
						}
						else if (a == 1)
						{
							f.password[b] = linha[i];
							b = b + 1;
						}
						else if (a == 2)
						{
							f.tipo[0] = linha[i];
						}
						else if (a == 3)
						{
							f.validacao[0] = linha[i];
						}
					}
					// ja temos a informa��o toda da linha
					if (strcmp(f.username, leitura) == 0)
					{
						c = 1;
						break;
					}
				}
				if (c == 1)
				{
					//escreve ao servidor que existe e volta a pedir um novo username
					char existe[BUF_SIZE] = "existe";
					write(client_fd, existe, BUF_SIZE - 1);
					fclose(fptr);
				}
				else
				{
					// escreve ao servidor que nao existe e faz grava��o do novo username
					char existe[BUF_SIZE] = "nexiste";
					write(client_fd, existe, BUF_SIZE - 1);
					fclose(fptr);
				}
				printf(" \npassword que envia %s\n", password);
				write(client_fd, password, BUF_SIZE - 1);
				char gravar[BUF_SIZE];
				read(client_fd, gravar, BUF_SIZE - 1);

				FILE *a1;
				FILE *a2;

				a1 = fopen("utilizadores.txt", "r");
				a2 = fopen("versao2.txt", "w+");

				if (a1 == NULL)
				{
					fputs("Error at opening File utilizadores \n", stderr);
					exit(1);
				}

				if (a2 == NULL)
				{
					fputs("Erros at opening File versao2\n", stderr);
					exit(1);
				}

				char linha[BUF_SIZE];

				while (fgets(linha, sizeof(linha), a1))
				{
					memset(g.username, 0, sizeof(g.username));
					memset(g.password, 0, sizeof(g.password));
					memset(g.tipo, 0, sizeof(g.tipo));
					memset(g.validacao, 0, sizeof(g.validacao));
					int i = 0;
					int b = 0;
					int a = 0;
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == '\n')
						{
							break;
						}
						else if (linha[i] == ';')
						{
							a = a + 1;
						}
						else if (a == 0)
						{
							g.username[i] = linha[i];
						}
						else if (a == 1)
						{
							g.password[b] = linha[i];
							b = b + 1;
						}
						else if (a == 2)
						{
							g.tipo[0] = linha[i];
						}
						else if (a == 3)
						{
							g.validacao[0] = linha[i];
						}
					}
					//neste ponto temos toda a informa�ao da linha atual
					// neste ponto temos que perceber atraves da informa�ao dada das aplicacoes o que queremos mudar
					//1- alterar username
					//2- alterar password
					//3- alterar deletar conta
					// esta informa�ao tem que ser dada antes do ciclo while(fgets para perceber o que alterar e introduzir la este codigo
					//atraves do nome de utilizador dado para alterar a informa�ao mexemos nessa linha
					if (strcmp(g.username, user1) == 0)
					{
						//aqui este if tem que ser alterado dependendo do que se quer alterar
						// se for para alterar o username insere se a linha com o resto das informa�oes e o novo username
						// se for a password inseres o resto da informa�ao e muda se a password
						// se for deletar a conta nao inserimos nada
						// vou dar o exemplo se for para mudar o username qualquer coisa dizem eu explico
						fprintf(a2, "%s;%s;%s;%s;\n", gravar, g.password, g.tipo, g.validacao);
					}
					else
					{
						//este else serve para passar a informa��o que nao � preciso mexer para o novo ficheiro
						fprintf(a2, "%s;%s;%s;%s;\n", g.username, g.password, g.tipo, g.validacao);
					}
				}
				// aqui o novo ficheiro ja existe com a informa��o alterada
				fclose(a1);
				fclose(a2);

				remove("utilizadores.txt");
				rename("versao2.txt", "utilizadores.txt");
				break;
			}
		}
		else if (strcmp(op1, "2") == 0)
		{
			char gravar[BUF_SIZE];
			read(client_fd, gravar, BUF_SIZE - 1);

			FILE *a1;
			FILE *a2;

			a1 = fopen("utilizadores.txt", "r");
			a2 = fopen("versao2.txt", "w+");

			if (a1 == NULL)
			{
				fputs("Error at opening File utilizadores \n", stderr);
				exit(1);
			}

			if (a2 == NULL)
			{
				fputs("Erros at opening File versao2\n", stderr);
				exit(1);
			}

			char linha[BUF_SIZE];

			while (fgets(linha, sizeof(linha), a1))
			{
				memset(g.username, 0, sizeof(g.username));
				memset(g.password, 0, sizeof(g.password));
				memset(g.tipo, 0, sizeof(g.tipo));
				memset(g.validacao, 0, sizeof(g.validacao));
				int i = 0;
				int b = 0;
				int a = 0;
				for (i = 0; i < (strlen(linha) - 1); i++)
				{
					if (linha[i] == '\n')
					{
						break;
					}
					else if (linha[i] == ';')
					{
						a = a + 1;
					}
					else if (a == 0)
					{
						g.username[i] = linha[i];
					}
					else if (a == 1)
					{
						g.password[b] = linha[i];
						b = b + 1;
					}
					else if (a == 2)
					{
						g.tipo[0] = linha[i];
					}
					else if (a == 3)
					{
						g.validacao[0] = linha[i];
					}
				}
				//neste ponto temos toda a informa�ao da linha atual
				// neste ponto temos que perceber atraves da informa�ao dada das aplicacoes o que queremos mudar
				//1- alterar username
				//2- alterar password
				//3- alterar deletar conta
				// esta informa�ao tem que ser dada antes do ciclo while(fgets para perceber o que alterar e introduzir la este codigo
				//atraves do nome de utilizador dado para alterar a informa�ao mexemos nessa linha
				if (strcmp(g.username, user1) == 0)
				{
					//aqui este if tem que ser alterado dependendo do que se quer alterar
					// se for para alterar o username insere se a linha com o resto das informa�oes e o novo username
					// se for a password inseres o resto da informa�ao e muda se a password
					// se for deletar a conta nao inserimos nada
					// vou dar o exemplo se for para mudar o username qualquer coisa dizem eu explico
					fprintf(a2, "%s;%s;%s;%s;\n", g.username, gravar, g.tipo, g.validacao);
				}
				else
				{
					//este else serve para passar a informa��o que nao � preciso mexer para o novo ficheiro
					fprintf(a2, "%s;%s;%s;%s;\n", g.username, g.password, g.tipo, g.validacao);
				}
			}
			// aqui o novo ficheiro ja existe com a informa��o alterada
			fclose(a1);
			fclose(a2);

			remove("utilizadores.txt");
			rename("versao2.txt", "utilizadores.txt");
		}
		else if (strcmp(op1, "3") == 0)
		{
			write(client_fd, password, BUF_SIZE - 1);
			char ver[BUF_SIZE];
			read(client_fd, ver, BUF_SIZE - 1);
			if (strcmp(ver, "1") == 0)
			{
				FILE *a1;
				FILE *a2;

				a1 = fopen("utilizadores.txt", "r");
				a2 = fopen("versao2.txt", "w+");

				if (a1 == NULL)
				{
					fputs("Error at opening File utilizadores \n", stderr);
					exit(1);
				}

				if (a2 == NULL)
				{
					fputs("Erros at opening File versao2\n", stderr);
					exit(1);
				}

				char linha[BUF_SIZE];

				while (fgets(linha, sizeof(linha), a1))
				{
					memset(g.username, 0, sizeof(g.username));
					memset(g.password, 0, sizeof(g.password));
					memset(g.tipo, 0, sizeof(g.tipo));
					memset(g.validacao, 0, sizeof(g.validacao));
					int i = 0;
					int b = 0;
					int a = 0;
					for (i = 0; i < (strlen(linha) - 1); i++)
					{
						if (linha[i] == '\n')
						{
							break;
						}
						else if (linha[i] == ';')
						{
							a = a + 1;
						}
						else if (a == 0)
						{
							g.username[i] = linha[i];
						}
						else if (a == 1)
						{
							g.password[b] = linha[i];
							b = b + 1;
						}
						else if (a == 2)
						{
							g.tipo[0] = linha[i];
						}
						else if (a == 3)
						{
							g.validacao[0] = linha[i];
						}
					}
					//neste ponto temos toda a informa�ao da linha atual
					// neste ponto temos que perceber atraves da informa�ao dada das aplicacoes o que queremos mudar
					//1- alterar username
					//2- alterar password
					//3- alterar deletar conta
					// esta informa�ao tem que ser dada antes do ciclo while(fgets para perceber o que alterar e introduzir la este codigo
					//atraves do nome de utilizador dado para alterar a informa�ao mexemos nessa linha
					if (strcmp(g.username, user1) == 0)
					{
						//aqui este if tem que ser alterado dependendo do que se quer alterar
						// se for para alterar o username insere se a linha com o resto das informa�oes e o novo username
						// se for a password inseres o resto da informa�ao e muda se a password
						// se for deletar a conta nao inserimos nada
						// vou dar o exemplo se for para mudar o username qualquer coisa dizem eu explico

						//como ele esta no utilizador para apagar ele nao grava nada
					}
					else
					{
						//este else serve para passar a informa��o que nao � preciso mexer para o novo ficheiro
						fprintf(a2, "%s;%s;%s;%s;\n", g.username, g.password, g.tipo, g.validacao);
					}
				}
				// aqui o novo ficheiro ja existe com a informa��o alterada
				fclose(a1);
				fclose(a2);

				remove("utilizadores.txt");
				rename("versao2.txt", "utilizadores.txt");
			}
		}
		else if (strcmp(op1, "4") == 0)
		{
		}
	}
	else if (strcmp(op1, "2") == 0)
	{
		char op[BUF_SIZE];
		int i;
		int a;
		int b;
		int c;
		int d;
		int e;
		int x;
		char *ocorrencias = 0;

		char mensagem1[BUF_SIZE] = "  Escolha a opção:\n 1 - Todas as ocorrências.\n 2 - Local. \n 3 - Nome.\n 4 - Data. \n";

		write(client_fd, mensagem1, BUF_SIZE - 1); // envia mensagem de login sucesso

		read(client_fd, op, BUF_SIZE - 1);

		printf("Escolheu %s \n", op);

		if (strcmp(op, "1") == 0)
		{ //escolheu opção 1 - Listar todas as ocorrências

			fptr = fopen("ocorrencias.txt", "r");

			if (fptr)
			{

				fseek(fptr, 0, SEEK_END);
				long length = ftell(fptr);
				fseek(fptr, 0, SEEK_SET);
				ocorrencias = malloc(length);

				fread(ocorrencias, 1, length, fptr);
				fclose(fptr);

				write(client_fd, ocorrencias, 1 + strlen(ocorrencias));
			}
			else
			{

				printf("Documento não existe. \n");
			}
			fclose(fptr);
		}

		if (strcmp(op, "2") == 0)
		{ //escolheu opção 2 - Local

			struct ocorrencia f;
			char leitura[BUF_SIZE];
			char linha[BUF_SIZE];
			char existe[BUF_SIZE];

			fptr = fopen("ocorrencias.txt", "r");

			if (fptr == NULL)
			{
				fputs("Error at opening File gestores \n", stderr);
				exit(1);
			}

			memset(leitura, 0, sizeof(leitura));
			read(client_fd, leitura, BUF_SIZE - 1);
			//printf("recebeu este local %s\n",leitura);
			x = 0;
			while (fgets(linha, sizeof(linha), fptr))
			{
				memset(f.data, 0, sizeof(f.data));
				memset(f.hora, 0, sizeof(f.hora));
				memset(f.local, 0, sizeof(f.local));
				memset(f.tipo, 0, sizeof(f.tipo));
				memset(f.nome, 0, sizeof(f.nome));

				a = 0;
				b = 0;
				c = 0;
				d = 0;
				e = 0;

				for (i = 0; i < (strlen(linha) - 1); i++)
				{
					if (linha[i] == '\n')
					{

						break;
					}
					else if (linha[i] == ';')
					{

						a = a + 1;
					}
					else if (a == 0)
					{

						f.data[i] = linha[i];
					}
					else if (a == 1)
					{

						f.hora[b] = linha[i];
						b = b + 1;
					}
					else if (a == 2)
					{
						f.local[c] = linha[i];
						c = c + 1;
					}
					else if (a == 3)
					{
						f.tipo[d] = linha[i];
						d = d + 1;
					}
					else if (a == 4)
					{

						f.nome[e] = linha[i];
						e = e + 1;
					}

					if (strcmp(f.local, leitura) == 0)
					{

						strcat(existe, linha);

						x = 1;
						break;
					}
				}
				// ja temos a informa��o toda da linha
			}
			if (x == 1)
			{
				//escreve ao servidor que existe e fecha o while
				write(client_fd, existe, BUF_SIZE - 1);
			}
			else
			{
				// escreve ao servidor que nao existe e pede para voltar a pedir o username
				char existe[BUF_SIZE] = "Não existe \n";
				write(client_fd, existe, BUF_SIZE - 1);
			}
			fclose(fptr);
		}

		if (strcmp(op, "3") == 0)
		{ //escolheu opção 3 - Nome

			struct ocorrencia f;
			char leitura[BUF_SIZE];
			char linha[BUF_SIZE];
			char existe[BUF_SIZE];

			fptr = fopen("ocorrencias.txt", "r");

			if (fptr == NULL)
			{
				fputs("Error at opening File gestores \n", stderr);
				exit(1);
			}

			memset(leitura, 0, sizeof(leitura));
			read(client_fd, leitura, BUF_SIZE - 1);
			//printf("recebeu este nome %s\n",leitura);
			x = 0;
			while (fgets(linha, sizeof(linha), fptr))
			{
				memset(f.data, 0, sizeof(f.data));
				memset(f.hora, 0, sizeof(f.hora));
				memset(f.local, 0, sizeof(f.local));
				memset(f.tipo, 0, sizeof(f.tipo));
				memset(f.nome, 0, sizeof(f.nome));

				a = 0;
				b = 0;
				c = 0;
				d = 0;
				e = 0;

				for (i = 0; i < (strlen(linha) - 1); i++)
				{
					if (linha[i] == '\n')
					{

						break;
					}
					else if (linha[i] == ';')
					{

						a = a + 1;
					}
					else if (a == 0)
					{

						f.data[i] = linha[i];
					}
					else if (a == 1)
					{

						f.hora[b] = linha[i];
						b = b + 1;
					}
					else if (a == 2)
					{
						f.local[c] = linha[i];
						c = c + 1;
					}
					else if (a == 3)
					{
						f.tipo[d] = linha[i];
						d = d + 1;
					}
					else if (a == 4)
					{

						f.nome[e] = linha[i];
						e = e + 1;
					}

					if (strcmp(f.nome, leitura) == 0)
					{
						printf("nome : %s \n", f.nome);
						printf("leitura : %s \n", leitura);

						strcat(existe, linha); // concatena a nova linha com uma ja existente no caso de haver mais do que uma ocorrência com o mesmo local
						printf("%s\n", existe);

						x = 1;
						break;
					}
				}
				// ja temos a informa��o toda da linha
			}
			if (x == 1)
			{
				//escreve ao servidor que existe e fecha o while
				write(client_fd, existe, BUF_SIZE - 1);
				printf("%s \n", existe);
			}
			else
			{
				// escreve ao servidor que nao existe e pede para voltar a pedir o username
				char existe[BUF_SIZE] = "Não existe \n";
				write(client_fd, existe, BUF_SIZE - 1);
			}
			fclose(fptr);
		}
		if (strcmp(op, "4") == 0)
		{ //escolheu opção 4 - data

			struct ocorrencia f;
			char leitura[BUF_SIZE];
			char linha[BUF_SIZE];
			char existe[BUF_SIZE];

			fptr = fopen("ocorrencias.txt", "r");

			if (fptr == NULL)
			{
				fputs("Error at opening File gestores \n", stderr);
				exit(1);
			}

			memset(leitura, 0, sizeof(leitura));
			read(client_fd, leitura, BUF_SIZE - 1);
			//printf("recebeu este nome %s\n",leitura);
			x = 0;
			while (fgets(linha, sizeof(linha), fptr))
			{
				memset(f.data, 0, sizeof(f.data));
				memset(f.hora, 0, sizeof(f.hora));
				memset(f.local, 0, sizeof(f.local));
				memset(f.tipo, 0, sizeof(f.tipo));
				memset(f.nome, 0, sizeof(f.nome));

				a = 0;
				b = 0;
				c = 0;
				d = 0;
				e = 0;

				for (i = 0; i < (strlen(linha) - 1); i++)
				{
					if (linha[i] == '\n')
					{

						break;
					}
					else if (linha[i] == ';')
					{

						a = a + 1;
					}
					else if (a == 0)
					{

						f.data[i] = linha[i];
					}
					else if (a == 1)
					{

						f.hora[b] = linha[i];
						b = b + 1;
					}
					else if (a == 2)
					{
						f.local[c] = linha[i];
						c = c + 1;
					}
					else if (a == 3)
					{
						f.tipo[d] = linha[i];
						d = d + 1;
					}
					else if (a == 4)
					{

						f.nome[e] = linha[i];
						e = e + 1;
					}

					if (strcmp(f.data, leitura) == 0)
					{
						printf("data : %s \n", f.nome);
						printf("leitura : %s \n", leitura);

						strcat(existe, linha); // concatena a nova linha com uma ja existente no caso de haver mais do que uma ocorrência com o mesmo local
						printf("%s\n", existe);

						x = 1;
						break;
					}
				}
				// ja temos a informa��o toda da linha
			}
			if (x == 1)
			{
				//escreve ao servidor que existe e fecha o while
				write(client_fd, existe, BUF_SIZE - 1);
				printf("%s \n", existe);
			}
			else
			{
				// escreve ao servidor que nao existe e pede para voltar a pedir o username
				char existe[BUF_SIZE] = "Não existe \n";
				write(client_fd, existe, BUF_SIZE - 1);
			}
			fclose(fptr);
		}
	}
}

void menuAGS(int client_fd)
{
	char op[BUF_SIZE];
	char *ocorrencias = 0;
	memset(op, 0, sizeof(op));
	read(client_fd, op, BUF_SIZE - 1);
	if (strcmp(op, "1") == 0)
	{
		printf("\npassa o if\n");
		fptr = fopen("utilizadores.txt", "r");

		fseek(fptr, 0, SEEK_END);
		long length = ftell(fptr);
		fseek(fptr, 0, SEEK_SET);
		ocorrencias = malloc(length);

		fread(ocorrencias, 1, length, fptr);
		fclose(fptr);
		printf(" \nutilizadores %s", ocorrencias);
		write(client_fd, ocorrencias, 1 + strlen(ocorrencias));
	}
	else if (strcmp(op, "2") == 0)
	{
		struct rg g;
		char user[BUF_SIZE];
		memset(user, 0, sizeof(user));
		read(client_fd, user, BUF_SIZE - 1);
		FILE *a1;
		FILE *a2;

		a1 = fopen("utilizadores.txt", "r");
		a2 = fopen("versao2.txt", "w+");

		if (a1 == NULL)
		{
			fputs("Error at opening File utilizadores \n", stderr);
			exit(1);
		}

		if (a2 == NULL)
		{
			fputs("Erros at opening File versao2\n", stderr);
			exit(1);
		}

		char linha[BUF_SIZE];

		while (fgets(linha, sizeof(linha), a1))
		{
			memset(g.username, 0, sizeof(g.username));
			memset(g.password, 0, sizeof(g.password));
			memset(g.tipo, 0, sizeof(g.tipo));
			memset(g.validacao, 0, sizeof(g.validacao));
			int i = 0;
			int b = 0;
			int a = 0;
			for (i = 0; i < (strlen(linha) - 1); i++)
			{
				if (linha[i] == '\n')
				{
					break;
				}
				else if (linha[i] == ';')
				{
					a = a + 1;
				}
				else if (a == 0)
				{
					g.username[i] = linha[i];
				}
				else if (a == 1)
				{
					g.password[b] = linha[i];
					b = b + 1;
				}
				else if (a == 2)
				{
					g.tipo[0] = linha[i];
				}
				else if (a == 3)
				{
					g.validacao[0] = linha[i];
				}
			}
			//neste ponto temos toda a informa�ao da linha atual
			// neste ponto temos que perceber atraves da informa�ao dada das aplicacoes o que queremos mudar
			//1- alterar username
			//2- alterar password
			//3- alterar deletar conta
			// esta informa�ao tem que ser dada antes do ciclo while(fgets para perceber o que alterar e introduzir la este codigo
			//atraves do nome de utilizador dado para alterar a informa�ao mexemos nessa linha
			if (strcmp(g.username, user) == 0)
			{
				//aqui este if tem que ser alterado dependendo do que se quer alterar
				// se for para alterar o username insere se a linha com o resto das informa�oes e o novo username
				// se for a password inseres o resto da informa�ao e muda se a password
				// se for deletar a conta nao inserimos nada
				// vou dar o exemplo se for para mudar o username qualquer coisa dizem eu explico

				//como ele esta no utilizador a autorizar
				fprintf(a2, "%s;%s;%s;2;\n", g.username, g.password, g.tipo);
			}
			else
			{
				//este else serve para passar a informa��o que nao � preciso mexer para o novo ficheiro
				fprintf(a2, "%s;%s;%s;%s;\n", g.username, g.password, g.tipo, g.validacao);
			}
		}
		// aqui o novo ficheiro ja existe com a informa��o alterada
		fclose(a1);
		fclose(a2);

		remove("utilizadores.txt");
		rename("versao2.txt", "utilizadores.txt");
	}
}
