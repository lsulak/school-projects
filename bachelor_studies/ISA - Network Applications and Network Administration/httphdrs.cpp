#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <signal.h>

#include <arpa/inet.h> 
#include <net/ethernet.h>

#include <netinet/ip6.h>

#include <pcap.h>

#include <libxml/xmlwriter.h>
#include <libxml/encoding.h>
#include <libxml/xmlmemory.h>
#include <netinet/ip6.h>

// libpcap (http://www.tcpdump.org/) - Example www.tcpdump.org/sniffex.c
// XML libxml2 (http://www.xmlsoft.org/) - Example http://www.xmlsoft.org/examples/testWriter.c

//Ethernet header from net/ethernet, IP and TCP headers a little modified, from www.tcpdump.org/sniffex.c
#define SIZE_ETHERNET 14

//Fixed IPv6 packet's header have 40B
//IPv6 has been tested from file http://wiki.wireshark.org/SampleCaptures  file v6-http.cap
#define SIZE_IP6_HDR 40

#define MY_ENCODING "UTF-8"
#define IPv6_ETH_TYPE 0x86DD
#define IPv4_ETH_TYPE 0x0800
//IPv4 header, from www.tcpdump.org/sniffex.c
struct ip_header {
        u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
        #define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
		#define IP_V(ip)                (((ip)->ip_vhl) >> 4)
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off;                 /* fragment offset field */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
};

// TCP header, , from www.tcpdump.org/sniffex.c
typedef u_int tcp_seq;
struct tcp_header {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
		
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};

#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)


//Struct for parameters
struct p_struct { 
    std::string source;
    std::string headers;
    int* p_port;
    std::string output;
    bool modeF;
    bool modeI;
} param;

struct r_struct {
	xmlTextWriterPtr writer; //Global handle to XML file
	pcap_t *handle;          //Handle of session

	bool IP_element; 		//If there is end of communication(file, interrupt..) write ending tag
	char IP_src[40];
	struct bpf_program fp;  //Compiled filter expression 
} runtime;

//Global structures
p_struct parsed_struct;
r_struct runtime_struct;


//Function prototypes
int arg_parsing(int count, char *argv[]);
void help(bool shortMsg);
void handle_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void payload_parse(const u_char *payload, int len, char tmpC2[40]);
void header_parse(std::string header);
int payload_continue(std::string request);
void sig_handler(int signal);

//My Function prototypes for XML work
int xmlStart();
int xmlAdd(const char* name,const char* attr,const char* value, bool end);
int xmlAdd2(const char* name,const char* attr,const char* value,const char* attr2,const char* value2);
int xmlEnd();



