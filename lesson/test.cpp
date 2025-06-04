#include<iostream>

using namespace std;
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


//int main() {
	// cout << "Hello" << endl;
	// cout << getenv("PATH") << endl;
	// extern char **environ;
	// int i = 0;
	// cout << getpid() << endl;
	// cout << "aaaa" << endl;
	//pid_t id = fork();
	//cout << getpid() << endl;
	//cout << id << endl;
	// cout << "aaaa" << endl;
	// for(; environ[i]; i++){
 		// printf("%s\n", environ[i]);
	//	cout << environ[i] << endl;
 	//}
//	pid_t id = fork();
//	if(id == 0) {
//		// child
//		int cnt = 5;
//		while(cnt) {
//			cout << "子进程：" << getpid() << "父进程：" << getppid() << endl;
//			cnt--;
//			sleep(1);
//		}
//		exit(12);
//	}
//
//	else if(id > 0) {
//		// parent
//		// wait child
//		int status = 0;
//		// int ret = waitpid(id, &status,WNOHANG);		// 非阻塞等待
//		// cout << "exitcode: " << ((status >> 8) & 0xFF) << " " << "exitsig: " << (status & 0x7F) << endl; 
//		// cout << "ret: " << ret << endl;
//		while(true) {	// 轮询
//			int ret = waitpid(id, &status,WNOHANG);		// 非阻塞等待
//			if(ret == 0) {
//				cout << "pid指定的子进程没有退出！" << endl;
//				// continue;
//			}
//			else if(ret > 0) {
//				// 返回子进程的pid了 ret == id
//				cout << "等待成功，子进程退出！" << endl;
//				cout << "exitcode: " << ((status >> 8) & 0xFF) << " " << "exitsig: " << (status & 0x7F) << endl;
//				cout << "ret: " << ret << endl;
//				break;
//			}
//			else {
//				// waitpid调用失败
//				cout << "waitpid call fail!" << endl;
//				break;
//			}
//			sleep(1);
//		}
//	}
//	else { 
//		cout << "fork error!" << endl;
//	}
//	return 0;

//}





//int main() {
//	execl("/usr/bin/ls","ls","-a","-l",NULL);	

//	return 0;
//}

//int global_val = 100;
//int main() {
//	// 物理空间 or 虚拟空间
//	pid_t id = fork();
//	if(id == 0) {
//		// 子进程
//		global_val = 200;
//		cout << "子进程：" << "PID = " << getpid() << " PPID = " << getppid() << " global_val = " << global_val << endl;
//	}
//	else if(id > 0) {
//		sleep(3);
//		cout << "父进程：" << "PID = " << getpid() << " PPID = " << getppid() << " global_val = " << global_val << endl;;
//	}
//	else {
//		cout << "fork error" << endl;
//	}
//	return 0;
//}

//#include <string.h>
//#include <fcntl.h>
//int main() {
//	int fopen = open("log.txt", O_RDWR | O_CREAT, 0666);
//	const char* mes = "我们是冠军！";
//	int wrsize = write(fopen, mes, strlen(mes));
//	// char buf[32];
//		char ch;
//	while (1){
//		ssize_t s = read(fopen, &ch, 1);
//		if (s <= 0){
//			break;
//		}
//		write(1, &ch, 1); //向文件描述符为1的文件写入数据，即向显示器写入数据
//	}
//	// int rdsize = read(fopen, buf, wrsize);
//	close(fopen);
//	return 0;
//}


#include <fcntl.h>
#include <unistd.h> // 包含 close, lseek, read, write 的声明
#include <cstring>  // 包含 strlen 的声明

//int main() {
//    int fd = open("log.txt", O_RDWR | O_CREAT, 0666);
//    if (fd == -1) {
//        // 错误处理：无法打开或创建文件
//        return 1;
//    }
//    
//    const char* mes = "我们是冠军！";
//    ssize_t wrsize = write(fd, mes, strlen(mes));
//    if(wrsize == -1){
//        // 错误处理：写入失败
//        close(fd);
//        return 1;
//    }
//
//    // 将文件位置指示器移到文件开头
//    if(lseek(fd, 0, SEEK_SET) == -1){
//        // 错误处理：定位失败
//        close(fd);
//        return 1;
//    }
//
//    char ch;
//    while (1){
//        ssize_t s = read(fd, &ch, 1);
//        if (s <= 0){
//            break;
//        }
//        write(1, &ch, 1); // 向文件描述符为1的文件写入数据，即向显示器写入数据
//    }
//
//    close(fd); // 关闭文件描述符
//    
//    return 0;
//}


void fun() {
    int a = 100;
    int b = 100;
    cout << a + b << endl;
}

int main() {
	// close(1);
	int fd = open("a.txt", O_RDWR | O_CREAT);
	// fflush(stdout);
	// cout << fd << endl;
	dup2(fd, 0);
	char line[64];
	// cout << fd << endl;
	while(1) {
		if(fgets(line, sizeof(line), stdin) == NULL)
			break;
		cout << line << endl;

	}
    fun();

	close(fd);
	return 0;
}
