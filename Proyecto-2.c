#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

typedef struct lista_archivos {
	int id;
  char name[50];
	long size;
  char path[150];
  char nombrecompleto[150];	
	struct lista_archivos *ant;
	struct lista_archivos *sig;
}lista_archivos;

void error(const char *s);
long procesarArchivo(char *archivo);
lista_archivos *agregar(lista_archivos *p, int id, char *path, struct dirent *info, int the_size, char *nombrecompleto);
void imprimirLista(lista_archivos *p);
void eliminarIguales(lista_archivos *p);
lista_archivos *buscarArchivos(char *ruta, lista_archivos *p, int *id);
unsigned char getFileType(char *nombre, struct dirent *ent);
unsigned char statFileType(char *fname);
char *getFullName(char *ruta, struct dirent *ent);

int main(int argc, char *argv[])
{
  int *id;
  *id = 0;
	lista_archivos *archivo;
  printf("\n");
  archivo = buscarArchivos(argv[1], archivo, id);
  printf("\n\n");

  if (archivo) {  
    printf("lista de archivos almacenados: \n\n");
    imprimirLista(archivo);
  	printf("\nSe procede a borrar archivos repetidos... Presione ENTER\n\n");  	
    getchar();
    eliminarIguales(archivo);
  	printf("Nueva lista:\n\n");  	
  	imprimirLista(archivo); 	
  }
  else {
     printf("ERROR INESPERADO... archivo es NULL\n");
     return(1);
  }
  return EXIT_SUCCESS;
} 

lista_archivos *buscarArchivos(char *ruta, lista_archivos *p, int *id) {  

  long tamano;  
  printf("Entro a buscarArchivos con ruta: %s\n", ruta);
  DIR *directorio;
  struct dirent *info_archivo; // es quien contiene información sobre el archivo que se tenga en el momento
  unsigned char tipo; 
  char *nombrecompleto;
  directorio = opendir (ruta);

  if (directorio == NULL) 
    error("No puedo abrir el directorio");
  while ((info_archivo = readdir (directorio)) != NULL) 
    {             
      if ( (strcmp(info_archivo->d_name, ".") != 0) && (strcmp(info_archivo->d_name, "..") != 0) )
      {
        nombrecompleto = getFullName(ruta, info_archivo); 
        tipo = getFileType(nombrecompleto, info_archivo);         
        if (tipo == DT_REG)
        {                  
            tamano = procesarArchivo(nombrecompleto);
            (*id)++;                       
            p = agregar(p, *id, ruta, info_archivo, tamano, nombrecompleto);                                                                
        }
        else if (tipo == DT_DIR)
        {                                               
          p = buscarArchivos(nombrecompleto, p, id);                 
                           
        }
        else
          printf("tipo descnocido\n");
        free(nombrecompleto); 
      }
    }
  closedir (directorio); 
  return p;
}

void error(const char *s)
{
  /* perror() devuelve la cadena S y el error (en cadena de caracteres) que tenga errno */
  perror (s);
  exit(EXIT_FAILURE);
}

long procesarArchivo(char *archivo)
{  
  FILE *fichero;
  long tamano_fichero;

  fichero = fopen(archivo, "r"); // abrimos para solo "lectura"
  if (fichero)
    {
      fseek(fichero, 0L, SEEK_END);
      tamano_fichero = ftell(fichero);
      fclose(fichero);              
      return tamano_fichero;
    }
  else    
    printf ("%30s (No info.)\n", archivo);
	return 0;
}

