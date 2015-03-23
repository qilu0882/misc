/*
 * uart.c
 *
 * with select() 
 *
 * Refer to :
 * ==> Serial Programming Guide for POSIX Operating Systems.pdf
 *
 * ==> kernel/include/uapi/asm-generic/termbits.h
 *
 * typedef unsigned chat	cc_t;
 * typedef unsigned int	speed_t;
 * typedef unsigned int	tcflag_t;
 *
 * #define NCCS	19
 *
 * struct termios {
 * 	tcflag_t c_iflag;	// input mode flags
 * 	tcflag_t c_oflag;	// output mode flags
 * 	tcflag_t c_cflag;	// control mode flags
 * 	tcflag_t c_lflag;	// local mode flags
 * 	cc_t c_line;		// line discipline
 * 	cc_t c_cc[NCCS];	// control characters
 * };
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define TRUE	0
#define FALSE	(-1)

int speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = { 115200, 57600, 38400,  19200,  9600,  4800,  2400,  1200,  300, };

void set_speed(int fd, int speed)
{
	int i;
	int ret;
	struct termios opt;

	if ( tcgetattr(fd, &opt) != 0)
	{
		fprintf(stderr,"set_speed: fail to tcgetattr!\n");
		return;
	}

	/* set baudrate */
	for ( i=0;  i < sizeof(speed_arr) / sizeof(int);  i++)
	{
		if (speed == name_arr[i])
		{
		    tcflush(fd, TCIOFLUSH);
		    cfsetispeed(&opt, speed_arr[i]);
		    cfsetospeed(&opt, speed_arr[i]);
		    ret = tcsetattr(fd, TCSANOW, &opt);
		    if (ret != 0)
			fprintf(stderr,"set_speed: fail to tcsetattr!\n");
		    return;
		}
		tcflush(fd,TCIOFLUSH);
	}

	if (i == sizeof(speed_arr) / sizeof(int))
		fprintf(stderr, "set_speed: unsupported baudrate!!!\n");
}

int set_parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;

	/*
	 * Get the current options for the port ...
	 */
	if ( tcgetattr(fd, &options)  !=  0)
	{
		fprintf(stderr,"set_parity: fail to tcgetattr!\n");
		return(FALSE);
	}

	tcflush(fd, TCIOFLUSH);		//better effect

	/*
	 * Enable the receiver and set local mode ...
	 */
	options.c_cflag |= (CLOCAL|CREAD);

//	options.c_iflag &= ~BRKINT;	// Send a SIGINT when a break condition is detected


	/* raw mode */
#if 0
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
	options.c_oflag &= ~OPOST;		// Postprocess output (not set = raw output)

	options.c_iflag &= ~(INLCR|IGNCR|ICRNL);
	options.c_oflag &= ~(ONLCR|OCRNL);
#else
	/**
	 * cfmakeraw() sets the terminal to something like the "raw" mode of the old Version 7 terminal driver: input is available character by  char‐
	 * acter,  echoing  is  disabled, and all special processing of terminal input and output characters is disabled.  The terminal attributes are
	 * set as follows:

           termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP
                           | INLCR | IGNCR | ICRNL | IXON);
           termios_p->c_oflag &= ~OPOST;
           termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
           termios_p->c_cflag &= ~(CSIZE | PARENB);
           termios_p->c_cflag |= CS8;
	 *
	 */
	cfmakeraw(&options);
#endif

	/* flow control */
	options.c_cflag &= ~CRTSCTS;		// No hardware flow control
	options.c_iflag &= ~(IXON|IXOFF|IXANY);	// No software flow control