//Main Function
int main(int argc , char *argv[])
{
	int port_counter = 0;
	int number_packets;
    char errbuf[PCAP_ERRBUF_SIZE];  //Err msges
   
    std::string finalExp;           //Final expression for filter

    bpf_u_int32 mask;               //Netmask of our device
    bpf_u_int32 net;                //32-bits IPv4 adress of our device

    std::string tmpS;               //For port (convert from int to string, tmp variable
    char tmpC[20];
    
    struct sigaction act;
    act.sa_handler = &sig_handler;
    
    // For valgrind...restart system call and Block every signal during handling
	act.sa_flags = SA_RESTART; 
	sigfillset(&act.sa_mask);
	
	//Signal handling - SIGINT, SIGQUIT, SIGTERM
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGTERM, &act, NULL);
	 
    if(argc >= 5)
    {
        if(arg_parsing(argc, argv) != 0)
        {
            fprintf(stderr, "Argument has wrong format!\n");
            help(true);
            free(parsed_struct.p_port);
            return 1;
        }
    }
    else 
    {
        fprintf(stderr, "Too few arguments!\n");
        help(true);
        free(parsed_struct.p_port);
        return 1;
    }
    //According to parameter -p or defualt port 80 capture only tcp port1 tcp port2....
    while(parsed_struct.p_port[port_counter] != 0)
    {
        if(port_counter != 0)
            finalExp += " or ";

        sprintf(tmpC, "%d", parsed_struct.p_port[port_counter]);
        tmpS = tmpC;
        finalExp += "tcp port " + tmpS;
        port_counter++;
    }
    // From manpage, accept packets ONLY with some data(payload). 
    // finalExp += " and (tcp[13] = 24 ) or (((ip[2:2] - ((ip[0]&0xf)<<2)) - ((tcp[12]&0xf0)>>2)) != 0))"; 
    // tcp[13] - flags in TCP: 00011000  : It means PSH and ACK, decimal:24
   
    //Reading from file, offline...
    if(parsed_struct.modeF == true)
    {
       number_packets = 0;
       if((runtime_struct.handle = pcap_open_offline(parsed_struct.source.c_str(), errbuf)) == NULL)
       {
            fprintf(stderr, "Couldn't open pcap file %s: %s\n", parsed_struct.source.c_str(), errbuf);
            return 1;
       }
       // Compile and apply the filter, don't optimalizate 
        if(pcap_compile(runtime_struct.handle, &runtime_struct.fp, finalExp.c_str(), 1, net) == -1) 
        {
            fprintf(stderr, "Wrong format of filter %s: %s\n", finalExp.c_str(), pcap_geterr(runtime_struct.handle));
            return 1;
        }
    }
    
	//Reading live from one device from parameter -i source
    else if(parsed_struct.modeI == true)
    {
        number_packets = -1;
         // Find the properties for the device 
        if(pcap_lookupnet(parsed_struct.source.c_str(), &net, &mask, errbuf) == -1) 
        {
            fprintf(stderr, "Can't get netmask for device %s\n", parsed_struct.source.c_str());
            net = 0;
            mask = 0;
        }

        // Open the session in promiscuous mode 
        if((runtime_struct.handle = pcap_open_live(parsed_struct.source.c_str(), BUFSIZ, 1, -1, errbuf)) == NULL) 
        {
            fprintf(stderr, "Couldn't open device %s: %s\n", parsed_struct.source.c_str(), errbuf);
            return 1;
        }
        // Fails if the device doesn't supply Ethernet headers, http://www.tcpdump.org/linktypes.html
        if(pcap_datalink(runtime_struct.handle) != DLT_EN10MB) 
        {
            fprintf(stderr, "Device %s doesn't provide Ethernet headers - not supported\n", parsed_struct.source.c_str());
            return 1;
        }
        //Not supported for 'offline' capturing, from a file. Not supported on all platforms, so if there is an error, nevermind
        pcap_setdirection(runtime_struct.handle, PCAP_D_OUT);

        // Compile and apply the filter, dont optimalizate 
        if(pcap_compile(runtime_struct.handle, &runtime_struct.fp, finalExp.c_str(), 0, net) == -1) 
        {
            fprintf(stderr, "Wrong format of filter %s: %s\n", finalExp.c_str(), pcap_geterr(runtime_struct.handle));
            return 1;
        }

        if(pcap_setfilter(runtime_struct.handle, &runtime_struct.fp) == -1) 
        {
            fprintf(stderr, "Couldn't set filter %s: %s\n", finalExp.c_str(), pcap_geterr(runtime_struct.handle));
            return 1;
        }
    }
	//Open a file and write starting elements...
	if(xmlStart() != 0)
		 return 1;
	
	//Clear IP_src variable, it will later carry IPv4 source adress of device	
    memset(&runtime_struct.IP_src, 0, sizeof(runtime_struct.IP_src));
	
	//Packet proessing, number_packets = -1 if online reading, infinite loop. 
	//Close f.e. with CTRL+C. number_packets = 0 if offline reading.
    pcap_loop(runtime_struct.handle, number_packets, handle_packet, NULL);
	
	//Write last ending elements, close file
	if(xmlEnd() != 0)
	 	return 1;
	
    pcap_freecode(&runtime_struct.fp);
    pcap_close(runtime_struct.handle);
    free(parsed_struct.p_port);
    //xmlMemoryDump(); //This is to debug memory for regression tests, 
    //from manpage:dumps all the information about the allocated memory block lefts in the .memdump file
    
    return 0;
}

