#include "fileo.h"
#include "types.h"
#include "socket.h"

#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief	�����ļ�
 * @param 	src_path		Ҫ���͵��ļ���·��
 * @param 	desc_fd		Ҫ���͵��ļ�������
 */
int send_file(const char *src_path, int desc_fd){
	int iRet = -1, src_fd;
	byte buffer[BUFFER_SIZE];
	ssize_t read_size;
	

	if(!src_path || (desc_fd < 0)){
		errno = EBADF;
		goto ERR;
	}
	if((src_fd = open(src_path, O_RDONLY)) == -1){
		goto ERR;
	}
	

	while((read_size = read(src_fd, buffer, BUFFER_SIZE))){
		if(read_size == -1 && errno != EINTR){
			goto READ_ERR;
		}else if(read_size > 0){				/*�������ݣ��ͽ��������ͳ�ȥ*/
			byte buf[BUFFER_SIZE + NAME_MAX_SIZE + 4];
			
			memset(buf, 0x00, sizeof(buf));
			*(int *)buf = strlen(src_path) + 1;			//�ļ�������
			memcpy(buf+4, src_path, strlen(src_path) + 1);	//�ļ���
			memcpy(buf + 4 + strlen(src_path) + 1, buffer, read_size);
			send_data(desc_fd, COMM_TYPE_SERVE, COMM_SERV_BITMAP, buf, read_size + 4 + strlen(src_path) + 1);
		}
	}
READ_ERR:
	close(src_fd);
ERR:
	return iRet;
}