//	options.c_cflag |= CRTSCTS;		// Harware flow control
//	options.c_iflag |= IXON|IXOFF|IXANY;	// Software flow control

	options.c_cflag &= ~CSIZE;		// Bit mask for data bits
	switch (databits)
	{
	case 5:
	    options.c_cflag |= CS5;
	    break;
	case 6:
	    options.c_cflag |= CS6;
	    break;
	case 7:
	    options.c_cflag |= CS7;
	    break;
	case 8:
	    options.c_cflag |= CS8;
	    break;
	default:
	    fprintf(stderr,"Unsupported data size\n");
	    return (FALSE);
	}

	switch (parity)
	{
	case 'n':
	case 'N':
	    options.c_cflag &= ~PARENB;		// Clear parity enable
	    options.c_iflag &= ~INPCK;		// Enable parity checking
	    options.c_iflag &= ~ISTRIP;		// Strip parity bits
	    break;
	case 'o':
	case 'O':
	    options.c_cflag |= PARENB;		// Enable parity bit
	    options.c_cflag |= PARODD;		// Odd parity 
	    options.c_iflag |= INPCK;		// Enable parity checking
	    options.c_iflag |= ISTRIP;
	    break;
	case 'e':
	case 'E':
	    options.c_cflag |= PARENB;		// Enable parity bit
	    options.c_cflag &= ~PARODD;		// Even parity
	    options.c_iflag |= INPCK;		// Enable parity checking
	    options.c_iflag |= ISTRIP;
	    break;
//	case 'S':
//	case 's':  /*as no parity*/
//	    options.c_cflag &= ~PARENB;
//	    options.c_cflag &= ~CSTOPB;
//	    break;
	default:
	    fprintf(stderr,"Unsupported parity\n");
	    return (FALSE);
	}

	switch (stopbits)
	{
	case 1:
	    options.c_cflag &= ~CSTOPB;		// 1 stop bits
	    break;
	case 2:
	    options.c_cflag |= CSTOPB;		// 2 stop bits
	    break;
	default:
	    fprintf(stderr,"Unsupported stop bits\n");
	    return (FALSE);
	}

	//options.c_cc[VTIME] = 100;		// Time to wait for data (tenths of seconds)
	options.c_cc[VMIN] = 0;

	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		fprintf(stderr,"set_parity: fail to tcsetattr!\n");
		return (FALSE);
	}
	return (TRUE);
}

int open_dev(char *dev)
{
	int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);	// O_NDELAY vs. O_NONBLOCK
//	int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (-1 == fd)
        {
            fprintf(stderr,"Can't Open Serial Port %s\n", dev);
            return -1;
        }

	if ( fcntl(fd, F_SETFL, 0) < 0)		// NonBlock I/O --> Block I/O
		fprintf(stderr,"fcntl failed!\n");

	if ( isatty(fd) == 0)
		fprintf(stderr,"This is not a tty device!\n");


	return fd;
}

#define TX	0	// send
#define RX	1	// receive
#define LOOP	2	// loop back

#define MAX_BUFFER_SIZE	512

#define BAUDRATE	115200

