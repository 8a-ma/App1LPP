/*
Integrantes:
Camilo López
Matías Ochoa
Fernando Zamora
Benjamín Wiedmaier
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 1024
#define MAX_RUT_LENGTH 12
#define MAX_PLAN_LENGTH 10
#define MAX_SEDE_LENGTH 10
#define MAX_DATE_LENGTH 10
#define MAX_NAME_LENGTH 50

// Estrucutra para los registros del csv.
typedef struct Registro {
  char *rut;
  char *nombre_completo;
  int edad;
  char *cod_plan;
  char *descripcion_plan;
  char *desde;
  char *hasta;
  char *cod_sede;
  char *ubicacion_sede;
} Registro;

// Estructura para las sedes.
typedef struct {
  char *cod_sede;
  char *ubicacion_sede;
} Sede;

// Estrucutra para los planes.
typedef struct {
  char *cod_plan;
  char *descripcion_plan;
} Plan;

//Función quecalcula la similitud entre dos cadenas de caracteres utilizando el algoritmo de distancia de Jaro.
double jaro_distance(char *s1, char *s2) {
  int s1_len = strlen(s1);
  int s2_len = strlen(s2);

  if (s1_len == 0 || s2_len == 0) {
    return 0;
  }

  int match_distance = (int)fmax(s1_len, s2_len) / 2 - 1;

  int s1_matches[s1_len];
  memset(s1_matches, 0, sizeof(s1_matches));
  int s2_matches[s2_len];
  memset(s2_matches, 0, sizeof(s2_matches));

  int matches = 0;
  for (int i = 0; i < s1_len; i++) {
    int start = fmax(0, i - match_distance);
    int end = fmin(i + match_distance + 1, s2_len);

    for (int k = start; k < end; k++) {
      if (s2_matches[k])
        continue;
      if (s1[i] != s2[k])
        continue;
      s1_matches[i] = 1;
      s2_matches[k] = 1;
      matches++;
      break;
    }
  }

  if (matches == 0) {
    return 0;
  }

  double t = 0.0;
  int k = 0;
  for (int i = 0; i < s1_len; i++) {
    if (!s1_matches[i])
      continue;
    while (!s2_matches[k])
      k++;
    if (s1[i] != s2[k])
      t += 0.5;
    k++;
  }

  double m = (double)matches;
  return (m / s1_len + m / s2_len + (m - t) / m) / 3.0;
}

//Función que calcula la similitud entre dos cadenas de caracteres utilizando el algoritmo de distancia de Jaro winkler.
double jaro_winkler_distance(char *s1, char *s2) {
  double jaro_dist = jaro_distance(s1, s2);

  int prefix = 0;
  for (int i = 0; i < fmin(4, fmin(strlen(s1), strlen(s2))); i++) {
    if (s1[i] == s2[i])
      prefix++;
    else
      break;
  }

  return jaro_dist + prefix * 0.1 * (1 - jaro_dist);
}

//Función que divide una línea de texto que contiene campos separados por comas en un array de cadenas de caracteres.
char **split_csv_line(char *line) {
  int num_fields = 1;
  for (int i = 0; line[i]; i++) {
    if (line[i] == ',') {
      num_fields++;
    }
  }

  char **fields = malloc(num_fields * sizeof(char *));
  int field_index = 0;
  char *field_start = line;
  for (int i = 0; line[i]; i++) {
    if (line[i] == ',') {
      line[i] = '\0';
      fields[field_index++] = strdup(field_start);
      field_start = &line[i + 1];
    }
  }
  fields[field_index] = strdup(field_start);

  return fields;
}

// Función para extraer los datos del csv a la estructura Registros.
Registro *leer_archivo(char *nombre_archivo, int *num_registros) {
  // Se abre el archivo y se verifica que se puede abrir y/o no existe.
  FILE *fp = fopen(nombre_archivo, "r");
  if (fp == NULL) {
    printf("No se pudo abrir el archivo %s.\n", nombre_archivo);
    return NULL;
  }

  Registro *registros = NULL;  // ?
  /* Se asigna el largo máximo de caracteres que
  puede tener la variable linea. */  
  char linea[MAX_LINE_LENGTH]; 
  /* Se crea el contador para saber la cantidad
  de registros válidos. */
  int num_lineas = 0;

  // Ignorar la primera línea
  if (fgets(linea, MAX_LINE_LENGTH, fp) == NULL) {
    printf("No se pudo leer la primera línea del archivo.\n");
  } else {
    printf("%s", "Se leyó la primera línea.");
  }

  // Se guardan los archivos del csv en la estructura de Registros.
  while (fgets(linea, MAX_LINE_LENGTH, fp) != NULL) {
    num_lineas++; // Se incrementa el número de líneas.
    // Se asgina espacio de memoriaa la variable registros?
    registros = realloc(registros,sizeof(Registro) * num_lineas);

    /* La variable resgistro_actual se le asgina una ubicación y su
    correspondiente memoria dinámica. */
    Registro *registro_actual = &registros[num_lineas - 1];

    char **fields = split_csv_line(linea); // ??
    int field_index =
        0; // Se crea una variable para el índice del registro_actual.

    /* Se guarda la información del csv en las variables de la 
    estructura Registros. */
    registro_actual->rut =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->nombre_completo =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->edad =
        fields[field_index++] ? atoi(fields[field_index - 1]) : 0;
    registro_actual->cod_plan =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->descripcion_plan =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->desde =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->hasta =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->cod_sede =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;
    registro_actual->ubicacion_sede =
        fields[field_index++] ? strdup(fields[field_index - 1]) : NULL;

    // Se libera la memoria de la variable fields. ?
    free(fields);
  }

  printf("%s\n", " "); // ?
  fclose(fp);          // Se cierra el archivo

  *num_registros = num_lineas; // Se guarda la cantidad de registros con la
                               // variable num_lineas.
  return registros;            // Se retorna registros??
}

