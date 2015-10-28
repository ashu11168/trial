// main.cc
//	Bootstrap code to initialize the operating system kernel.
//
//	Allows direct calls into internal operating system functions,
//	to simplify debugging and testing.  In practice, the
//	bootstrap code would just initialize data structures,
//	and start a user program to print the login prompt.
//
// 	Most of this file is not needed until later assignments.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//		-s -x <nachos file> -c <consoleIn> <consoleOut>
//		-f -cp <unix file> <nachos file>
//		-p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -rs causes Yield to occur at random (but repeatable) spots
//    -z prints the copyright message
//
//  USER_PROGRAM
//    -s causes user programs to be executed in single-step mode
//    -x runs a user program
//    -c tests the console
//
//  FILESYS
//    -f causes the physical disk to be formatted
//    -cp copies a file from UNIX to Nachos
//    -p prints a Nachos file to stdout
//    -r removes a Nachos file from the file system
//    -l lists the contents of the Nachos directory
//    -D prints the contents of the entire file system
//    -t tests the performance of the Nachos file system
//
//  NETWORK
//    -n sets the network reliability
//    -m sets this machine's host id (needed for the network)
//    -o runs a simple test of the Nachos network software
//
//  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include "system.h"


// External functions used by this file

extern void ThreadTest(void), Copy(char *unixFile, char *nachosFile);
extern void Print(char *file), PerformanceTest(void);
extern void StartProcess(char *file), ConsoleTest(char *in, char *out);
extern void MailTest(int networkID);
void function_batchstart (int temp);
void function_threadrun(char* name_executablefile, int Priority);
//----------------------------------------------------------------------
// main
// 	Bootstrap the operating system kernel.
//
//	Check command line arguments
//	Initialize data structures
//	(optionally) Call test procedure
//
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------












