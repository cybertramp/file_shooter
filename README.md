# file_shooter
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