int arg_parsing(int count, char *argv[])
{
    bool paramH = false;
    bool paramP = false;
    bool paramO = false;

    std::string ports;
    std::string portEnd;
    std::size_t position;
    int j = 0;

	//Alloc space for integers - for ports
    parsed_struct.p_port = (int*)malloc(1024 * sizeof(int));
    memset(parsed_struct.p_port, 0, 1024);
    for(int i=1; i < count; i++)
    {
        if(strcmp(argv[i], "-f") == 0) //-f source
        {
            if(parsed_struct.modeF == true || parsed_struct.modeI == true)
                return 1;

            parsed_struct.modeF = true;
            if(i+1 == count)
                return 1;

            parsed_struct.source = argv[++i];
        }
        else if(strcmp(argv[i], "-i") == 0) //-i source
        {
            if(parsed_struct.modeF == true || parsed_struct.modeI == true)
                return 1;

            parsed_struct.modeI = true;
            if(i+1 == count)
                return 1;

            parsed_struct.source = argv[++i];
        }
        else if(strcmp(argv[i], "-o") == 0)  //-o output_file
        {
            if(paramO == true)
                return 1;

            paramO = true;
            if(i+1 == count)
                return 1;

            parsed_struct.output = argv[++i];
        }
        else if(strcmp(argv[i], "-p") == 0)  //-p ports , default 80
        {
            if(paramP == true)
                return 1;

            paramP = true;
            if(i+1 == count)
                return 1;

            ports = argv[++i];
            if((position = ports.find(",")) == std::string::npos)
            {
           		for(std::string::size_type l = 0; l < ports.size(); ++l) 
                	if(!isdigit(ports[l]))
               			return 1;
               			
                parsed_struct.p_port[0] = atoi(ports.substr(0).c_str());
            }

            else
            {
                if(position == 0)
                    return 1;

                ports += ",end";

                portEnd = ports.at(ports.size()-5);
                if(portEnd.compare(",") == 0)
                    return 1;
	
				//Ports are separated with ',' and there is no spaces between them
                while((position = ports.find(",")) != std::string::npos)
                {
                    for(std::string::size_type l = 0; l < (ports.substr(0, position)).size(); ++l) 
                        if(!isdigit(ports.substr(0, position)[l]))
                            return 1;

                    parsed_struct.p_port[j] = atoi(ports.substr(0, position).c_str());
                    if(parsed_struct.p_port[j] > 65535)
                    {
                        fprintf(stderr, "Port can be only in range 0-65535\n");
                        return 1;
                    }
                    ports = ports.substr(position+1);
                    j++;
                }
            }
        }
        else if(strcmp(argv[i], "-H") == 0)  //-H header_list - default "User-Agent,Accept,Accept-Encoding,Accept-Language"
        {
            if(paramH == true)
                return 1;

            paramH = true;
            if(i+1 == count)
                return 1;

            parsed_struct.headers = argv[++i];
        }
        else if(strcmp(argv[i], "--help") == 0) 
            help(false); //print help

        else
            return 1;
    }

    if(parsed_struct.modeF != true && parsed_struct.modeI != true)
       return 1;

    if(ports.empty())
       parsed_struct.p_port[0] = 80;

    if(parsed_struct.headers.empty())
       parsed_struct.headers = "User-Agent,Accept,Accept-Encoding,Accept-Language";
  	
    return 0;
}