// Función para extraer la información de las sedes del csv a la estructura
// Sede.
Sede *extraer_sedes(Registro *registros, int num_registros, int *num_sedes) {
  Sede *sedes = NULL; // ??
  *num_sedes = 0;     // ??

  // Se utiliza un ciclo for para recorrer el csv y extraer la información de
  // las sedes.
  for (int i = 0; i < num_registros; i++) {
    Registro *registro_actual = &registros[i];

    // Verificar si el código o la ubicación de la sede están vacíos
    if (registro_actual->cod_sede == NULL ||
        registro_actual->ubicacion_sede == NULL ||
        strcmp(registro_actual->cod_sede, "") == 0 ||
        strcmp(registro_actual->ubicacion_sede, "") == 0) {
      continue;
    }

    // Verificar si la sede ya está en el arreglo de sedes
    int sede_existente = 0;
    for (int j = 0; j < *num_sedes; j++) {
      if (strcmp(sedes[j].cod_sede, registro_actual->cod_sede) == 0) {
        sede_existente = 1;
        // Reemplazar la ubicación de la sede si la nueva ubicación es más larga
        if (strlen(registro_actual->ubicacion_sede) >
            strlen(sedes[j].ubicacion_sede)) {
          free(sedes[j].ubicacion_sede);
          sedes[j].ubicacion_sede = strdup(registro_actual->ubicacion_sede);
        }
        break;
      }
    }

    if (!sede_existente) {
      // Agregar la sede al arreglo de sedes
      sedes = realloc(sedes, sizeof(Sede) * (*num_sedes + 1));
      Sede *nueva_sede = &sedes[*num_sedes];
      nueva_sede->cod_sede = strdup(registro_actual->cod_sede);
      nueva_sede->ubicacion_sede = strdup(registro_actual->ubicacion_sede);
      (*num_sedes)++;
    }
  }

  return sedes;
}

// Función para extraer la información de los planes del csv a la estructura
// Plan.
Plan *extraer_planes(Registro *registros, int num_registros, int *num_planes) {
  Plan *planes = NULL; // ??
  *num_planes = 0;     // ??

  // Se utiliza un ciclo for para recorrer el csv y extraer la información de
  // los planes.
  for (int i = 0; i < num_registros; i++) {
    Registro *registro_actual = &registros[i];

    // Verificar si el código o la descripción del plan están vacíos
    if (registro_actual->cod_plan == NULL ||
        registro_actual->descripcion_plan == NULL ||
        strcmp(registro_actual->cod_plan, "") == 0 ||
        strcmp(registro_actual->descripcion_plan, "") == 0) {
      continue;
    }

    // Verificar si el plan ya está en el arreglo de planes
    int plan_existente = 0;
    for (int j = 0; j < *num_planes; j++) {
      if (strcmp(planes[j].cod_plan, registro_actual->cod_plan) == 0) {
        plan_existente = 1;
        // Reemplazar la descripción del plan si la nueva descripción es más
        // larga
        if (strlen(registro_actual->descripcion_plan) >
            strlen(planes[j].descripcion_plan)) {
          free(planes[j].descripcion_plan);
          planes[j].descripcion_plan =
              strdup(registro_actual->descripcion_plan);
        }
        break;
      }
    }

    if (!plan_existente) {
      // Agregar el plan al arreglo de planes
      planes = realloc(planes, sizeof(Plan) * (*num_planes + 1));
      Plan *nuevo_plan = &planes[*num_planes];
      nuevo_plan->cod_plan = strdup(registro_actual->cod_plan);
      nuevo_plan->descripcion_plan = strdup(registro_actual->descripcion_plan);
      (*num_planes)++;
    }
  }

  return planes;
}

// Función para imprimir los datos el cliente por su rut
void print_cliente_rut(Registro *registros, int num_registros, char *rut) {
  // Se utiliza un ciclo for para recorre el csv
  for (int i = 0; i < num_registros; i++) {
    /* Con la función strcmp podemos comparar si el rut ingresado es igual
    a algún rut del csv (Esta función retorna 0 si son iguales) y se
    imprimen los datos del cliente. */
    if (strcmp(registros[i].rut, rut) == 0) {
      printf("RUT: %s\n", registros[i].rut);
      printf("Nombre completo: %s\n", registros[i].nombre_completo);
      printf("Edad: %d\n", registros[i].edad);
      printf("Código de plan: %s\n", registros[i].cod_plan);
      printf("Descripción de plan: %s\n", registros[i].descripcion_plan);
      printf("Desde: %s\n", registros[i].desde);
      printf("Hasta: %s\n", registros[i].hasta);
      printf("Código de sede: %s\n", registros[i].cod_sede);
      printf("Ubicación de sede: %s\n", registros[i].ubicacion_sede);
      break;
    }
  }
}

// Función para validar si el rut es válido.
int validar_rut(char *rut) {
  int suma = 0;
  int multiplicador = 2;
  int verificador;
  int largo = strlen(rut);

  if (rut[largo - 2] != '-') {
    return 0;
  }

  if (rut[largo - 1] == 'K' || rut[largo - 1] == 'k') {
    verificador = 10;
  } else if (rut[largo - 1] >= '0' && rut[largo - 1] <= '9') {
    verificador = rut[largo - 1] - '0';
  } else {
    return 0;
  }

  for (int i = largo - 3; i >= 0; i--) {
    if (rut[i] >= '0' && rut[i] <= '9') {
      suma += (rut[i] - '0') * multiplicador;
      multiplicador++;
      if (multiplicador == 8) {
        multiplicador = 2;
      }
    } else {
      return 0;
    }
  }

  int resto = suma % 11;
  int resultado = 11 - resto;

  if (resultado == verificador) {
    return 1;
  } else {
    return 0;
  }
}

int existencia_rut(Registro *registros, int num_registros, char *rut) {
    for (int i = 0; i < num_registros; i++) {
        if (strcmp(registros[i].rut, rut) == 0) {
            return 1;
        }
    }
    return 0;
}

// Función para validar si la fecha de inicio es menor a la fecha de término de la inscripción.
int comparar_fechas(char *fecha1, char *fecha2) {
  int ano1, mes1, dia1;
  int ano2, mes2, dia2;

  sscanf(fecha1, "%d-%d-%d", &ano1, &mes1, &dia1);
  sscanf(fecha2, "%d-%d-%d", &ano2, &mes2, &dia2);

  if (ano1 < ano2) {
    return 1;
  } else if (ano1 == ano2) {
    if (mes1 < mes2) {
      return 1;
    } else if (mes1 == mes2) {
      if (dia1 < dia2) {
        return 1;
      }
    }
  }

  return 0;
}

// Esta función permite buscar al cliente por su rut.
int buscar_cliente_por_rut(Registro *registros, int num_registros, char *rut) {
  for (int i = 0; i < num_registros; i++) {
    if (strcasecmp(registros[i].rut, rut) == 0) {
      return i;
    }
  }
  return -1;
}