lista_archivos *agregar(lista_archivos *p, int id, char *path, struct dirent *info, int the_size, char *nombrecompleto) {

  if (p == NULL) {
    p = malloc(sizeof(lista_archivos));
    p->id = id;
    snprintf(p->path, sizeof(p->path), "%s", path);
    //p->ruta = path;
    snprintf(p->name, sizeof(p->name), "%s", info->d_name);
    //p->nombre = info->d_name;
    snprintf(p->nombrecompleto, sizeof(p->nombrecompleto), "%s", nombrecompleto);
    p->ant = NULL;
    p->sig = NULL; 
    p->size = the_size;   
       
  } 
  else {
    if (p->sig == NULL){
      p->sig = malloc(sizeof(lista_archivos));
      p->sig->id = id;
      snprintf(p->sig->path, sizeof(p->sig->path), "%s", path);     
      snprintf(p->sig->name, sizeof(p->sig->name), "%s", info->d_name);     
      snprintf(p->sig->nombrecompleto, sizeof(p->sig->nombrecompleto), "%s", nombrecompleto);
      p->sig->ant = p;
      p->sig->sig = NULL;
      p->sig->size = the_size;
      
    }
    else {
      lista_archivos *i = p, *j = p->sig;
        while(j) {
           i = j;
           j = j->sig;
        }
      j = malloc(sizeof(lista_archivos));
      i->sig = j;
      j->ant = i;
      j->id = id;
      snprintf(j->path, sizeof(j->path), "%s", path);      
      snprintf(j->name, sizeof(j->name), "%s", info->d_name);     
      snprintf(j->nombrecompleto, sizeof(j->nombrecompleto), "%s", nombrecompleto);
      j->sig = NULL;
      j->size = the_size;
      
    }
  } 
  return p;
}

void imprimirLista(lista_archivos *p) {
 
 while(p){     
      printf("\t\tid: %i\n",p->id);
      printf("\t\tnombre: %s\n", p->name);
      printf("\t\tsize: %ld bytes\n",p->size);      
      printf("\t\truta: %s\n",p->path);
      printf("\t\tnombre completo: %s\n", p->nombrecompleto);
      printf("\n");
      p = p->sig;
  }
}

void eliminarIguales(lista_archivos *p) {

  while(p->sig){
      lista_archivos *k = p->sig;
      while (k){
        if (p->size == k->size) {                    
          printf("Son iguales: %s y %s\n\n", p->name, k->name);
          if (k->sig == NULL){                              
              k->ant->sig = NULL;            
            if (remove(k->nombrecompleto) == 0) {
              free(k);
              k = NULL;              
            }
            else
              printf("no borro\n");
          }
          else {                        
            k->ant->sig = k->sig;
            k->sig->ant = k->ant;
            lista_archivos *aux = k;
            k = k->sig;                                            
            if(remove(aux->nombrecompleto) == 0)
              free(aux);          
          }          
        }
        else
          k = k->sig;
      }
      p = p->sig;
    }
}

unsigned char getFileType(char *nombre, struct dirent *ent)
{
  unsigned char tipo;

  tipo = ent->d_type;
  if (tipo == DT_UNKNOWN)
    {
      tipo = statFileType(nombre);
    }

  return tipo;
}

unsigned char statFileType(char *fname)
{
  struct stat sdata;

  /* Intentamos el stat() si no funciona, devolvemos tipo desconocido */
  if (stat(fname, &sdata) == -1)
    {
      return DT_UNKNOWN;
    }

  switch (sdata.st_mode & S_IFMT) 
    {
    case S_IFBLK:  return DT_BLK;
    case S_IFCHR:  return DT_CHR;
    case S_IFDIR:  return DT_DIR;
    case S_IFIFO:  return DT_FIFO;
    case S_IFLNK:  return DT_LNK;
    case S_IFREG:  return DT_REG;
    case S_IFSOCK: return DT_SOCK;
    default:       return DT_UNKNOWN;
    }
}

char *getFullName(char *ruta, struct dirent *ent)
{
  char *nombrecompleto;
  int tmp;

  tmp = strlen(ruta);
  nombrecompleto = malloc(tmp + strlen(ent->d_name) + 2); /* Sumamos 2, por el \0 y la barra de directorios (/) no sabemos si falta */
  if (ruta[tmp-1] == '/')
    sprintf(nombrecompleto,"%s%s", ruta, ent->d_name);
  else
    sprintf(nombrecompleto,"%s/%s", ruta, ent->d_name);
  
  return nombrecompleto;
}