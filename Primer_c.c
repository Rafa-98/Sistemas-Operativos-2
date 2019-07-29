// C code for testing files in Linux

/*#include <dirent.h>
#include <stdio.h>
 
int main()
{
 	

	// ------------------------------------ ESTE CODIGO SOLO MUESTRA LOS DIRECTORIOS Y ARCHIVOS QUE SE ENCUENTRAN EN LA CARPETA QUE EN DIR *C SE LE INDICA --------------------------------------------------

 	DIR *d = opendir("/home/rafa-98/Descargas");
 	struct dirent *dentry;
 	size_t i=1;
 	long ftam;
 
 	while((dentry = readdir(d)) != NULL)
 	{
 	ftam = ftell(d);
  	printf("Nombre: %s\nTamaño: %ld bytes", dentry->d_name, ftam);
 	}
	return 0;
}

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------||*/

typedef struct lista
{
	int id;
	struct lista *sig;	
}lista;

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

/* Función para devolver un error en caso de que ocurra */
void error(const char *s);

/* Calculamos el tamaño del archivo */
long fileSize(char *fname);

/* Sacamos el tipo de archivo haciendo un stat(), es como el stat de la línea de comandos */
unsigned char statFileType(char *fname);

/* Intenta sacar el tipo de archivo del ent */
unsigned char getFileType(char *ruta, struct dirent *ent);

/* Obtiene el nombre del fichero con la ruta completa */
char *getFullName(char *ruta, struct dirent *ent);

/* Genera una cadena de espacios, para dibujar el árbol */
char *generaPosStr(int niv);

/* Función principal, que cuenta archivos */
unsigned cuentaArchivos(char *ruta, int niv);

int main(int argc, char *argv[])
{
  
	/*int *p = malloc(1 * sizeof(int));
	p[0] = 10;
	p = realloc(p, 3 * sizeof(int));
	p[1] = 20;
	p[2] = 30;	
	for (int i = 0; i < 3; i++) {
		printf("%i -- Direccion de memoria: %p\n", p[i], &(p[i]));
		if (p < &(p[i]))
			printf("Menor p que p[i]\n");
		else
			printf("mayor\n");
	}
	free(p);*/

	/*
	personas *persona = malloc(sizeof(personas));
	persona->ruta = argv[1];
	persona->cedula = 202;
	printf("%s y %i\n", persona->ruta, persona->cedula);*/
	
	
	/*lista *p;
	p = malloc(sizeof(lista));
	p->id = 1;
	p->sig = malloc(sizeof(lista));
	lista *k;
	k = p->sig;
	p->sig->id = 2;
	printf("Padre id: %i\n", p->id);
	printf("Hijo id: %i\n", k->id);*/
	

	

	
	


  unsigned num;

  if (argc != 2) 
    {
      error("Uso: ./directorio_2 <ruta>\n");
    }
  printf("Entrando en: %s\n", argv[1]);
  num = cuentaArchivos(argv[1], 1);
  printf("%s . Total: %u archivos\n", argv[1], num);
  /* Empezaremos a leer en el directorio actual */

  return EXIT_SUCCESS;
} 

void error(const char *s)
{
  /* perror() devuelve la cadena S y el error (en cadena de caracteres) que tenga errno */
  perror (s);
  exit(EXIT_FAILURE);
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

char *generaPosStr(int niv)
{
  int i;
  char *tmp = malloc(niv * 2 + 1);    /* Dos espacios por nivel más terminador */
  for (i = 0; i < niv * 2; ++i)
    tmp[i] = ' ';
  tmp[niv * 2] = '\0';
  return tmp;
}

unsigned cuentaArchivos(char *ruta, int niv)
{	long ftam;
  /* Con un puntero a DIR abriremos el directorio */
  DIR *dir;
  /* en *ent habrá información sobre el archivo que se está "sacando" a cada momento */
  struct dirent *ent;
  unsigned numfiles = 0;          /* Ficheros en el directorio actual */
  unsigned char tipo;       /* Tipo: fichero /directorio/enlace/etc */
  char *nombrecompleto;     /* Nombre completo del fichero */
  char *posstr;         /* Cadena usada para posicionarnos horizontalmente */
  dir = opendir (ruta);

  /* Miramos que no haya error */
  if (dir == NULL) 
    error("No puedo abrir el directorio");
  
  while ((ent = readdir (dir)) != NULL) 
    {
      if ( (strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0) )
    {
      nombrecompleto = getFullName(ruta, ent);
      tipo = getFileType(nombrecompleto, ent);
      if (tipo == DT_REG)
        {
          FILE *f = fopen(nombrecompleto, "r");
          fseek(f, 0L, SEEK_END);
          ftam = ftell(f);
          printf("Ruta: %s\n", nombrecompleto);
          printf("Tamaño: %ld Bytes\n", ftam);
          ++numfiles;
          fclose(f);
        }
      else if (tipo == DT_DIR)
        {
          posstr = generaPosStr(niv);
          printf("%sEntrando en: %s\n", posstr, nombrecompleto);          
          printf("%s%s . Total: %u archivos ", posstr, nombrecompleto, cuentaArchivos(nombrecompleto, niv + 1));
          /* Podemos poner las líneas que queramos */
          printf("\n");
          free(posstr);
        }
      free(nombrecompleto);
    }
    }
  closedir (dir);
  
  return numfiles;
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

/* stat() vale para mucho más, pero sólo queremos el tipo ahora */
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