// Función que busca en el registro por nombre a un cliente
int buscar_registro_por_nombre(Registro *registros, int num_registros) {
    char nombre[1024];
    printf("Ingrese el nombre a buscar: ");
    getchar(); // Eliminar el carácter de nueva línea en el buffer
    fgets(nombre, sizeof(nombre), stdin);
    nombre[strcspn(nombre, "\n")] =
        '\0'; // Eliminar el carácter de nueva línea al final de la línea

    // Convertir el nombre a minúsculas para hacer la comparación case-insensitive
    for (int i = 0; nombre[i]; i++) {
        nombre[i] = tolower(nombre[i]);
    }

    // Calcular las distancias de Jaro-Winkler para cada registro
    double distancias[num_registros];
    for (int i = 0; i < num_registros; i++) {
        char nombre_completo[1024];
        strcpy(nombre_completo, registros[i].nombre_completo);

        // Convertir el nombre completo a minúsculas para hacer la comparación
        // case-insensitive
        for (int j = 0; nombre_completo[j]; j++) {
            nombre_completo[j] = tolower(nombre_completo[j]);
        }

        distancias[i] = jaro_winkler_distance(nombre, nombre_completo);
    }

    // Encontrar los 5 nombres con mayor coincidencia
    int indices[5] = {-1, -1, -1, -1, -1};
    double max_distancias[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < num_registros; i++) {
        for (int j = 0; j < 5; j++) {
            if (distancias[i] > max_distancias[j]) {
                for (int k = 4; k > j; k--) {
                    max_distancias[k] = max_distancias[k - 1];
                    indices[k] = indices[k - 1];
                }
                max_distancias[j] = distancias[i];
                indices[j] = i;
                break;
            }
        }
    }

    // Mostrar un menú con los nombres y RUT encontrados
    printf("\n");
    printf("Nombres encontrados:\n");
    printf("\n");
    for (int i = 0; i < 5 && indices[i] != -1; i++) {
        printf("%d. %s (%s)\n", i + 1, registros[indices[i]].nombre_completo,
               registros[indices[i]].rut);
    }
    printf("0. Salir\n");
    printf("\n");
    // Pedir al usuario que seleccione una opción
    int opcion;
    printf("Seleccione una opción: ");
    scanf("%d", &opcion);
    printf("\n");
    if (opcion >= 1 && opcion <= 5) {
        return indices[opcion - 1];
    } else {
        return -1;
    }
}

// Función que imprime un cliente
void imprimir_cliente(Registro *registros, int num_registros) {
  int opcion;
  int indice;
  printf("1. Buscar por RUT.\n");
  printf("2. Buscar por nombre.\n");
  printf("\n");
  printf("Seleccione una opción: ");
  scanf("%d", &opcion);
  printf("\n");

  if (opcion == 1) {
    char rut[20];
    printf("Ingrese el RUT del cliente: ");
    scanf("%s", rut);
    indice = buscar_cliente_por_rut(registros, num_registros, rut);
    if (indice == -1) {
      printf("No se encontro un cliente con el rut %s.\n", rut);
      return;
    }
  } else if (opcion == 2) {
    indice = buscar_registro_por_nombre(registros, num_registros);
    if (indice == -1) {
      printf("No se econtró un cliente con ese nombre.\n");
      return;
    }
  } else {
    printf("Opción inválida.\n");
    return;
  }

  printf("RUT: %s\n", registros[indice].rut);
  printf("Nombre completo: %s\n", registros[indice].nombre_completo);
  printf("Edad: %d\n", registros[indice].edad);
  printf("Código de plan: %s\n", registros[indice].cod_plan);
  printf("Descripción de plan: %s\n", registros[indice].descripcion_plan);
  printf("Desde: %s\n", registros[indice].desde);
  printf("Hasta: %s\n", registros[indice].hasta);
  printf("Código de sede: %s\n", registros[indice].cod_sede);
  printf("Ubicación de sede: %s\n", registros[indice].ubicacion_sede);
}

// Función para imprimir las sedes disponibles.
void imprimir_sedes(Sede *sedes, int num_sedes) {
  printf("Sedes:\n");
  printf("\n");
  /* Se utiliza un ciclo for para recorre el csv y
  extraer las distintas sedes que hay en éste. */
  for (int i = 0; i < num_sedes; i++) {
    printf("  Código sede: %s, Ubicación: %s\n", sedes[i].cod_sede,
           sedes[i].ubicacion_sede);
  }
}

// Función para imprimir los planes disponibles.
void imprimir_planes(Plan *planes, int num_planes) {
  printf("Planes:\n");
  printf("\n");
  /* Se utiliza un ciclo for para recorre el csv y
  extraer los distintos planes que hay en éste. */
  for (int i = 0; i < num_planes; i++) {
    printf("  Código plan: %s, Descripción: %s\n", planes[i].cod_plan,
           planes[i].descripcion_plan);
  }
}

