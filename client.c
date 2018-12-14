#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

// for receiving
#define SHM_KEY1 70
#define SHM_KEY2 71
#define	SHM_KEY3 72
#define SHM_KEY4 73

// for sending
#define SHM_KEY10 80
#define SHM_KEY11 81
#define SHM_KEY12 82
#define SHM_KEY13 83
#define SHM_KEY14 84
#define SHM_KEY15 85

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
	
	if(argc>1 && argc<4){
		if(strcmp(argv[1],"-r")==0&& argv[2]){
			int shm_id[4];
			void *shared_mem[4];
			
			unsigned long file_size;
			char *file_data;
			unsigned int file_name_len;
			char *file_name;

			shm_id[0] = shmget(SHM_KEY1, sizeof(file_size), IPC_CREAT|0666);
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			file_size = atoi(shared_mem[0]);
			printf("1\n");
			
			shm_id[1] = shmget(SHM_KEY2, file_size, IPC_CREAT|0666);
			file_data=shmat(shm_id[1], (void *)0,0);
			printf("2\n");
			
			shm_id[2] = shmget(SHM_KEY3, sizeof(file_name_len), IPC_CREAT|0666);
			shared_mem[1] = shmat(shm_id[2], (void *)0,0);
			file_name_len = atoi(shared_mem[1]);
			printf("%d\n",file_name_len);
			printf("3\n");

			shm_id[3] = shmget(SHM_KEY4, file_name_len, IPC_CREAT|0666);
			file_name=shmat(shm_id[3], (void *)0,0);
			
			FILE *fp;
			printf("4\n");
			// pointer to string
			fp = fopen(argv[2],"w");
			fwrite((char *)file_data, 1, file_size, fp);
			fclose(fp);

			printf("shm_id: %d %d %d %d\n",shm_id[0],shm_id[1],shm_id[2],shm_id[3]);
			printf("%d\n",file_size);
			printf("%s\n",(char *)file_data);
			printf("%d\n",file_name_len);
			printf("%s\n",file_name);
			sleep(1);

			/* clear shared_memory
			for(int i=0;i<4;i++)
				shmctl(shm_id[i], IPC_RMID, 0);
			 */
		}
		else if(strcmp(argv[1],"-s")==0){
			
			int shm_id[6];
			void *shared_mem[6];

			long filelen;
			char *buf;

			unsigned int count;
			unsigned int user;
			unsigned int working;

			unsigned int file_size;
			char *file_data;
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
			printf("## Loaded file. bytes: %d\n",filelen);
			
			// read working 0:finished 1: working
			shm_id[0] = shmget(SHM_KEY15, sizeof(unsigned int), IPC_CREAT|0666);
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			
			printf("## waiting for my turn.. -");
			while(1){
				loading(100000);
				working = atoi(shared_mem[0]);
				if(working == 0){
					// switch work mode
					working=1;
					sprintf((char *)shared_mem[0], "%d",working);
					
					printf("\n## client: %d start to transfer!\n");
					
					// read count and count++
					shm_id[1] = shmget(SHM_KEY10, sizeof(int), IPC_CREAT|0666);
					shared_mem[1]=shmat(shm_id[1], (void *)0,0);
					count = atoi(shared_mem[1]);
					count++;
					sprintf((char *)shared_mem[1], "%d",count);
					
					// file_size
					printf("## work\n");
					shm_id[2] = shmget(SHM_KEY11, sizeof(file_size), IPC_CREAT|0666);
					shared_mem[2]=shmat(shm_id[2], (void *)0,0);
					file_size = filelen;
					sprintf((char *)shared_mem[2], "%ld",file_size);

					// send data
					printf("## Client %d send to master..\n",count);
					printf("## work\n");
					shm_id[3] = shmget(SHM_KEY12, sizeof(file_size), IPC_CREAT|0666);
					file_data=shmat(shm_id[3], (void *)0,0);
					strcpy(file_data,buf);

					// file_name_len
					printf("3\n");
					shm_id[4] = shmget(SHM_KEY13, sizeof(file_name_len), IPC_CREAT|0666);
					shared_mem[3]=shmat(shm_id[4], (void *)0,0);
					file_name_len = strlen(argv[2]);
					sprintf((char *)shared_mem[3], "%d",file_name_len);

					// file_name
					printf("4\n");
					shm_id[5] = shmget(SHM_KEY14, file_name_len, IPC_CREAT|0666);
					file_name=shmat(shm_id[5], (void *)0,0);
					strcpy(file_name,argv[2]);
					
					// switch work mode
					working=2;
					sprintf((char *)shared_mem[0], "%d",working);
					printf("%d %d\n",working,count);
					printf("## work finished!\n");
					break;
				}
			}
		}
	}
	
	return 0;
}