int
main(int argc, char **argv)
{
    int argCount;			// the number of arguments
					// for a particular command

    DEBUG('t', "Entering main");
    (void) Initialize(argc, argv);

#ifdef THREADS
    ThreadTest();
#endif

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
        if (!strcmp(*argv, "-z"))               // print copyright
            printf (copyright);
#ifdef USER_PROGRAM
        if (!strcmp(*argv, "-x")) {        	// run a user program
	    ASSERT(argc > 1);
            StartProcess(*(argv + 1));
            argCount = 2;
        } else if (!strcmp(*argv, "-c")) {      // test the console
	    if (argc == 1)
	        ConsoleTest(NULL, NULL);
	    else {
		ASSERT(argc > 2);
	        ConsoleTest(*(argv + 1), *(argv + 2));
	        argCount = 3;
	    }
	    interrupt->Halt();		// once we start the console, then
					// Nachos will loop forever waiting
					// for console input
	}

//CHANGE BEGINS
  else if(!strcmp(*argv, "-F"))
  { //filename as instructed in the assignment
        if(argc <2){
        	printf("Please enter filename along the executable for the -F option");
        	ASSERT(0);
        }
        argCount = 2;

        //FILENAME READ
    	char * name_file = *(argv+1);

      //OPEN THE FILE
	    	OpenFile *file_open = fileSystem->Open(name_file);

        //PRINT ERROR IF YOU CANT OPEN THE FILE
		    if (file_open == NULL)
      		{
				printf("Sorry, cannot open the file %s\n", name_file);
				delete file_open;
			}

      //IF YOU ARE ABLE TO OPEN THE FILE
			else
      			{

				char *buf = new char[1000];

				int byte_number = file_open->Read(buf, 1500);
        		//Make last position of string as the null character
			   	buf[byte_number] = '\0';

          		//This variable basically loops over the string, if this gives error, change to normal indexing
			   	int buffer_position = 0;


  				scheduling_algorithm_number = 0;
  				while (buf[buffer_position] != '\n' && buf[buffer_position] != '\0')
                  {
          					scheduling_algorithm_number = 10*scheduling_algorithm_number + (buf[buffer_position] - '0');
          					buffer_position++;
	  				}
          // Read batch processes and schedule them



			   	while(buf[buffer_position]!= '\0')
          {
			   		while (buf[buffer_position] != '\0' && buf[buffer_position] != '\n')
            {
			   			char executable[1000];
				   		int executable_priority = 0;
			   			int position = 0;
			   			while (buf[buffer_position] != ' ' && buf[buffer_position] != '\n')
              {
				   			executable[position] = buf[buffer_position];
				   			buffer_position++; position++;
				   		}
				   		executable[position] = '\0';

				   		if(buf[buffer_position] == '\n')
              {
				   			executable_priority = 100;
				   		}
              else
              {
				   			buffer_position++;
							  while (buf[buffer_position] != '\0' && buf[buffer_position] != '\n')
               {
				   				executable_priority = executable_priority*10 + (buf[buffer_position] - '0');
				   				buffer_position++;
				   			}
				   		}
				   		printf("%s %d\n", executable, executable_priority);
				   		function_threadrun(executable, executable_priority);
			   		}
			   		if (buf[buffer_position] == '\n'){
				   		buffer_position++;
			   		}
		   		}
		    	delete [] buf;
		    	delete file_open;
			}
		unsigned int k;
		int code_exit;
		code_exit = 0;
        printf("Exit has been called for [pid %d]. The Exit code is : %d\n", currentThread->GetPID(), code_exit);

        exitThreadArray[currentThread->GetPID()] = true;

        for (k=0; k<thread_index; k++)
        {
          if (!exitThreadArray[k]) break;
        }
        currentThread->Exit(k==thread_index, code_exit);
	}

//ENDS




#endif // USER_PROGRAM
#ifdef FILESYS
	if (!strcmp(*argv, "-cp")) { 		// copy from UNIX to Nachos
	    ASSERT(argc > 2);
	    Copy(*(argv + 1), *(argv + 2));
	    argCount = 3;
	} else if (!strcmp(*argv, "-p")) {	// print a Nachos file
	    ASSERT(argc > 1);
	    Print(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-r")) {	// remove Nachos file
	    ASSERT(argc > 1);
	    fileSystem->Remove(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-l")) {	// list Nachos directory
            fileSystem->List();
	} else if (!strcmp(*argv, "-D")) {	// print entire filesystem
            fileSystem->Print();
	} else if (!strcmp(*argv, "-t")) {	// performance test
            PerformanceTest();
	}
#endif // FILESYS
#ifdef NETWORK
        if (!strcmp(*argv, "-o")) {
	    ASSERT(argc > 1);
            Delay(2); 				// delay for 2 seconds
						// to give the user time to
						// start up another nachos
            MailTest(atoi(*(argv + 1)));
            argCount = 2;
        }
#endif // NETWORK
    }

    currentThread->FinishThread();	// NOTE: if the procedure "main"
				// returns, then the program "nachos"
				// will exit (as any other normal program
				// would).  But there may be other
				// threads on the ready list.  We switch
				// to those threads by saying that the
				// "main" thread is finished, preventing
				// it from returning.
    return(0);			// Not reached...
}





//CHANGE BEGINS


void function_threadrun(char* name_executablefile, int Priority){

//open the file
	OpenFile *input_file = fileSystem->Open(name_executablefile);

    if (input_file == NULL) {
    	printf("Sorry, cannot open the file %s\n", name_executablefile);
      return;
    }

    NachOSThread * thread_child = new NachOSThread("newthread_batch");
    thread_child->updateBasePriority(Priority);
    thread_child->space = new AddrSpace (input_file);
    delete input_file;
    thread_child->space->InitRegisters();             // set the initial register values
    thread_child->SaveUserState ();
    thread_child->ThreadStackAllocate (function_batchstart, 0);
    thread_child->Schedule ();
}


void function_batchstart (int temp)
{
   currentThread->Startup();
   machine->Run();
}


//END