// Función para agregar un cliente nuevo.
void agregar_cliente(Registro **registros_ptr, int *num_registros_ptr, Sede *sedes, int num_sedes, Plan *planes, int num_planes) {
  Registro nuevo_registro;

  // Se ingresa el rut del cliente y se guarda su valor en memoria dinámica.
  int opcion;
  do {
      // Se ingresa el rut del cliente y se guarda su valor en memoria dinámica.
      printf("Ingrese el rut del cliente: ");
      scanf("%ms", &nuevo_registro.rut);
      getchar();
      // Se verifica que el rut es válido.
      while (!validar_rut(nuevo_registro.rut)) {
          printf("El RUT ingresado no es válido. Por favor, ingréselo nuevamente: ");
          scanf("%ms", &nuevo_registro.rut);
          getchar();
      }
      // Se verifica si el rut ya existe en los registros.
      if (existencia_rut(*registros_ptr, *num_registros_ptr, nuevo_registro.rut)) {
          printf("RUT ya registrado.\n");
          printf("1. Ingresar otro RUT\n");
          printf("2. Salir\n");
          printf("Ingrese una opción: ");
          scanf("%d", &opcion);
          if (opcion == 2) {
              return;
          }
      } else {
          break;
      }
  } while (1);
  // ¿Se puede ingresar el mismo rut?

  /* Se ingresa el nombre completo del cliente y se guarda su valor
  en memoria dinámica */
  printf("Ingrese el nombre completo del cliente: ");
  size_t buffer_size = 0;                                        // ??
  getline(&nuevo_registro.nombre_completo, &buffer_size, stdin); // ??

  // Elimino la linea vacia que genera el getline
  size_t len = strlen(nuevo_registro.nombre_completo);
  if (len > 0 && nuevo_registro.nombre_completo[len - 1] == '\n') {
    nuevo_registro.nombre_completo[len - 1] = '\0';
  }

  // Se ingresa la edad del cliente
  printf("Ingrese la edad del cliente: ");
  scanf("%i", &nuevo_registro.edad);

  /* Se muestran las sedes en un listado, se pide que se ingrese el código
  de ésta y se guarda su valor en memoria dinámica. */
  imprimir_sedes(sedes, num_sedes);
  printf("\n");
  printf("Ingrese el código de la sede del cliente: ");
  scanf("%ms", &nuevo_registro.cod_sede);
  getchar();

  /* Se verifica que el código ingresado sea válido y/o corresponde
  a una sede existente. */
  int sede_encontrada = 0;
  while (!sede_encontrada) {
    for (int i = 0; i < num_sedes; i++) {
      if (strcmp(sedes[i].cod_sede, nuevo_registro.cod_sede) == 0) {
        nuevo_registro.ubicacion_sede = strdup(sedes[i].ubicacion_sede);
        sede_encontrada = 1;
        break;
      }
    }
    if (!sede_encontrada) {
      printf("El código de sede ingresado no es válido. Por favor, ingréselo nuevamente: ");
      scanf("%ms", &nuevo_registro.cod_sede);
      getchar();
    }
  }
  printf("\n");
  /* Se muestran los planes en un listado, se pide que se ingrese el código
  de ésta y se guarda su valor en memoria dinámica. */
  imprimir_planes(planes, num_planes);
  printf("\n");
  printf("Ingrese el codigo del plan del cliente: ");
  scanf("%ms", &nuevo_registro.cod_plan);
  getchar();
  /* Se verifica que el código ingresado sea válido y/o corresponde
  a un plan existente. */
  int plan_encontrado = 0;
  while (!plan_encontrado) {
    for (int i = 0; i < num_planes; i++) {
      if (strcmp(planes[i].cod_plan, nuevo_registro.cod_plan) == 0) {
        nuevo_registro.descripcion_plan = strdup(planes[i].descripcion_plan);
        plan_encontrado = 1;
        break;
      }
    }
    if (!plan_encontrado) {
      printf("El código de plan ingresado no es válido. Por favor, ingréselo nuevamente: ");
      scanf("%ms", &nuevo_registro.cod_plan);
      getchar();
    }
  }

  // Se ingresan las fechas de incio y término del plan del cliente.
  printf("Ingrese la fecha de inicio del plan del cliente (formato AAAA-MM-DD): ");
  scanf("%ms", &nuevo_registro.desde);
  // Truncar la fecha a 10 caracteres
  char *desde_truncado = malloc(11);
  strncpy(desde_truncado, nuevo_registro.desde, 10);
  desde_truncado[10] = '\0';
  free(nuevo_registro.desde);
  nuevo_registro.desde = desde_truncado;
  printf("Ingrese la fecha de termino del plan del cliente (formato AAAA-MM-DD): ");
  scanf("%ms", &nuevo_registro.hasta);
  // Truncar la fecha a 10 caracteres
  char *hasta_truncado = malloc(11);
  strncpy(hasta_truncado, nuevo_registro.hasta, 10);
  hasta_truncado[10] = '\0';
  free(nuevo_registro.hasta);
  nuevo_registro.hasta = hasta_truncado;
  // Se valida que las fechas ingresadas sean coherentes.
  while (!comparar_fechas(nuevo_registro.desde, nuevo_registro.hasta)) {
      printf("La fecha hasta debe ser posterior a la fecha desde. Por favor, ingréselas nuevamente.\n");
      printf("Ingrese la fecha de inicio del plan del cliente (formato AAAA-MM-DD): ");
      scanf("%ms", &nuevo_registro.desde);
      // Truncar la fecha a 10 caracteres
      strncpy(desde_truncado, nuevo_registro.desde, 10);
      desde_truncado[10] = '\0';
      free(nuevo_registro.desde);
      nuevo_registro.desde = desde_truncado;
      printf("Ingrese la fecha de termino del plan del cliente (formato AAAA-MM-DD): ");
      scanf("%ms", &nuevo_registro.hasta);
      // Truncar la fecha a 10 caracteres
      strncpy(hasta_truncado, nuevo_registro.hasta, 10);
      hasta_truncado[10] = '\0';
      free(nuevo_registro.hasta);
      nuevo_registro.hasta = hasta_truncado;
  }

  // Se incrementa el número de registros.
  (*num_registros_ptr)++;
  // Se reasigna el espacio de memoria dinámica.
  *registros_ptr =
      realloc(*registros_ptr, sizeof(Registro) * (*num_registros_ptr));
  (*registros_ptr)[*num_registros_ptr - 1] = nuevo_registro; // ?
}

// Función para agregar planes nuevos.
void agregar_plan(Plan **planes_ptr, int *num_planes) {
  Plan *planes = *planes_ptr;

  // Mostrar los planes existentes
  printf("Planes existentes:\n");
  printf("\n");
  for (int i = 0; i < *num_planes; i++) {
    Plan *plan_actual = &planes[i];
    printf("  - Código plan: %s, descripcion_plan: %s.\n", plan_actual->cod_plan, plan_actual->descripcion_plan);
  }

  // Pedir al usuario que ingrese los datos del nuevo plan
  char cod_plan[MAX_PLAN_LENGTH];
  char descripcion_plan[1024];
  printf("\n");
  printf("Ingrese el código del nuevo plan: ");
  scanf("%s", cod_plan);
  printf("Ingrese la descripción del nuevo plan: ");
  getchar(); // Eliminar el carácter de nueva línea en el buffer
  fgets(descripcion_plan, sizeof(descripcion_plan), stdin);
  // Eliminar el carácter de nueva línea al final de la línea
  descripcion_plan[strcspn(descripcion_plan, "\n")] = '\0';

  // Agregar el nuevo plan al arreglo de planes
  planes = realloc(planes, sizeof(Plan) * (*num_planes + 1));
  Plan *nuevo_plan = &planes[*num_planes];
  nuevo_plan->cod_plan = strdup(cod_plan);
  nuevo_plan->descripcion_plan = strdup(descripcion_plan);
  (*num_planes)++;

  // Actualizar el puntero al arreglo de planes
  *planes_ptr = planes;
}

// Función para eliminar planes.
void eliminar_plan(Registro *registros, int num_registros, Plan **planes_ptr, int *num_planes_ptr) {
  // Se imprimen los planes que no tienen ningún cliente registrado.
  printf("%s\n", "Planes sin registro:");
  int planes_sin_registros = 0;
  for (int i = 0; i < *num_planes_ptr; i++) {
    int plan_con_registro = 0;
    for (int j = 0; j < num_registros; j++) {
      if (strcmp(registros[j].cod_plan, (*planes_ptr)[i].cod_plan) == 0) {
        plan_con_registro = 1;
        break;
      }
    }
    if (!plan_con_registro) {
      planes_sin_registros++;
      printf("%s: %s\n", (*planes_ptr)[i].cod_plan,
             (*planes_ptr)[i].descripcion_plan);
    }
  }

  if (planes_sin_registros == 0) {
    printf("No hay planes sin registros asociados.\n");
    return;
  }

  printf("Ingrese el código del plan a eliminar: ");
  char cod_plan[MAX_PLAN_LENGTH];
  scanf("%s", cod_plan);

  // ???
  int plan_encontrado = 0;
  for (int i = 0; i < *num_planes_ptr; i++) {
    if (strcmp((*planes_ptr)[i].cod_plan, cod_plan) == 0) {
      plan_encontrado = 1;
      int plan_con_registro = 0;
      for (int j = 0; j < num_registros; j++) {
        if (strcmp(registros[j].cod_plan, cod_plan) == 0) {
          plan_con_registro = 1;
          break;
        }
      }
      if (plan_con_registro) {
        printf("No se puede eliminar el plan %s porque tiene registros asociados.\n",
               cod_plan);
      } else {
        free((*planes_ptr)[i].cod_plan);
        free((*planes_ptr)[i].descripcion_plan);
        for (int j = i + 1; j < *num_planes_ptr; j++) {
          (*planes_ptr)[j - 1] = (*planes_ptr)[j];
        }
        (*num_planes_ptr)--;
        *planes_ptr = realloc(*planes_ptr, sizeof(Plan) * (*num_planes_ptr));
        printf("Plan %s eliminado.\n", cod_plan);
      }
      break;
    }
  }

  if (!plan_encontrado) {
    printf("No se encontró el plan %s.\n", cod_plan);
  }
}

