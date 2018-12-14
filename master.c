#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_KEY1 70
#define SHM_KEY2 71
#define SHM_KEY3 72
#define SHM_KEY4 73

#define SHM_KEY10 80
#define SHM_KEY11 81
#define SHM_KEY12 82
#define SHM_KEY13 83
#define SHM_KEY14 84
#define SHM_KEY15 85

int main(int argc,char *argv[]){
	int tmp;
	if(argc>1 && argc<4){
		if(strcmp(argv[1],"-s")==0){
			
			int shm_id[4];
			void *shared_mem[4];
			long filelen;
			char *buf;
			
			printf("## Start sending data.\n");
			FILE *fp = fopen(argv[2],"r");
			fseek(fp,0,SEEK_END);
			filelen = ftell(fp);
			rewind(fp);
			buf = (char *)malloc((filelen+1)*sizeof(char));
			fread(buf, filelen, 1, fp);
			fclose(fp);
			printf("## Loaded file. bytes: %d\n",filelen);
			
			unsigned long file_size;
			char (*file_data)[filelen];
			unsigned int file_name_len;
			char (*file_name);

			printf("## work\n");
			shm_id[0] = shmget(SHM_KEY1, sizeof(file_size), IPC_CREAT|0666);
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			file_size = filelen;
			sprintf((char *)shared_mem[0], "%ld",file_size);
			
			printf("2\n");
			shm_id[1] = shmget(SHM_KEY2, filelen, IPC_CREAT|0666);
			file_data=shmat(shm_id[1], (void *)0,0);
			strcpy(file_data,buf);
			
			printf("3\n");
			shm_id[2] = shmget(SHM_KEY3, sizeof(file_name_len), IPC_CREAT|0666);
			shared_mem[1]=shmat(shm_id[2], (void *)0,0);
			file_name_len = strlen(argv[2]);
			sprintf((char *)shared_mem[1], "%d",file_name_len);
			
			printf("4\n");
			shm_id[3] = shmget(SHM_KEY4, file_name_len, IPC_CREAT|0666);
			file_name=shmat(shm_id[3], (void *)0,0);
			strcpy(file_name,argv[2]);
			printf("5\n");
			while(1){
				printf("shm_id: %d %d %d %d\n", shm_id[0],shm_id[1],shm_id[2],shm_id[3]);
				printf("%d\n",file_size);
				printf("%s\n",file_data);
				printf("%d\n",file_name_len);
				printf("%s\n",file_name);
				sleep(1);
			}
		}
		else if(strcmp(argv[1],"-r")==0){
			printf("## Start collecting data.\n");
			printf("## How many number of user?\n");
			scanf("%d",&tmp);
			
			int user = tmp;
			int count=0;

			if(user < 1){
				printf("## Error ! ");
				return -1;
			}
			
			int shm_id[6];
			void *shared_mem[6];
			
			unsigned long file_size;
			char (*file_data);
			unsigned int file_name_len;
			char (*file_name);

			char (*file_data2);
			char (*file_name2);

			// user
			shm_id[0] = shmget(SHM_KEY10, sizeof(int), IPC_CREAT|0666);
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			
			// count
			shm_id[1] = shmget(SHM_KEY11, sizeof(int), IPC_CREAT|0666);
			shared_mem[1]=shmat(shm_id[1], (void *)0,0);
			
			// file_size
			printf("## work\n");
			shm_id[2] = shmget(SHM_KEY12, sizeof(file_size), IPC_CREAT|0666);
			shared_mem[2]=shmat(shm_id[2], (void *)0,0);
			
			//file_data
			printf("2\n");
			shm_id[3] = shmget(SHM_KEY13, file_size, IPC_CREAT|0666);
			file_data=shmat(shm_id[3], (void *)0,0);
			
			// file_name_len
			printf("3\n");
			shm_id[4] = shmget(SHM_KEY14, sizeof(file_name_len), IPC_CREAT|0666);
			shared_mem[3]=shmat(shm_id[4], (void *)0,0);
			
			// file_name
			printf("4\n");
			shm_id[5] = shmget(SHM_KEY15, file_name_len, IPC_CREAT|0666);
			file_name=shmat(shm_id[5], (void *)0,0);
			
			printf("## set up success!\n");
			FILE *fp;
			int i=0;

			// read count
			while(1){
				count = atoi(shared_mem[1]);
				sleep(1);
				if(i == count){
					printf("## GET FILE user %d / %d.\n",count+1,user);
					
					printf("5\n");
					file_size = atoi(shared_mem[2]);
					file_data2 = file_data;
					file_name_len = atoi(shared_mem[3]);
					file_name2 = file_name;
					

					printf("6\n");
					*fp = fopen(file_name,"w");
					fwrite((char *)file_data2, 1, file_size, fp);
					fclose(fp);
					i++;
					
					printf("## FILE WRITED SUCCESS !\n");
				}
				else if(i > count){
					break;
				}
			}
			printf("## Files collected Success. %d users",count);
			
			// clear shared memory
			for(int i=0;i<6;i++)
				shmctl(shm_id[i], IPC_RMID, 0);
		}
		
	}
	return 0;
}