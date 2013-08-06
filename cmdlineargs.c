#include "cmdlineargs.h"

Options parseOptions(int argc, char **argv)
{
	Options o;
	resetOptions(&o);

	int ch;
	static struct option longopts[] = {
		{"remote", optional_argument, NULL, 'r'},
		{"port", required_argument, NULL, 'p'}
	};

	bool failure = false;
	while ((ch = getopt_long(argc, argv, "rp:", longopts, NULL)) 
			!= -1) {
		switch (ch) {
			case 'r':
				o.remote.argument = optarg;
				o.remote.set = true;
				break;				
			case 'p':
				o.port.argument = optarg;
				o.port.set = true;
				break;
			case '?':
			default:
				failure = true;
				break;
		}
	}

	if (failure == true) {
		printHelp();
		exit(EXIT_FAILURE);
	} else if (o.remote.set == true && o.port.set == false) {
		fprintf(stderr, "Please give a port with -p port.\n");
		exit(EXIT_FAILURE);
	}

	return o;
}

void printHelp() 
{
	printf("Help:\n");
	printf("  -r listen on a socket for movement messages\n");
	printf("  -p the port to listen on\n");
}

void resetOptions(Options *o)
{
	o->port.argument=NULL;
	o->port.set=false;
	o->remote.argument=NULL;
	o->remote.set=false;
}