// Función para agregar sedes nuevas.
void agregar_sede(Sede **sedes_ptr, int *num_sedes) {
  Sede *sedes = *sedes_ptr;

  // Mostrar las sedes existentes
  printf("Sedes existentes:\n");
  printf("\n");
  for (int i = 0; i < *num_sedes; i++) {
    Sede *sede_actual = &sedes[i];
    printf("  - Código sede: %s, ubicacion_sede: %s\n", sede_actual->cod_sede, sede_actual->ubicacion_sede);
  }

  // Pedir al usuario que ingrese los datos de la nueva sede
  char cod_sede[MAX_SEDE_LENGTH];
  char ubicacion_sede[1024];
  printf("Ingrese el código de la nueva sede: ");
  scanf("%s", cod_sede);
  printf("Ingrese la ubicación de la nueva sede: ");
  getchar(); // Eliminar el carácter de nueva línea en el buffer
  fgets(ubicacion_sede, sizeof(ubicacion_sede), stdin);
  // Eliminar el carácter de nueva línea al final de la línea
  ubicacion_sede[strcspn(ubicacion_sede, "\n")] = '\0';

  // Agregar la nueva sede al arreglo de sedes
  sedes = realloc(sedes, sizeof(Sede) * (*num_sedes + 1));
  Sede *nueva_sede = &sedes[*num_sedes];
  nueva_sede->cod_sede = strdup(cod_sede);
  nueva_sede->ubicacion_sede = strdup(ubicacion_sede);
  (*num_sedes)++;

  // Actualizar el puntero al arreglo de sedes
  *sedes_ptr = sedes;
}

// Función para eliminar sedes existentes.
void eliminar_sede(Registro *registros, int num_registros, Sede **sedes_ptr, int *num_sedes_ptr) {
  // Se imprimen las sedes que no tienen ningún cliente registrado.
  printf("%s\n", "Sedes sin registro:");
  int sedes_sin_registros = 0;
  for (int i = 0; i < *num_sedes_ptr; i++) {
    int sede_con_registro = 0;
    for (int j = 0; j < num_registros; j++) {
      if (strcmp(registros[j].cod_sede, (*sedes_ptr)[i].cod_sede) == 0) {
        sede_con_registro = 1;
        break;
      }
    }

    if (!sede_con_registro) {
      sedes_sin_registros++;
      printf("%s: %s\n", (*sedes_ptr)[i].cod_sede,
             (*sedes_ptr)[i].ubicacion_sede);
    }
  }

  if (sedes_sin_registros == 0) {
    printf("No hay sedes sin registros asociados.\n");
    return;
  }

  printf("Ingrese el código de la sede a eliminar: ");
  char cod_sede[MAX_SEDE_LENGTH];
  scanf("%s", cod_sede);

  // ??
  int sede_encontrada = 0;
  for (int i = 0; i < *num_sedes_ptr; i++) {
    if (strcmp((*sedes_ptr)[i].cod_sede, cod_sede) == 0) {
      sede_encontrada = 1;
      int sede_con_registro = 0;
      for (int j = 0; j < num_registros; j++) {
        if (strcmp(registros[j].cod_sede, cod_sede) == 0) {
          sede_con_registro = 1;
          break;
        }
      }

      if (sede_con_registro) {
        printf("No se puede eliminar la sede %s porque tiene registros asociados.\n",
               cod_sede);
      } else {
        free((*sedes_ptr)[i].cod_sede);
        free((*sedes_ptr)[i].ubicacion_sede);
        for (int j = i + 1; j < *num_sedes_ptr; j++) {
          (*sedes_ptr)[j - 1] = (*sedes_ptr)[j];
        }
        (*num_sedes_ptr)--;
        *sedes_ptr = realloc(*sedes_ptr, sizeof(Sede) * (*num_sedes_ptr));
        printf("Sede %s eliminada.\n", cod_sede);
      }
      break;
    }
  }

  if (!sede_encontrada) {
    printf("No se encontró una sede con el código %s.\n", cod_sede);
  }
}

// Función para eliminar los registros con un rut inválido.
void eliminar_registros_invalidos_rut(Registro **registros_ptr,int *num_registros) {
  Registro *registros = *registros_ptr;
  int num_validos = 0;

  /* Se utiliza un ciclo for para recorrer el csv y
  obtener los registros con un rut inválido. */
  for (int i = 0; i < *num_registros; i++) {
    if (registros[i].rut != NULL && validar_rut(registros[i].rut)) {
      registros[num_validos] = registros[i];
      num_validos++;
    } else {
      free(registros[i].rut);
      free(registros[i].nombre_completo);
      free(registros[i].cod_plan);
      free(registros[i].descripcion_plan);
      free(registros[i].desde);
      free(registros[i].hasta);
      free(registros[i].cod_sede);
      free(registros[i].ubicacion_sede);
    }
  }

  *num_registros = num_validos;
  // Se reasigna el espacio de memoria de la estructura Registro.
  *registros_ptr = realloc(registros, sizeof(Registro) * num_validos);
}

// Función para eliminar los registros sin una fecha de término.
void eliminar_registros_sin_hasta(Registro **registros_ptr, int *num_registros) {
  Registro *registros = *registros_ptr;
  int num_validos = 0;

  /* Se utiliza un ciclo for para recorrer el csv y
  obtener los registros sin fechas de término. */
  for (int i = 0; i < *num_registros; i++) {
    if (registros[i].hasta != NULL) {
      registros[num_validos] = registros[i];
      num_validos++;
    } else {
      free(registros[i].rut);
      free(registros[i].nombre_completo);
      free(registros[i].cod_plan);
      free(registros[i].descripcion_plan);
      free(registros[i].desde);
      free(registros[i].hasta);
      free(registros[i].cod_sede);
      free(registros[i].ubicacion_sede);
    }
  }

  *num_registros = num_validos;
  // Se reasigna el espacio de memoria de la estructura Registro.
  *registros_ptr = realloc(registros, sizeof(Registro) * num_validos);
}

