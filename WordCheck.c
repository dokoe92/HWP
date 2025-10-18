/*-----------------------------------------
WordCheck.c	a simple console game
-----------------------------------------*/
#include<errno.h>
#include<syslog.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>

#define WORDLEN 		80
#define MAXOUTPUT_LEN 	(WORDLEN + 20)
#define INCOMPLETE 		1
#define WON 			2
#define LOST 			3


/*
 * ServerProcess plays Hangman with a single player
 */
void ServerProcess (
  int in, 	/* stream to read input from */
  int out	/* stream to write output to */
  )
{
	int	  max_lives=10;	/* number of guesses we offer */
	char  *words []= { 		/* the words to be guessed */
		"applicationlayer",
		"presentationlayer",
		"sessionlayer",
		"transportlayer",
		"datalinklayer",
		"networklayer",
		"physicallayer",
		"transmissioncontrolprotocol",
		"userdatagramprotocol",

		"arpa",
		"internet",
		"rfc",
		"addressresolutionprotocol"
		"reverseaddressresolutionprotocol",
		"fragmentation",
		"networkaccesslayer",
		"internetcontrolmessageprotocol",
		"filetransferprotocol",
		"hypertexttransferprotocol",
		"simplemailtransferprotocol",
		"networknewsprotocol",


		"asterix",
		"obelix",
		"miraculix",
		"idefix",
		"majestix",
		"gutemine",
		"methusalix",
		"verleihnix",
		"troubardix"
        };

 	char  part_word [WORDLEN],
 		  *whole_word,
 		  guess_word[WORDLEN],
 		  hostname[WORDLEN],
 		  outbuff[MAXOUTPUT_LEN];
 	int	 lives,
     	 word_len,
     	 game_status = INCOMPLETE,
     	 hits,
     	 i;
 	time_t  timer;
    struct tm *t;

	/*
	 * initialize
	 */
 	lives = max_lives;

	/*
	 * pick up a random word
	 */
 	time (&timer);
 	t = (struct tm *) malloc (sizeof (struct tm));
 	t = localtime (&timer);
 	whole_word = words [
	  (t->tm_sec + rand()) %
	  (sizeof (words) / sizeof (char*))
	  ];
 	word_len = strlen (whole_word);
 	syslog (LOG_USER|LOG_INFO,
  		"wordd server chose word %s ",whole_word);

	/*
	 * initialize empty word
	 */
 	for (i=0; i<word_len; i++)
		part_word [i]='-';
 	part_word [i]= '\0';

	/*
	 * output empty word
	 */
 	sprintf (outbuff, "%s  lives:%d \n", part_word, lives);
 	write (out, outbuff, strlen (outbuff));

	/*
	 * do the game
	 */
 	while (game_status == INCOMPLETE) {
  		while (read (in, guess_word, WORDLEN) < 0) {
			/*
			 * restart if interrupted by signal ??
			 */
			if (errno != EINTR)
				perror ("reading players guess");
				exit(1);
   			printf("re-starting the read\n");
  		}

		/*
		 * check for hits
		 */
  		hits = 0;
  		for (i=0; i<word_len; i++) {
			if (guess_word[0] == whole_word [i]) {
    			hits=1;
    			part_word[i] = whole_word[i];
   			} /* if */
  		} /* for */

		/*
		 * check for end of game
		 */
  		if (!hits) {
			lives--;
			if (lives == 0) {
				game_status = LOST;
			} /* game is over */
		} /* lost one life */
  		if (strcmp (part_word, whole_word) == 0) {
			/*
			 * player has won
			 */
			game_status = WON;
   			sprintf (outbuff, "You won!\n");
   			write (out, outbuff, strlen(outbuff));
   			return;
  		} else if (lives == 0) {
   			game_status = LOST;
   			strcpy (part_word, whole_word);
  		}
		/*
		 * show word
		 */
  		sprintf (outbuff,
		  "%s  lives: %d \n",
		  part_word,lives);
  		write (out, outbuff, strlen (outbuff));
  		if (game_status == LOST) {
   			sprintf (outbuff, "\nGame over.\n");
   			write (out, outbuff, strlen (outbuff));
  		} /* player lost */
 	} /* game is incomplete */
} /* ServerProcess */