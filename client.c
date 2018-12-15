#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

// for receiving
#define SHM_KEY1 800
#define SHM_KEY2 801
#define	SHM_KEY3 802
#define SHM_KEY4 803

// for sending
#define SHM_KEY10 804
#define SHM_KEY11 805
#define SHM_KEY12 806
#define SHM_KEY13 807
#define SHM_KEY14 808
#define SHM_KEY15 809

void loading(unsigned int millisecond){
	for(int i=0;i<4;i++){
		usleep(millisecond);
		printf("\b\\");
		fflush(stdout);
		usleep(millisecond);
		printf("\b|");
		fflush(stdout);
		usleep(millisecond);
		printf("\b/");
		fflush(stdout);
		usleep(millisecond);
		printf("\b-");
		fflush(stdout);
	}
}

int main(int argc, char *argv[]){
	
	printf("==========================================================================\n");
	printf("###### # #      ######     ####  #    #  ####   ####  ##### ###### #####  \n");
	printf("#      # #      #         #      #    # #    # #    #   #   #      #    # \n");
	printf("#####  # #      #####      ####  ###### #    # #    #   #   #####  #    # \n");
	printf("#      # #      #              # #    # #    # #    #   #   #      #####  \n");
	printf("#      # #      #         #    # #    # #    # #    #   #   #      #   #  \n");
	printf("#      # ###### ######     ####  #    #  ####   ####    #   ###### #    # \n");
	printf("==========================================================================\n\n");

	if(argc>1 && argc<4){
		if(strcmp(argv[1],"-r")==0&& argv[2]){
			
			int shm_id[4];
			void *shared_mem[4];
			
			unsigned long file_size;
			char *file_data;
			unsigned int file_name_len;
			char *file_name;
			
			printf("## Start receive data.\n");
			printf("## Get shared memory to save");
			
			shm_id[0] = shmget(SHM_KEY1, 0, IPC_CREAT|0666);
			// check shared memory
			if(shm_id[0] == -1){
				printf("\n## The master process is not running.\n");
				return -1;
			}
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			file_size = atoi(shared_mem[0]);
			printf("..1");
			
			shm_id[1] = shmget(SHM_KEY2, 0, IPC_CREAT|0666);
			file_data=shmat(shm_id[1], (void *)0,0);
			printf("..2");
			
			shm_id[2] = shmget(SHM_KEY3, 0, IPC_CREAT|0666);
			shared_mem[1] = shmat(shm_id[2], (void *)0,0);
			file_name_len = atoi(shared_mem[1]);
			printf("..3");

			shm_id[3] = shmget(SHM_KEY4, 0,IPC_CREAT|0666);
			file_name=shmat(shm_id[3], (void *)0,0);
			printf("..4");

			// pointer to string
			FILE *fp = fopen(argv[2],"w");
			fwrite((char *)file_data, 1, file_size, fp);
			fclose(fp);
			printf("..5\n");
			
			printf("## Received success!\n");
			shmdt(shared_mem[0]);
			shmdt(file_data);
			shmdt(shared_mem[1]);
			shmdt(file_name);

		}
		else if(strcmp(argv[1],"-s")==0){
			
			unsigned long filelen;
			char *buf;

			unsigned int count;
			unsigned int user;
			unsigned int working;
			
			int shm_id[6];
			void *shared_mem[6];

			unsigned long file_size;
			char *file_data;
			char *file_data2;
			unsigned int file_name_len;
			char *file_name;

			printf("## Start sending data.\n");
			FILE *fp = fopen(argv[2],"r");
			fseek(fp,0,SEEK_END);
			filelen = ftell(fp);
			rewind(fp);
			buf = (char *)malloc((filelen+1)*sizeof(char));
			fread(buf, filelen, 1, fp);
			fclose(fp);
			printf("## Loaded file. %dbytes.\n");
			
			// read working 0:finished 1: working
			shm_id[0] = shmget(SHM_KEY15, 0, IPC_CREAT|0666);
			// check shared memory
			if(shm_id[0] == -1){
				printf("\n## The master process is not running.\n");
				return -1;
			}
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			
			printf("## waiting for my turn.. -");
			while(1){
				loading(100000);
				working = atoi(shared_mem[0]);
				if(working == 0){
					// switch work mode
					working=1;
					sprintf((char *)shared_mem[0], "%d",working);
										
					// read count and count++
					shm_id[1] = shmget(SHM_KEY10, 0, IPC_CREAT|0666);
					shared_mem[1]=shmat(shm_id[1], (void *)0,0);
					count = atoi(shared_mem[1]);
					count++;
					sprintf((char *)shared_mem[1], "%d",count);
					
					printf("\n## Connected. \n",count);

					// file_size
					shm_id[2] = shmget(SHM_KEY11, sizeof(file_size), IPC_CREAT|0666);
					shared_mem[2]=shmat(shm_id[2], (void *)0,0);
					file_size = filelen;
					sprintf((char *)shared_mem[2], "%ld",file_size);
					printf("%d %d %s\n",file_size,filelen,shared_mem[2]);

					// file_data
					shm_id[3] = shmget(SHM_KEY12, filelen, IPC_CREAT|0666);
					file_data=shmat(shm_id[3], (void *)0,0);
					strcpy(file_data,buf);
					//file_data = buf;
					free(buf);

					// file_name_len
					shm_id[4] = shmget(SHM_KEY13, sizeof(file_name_len), IPC_CREAT|0666);
					shared_mem[3]=shmat(shm_id[4], (void *)0,0);
					file_name_len = strlen(argv[2]);
					sprintf((char *)shared_mem[3], "%d",file_name_len);

					// file_name
					shm_id[5] = shmget(SHM_KEY14, file_name_len, IPC_CREAT|0666);
					file_name=shmat(shm_id[5], (void *)0,0);
					strcpy(file_name,argv[2]);
					
					// switch work mode
					working=2;
					sprintf((char *)shared_mem[0], "%d",working);
					printf("## Transfer success!\n");
					break;
				}
			}
			shmdt(shared_mem[0]);
			shmdt(shared_mem[1]);
			shmdt(shared_mem[2]);
			shmdt(shared_mem[3]);
			shmdt(file_data);
			shmdt(file_name_len);
			shmdt(file_name);
			return 0;
		}
		return 0;
	}
	printf("## Parameter error, please retry it.\n");
	return 0;
}
