# file_shooter
> 파일을 배포하고 클라이언트로 부터 수집을 쉽게할 수 있는 프로그램입니다.
> created by cybertramp(paran_son@outlook.com)

- File broadcast or collect program with IPC.
- OS: Linux x64
- Language: C
- IPC: shared memory

- Functions
	- File Transfer
	- File Collect

- Use
	- File Transfer
		``` c
		command$./master -s <FILE_NAME>
		command$./client -r

	- File Collect
		``` c
		command$./master -r
		command$./client -s <FILE_NAME>
		
- example files
	- test.txt
	- test1.txt
	- test2.txt

- compiled binary file
	- master
	- client
