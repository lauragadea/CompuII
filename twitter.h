#ifndef _TWITTER_H_

	#define _TWITTER_H_

	
	int getTimeline(int sd, int pipefd2);
	int showTimeline (int pipefd2);
	int verifyTweetLength(char line_original[150]);
	int getTwitterResponse(int sd, int pipefd2);
	int saveTimeline(int sd);
	int searchWord(char palabrita[20]);

	
	
#endif