void help(bool shortMsg)
{   
    if(!shortMsg)
    {
        fprintf(stdout, "Project for subject ISA, variant : httphdrs\n");
        fprintf(stdout, "  C++ program for monitoring HTTP headers.\n");
        fprintf(stdout, "  Author: Ladislav Sulak\n");
        fprintf(stdout, "  Login: xsulak04\n");
        fprintf(stdout, "  Datum: 11/2014\n");
    }
    fprintf(stdout, "\n  Format:\n"
    "  httphdrs {-f | -i } source [-H header1,header2,headern] [-p 80,8080] -o output\n\n");
    fprintf(stdout, "  Parameters:\n");
    fprintf(stdout, "\t--help              \n\n");
    fprintf(stdout, "\t-f | -i  source"
        "\n\t\tThere must be -f or -i, both is an error."
        "\n\t\t-f is for pcap files,"
        "\n\t\t-i is for obtaining information from devices."
        "\n\t\tBoth are specified by argument 'source' \n\n");
    fprintf(stdout, "\t-H http_header1,http_header2,http_headern"  
        "\n\t\tOptional argument, followed by list of specificated headers."
        "\n\t\tDefault is 'User-Agent,Accept,Accept-Encoding,Accept-Language'\n\n");
    fprintf(stdout, "\t-p port1,port2,portn"
        "\n\t\tOptional argument, list of TCP ports, default value is 80\n\n");
    fprintf(stdout, "\t-o output"   
       " \n\t\tArbitrary argument, 'output' is name of output file\n");
}