/**
*@breif     main()
*/
int main(int argc, char **argv)
{
	int fd;
	int nread = 0;
	int count = 0;
	char buf[MAX_BUFFER_SIZE*2];
	char *sdir[] = {"tx", "rx", "loop"};
	char *dev ="/dev/ttyS0";
	char *filename = NULL;
	FILE *fp = NULL;
	int dir = RX;	// direction
	int i;
	int ret;

	/*parse commond line arguments*/
	if (argc < 2) {
		fprintf(stderr, "Usage: uart <-s/-r/-l> <-d devicenode> <-f filename>\n");
		exit(0);
	}

	for (i=0; i<argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			dir = TX;
		}
		if (strcmp(argv[i], "-r") == 0) {
			dir = RX;
		}
		if (strcmp(argv[i], "-l") == 0) {
			dir = LOOP;
		}
		if (strcmp(argv[i], "-d") == 0) {
			dev = argv[i+1];
		}
		if (strcmp(argv[i], "-f") == 0) {
			filename = argv[i+1];
		}
	}
	fprintf(stdout, "==>direction=%s(%d), filename=%s\n", sdir[dir], dir, filename);

	fd = open_dev(dev);
	if (fd < 0) {
		fprintf(stderr,"Can't Open Serial Port!\n");
		exit(1);
	}

	set_speed(fd, BAUDRATE);

	if (set_parity(fd, 8, 1, 'N') == FALSE)
	{
		fprintf(stderr,"Set Parity Error\n");
		exit(1);
	}

	fprintf(stdout,"==>%s are ready ^_^\n", dev);

	if (RX == dir) {
		fp = fopen(filename, "wb");
		if (!fp) {
			fprintf(stderr, "can't open/create file %s!\n", filename);
			exit(2);
		}

		while(1)
		{
			fd_set rset;
			struct timeval tv;

			FD_ZERO(&rset);
			FD_SET(fd, &rset);

			// timeout
			tv.tv_sec = 10;
			tv.tv_usec = 0;

			ret = select(fd+1, &rset, NULL, NULL, &tv);
			if (-1 == ret) {
				if (EINTR == errno)
					continue;
				else {
					fprintf(stderr, "fail to select(errno=%d)\n", errno);
					goto out;
				}
			}

			if (0 == ret) {
				fprintf(stderr, "select timeout\n");
				break;
			}

			nread = read(fd, buf, MAX_BUFFER_SIZE);

			if (nread > 0) {
				ret = fwrite(buf, nread, 1, fp);
				if (ret != 1) {
					fprintf(stderr, "fail to fwrite(ret=%d, errno=%d)\n", ret, errno);
					goto out;
				}
			} else if (errno == EAGAIN) {
				continue;
			} else if (nread < 0) {
				fprintf(stderr,"fail to read (nread=%d, errno=%d)\n", nread, errno);
				goto out;
			} else {
				fprintf(stderr,"EOF or Timeout (nread=%d, errno=%d)\n", nread, errno);
				break;
			}

			count += nread;
			fflush(fp);
		}

	} else if (TX == dir) {
		fp = fopen(filename, "rb");
		if (!fp) {
			fprintf(stderr, "can't open file %s!\n", filename);
			exit(2);
		}

		while (1) {

			nread = fread(buf, 1, MAX_BUFFER_SIZE, fp);

			if (nread > 0) {

				fd_set wset;
				struct timeval tv;
	
				FD_ZERO(&wset);
				FD_SET(fd, &wset);
	
				// timeout
				tv.tv_sec = 10;
				tv.tv_usec = 0;
	
				ret = select(fd+1, NULL, &wset, NULL, &tv);
				if (-1 == ret) {
					if (EINTR == errno)
						continue;
					else {
						fprintf(stderr, "fail to select(errno=%d)\n", errno);
						goto out;
					}
				}
	
				if (0 == ret) {
					fprintf(stderr, "select timeout\n");
					break;
				}

				ret = write(fd, buf, nread);
				if (ret != nread) {
					fprintf(stderr, "fail to write (nread=%d, ret=%d, errno=%d)\n", nread, ret, errno);
					goto out;
				}
			} else if (feof(fp)) {
				break;
			} else {
				fprintf(stderr, "fail to fread (nread=%d, errno=%d)\n", nread, errno);
				goto out;
			}

			count += nread;
		}
	} else if (LOOP == dir) {
		while(1)
		{
			if ((nread = read(fd, buf, MAX_BUFFER_SIZE)) > 0) {
				buf[nread+1]='\0';

				ret = write(fd, buf, nread);
				if (ret != nread) {
					fprintf(stderr, "fail to write (nread=%d, ret=%d, errno=%d)\n", nread, ret, errno);
					goto out;
				}
			} else if (errno == EAGAIN) {
				continue;
			} else {
				fprintf(stderr,"fail to read (nread=%d, errno=%d)\n", nread, errno);
			}
		}
	} else {
		fprintf(stderr, "invalid direction %s\n", sdir[dir]);
	}

	fprintf(stdout, "==>count=%d\n", count);

out:
	close(fd);
	fclose(fp);
	return 0;
}
