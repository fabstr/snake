#include "cmdlineargs.h"

Options parseOptions(int argc, char **argv)
{
	Options o;
	resetOptions(&o);

	int ch;
	static struct option longopts[] = {
		{"remote", no_argument, NULL, 'r'},
		{"port", required_argument, NULL, 'p'},
		{"ai", no_argument, NULL, 'a'},
		{"growthspeed", required_argument, NULL, 'g'},
		{"movementspeed", required_argument, NULL, 'm'},
		{"color", required_argument, NULL, 'c'},
		{"help", no_argument, NULL, 'h'}
	};

	bool failure = false;
	while ((ch = getopt_long(argc, argv, "hrac:p:g:m:", longopts, NULL)) 
			!= -1) {
		switch (ch) {
			case 'r':
				o.remote.argument = NULL;
				o.remote.set = true;
				break;				
			case 'p':
				o.port.argument = optarg;
				o.port.set = true;
				break;
			case 'a':
				o.ai.argument = NULL;
				o.ai.set = true;
				break;
			case 'g':
				o.growthSpeed.argument = optarg;
				o.growthSpeed.set = true;
				break;
			case 'm':
				o.movementSpeed.argument = optarg;
				o.movementSpeed.set = true;
				break;
			case 'c':
				o.color.argument = optarg;
				o.color.set = true;
				break;
			case 'h':
				printHelp();
				exit(EXIT_SUCCESS);
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

	if (o.color.set == false) {
		o.color.argument = (char *) "blue";
	}

	return o;
}

void printHelp() 
{
	printf("Help:\n");
	printf("  -r --remote listen on a socket for movement messages\n");
	printf("  -p --port the port to listen on\n");
	printf("  -a --ai run the ai\n");
	printf("  -g --growthspeed the speed the snake grows with\n");
	printf("  -m --movementspeed the speed the snake moves with\n");
	printf("  -c --color the color of the snake, can be red, green, blue,"
			" yellow or white\n");
}

void resetOptions(Options *o)
{
	o->port.argument = NULL;
	o->port.set = false;
	o->remote.argument = NULL;
	o->remote.set = false;
	o->growthSpeed.argument = NULL;
	o->growthSpeed.set = false;
	o->movementSpeed.argument = NULL;
	o->movementSpeed.set = false;
	o->color.argument = NULL;
	o->color.set = false;
}