// Función para eliminar los registros con fecha de término inválida.
void eliminar_registros_hasta_invalidos(Registro **registros_ptr, int *num_registros_ptr) {
  Registro *registros = *registros_ptr;
  int num_registros = *num_registros_ptr;
  int num_registros_validos = 0;

  /* Se utiliza un ciclo for para recorrer el csv y
  obtener los registros con fecha de término inválidas. */
  for (int i = 0; i < num_registros; i++) {
    struct tm tm;
    if (strptime(registros[i].hasta, "%Y-%m-%d", &tm) != NULL) {
      registros[num_registros_validos++] = registros[i];
    } else {
      free(registros[i].rut);
      free(registros[i].nombre_completo);
      free(registros[i].cod_plan);
      free(registros[i].descripcion_plan);
      free(registros[i].desde);
      free(registros[i].hasta);
      free(registros[i].cod_sede);
      free(registros[i].ubicacion_sede);
    }
  }

  // Se reasigna el espacio de memoria de la estructura Registro.
  registros = realloc(registros, sizeof(Registro) * num_registros_validos);
  *registros_ptr = registros;
  *num_registros_ptr = num_registros_validos;
}

void eliminar_registros_fechas_invalidas(Registro **registros_ptr, int *num_registros_ptr) {
  Registro *registros = *registros_ptr;
  int num_registros = *num_registros_ptr;
  int num_registros_validos = 0;
  //Recorre el registro y elimina los que tienen fecha desde mayores a hasta
  for (int i = 0; i < num_registros; i++) {
    struct tm tm;
    if (comparar_fechas(registros[i].desde, registros[i].hasta) == 1 ) {
      registros[num_registros_validos++] = registros[i];
    } else {
      free(registros[i].rut);
      free(registros[i].nombre_completo);
      free(registros[i].cod_plan);
      free(registros[i].descripcion_plan);
      free(registros[i].desde);
      free(registros[i].hasta);
      free(registros[i].cod_sede);
      free(registros[i].ubicacion_sede);
    }
  }

  // Se reasigna el espacio de memoria de la estructura Registro.
  registros = realloc(registros, sizeof(Registro) * num_registros_validos);
  *registros_ptr = registros;
  *num_registros_ptr = num_registros_validos;
  
}

void agregar_registro_fecha_desde_nulas(Registro **registros_ptr, int *num_registros_ptr) {
  Registro *registros = *registros_ptr;
  int num_registros = *num_registros_ptr;
  

  for(int i = 0; i <num_registros; i++){
    
    if(strlen(registros[i].desde) < 1){
      int ano2, mes2, day;
      strdup(registros[i].desde);

      
      sscanf(registros[i].hasta, "%d-%d-%i", &ano2, &mes2, &day);
      struct tm fecha = {.tm_year=ano2-1900, .tm_mon=mes2-1, .tm_mday=day};
      fecha.tm_mon -= 3;
      mktime(&fecha);

      strftime(registros[i].desde, 13, "%Y-%m-%d", &fecha);
    }
    
  }
  
  registros = realloc(registros, sizeof(Registro) * num_registros);
  *registros_ptr = registros;
  *num_registros_ptr = num_registros;
}

void eliminar_registros_sin_sede(Registro **registros_ptr, int *num_registros) {
  Registro *registros = *registros_ptr;
  int num_nuevos_registros = 0;

  for (int i = 0; i < *num_registros; i++) {
    Registro *registro_actual = &registros[i];

    // Verificar si el código o la ubicación de la sede están vacíos
    if (registro_actual->cod_sede == NULL || registro_actual->ubicacion_sede == NULL ||
    strcmp(registro_actual->cod_sede, "") == 0 ||
    strcmp(registro_actual->ubicacion_sede, "") == 0) {
      continue;
    }

    // Copiar el registro al nuevo arreglo de registros
    registros[num_nuevos_registros++] = *registro_actual;
  }

  // Reasignar el arreglo de registros y actualizar el número de registros
  registros = realloc(registros, sizeof(Registro) * num_nuevos_registros);
  *registros_ptr = registros;
  *num_registros = num_nuevos_registros;
}

// Función para agregar la ubicación de los registros que no tienen una
// asociada.
void reemplazar_ubicaciones_sede_vacias(Registro *registros, int num_registros, Sede *sedes, int num_sedes) {
  for (int i = 0; i < num_registros; i++) {
    Registro *registro_actual = &registros[i];

    // Buscar la sede correspondiente en el arreglo de sedes
    for (int j = 0; j < num_sedes; j++) {
      Sede *sede_actual = &sedes[j];
      if (strcmp(sede_actual->cod_sede, registro_actual->cod_sede) == 0) {
        // Verificar si la longitud de la ubicación de la sede es diferente
        if (strlen(registro_actual->ubicacion_sede) !=
            strlen(sede_actual->ubicacion_sede)) {
          // Reemplazar la ubicación de la sede con la ubicación de la sede
          // correspondiente
          free(registro_actual->ubicacion_sede);
          registro_actual->ubicacion_sede = strdup(sede_actual->ubicacion_sede);
        }
        break;
      }
    }
  }
}

// Función para eliminar los registros sin un plan asociado.
void eliminar_registros_sin_cod_plan(Registro **registros_ptr, int *num_registros) {
  Registro *registros = *registros_ptr;
  int num_nuevos_registros = 0;

  /* Se utiliza un ciclo for para recorrer el csv y
  obtener los registros sin un plan asociado. */
  for (int i = 0; i < *num_registros; i++) {
    Registro *registro_actual = &registros[i];
    // Verificar si el código del plan está vacío
    if (registro_actual->cod_plan == NULL ||
        strcmp(registro_actual->cod_plan, "") == 0) {
      continue;
    }

    // Copiar el registro al nuevo arreglo de registros
    registros[num_nuevos_registros++] = *registro_actual;
  }

  // Reasignar el arreglo de registros y actualizar el número de registros
  registros = realloc(registros, sizeof(Registro) * num_nuevos_registros);
  *registros_ptr = registros;
  *num_registros = num_nuevos_registros;
}

// Función para agregar la descripción del plan de los registros, si éste no
// tiene una asociada.
void reemplazar_descripciones_plan_vacias(Registro *registros, int num_registros, Plan *planes, int num_planes) {
  for (int i = 0; i < num_registros; i++) {
    Registro *registro_actual = &registros[i];

    // Verificar si la descripción del plan está vacía
    if (registro_actual->descripcion_plan == NULL || strcmp(registro_actual->descripcion_plan, "") == 0) {
      // Buscar el plan correspondiente en el arreglo de planes
      for (int j = 0; j < num_planes; j++) {
        Plan *plan_actual = &planes[j];
        if (strcmp(plan_actual->cod_plan, registro_actual->cod_plan) == 0) {
          // Reemplazar la descripción del plan vacía con la descripción del
          // plan correspondiente
          free(registro_actual->descripcion_plan);
          registro_actual->descripcion_plan = strdup(plan_actual->descripcion_plan);
          break;
        }
      }
    }
  }
}

