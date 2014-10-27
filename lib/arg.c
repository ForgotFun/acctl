/*
 * ============================================================================
 *
 *       Filename:  arg.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年08月19日 10时19分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jianxi sun (jianxi), ycsunjane@gmail.com
 *   Organization:  
 *
 * ============================================================================
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <linux/if_ether.h>

#include "arg.h"
#include "log.h"

struct arg_t argument = {0};
int debug = 0;
int daemon_mode = 0;

static void __getmac(char *nic, char *mac)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)  {
		sys_err("Create sock for get mac failed: %s\n",
			strerror(errno));
		exit(-1);
	}

	struct ifreq req;
	strncpy(&req.ifr_name[0], nic, IFNAMSIZ);
	ioctl(sockfd, SIOCGIFHWADDR, &req);
	memcpy(mac, req.ifr_hwaddr.sa_data, ETH_ALEN);
	close(sockfd);
}

static  void __getaddr(char *nic, struct sockaddr_in *addr)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)  {
		sys_err("Create sock for get mac failed: %s\n",
			strerror(errno));
		exit(-1);
	}

	struct ifreq req;
	strncpy(&req.ifr_name[0], nic, IFNAMSIZ);
	ioctl(sockfd, SIOCGIFADDR, &req);
	memcpy(addr, &req.ifr_addr, sizeof(struct sockaddr_in));
	close(sockfd);
}

/* check argment */
static void __check_arg()
{
	if(argument.nic[0] == 0) {
		sys_err("No nic specified\n");
		help();
		exit(-1);
	}
	__getmac(&argument.nic[0], &argument.mac[0]);
	__getaddr(&argument.nic[0], &argument.addr);

#ifdef SERVER
	/* default ac broadcast interval */
	argument.brditv = (argument.brditv == 0) ? 30 : argument.brditv;
	argument.port = (argument.port == 0) ? 7960 : argument.port;
	argument.addr.sin_port = htons(argument.port);
#endif
#ifdef CLIENT
	argument.reportitv = (argument.reportitv == 0) ? 30 : argument.reportitv;
	argument.msgitv = (argument.msgitv == 0) ? 30 : argument.msgitv;
#endif
}

void proc_arg(int argc, char *argv[])
{
	proc_cfgarg();
	proc_cmdarg(argc, argv);
	__check_arg();
}
