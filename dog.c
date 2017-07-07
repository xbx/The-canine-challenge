/**
 * The canine challenge
 * @see http://1.61803398874.com/canine/
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>

#define BUFFERSIZE    1


struct thread_arg
{
  int quantity;
  char* filename;
  int* fd;
};

void write_to_file(void* fd);

int main(int argc, char **argv){
    unsigned char buffer[BUFFERSIZE];
  int thread_quantity = argc - 1;
    FILE *instream;
    int bytes_read=0;
    int buffer_size=0;
    int i;
  
  pthread_t* threadsIds = malloc(sizeof(pthread_t) * thread_quantity); 
  int** pipes = malloc(sizeof(int*) * thread_quantity);
  
    buffer_size=sizeof(unsigned char)*BUFFERSIZE;
    /* open stdin for reading */
    instream=fopen("/dev/stdin","r");
   
    if(instream!=NULL) {
  
    /** 
     * Create one thread per output file
     * Also generate the pipes needed to IPC
     */
    for(i=0; i < thread_quantity; i++) {
      int* fd = malloc(sizeof(int)*2);
      struct thread_arg* param = malloc(sizeof(struct thread_arg));
      
      pipes[i] = fd;
      pipe(fd);
      
      param->quantity = thread_quantity;
      param->fd = fd;
      param->filename = argv[i + 1];      
      
      
      pthread_create (&threadsIds[i], NULL, (void*)write_to_file, (void*)param);
    }

      /* read from stdin until it's end */
      while((bytes_read=fread(&buffer, buffer_size, 1, instream))==buffer_size) {
     /* Write on thread_quantity pipes */
     for(i=0; i < thread_quantity; i++) {
       write(pipes[i][1], buffer, buffer_size);
     }
      }
    
    /* Waiting for others threads to join this*/
    
    for(i=0; i < thread_quantity; i++) {
    close(pipes[i][1]);
    pthread_join(threadsIds[i], NULL);
    }
    
    /**
     * Free all resources
     */
    free(threadsIds);
    free(pipes);
    
    
    } else {
      /* if any error occured, exit with an error message */
          fprintf(stderr, "ERROR opening stdin. aborting.\n");
          exit(1);
      }

    return(0);
}

void write_to_file(void* t_param) {
  struct thread_arg* param = (struct thread_arg*) t_param;
  int buffer_size = sizeof(unsigned char)*BUFFERSIZE;
  unsigned char buffer[BUFFERSIZE];
  FILE * ostream;
  ostream = fopen(param->filename, "w");
  
  /* Start reading pipe and writing output*/
  while(read(param->fd[0], buffer, buffer_size)) {
    fprintf(stdout, "%c", buffer[0]);
    fwrite(buffer, buffer_size, 1, ostream);
  }
  free(param);
  fclose(ostream);
}