// Función para eliminar a un cliente.
void eliminar_cliente(Registro **registros_ptr, int *num_registros_ptr) {
  char *rut;
  printf("Ingrese el rut del cliente a eliminar: ");
  scanf("%ms", &rut);

  int indice = buscar_cliente_por_rut(*registros_ptr, *num_registros_ptr, rut);
  if (indice == -1) {
    printf("No se encontró un cliente con el rut %s.\n", rut);
    return;
  }

  Registro *registros = *registros_ptr;
  int num_registros = *num_registros_ptr;

  // Se libera la memoria utilizada por el cliente.
  free(registros[indice].rut);
  free(registros[indice].nombre_completo);
  free(registros[indice].cod_plan);
  free(registros[indice].descripcion_plan);
  free(registros[indice].desde);
  free(registros[indice].hasta);
  free(registros[indice].cod_sede);
  free(registros[indice].ubicacion_sede);

  for (int i = indice; i < num_registros - 1; i++) {
    registros[i] = registros[i + 1];
  }

  (*num_registros_ptr)--;
  *registros_ptr =realloc(*registros_ptr, sizeof(Registro) * (*num_registros_ptr));
}

// Función para modificar a un cliente.
void modificar_registro_interactivo(Registro *registros, int num_registros, Plan *planes, int num_planes, Sede *sedes, int num_sedes) {
    printf("1. Buscar por RUT\n");
    printf("2. Buscar por nombre\n");
    printf("Seleccione una opción: ");
    int opcion;
    scanf("%d", &opcion);

    int registro_index;
    if (opcion == 1) {
        char rut[MAX_RUT_LENGTH];
        printf("Ingrese el RUT del cliente a modificar: ");
        scanf("%s", rut);
        registro_index = buscar_cliente_por_rut(registros, num_registros, rut);
        if (registro_index == -1) {
            printf("No se encontró un cliente con el RUT %s\n", rut);
            return;
        }
    } else if (opcion == 2) {
        registro_index = buscar_registro_por_nombre(registros, num_registros);
        if (registro_index == -1) {
            printf("No se encontró un cliente con ese nombre\n");
            return;
        }
    } else {
        printf("Opción inválida\n");
        return;
    }

    printf("Datos actuales del cliente:\n");
    print_cliente_rut(registros, num_registros, registros[registro_index].rut);

    opcion = 1;
    while (opcion != 0) {
        printf("\n¿Qué dato desea modificar?\n");
        printf("1. Nombre completo\n");
        printf("2. Edad\n");
        printf("3. Plan\n");
        printf("4. Desde\n");
        printf("5. Hasta\n");
        printf("6. Sede\n");
        printf("0. Salir\n");
        printf("Opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                printf("Nuevo nombre completo: ");
                scanf(" %[^\n]s", registros[registro_index].nombre_completo);
                break;
            case 2:
                printf("Nueva edad: ");
                scanf("%d", &registros[registro_index].edad);
                break;
            case 3:
                imprimir_planes(planes, num_planes);
                printf("%s","Nuevo código de plan: ");
                char cod_plan[MAX_PLAN_LENGTH];
                scanf("%s", cod_plan);
                int plan_valido = 0;
                for (int i = 0; i < num_planes; i++) {
                    if (strcmp(planes[i].cod_plan, cod_plan) == 0) {
                        plan_valido = 1;
                        free(registros[registro_index].cod_plan);
                        registros[registro_index].cod_plan = strdup(cod_plan);
                        free(registros[registro_index].descripcion_plan);
                        registros[registro_index].descripcion_plan = strdup(planes[i].descripcion_plan);
                        break;
                    }
                }
                if (!plan_valido) {
                    printf("Código de plan inválido\n");
                } else {
                    char desde[MAX_DATE_LENGTH];
                    char hasta[MAX_DATE_LENGTH];
                    do {
                        printf("Nueva fecha desde (AAAA-MM-DD): ");
                        scanf("%s", desde);
                        printf("Nueva fecha hasta(AAAA-MM-DD): ");
                        scanf("%s", hasta);
                        if (!comparar_fechas(desde, hasta)) {
                            printf("La fecha hasta debe ser posterior a la fecha desde. Por favor, ingréselas nuevamente.\n");
                        }
                    } while (!comparar_fechas(desde, hasta));
                    free(registros[registro_index].desde);
                    registros[registro_index].desde = strdup(desde);
                    free(registros[registro_index].hasta);
                    registros[registro_index].hasta = strdup(hasta);
                }
                break;
            case 4:
                printf("Nuevo desde: ");
                scanf("%s", registros[registro_index].desde);
                char *desde_truncado = malloc(11);
                strncpy(desde_truncado, registros[registro_index].desde, 10);
                desde_truncado[10] = '\0';
                free(registros[registro_index].desde);
                registros[registro_index].desde = desde_truncado;
                break;
            case 5:
                printf("Nuevo hasta: ");
                scanf("%s", registros[registro_index].hasta);
                char *hasta_truncado = malloc(11);
                strncpy(hasta_truncado, registros[registro_index].hasta, 10);
                hasta_truncado[10] = '\0';
                free(registros[registro_index].hasta);
                registros[registro_index].hasta = hasta_truncado;
                break;
            case 6:
                imprimir_sedes(sedes, num_sedes);
                printf("Nuevo código de sede: ");
                char cod_sede[MAX_SEDE_LENGTH];
                scanf("%s", cod_sede);
                int sede_valida = 0;
                for (int i = 0; i < num_sedes; i++) {
                    if (strcmp(sedes[i].cod_sede, cod_sede) == 0) {
                        sede_valida = 1;
                        free(registros[registro_index].cod_sede);
                        registros[registro_index].cod_sede = strdup(cod_sede);
                        free(registros[registro_index].ubicacion_sede);
                        registros[registro_index].ubicacion_sede = strdup(sedes[i].ubicacion_sede);
                        break;
                    }
                }
                if (!sede_valida) {
                    printf("Código de sede inválido\n");
                }
                break;
            case 0:
                break;
            default:
                printf("Opción inválida\n");
        }
    }
}

