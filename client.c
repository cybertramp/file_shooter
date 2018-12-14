#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

#define SHM_KEY1 70
#define SHM_KEY2 71
#define	SHM_KEY3 72
#define SHM_KEY4 73

int main(int argc, char *argv[]){
	
	if(argc>1 && argc<4){
		if(strcmp(argv[1],"-r")==0){
			int shm_id[4];
			void *shared_mem[4];
			
			unsigned long file_size;
			char *file_data;
			unsigned int file_name_len;
			char (*file_name);

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
			
			printf("4\n");
			
			FILE *fp = fopen("2.txt","w");
			fwrite((char *)file_data, 1, file_size, fp);
			fclose(fp);

			printf("shm_id: %d %d %d %d\n",shm_id[0],shm_id[1],shm_id[2],shm_id[3]);
			printf("%d\n",file_size);
			printf("%s\n",(char *)file_data);
			printf("%d\n",file_name_len);
			printf("%s\n",file_name);
			sleep(1);

			// clear shared_memory
			for(int i=0;i<4;i++)
				shmctl(shm_id[i], IPC_RMID, 0);
		}
		else if(strcmp(argv[1],"-s")==0){
			
			printf("## Start sending data.\n");
			FILE *fp = fopen(argv[2],"r");
			fseek(fp,0,SEEK_END);
			filelen = ftell(fp);
			rewind(fp);
			buf = (char *)malloc((filelen+1)*sizeof(char));
			fread(buf, filelen, 1, fp);
			fclose(fp);
			printf("## Loaded file. bytes: %d\n",filelen);

			// read count
			

		}
	}
	
	return 0;
}