//Callback function, called from pcap_loop with every packet
void handle_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	//If there is a change of source port, print ending element
	static bool IP_end_tag = false; 
	char *IP6_src_char; //Source port of device, if IPv6 packet
	
	//Structures for ETH,IP,TCP headers - for calculating size of payload etc.   
    const struct ether_header *ethernet = (struct ether_header*)(packet);
    const struct ip_header *ip4 = (struct ip_header*)(packet + SIZE_ETHERNET);   /* The IPv4 header */  
    const struct ip6_hdr *ip6 = (struct ip6_hdr *)(packet + SIZE_ETHERNET);	    /* The IPv6 header */           
   	
    const struct tcp_header *tcp;   
   
     // Packet payload 
    u_char *payload;                       

    int size_ip;
    int size_tcp;
    int size_payload;
    
    //For port processing
    int port_counter = 0;
    
    //Tmp variable, for changing INT to CHAR[] , src port from TCP....
    char tmpC2[20];
    memset(&tmpC2, 0, sizeof(tmpC2));
	
	//These parameters won't be used
	(void)args;
	(void)header;
	
	//-f - reading from file, need to set offline filter with every packet
	if(parsed_struct.modeF)
	 	if(pcap_offline_filter(&runtime_struct.fp, header, packet) == 0) 
	        return;
    
    //I totally separated IPv6 and IPv4, because code will be more readable.
    if(ntohs(ethernet->ether_type) == IPv6_ETH_TYPE)
    {
		IP6_src_char = (char*)malloc(INET6_ADDRSTRLEN);
		inet_ntop(AF_INET6, &ip6->ip6_src, IP6_src_char, INET6_ADDRSTRLEN);
		
		// define/compute tcp header offset 
	    tcp = (struct tcp_header*)(packet + SIZE_ETHERNET + SIZE_IP6_HDR);
	    if((size_tcp = TH_OFF(tcp)*4) < 20) 
	    {
	        fprintf(stderr, "   * Invalid TCP header length: %u bytes.Packet will be skipped\n", size_tcp);
	        return;
	    }

	    // define/compute tcp payload (segment) offset 
	    payload = (u_char *)(packet + SIZE_ETHERNET + SIZE_IP6_HDR + size_tcp);
		
		//Change integer to string
		sprintf(tmpC2, "%d", ntohs(tcp->th_sport));
			
		//Check if dest port isn't source port
		port_counter = 0;
		while(parsed_struct.p_port[port_counter] != 0)
		{
			if(ntohs(tcp->th_sport) == parsed_struct.p_port[port_counter++]) 
				return;
		}
		
		if((size_payload = ntohs(ip6->ip6_plen) - size_tcp) <= 0)
    		return;
	
		//If Source IP adress is changed (or new - first time) for this packet, write to file
		if(strcmp(runtime_struct.IP_src, IP6_src_char) != 0 )
    	{   
		 	if(IP_end_tag)
		 	{
		 		// Close the element named ip. 
			    if (xmlTextWriterEndElement(runtime_struct.writer) < 0) 
			    {
			        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
			        return;
			    }
			    xmlTextWriterFlush(runtime_struct.writer); 
			    IP_end_tag = false;
			}
		
    		if(xmlAdd("ip", "addr", IP6_src_char, true ) != 0)
    			return;
	    }
	    else
    		IP_end_tag = false;
	    
	    //IF there is and end(interrupt, EOF) we have to write ending tag do IP
    	runtime_struct.IP_element = true;

    	    	strncpy(runtime_struct.IP_src, IP6_src_char, sizeof(runtime_struct.IP_src));
   		payload_parse(payload, size_payload, tmpC2);
    }
    //IPv4 Packet header type!
    else if(ntohs(ethernet->ether_type) == IPv4_ETH_TYPE)
    {
	    // define/compute ip header offset 
	    if((size_ip = IP_HL(ip4)*4) < 20) 
	    {
	        fprintf(stderr, "   * Invalid IP header length: %u bytes.Packet will be skipped\n", size_ip);
	        return;
	    }
	    
	    // define/compute tcp header offset 
	    tcp = (struct tcp_header*)(packet + SIZE_ETHERNET + size_ip);
	    if((size_tcp = TH_OFF(tcp)*4) < 20) 
	    {
	        fprintf(stderr, "   * Invalid TCP header length: %u bytes.Packet will be skipped\n", size_tcp);
	        return;
	    }

	    // define/compute tcp payload (segment) offset 
	    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
		
		//if(ntohs(ethernet->ether_type) != IPv6_ETH_TYPE)
			//Change integer to string
		sprintf(tmpC2, "%d", ntohs(tcp->th_sport));
			
		//Check if dest port isn't source port
		port_counter = 0;
		while(parsed_struct.p_port[port_counter] != 0)
		{
			if(ntohs(tcp->th_sport) == parsed_struct.p_port[port_counter++]) 
				return;
		}
		
		if((size_payload = ntohs(ip4->ip_len) - (size_ip + size_tcp)) <= 0)
			return;
		
		//If Source IP adress is changed (or new - first time) for this packet, write to file
		if(strcmp(runtime_struct.IP_src,inet_ntoa(ip4->ip_src)) != 0)
		{   
		 	if(IP_end_tag)
		 	{
		 		// Close the element named ip. 
				if (xmlTextWriterEndElement(runtime_struct.writer) < 0) 
			    {
			        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
			        return;
			    }
			    xmlTextWriterFlush(runtime_struct.writer); 
			    IP_end_tag = false;
			}
    		if(xmlAdd("ip", "addr", inet_ntoa(ip4->ip_src), true ) != 0)
    			return;
	    }
   		else
    		IP_end_tag = false;
    	
    	//IF there is and end(interrupt, EOF) we have to write ending tag do IP	
		runtime_struct.IP_element = true; 
		strncpy(runtime_struct.IP_src, inet_ntoa(ip4->ip_src), sizeof(runtime_struct.IP_src));
       	payload_parse(payload, size_payload, tmpC2);
	}
	else
		return;
    //A bit of tricking :) 
    //For the first IP dont write ending tag, only if there is a change on IP
    if(IP_end_tag)
    {
	    // Close the element named ip. 
	    if(xmlTextWriterEndElement(runtime_struct.writer) < 0) 
	    {
	        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
	        return;
	    }
	    xmlTextWriterFlush(runtime_struct.writer);
	 }
	 else
	 	IP_end_tag = true;
}

