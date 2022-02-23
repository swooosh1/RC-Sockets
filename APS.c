//APS - Aplicacao para o profissional de saude
//Cliente UDP

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define BUF_SIZE 1024
#define BUFFSZ 1024

int main()
{

	int fd[2]; // cria pipe entre processos
	if (pipe(fd) == -1 ){

		printf("PIPE ERROR \n");
	}
	
	
	
	
	int pid = fork();

	if (pid == 0) //processo de funcionalidades normais 
	{

		close(fd[0]);


		int clientSocket, portNum, nBytes;
		char buffer[1024], buffer1[1024];
		struct sockaddr_in serverAddr;
		socklen_t addr_size;

		/*Create UDP socket*/
		clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

		/*Configure settings in address struct*/
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(7891);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

		/*Initialize size variable to be used later on*/
		addr_size = sizeof serverAddr;

		//Variáveis do programa

		char pontovirgula[] = ";";
		char local[BUFFSZ];
		char tipo[BUFFSZ];
		char nome[BUFFSZ];
		char TOTAL[BUFFSZ];
		char opc[10];
		char anonimo[BUFFSZ] = "Anónimo";
		char user[BUFFSZ];

		system("clear"); //clear na console

		memset(local, 0, sizeof(local));
		memset(tipo, 0, sizeof(tipo));
		memset(nome, 0, sizeof(nome));
		memset(TOTAL, 0, sizeof(TOTAL));
		//Aquisição de dados ao cliente
		//strcspn copia a string inicial para outra ignorando os caracteres que se encontram no segundo argumento da função, neste caso \n

		printf("Aplicação para  o Profissional de Saúde (APS) \n Bem vindo!\n");

		printf("Escolha a sua opção: \n 1 - Login.\n 2 - Criar conta.\n 9 - Help \n 0 - Sair. \n");

		scanf("%s", opc);
		int nopc = atoi(opc);
		sendto(clientSocket, opc, strlen(opc) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);

		if (nopc == 9){

		system("clear");
		printf("Login - Permite-lhe aceder a sua conta atraves das suas credenciais \n Criar conta - Caso ainda nao tenha conta pode criar a sua\n inserindo um username e password, a sua conta ficara pendente de validacao por parte do admin de sistema.\n Sair - Fecha a aplicacao. \n\n");

		}
		if (nopc == 1)
		{
			//fazer login
			system("clear");
			printf("\nPara fazer login precisamos de algumas informacoes...\n");
			printf("Username: ");
			memset(buffer, 0, sizeof(buffer));
			scanf("%s", buffer);
			strcpy(user, buffer);
			sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);
			while (1)
			{
				//envia username

				memset(buffer, 0, sizeof(buffer));
				recvfrom(clientSocket, buffer, 1024, 0, NULL, NULL);
				if (strcmp(buffer, "existe") == 0)
				{
					break;
				}
				else
				{
					system("clear");
					printf("\nO utilizador indicado nao existe por favor volte a tentar: ");
					memset(buffer, 0, sizeof(buffer));
					scanf("%s", buffer);
					sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);
				}
			}
			// se chega aqui o utilizador existe logo vamos ver se esta validado
			memset(buffer, 0, sizeof(buffer));
			recvfrom(clientSocket, buffer, 1024, 0, NULL, NULL);
			//printf(" validacao %s \n",buffer);
			if (strcmp(buffer, "validado") == 0)
			{
				memset(buffer1, 0, sizeof(buffer1));
				recvfrom(clientSocket, buffer1, 1024, 0, NULL, NULL);
				system("clear");
				printf("\nPassword: ");
				memset(buffer, 0, sizeof(buffer));
				scanf("%s", buffer);
				while (strcmp(buffer, buffer1) != 0)
				{
					system("clear");
					printf("\nPassword incorreta, volte a repetir: ");
					memset(buffer, 0, sizeof(buffer));
					scanf("%s", buffer);
				}
				system("clear");
				printf("\nLogin Efectuado com SUCESSO!!!\n\n");


				printf("\n Menu \n 1 - Inserir ocorrência \n 2 - ALARME\n 9 - Help \n 0 - Exit\n");
				
				memset(buffer, 0, sizeof(buffer));
				scanf("%s", buffer);
				buffer[strcspn(buffer, "\n")] = 0;
				

				if(strcmp(buffer,"9") == 0){
					
					system("clear");
					printf("Inserir ocorrencia - permite registar uma ocorrencia, inserindo os seus dados, local, data, tipo de ocorrencia\n ALARME - Comunica as autoridades que esta a ocorrer uma situacao de perigo eminente! \n\n");
					
				}
				if(strcmp(buffer,"1") == 0){



					char data[BUFFSZ];
					char hora[BUFFSZ];
					time_t now = time(NULL);
					struct tm *t = localtime(&now);

					strftime(data, sizeof(data) - 1, "%d/%m/%Y", t);
					//printf("Current Date: %s \n", data);

					strftime(hora, sizeof(hora) - 1, "%H:%M", t);
					//printf("Current hour: %s \n", hora);

					printf("Aplicação para profissional de saúde\n");

					printf("Data da occorencia: %s \n", data);
					strcat(data, pontovirgula);
					strcat(TOTAL, data);

					printf("Hora: %s \n", hora);
					strcat(hora, pontovirgula);
					strcat(TOTAL, hora);

					fflush(stdin);
					fflush(stdout);

					printf("Local:\n");
					fgets(local, 1024, stdin);
					local[strcspn(local, "\n")] = 0;
					strcat(local, pontovirgula);
					strcat(TOTAL, local);

					printf("Tipo de Agressão:\n");
					fgets(tipo, 1024, stdin);
					tipo[strcspn(tipo, "\n")] = 0;
					strcat(tipo, pontovirgula);
					strcat(TOTAL, tipo);

					printf("Deseja anonimato?\n 1 - Sim \n 2 - Não \n");
					fgets(nome, 1024, stdin);
					nome[strcspn(nome, "\n")] = 0;
					if (strcmp(nome, "1") == 0)
					{

						printf("%s\n", anonimo);
						strcat(anonimo, pontovirgula);
						strcat(TOTAL, anonimo);
					}

					else if (strcmp(nome, "2") == 0)
					{

						printf("%s\n", user);
						strcat(user, pontovirgula);
						strcat(TOTAL, user);
					}

					// Modo de funcionamento: A cada pedido ao utilizador concatenamos a string com um ponto e virgula
					//e de seguida juntamos os dados a string principal

					printf("Voce inseriu: %s\n", TOTAL);

					nBytes = strlen(TOTAL) + 1;

					/*Send message to server*/
					sendto(clientSocket, TOTAL, nBytes, 0, (struct sockaddr *)&serverAddr, addr_size);

					/*Receive message from server*/
					nBytes = recvfrom(clientSocket, buffer, 1024, 0, NULL, NULL);

					printf("O nosso servidor diz: %s\n", buffer);


				}


				else if (strcmp(buffer,"2") == 0){


					write(fd[1], user , sizeof(user));

					close(fd[1]);

					printf("Enviou %s \n" , user);
				}

				
			}
			else
			{
				printf("\nO utilizador existe mas nao se encontra validado.\n");
				exit(-1);
			}
		}
		else if (nopc == 2)
		{

			char password[BUF_SIZE], password1[BUF_SIZE];
			//fazer registo de novo agente de seguran�a
			system("clear");
			printf("\nPara procedermos ao registo precisamos de algumas informacoes...\n");
			printf("Username: ");
			memset(buffer, 0, sizeof(buffer));
			scanf("%s", buffer);
			sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);
			while (1)
			{
				//envia username

				memset(buffer, 0, sizeof(buffer));
				recvfrom(clientSocket, buffer, 1024, 0, (struct sockaddr *)&serverAddr, &addr_size);
				if (strcmp(buffer, "nexiste") == 0)
				{
					//o utilizador nao existe e sai do ciclo
					break;
				}
				else
				{
					system("clear");
					printf("\nO utilizador indicado ja existe por favor volte a tentar: ");
					memset(buffer, 0, sizeof(buffer));
					scanf("%s", buffer);
					sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);
				}
			}
			system("clear");
			printf("\nPor ultimo indique uma password: ");
			scanf("%s", password);
			printf("Repita a password:	");
			scanf("%s", password1);
			printf("\n");
			while (strcmp(password, password1) != 0)
			{
				system("clear");
				printf("As passwords nao coincidem por favor volte a inserir:	 ");
				scanf("%s", password);
				printf("Repita a password:	");
				scanf("%s", password1);
			}
			sendto(clientSocket, password, strlen(password) + 1, 0, (struct sockaddr *)&serverAddr, addr_size);
			memset(buffer, 0, sizeof(buffer));
			recvfrom(clientSocket, buffer, 1024, 0, (struct sockaddr *)&serverAddr, &addr_size);
			printf("\n%s \n", buffer);
		}
		else
		{
			return 0;
		}

		return 0;
	}

	else { // alarme


		close(fd[1]);

		int clientSocket, portNum, nBytes;
		char buffer[1024], buffer1[1024];
		struct sockaddr_in serverAddr;
		socklen_t addr_size;

		/*Create UDP socket*/
		clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

		/*Configure settings in address struct*/
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(7892);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

		/*Initialize size variable to be used later on*/
		addr_size = sizeof serverAddr;


		read(fd[0], buffer, sizeof(buffer));
		
		close(fd[0]);

		printf("ALARME : %s \n", buffer);
		sendto(clientSocket, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddr, addr_size); //envia mensagem de alarme


	}

}



void menu()
{
	printf("Menu principal...\n\n");
	exit(-1);
}
