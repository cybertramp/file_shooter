#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_KEY1 800
#define SHM_KEY2 801
#define SHM_KEY3 802
#define SHM_KEY4 803

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

int main(int argc,char *argv[]){
	int tmp;
	
	printf("==========================================================================\n");
	printf("###### # #      ######     ####  #    #  ####   ####  ##### ###### #####  \n");
	printf("#      # #      #         #      #    # #    # #    #   #   #      #    # \n");
	printf("#####  # #      #####      ####  ###### #    # #    #   #   #####  #    # \n");
	printf("#      # #      #              # #    # #    # #    #   #   #      #####  \n");
	printf("#      # #      #         #    # #    # #    # #    #   #   #      #   #  \n");
	printf("#      # ###### ######     ####  #    #  ####   ####    #   ###### #    # \n");
	printf("==========================================================================\n\n");
	
	if(argc>1 && argc<4){
		if(strcmp(argv[1],"-s")==0&& argv[2]){
			
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
			printf("## Loaded file. %dbytes.\n",filelen);
			
			unsigned long file_size;
			char (*file_data)[filelen];
			unsigned int file_name_len;
			char (*file_name);

			printf("## Set up shared memory.");
			shm_id[0] = shmget(SHM_KEY1, sizeof(file_size), IPC_CREAT|0666);
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			file_size = filelen;
			sprintf((char *)shared_mem[0], "%ld",file_size);
			printf("..1");
			
			shm_id[1] = shmget(SHM_KEY2, filelen, IPC_CREAT|0666);
			file_data=shmat(shm_id[1], (void *)0,0);
			strcpy(file_data,buf);
			printf("..2");
			
			shm_id[2] = shmget(SHM_KEY3, sizeof(file_name_len), IPC_CREAT|0666);
			shared_mem[1]=shmat(shm_id[2], (void *)0,0);
			file_name_len = strlen(argv[2]);
			sprintf((char *)shared_mem[1], "%d",file_name_len);
			printf("..3");
			
			shm_id[3] = shmget(SHM_KEY4, file_name_len, IPC_CREAT|0666);
			file_name=shmat(shm_id[3], (void *)0,0);
			strcpy(file_name,argv[2]);
			printf("..4\n");

			printf("## Transfer success!\n");
			char c=NULL;
			while(c!='q'){
				printf("## If you finished transmission, please input 'q' with ENTER.\n");
				scanf("%c",&c);
			}
			printf("## Shared memory clear success!\n");
			for(int i=0;i<4;i++)
				shmctl(shm_id[i], IPC_RMID, 0);
			shmdt(shared_mem[0]);
			shmdt(shared_mem[1]);
			shmdt(file_data);
			shmdt(file_name);
		}
		else if(strcmp(argv[1],"-r")==0){
			printf("## Start collecting data.\n");
			printf("## How many number of user?\n");
			scanf("%d",&tmp);
			
			unsigned int user = tmp;
			unsigned int count=0;

			if(user < 1){
				printf("## Error ! ");
				return -1;
			}
			
			int shm_id[6];
			void *shared_mem[4];
			
			unsigned long file_size;
			char *file_data;
			unsigned int file_name_len;
			char *file_name;
			char *file_name2;
			
			unsigned int working=0;
					
			// count
			shm_id[0] = shmget(SHM_KEY10, sizeof(unsigned int), IPC_CREAT|0666);
			shared_mem[0]=shmat(shm_id[0], (void *)0,0);
			
			// working 0: finished 1: working
			shm_id[5] = shmget(SHM_KEY15, sizeof(unsigned int), IPC_CREAT|0666);
			shared_mem[3]=shmat(shm_id[5], (void *)0,0);

			// set count to zero
			sprintf((char *)shared_mem[0], "%d",count);
			// set not working
			sprintf((char *)shared_mem[3], "%d",working);
			printf("## set up success!\n");
			
			int prev_count=0;

			printf("## Waiting for clients -");
			// read count
			while(1){
				count = atoi(shared_mem[0]);
				working = atoi(shared_mem[3]);
				loading(100000);
				if((prev_count+1 == count) && (working ==2)){// upper 1
					working =3;
					sprintf((char *)shared_mem[3], "%d",working);
			
					// file_size
					shm_id[1] = shmget(SHM_KEY11, 0, IPC_CREAT|0666);
					shared_mem[1]=shmat(shm_id[1], (void *)0,0);
					file_size = atoi(shared_mem[1]);

					//file_data
					shm_id[2] = shmget(SHM_KEY12, 0, IPC_CREAT|0666);
					file_data=shmat(shm_id[2], (void *)0,0);

					// file_name_len
					shm_id[3] = shmget(SHM_KEY13, 0, IPC_CREAT|0666);
					shared_mem[2]=shmat(shm_id[3], (void *)0,0);
					file_name_len = atoi(shared_mem[2]);
					
					// file_name
					shm_id[4] = shmget(SHM_KEY14, 0, IPC_CREAT|0666);
					file_name=shmat(shm_id[4], (void *)0,0);

					printf("\n## Connected: %d\n",count);
					printf("## GET FILE user %d / %d.\n",count,user);

					mkdir("output",0755);
					
					// pointer to string
					char path[file_name_len+1];
					for(int i=0;i<file_name_len+1;i++){
						path[i] = file_name[i];
					}
					char path2[]="./output/";
					strcat(path2,path);
					
					FILE *fp = fopen(path2,"w+");

					fwrite((char *)file_data, 1, file_size, fp);
					fclose(fp);
					printf("## FILE WROTE SUCCESS !\n\n");
					prev_count = count;
					working = 0;
					sprintf((char *)shared_mem[3], "%d",working);
				}
				else if(user == count){
					break;
				}
			}
			printf("## Files collected Success. %d users\n",count);
			for(int i=0;i<6;i++)
				shmctl(shm_id[i], IPC_RMID, 0);
			shmdt(shared_mem[0]);
			shmdt(shared_mem[1]);
			shmdt(shared_mem[2]);
			shmdt(shared_mem[3]);
			shmdt(file_data);
			shmdt(file_name);
			return 0;		
		}
		return 0;
	}
	printf("## Parameter error, please retry it.\n");
	return 0;
}