//Function have data (payload) from TCP and its lenght (len)
void payload_parse(const u_char *payload, int len, char tmpC2[40])
{
	std::string first_line = (char*)payload;
	for (int i = 0; i < len; i++) 
    {	
        if(payload[i] == 13 && payload[i+1] == 10)
        {
			if(payload_continue(first_line.substr(0, i+1)) != 0)
				return;
			break;
		}
		
	}
		
	if(xmlAdd("connection", "port", tmpC2, true ) != 0)
    	return;
    
    std::string header;
    for (int i = 0; i < len; i++) 
    {	//Header (values) ends with /r/n <=> CR and New line >=> ASCII decimal 13,10
    	//Everything else can be taken as a header name or value
        if(payload[i] != 13 && payload[i] != 10)
        	header += payload[i];	

        else
        {
        	if(payload[i++] == 13)
        	{
        		if(payload[i] == 10)
        		{
        			header_parse(header);
        			header.clear();
        		}
        	}
        }		
    }
    // Close the element named connection. 
    if (xmlTextWriterEndElement(runtime_struct.writer) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
        return;
    }
    xmlTextWriterFlush(runtime_struct.writer);
}

//We have one line (more specific one header) - param header
//This function will parse it on name and value and write to file (format XML)
void header_parse(std::string header)
{
	std::string header_name, header_value, tmpHeader;
	std::size_t position, position2;
	
	tmpHeader = parsed_struct.headers;
	//Header name and value are separated by ':' and 0-n (1 optional and most of the times) spaces
	if((position = header.find(":")) != std::string::npos)
	{
		header_name = header.substr(0, position);
		header_value = header.substr(position+1);
		for(unsigned int i=0; i < sizeof(header_value); i++)
		{
			if(isspace(header_value[i]))
				header_value = header_value.substr(1);
			else
				break;
		}
		while(1)
		{
			//Compare headers from argument with actual header 
			if((position2 = tmpHeader.find(",")) != std::string::npos)
			{
				//If are equal (case insensitive) then this header we want to write to output file
				if(strcasecmp(tmpHeader.substr(0, position2).c_str(), header_name.c_str()) == 0)
				{	
					if(xmlAdd2("header", "name", header_name.c_str(), "value", header_value.c_str()) != 0)
						return;
						
					tmpHeader = tmpHeader.substr(position2+1);
					break;
				} 
				else
					tmpHeader = tmpHeader.substr(position2+1);
			}
			else
			{
				if(strcasecmp(tmpHeader.substr(0, position2).c_str(), header_name.c_str()) == 0)
				{	
					if(xmlAdd2("header", "name", header_name.c_str(), "value", header_value.c_str()) != 0)
						return;
				} 
				break;
			}
		}
	}
}

//From RFC 7231, chapter 5 - Request header fields.
//HEAD and GET MUST be supported, others are optional, but program will accept them all.
int payload_continue(std::string request)	
{
	if((request.substr(0,7).compare("OPTIONS") == 0) ||
	(request.substr(0,3).compare("GET") == 0) ||
	(request.substr(0,4).compare("HEAD") == 0) ||
	(request.substr(0,4).compare("POST") == 0) ||
	(request.substr(0,3).compare("PUT") == 0) ||
	(request.substr(0,6).compare("DELETE") == 0) ||
	(request.substr(0,5).compare("TRACE") == 0) ||
	(request.substr(0,7).compare("CONNECT") == 0))
		return 0;
	else 
		return 1;
}

