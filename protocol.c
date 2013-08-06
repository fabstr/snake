#include "protocol.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
}
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Message *listenForMessage(Connection *c)
{
        if (c == NULL) {
                mlog("PROTOCOL ERROR: c is NULL");
                return NULL;
        }

        /* to hold the message */
        char *buff = (char *) malloc(MAXBUFFLEN);
        if (buff == NULL) {
                return NULL;
        }

        /* the number of bytes received */
        int bytesreceived;

        /* the address the packet was received from and the address's size */
        struct sockaddr_storage remote_addr;
        socklen_t addr_len;

        if ((bytesreceived = recvfrom(c->sockfd, buff, MAXBUFFLEN - 1, 0, 
                                        (struct sockaddr*)&remote_addr, 
                                        &addr_len)) == -1) {
                mlog("PROTOCOL WARNING: could not receive packet: %s", 
                                strerror(errno));
                free(buff);
                return NULL;
        }

        char remoteAddress[INET6_ADDRSTRLEN];
        inet_ntop(remote_addr.ss_family, 
                        get_in_addr((struct sockaddr *)&remote_addr), 
                        remoteAddress, INET6_ADDRSTRLEN);
        if (remoteAddress == NULL) {
                mlog("PROTOCOL ERROR: could not get remote address: %s", 
                                strerror(errno));
                free(buff);
                return NULL;
        }

        /* add a NULL to make the buffer a string */
        buff[bytesreceived] = '\0';

        /* parse the message and return it. (+1 is for the null char) */
        return parseToMessage(buff, bytesreceived+1, remoteAddress);
}

Message *listenForMessageType(Connection *c, enum MessageTypes type)
{
	Message *m = listenForMessage(c);
	if (m == NULL) {
		return NULL;
	}

	/* make sure we have a board message */
	while (m->type != type) {
		free(m);
		if ((m = listenForMessage(c)) == NULL) {
			return NULL;
		}
	}

        return m;
}

int sendMessage(Message *msg, Connection *c)
{
        if (msg == NULL) {
                mlog("PROTOCOL ERROR: msg is null");
                return -1;
        } else if (c == NULL) {
                mlog("PROTOCOL ERROR: c is null");
                return -1;
        }

        mlog("PROTOCOL: sending message '%s'", msg->msg);
        /* send the message */
        int bytessent = sendto(c->sockfd, msg->msg, msg->length, 0, 
                                c->addr_info->ai_addr, 
                                c->addr_info->ai_addrlen);
        if (bytessent == -1) {
                mlog("PROTOCOL: could not send: %s", strerror(errno));
                return -1;
        }

        return 0;
}

Connection* openSendConnection(char *host, char *port)
{
         /* 
         * addr_info_list contains the list of addr_info's set by gedaddrinfo.
         * hints specifies hints for the socket
         * addr_info will be used in socket()
         */
        struct addrinfo *addr_info_list, hints, *addr_info;

        /* zero-ise hints and set the elements */
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        mlog("PROTOCOL: getting address info");
        int gai_error = getaddrinfo(host, port, &hints, &addr_info_list);
        if (gai_error != 0) {
                mlog("PROTOCOL ERROR: getaddrinfo error, %s", gai_strerror(gai_error));
                mlog("PROTOCOL ERROR: host=%s port=%s", host, port);
                return NULL;
        }

        /* the socket file descriptor */
        int sockfd;

        mlog("PROTOCOL: socket()");
        for (addr_info = addr_info_list; addr_info != NULL; 
                        addr_info = addr_info->ai_next) {
                if ((sockfd = socket(addr_info->ai_family, 
                                                addr_info->ai_socktype,
                                                addr_info->ai_protocol)) 
                                == -1) {
                        mlog("PROTOCOL: could not create socket: %s", 
                                        strerror(errno));
                        continue;
                } 

                /* we've got our socket */
                mlog("PROTOCOL: got socket to (host=%s port=%s)", host, port);
                Connection *c = (Connection *) malloc(sizeof(Connection));
                c->sockfd = sockfd;
                c->addr_info = addr_info;
                c->addr_info_list = addr_info_list;
                return c;
        }

        mlog("PROTOCOL ERROR: could not get the socket (host=%s, port=%s)", 
                        host, port);

        return NULL;
}

Connection* openListenConnection(char *port)
{
        /* 
         * addr_info_list contains the list of addr_info's set by gedaddrinfo.
         * hints specifies hints for the socket
         * addr_info will be used in socket()
         */
        struct addrinfo *addr_info_list, hints, *addr_info;

        /* zero-ise hints and set the elements */
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;

        int gai_error = gai_error = 
                getaddrinfo(NULL, port, &hints, &addr_info_list);
        if (gai_error != 0) {
                mlog("PROTOCOL ERROR: getaddrinfo error, %s", 
                                gai_strerror(gai_error));
                mlog("PROTOCOL ERROR: port=%s", port);
                return NULL;
        }

        /* the socket file descriptor */
        int sockfd;

        for (addr_info = addr_info_list; addr_info != NULL; 
                        addr_info = addr_info->ai_next) {
                if ((sockfd = socket(addr_info->ai_family, 
                                                addr_info->ai_socktype,
                                                addr_info->ai_protocol)) 
                                == -1) {
                        mlog("PROTOCOL: could not create socket: %s", 
                                        strerror(errno));
                        continue;
                } 

                int optval = 1;
                if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, 
                                        sizeof(optval)) == -1) {
                        mlog("PROTOCOL WARNING: could not set SO_REUSEADDR: %s",
                                        strerror(errno));
                }

                if (bind(sockfd, addr_info->ai_addr, addr_info->ai_addrlen) 
                                == -1) {
                        mlog("PROTOCOL: could not bind: %s", strerror(errno));
                        continue;
                }

                /* we've got our socket */
                Connection *c = (Connection *) malloc(sizeof(Connection));

               
                c->sockfd = sockfd;
                c->addr_info = addr_info;
                c->addr_info_list = addr_info_list;

                return c;
        }

        return NULL;
}

void freeConnection(Connection *c)
{
        freeaddrinfo(c->addr_info_list);
        close(c->sockfd);
        free(c);
}

Message* parseToMessage(char *str, size_t length, char *remoteAddress)
{
        /* get the message type according to the string */
        char messageType[MAXMESSAGETYPELEN];
        if (sscanf(str, "%s", messageType) == 0) {
                mlog("PROTOCOL: could not get the message type of '%s'", str);
                return NULL;
        }

        /* convert into an enum value */
        enum MessageTypes mt;
	
        if (strcmp("INPUT", messageType) == 0) { 
                mt = INPUT;
        } else {
                mlog("PROTOCOL ERROR: could not determine type of '%s'", 
                                messageType);
                return NULL;
        }

        /* create the Message */
        Message *m = (Message *) malloc(sizeof(Message));
        m->type = mt;
        m->msg = str;
        m->length = length;
        m->host = remoteAddress;

        return m;
}