// Función para mostrar el listado de clientes.
void mostrar_clientes(Registro *registros, int num_registros) {
  // Se recorre el csv y se van imprimiendo los datos de los clientes.
  for (int i = 0; i < num_registros; i++) {
    printf("Reg. %i: %s | %s | %i | %s | %s | %s | %s | %s | %s \n", i + 1,
           registros[i].rut, registros[i].nombre_completo, registros[i].edad,
           registros[i].cod_plan, registros[i].descripcion_plan,
           registros[i].desde, registros[i].hasta, registros[i].cod_sede,
           registros[i].ubicacion_sede);
  }
  return;
}

void ordenar_registros(Registro *registros, int num_registros) {
  for (int i = 0; i < num_registros - 1; i++) {
    for (int j = 0; j < num_registros - i - 1; j++) {
      if (strcasecmp(registros[j].nombre_completo,
                 registros[j + 1].nombre_completo) > 0) {
        Registro temp = registros[j];
        registros[j] = registros[j + 1];
        registros[j + 1] = temp;
      }
    }
  }
}

// Función para guardar los datos nuevo y/o modificados al csv.
void guardar_archivo_csv(char *nombre_archivo, Registro *registros, int num_registros) {
  FILE *fp_csv = fopen(nombre_archivo, "w");

  if (fp_csv == NULL) {
    fclose(fp_csv);
    fp_csv = fopen(nombre_archivo, "a+");
    fclose(fp_csv);
    fp_csv = fopen(nombre_archivo, "w");
    return;
  }

  if (ftruncate(fileno(fp_csv), 0) != 0) {
    printf("Error truncado CSV.\n");
    return;
  }
  ftruncate(fileno(fp_csv), 0);

  fprintf(fp_csv, "rut,nombre_completo,edad,cod_plan,descripcion_plan,desde,hasta,cod_sede,ubicacion_sede\n");

  for (int i = 0; i < num_registros; i++) {
    fprintf(fp_csv, "%s,%s,%i,%s,%s,%s,%s,%s,%s",
      registros[i].rut,
      registros[i].nombre_completo,
      registros[i].edad,
      registros[i].cod_plan,
      registros[i].descripcion_plan,
      registros[i].desde,
      registros[i].hasta,
      registros[i].cod_sede,
      registros[i].ubicacion_sede);
  }

  fclose(fp_csv);
}

// Función para guardar los datos del csv antes de las modificaciones hechas por el usuario.
void guardar_archivo_bak(char *nombre_archivo, Registro *registros, int num_registros) {
  FILE *fp_bak = fopen(nombre_archivo, "w");

  if (fp_bak == NULL) {
    fclose(fp_bak);
    fp_bak = fopen(nombre_archivo, "a+");
    fclose(fp_bak);
    fp_bak = fopen(nombre_archivo, "w");
    return;
  }

  if (ftruncate(fileno(fp_bak), 0) != 0) {
    printf("Error truncado BAK.\n");
    return;
  }
  ftruncate(fileno(fp_bak), 0);

  fprintf(fp_bak, "rut,nombre_completo,edad,cod_plan,descripcion_plan,desde,hasta,cod_sede,ubicacion_sede\n");

  for (int i = 0; i < num_registros; i++) {
    fprintf(fp_bak, "%s,%s,%i,%s,%s,%s,%s,%s,%s",
    registros[i].rut,
    registros[i].nombre_completo,
    registros[i].edad,
    registros[i].cod_plan,
    registros[i].descripcion_plan,
    registros[i].desde,
    registros[i].hasta,
    registros[i].cod_sede,
    registros[i].ubicacion_sede);
  }

  fclose(fp_bak);
}

// Esta función permite hacer un display de un menú con las distintas funcionalidades solicitadas.

void menu(Registro **registros, int *num_registros, Sede *sedes, int num_sedes, Plan *planes, int num_planes) {
    int opcion;
    do {
        printf("%s %i\n", "numero de registros: ", *num_registros);
        printf("Menu:\n");
        printf("01. Agregar cliente\n");
        printf("02. Eliminar cliente\n");
        printf("03. Editar cliente\n");
        printf("04. Visualizar cliente\n");
        printf("05. Agregar sede\n");
        printf("06. Eliminar sede\n");
        printf("07. Ver sedes\n");
        printf("08. Agregar plan\n");
        printf("09. Eliminar plan\n");
        printf("10. Ver planes\n");
        printf("11. Ver plantilla clientes\n");
        printf("12. Ordenar registros alfabeticamente\n");
        printf("0. Salir\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                agregar_cliente(registros, num_registros, sedes, num_sedes, planes, num_planes);
                break;
            case 2:
                eliminar_cliente(registros, num_registros);
                break;
            case 3:
                modificar_registro_interactivo(*registros, *num_registros, planes, num_planes, sedes, num_sedes);
                break;
            case 4:
                imprimir_cliente(*registros, *num_registros);
                break;
            case 5:
                agregar_sede(&sedes, &num_sedes);
                break;
            case 6:
                eliminar_sede(*registros, *num_registros, &sedes, &num_sedes);
                break;
            case 7:
                imprimir_sedes(sedes, num_sedes);
                break;
            case 8:
                agregar_plan(&planes, &num_planes);
                break;
            case 9:
                eliminar_plan(*registros,*num_registros,&planes,&num_planes);
                break;
            case 10:
                imprimir_planes(planes,num_planes);
                break;
            case 11:
                mostrar_clientes(*registros,*num_registros);
                break;
            case 12:
                ordenar_registros(*registros,*num_registros);
                break;
            case 0:
                break;
            default:
                printf("Opción inválida\n");
                break;
        }
    } while (opcion != 0);
}

int main() {
  int num_registros;
  Registro *registros = leer_archivo("BigMuscle.csv", &num_registros);
  guardar_archivo_bak("BM.bak", registros,num_registros);
  eliminar_registros_invalidos_rut(&registros, &num_registros);
  eliminar_registros_sin_hasta(&registros, &num_registros);
  eliminar_registros_hasta_invalidos(&registros, &num_registros);
  eliminar_registros_fechas_invalidas(&registros, &num_registros);
  eliminar_registros_sin_sede(&registros, &num_registros);
  eliminar_registros_sin_cod_plan(&registros, &num_registros);
  
  int num_sedes;
  Sede *sedes = extraer_sedes(registros, num_registros, &num_sedes);
  reemplazar_ubicaciones_sede_vacias(registros, num_registros, sedes, num_sedes);
  int num_planes;
  Plan *planes = extraer_planes(registros, num_registros, &num_planes);
  reemplazar_descripciones_plan_vacias(registros, num_registros, planes, num_planes);
  
  agregar_registro_fecha_desde_nulas(&registros, &num_registros);
  
  printf("%s %i\n", "numero de registros iniciales: ", num_registros);
  
  menu(&registros,&num_registros,sedes,num_sedes,planes,num_planes);
  
  
  guardar_archivo_csv("BigMuscle.csv", registros, num_registros);
  return 0;
}