Commandline run:
gcc (file).c
./a.out

poo.c (I was not the one that named this. Please do not place the blame on me)
 - This is where we were testing out reading and printing individual lines in a text document when it all went to hell.
 - (RANT)we initially were rising and recycling the buffer in main, but that was causing absolute fucking chaos, so Matt 
	changed it to a different buffer.... we ideally would like the whole functionality of the buffer in its own function...
	Which I was gonna do until everything went to hell in a hand basket, now I dont know whats actually happening.
 **gcc poo.c

head.c
 - Where everything will go eventually.
 - I plan on rewriting main to make it look less crap, but I've mainly been tinkering with poo.c anyways.
 - You can kind of run it.... but it doesn't really do anything besides slap print statments in your face like "Hey, good job.
	Elly can read commandline arguments"
 **gcc head.c nanpa OR gcc head.c -n

ahhhhhhhh(??).txt
 - a couple different implementations of poo.c ..... Most of which shit themselves before they could continue to run... 
 ....good times.....