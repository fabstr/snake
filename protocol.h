#ifndef PROTOCOL_H
#define PROTOCOL_H
/**
 * Functions for the networking.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "mlog.h"

/** the maximum size of a network message */
#define MAXBUFFLEN 1024

/** the maximum length of a message type */
#define MAXMESSAGETYPELEN 16

enum MessageTypes {INPUT};

typedef struct {
        /**< the message's type */
        enum MessageTypes type;

        /**< the message */
        char *msg;

        /**< the length of the message */
        size_t length;

	/**< the sender's hostname / ip address */
	char *host;
} Message;

/**
 * Holds an open socket file descriptor and the addr_info used to create the 
 * socket.
 */
typedef struct {
        /**< the socket file descriptor */
        int sockfd;

        /**< the addr_info used to create sockfd */
        struct addrinfo *addr_info;
        
        /**< the addr_info_list used to create sockfd (needed to free 
         * addr_info correctly) */
        struct addrinfo *addr_info_list;
} Connection;

/**
 * Listen on port and return the first message.
 * @param c The Connection to listen on.
 * @return The message, or NULL on failure.
 */
Message *listenForMessage(Connection *c);

/**
 * Like listenForMessage, but return only once a message of the given type
 * has been received.
 * @param c The Connection to listen on.
 * @param type The type of the message
 * @return The message, or NULL on failure.
 */
Message *listenForMessageType(Connection *c, enum MessageTypes type);

/**
 * Send the message via the socket.
 * @pararm msg The message
 * @param sockfd The socket file descriptor, as returned by socket() called 
 *               with addr_info.
 * @param addr_info The addr_info used in socket().
 * @return -1 on error, 0 on success
 */
int sendMessage(Message *msg, Connection *c);

/**
 * Open a socket to the host and return a Connection.
 * The socked should be used with sendto().
 * The Connection should be free()-ed after use.
 * This function is not associated with connect(2).
 * @param host The host to open an socket to
 * @param port The port to connect on
 * @return The Connection, or NULL on error.
 */
Connection* openSendConnection(char *host, char *port);

/**
 * Open a socket and bind to it.
 * The Connection should be free()-ed after use.
 * @param port The port to listen on
 * @return The connection or NULL on error
 */
Connection* openListenConnection(char *port);

/**
 * Free a Connection.
 * The socket will be closed.
 * @param c The connection to free
 */
void freeConnection(Connection *c);

/**
 * Parse the string into a Message.
 * @param str The string to parse.
 * @param length The length of the string (including the NULL character).
 * @param remote The host/ip address of the sender
 * @return The message or NULL on error.
 */
Message* parseToMessage(char *str, size_t length, char *remoteAddress);
#endif