//My function for writing header to XML file
//Name is header name, followed by attribute and value
//Variable end - for determining if there is need to close element or not,
//Some elements are not closed immediately, for example IP addresses
int xmlAdd(const char* name,const char* attr,const char* value, bool end)
{
	// Start an element named name. 
    if(xmlTextWriterStartElement(runtime_struct.writer, BAD_CAST name) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterStartElement\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    
    // Add an attribute attr with value as param value 
    if(xmlTextWriterWriteAttribute(runtime_struct.writer, BAD_CAST attr, BAD_CAST value) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterWriteAttribute\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    if(end)
    	return 0;
    	
   	// Close the element named name. 
    if(xmlTextWriterEndElement(runtime_struct.writer) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    return 0;
}

//My function for writing header with 2 attributes to XML file
//Name is header name, followed by attribute,value followed by attribute and value
int xmlAdd2(const char* name,const char* attr,const char* value,const char* attr2,const char* value2)
{
	// Start an element named name. 
    if(xmlTextWriterStartElement(runtime_struct.writer, BAD_CAST name) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterStartElement\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    
    // Add an attributes "name" and "value" with values "header_name" and "header_value" 
    if(xmlTextWriterWriteAttribute(runtime_struct.writer, BAD_CAST attr, BAD_CAST value) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterWriteAttribute\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    
    // Add an attributes "name" and "value" with values "header_name" and "header_value" 
    if(xmlTextWriterWriteAttribute(runtime_struct.writer, BAD_CAST attr2, BAD_CAST value2) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterWriteAttribute\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    
   	// Close the element named header. 
    if(xmlTextWriterEndElement(runtime_struct.writer) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    return 0;
}

//Open a file, write starting element
int xmlStart()
{
    // Create a new XmlWriter for filename, with no compression. 
    if((runtime_struct.writer = xmlNewTextWriterFilename(parsed_struct.output.c_str(), 0)) == NULL) 
    {
        fprintf(stderr, "Error creating the xml writer\n");
        return 1;
    }
    /* Start the document with the xml default for the version,
     * encoding UTF-8 and the default for the standalone
     * declaration. */
    if(xmlTextWriterStartDocument(runtime_struct.writer, NULL, MY_ENCODING, NULL) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterStartDocument\n");
        return 1;
    }
    /*Indentation can cause problems! Specification in WIS is MISSING
     http://www.w3.org/TR/2008/REC-xml-20081126/#sec-white-space */
	if(xmlTextWriterSetIndent(runtime_struct.writer,1) < 0) 
		return 1;
		
    /* Start an element named "httphdrs". Since thist is the first
     * element, this will be the root element of the document. */
    if(xmlTextWriterStartElement(runtime_struct.writer, BAD_CAST "httphdrs") < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterStartElement\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    return 0;
}

//Write ending tags for element, close file
//Cleaning XML from memory - only if work with actuall XML file ends! Segfault otherwise 
int xmlEnd()
{
	if(runtime_struct.IP_element)
	{
		// Close the element named ip 
		// last(well...most of the times first) source IP addres is wrote in the end 
	    if(xmlTextWriterEndElement(runtime_struct.writer) < 0) 
	    {
	        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
	        return 1;
	    }
	    xmlTextWriterFlush(runtime_struct.writer);
	}
	// Close the element named httphdrs. 
    if(xmlTextWriterEndElement(runtime_struct.writer) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterEndElement\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);

    /* Here we could close the elements using the
     * function xmlTextWriterEndElement, but since we do not want to
     * write any other elements, we simply call xmlTextWriterEndDocument,
     * which will do all the work. */
    if(xmlTextWriterEndDocument(runtime_struct.writer) < 0) 
    {
        fprintf(stderr, "Error at xmlTextWriterEndDocument\n");
        return 1;
    }
    xmlTextWriterFlush(runtime_struct.writer);
    
    //With SIGINT program never get so far, but it doesn't matter, 
    //program that exits have no memory leaks, only running programs does. 
    //On the other hand, with parameter -f , program is ended this way, returns 0.
    xmlFreeTextWriter(runtime_struct.writer);
    
    // Cleanup function for the XML library.
    xmlCleanupParser();
    return 0;
}

// If there is an interrupt of main cycle:
// SIGINT - 2, SIGQUIT - 3, SIGTERM - 15 
// Numbers from /usr/include/asm-generic/signal.h
void sig_handler(int signal)
{
	(void)signal;
    xmlEnd();
    pcap_close(runtime_struct.handle);
    free(parsed_struct.p_port);
    exit(0);
}
//EOF - End of the fun :)