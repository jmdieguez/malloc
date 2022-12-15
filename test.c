#include "malloc.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#include "printfmt.h"

#include <assert.h>

#define HELLO "TEST\n"
#define TEST_STRING "FISOP malloc is working!"

#define GREEN "\e[0;32m"
#define RED "\e[0;31m"
#define RESET "\e[0m"

extern struct block *block_list;
extern int amount_of_blocks;
extern int amount_of_mallocs;
extern int amount_of_frees;
extern int requested_memory;
extern int amount_of_splits;
extern int amount_of_coalesing;

void iniciliazar_malloc(void);
void liberar_memoria(void);
int get_size_of_total_memory(void);
int test_001_malloc_menor_a_MIN_SIZE(void);
int test_002_malloc_igual_a_0(void);
int test_003_malloc_menor_a_0(void);
int test_007_malloc_falla_bloque_mayor_tamanio_grande(void);
int test_021_3_malloc_1_free_1_malloc_menor_debe_splittar(void);
int test_017_3_mallocs_2_free_debe_hacer_coaleasing_bis(void);
int test_018_3_mallocs_2_free_debe_hacer_coaleasing(void);
int test_020_4_mallocs_2_free_NO_debe_hacer_colleasing(void);
int test_008_malloc_y_free_pruebo_unmap_free_blocks(void);
int test_009_doble_free(void);
int test_010_free_region_invalida(void);
int test_011_doble_malloc_pruebo_splitting(void);
int test_012_doble_malloc_y_free_en_bloque_pruebo_coalesing(void);
int test_012bis_doble_malloc_y_free_en_bloque_pruebo_coalesing(void);
int test_013_doble_malloc_y_libero_ultimo_primero_en_bloque_pruebo_coalesing(void);
int test_014_doble_malloc_y_free_intercalado_en_un_bloque(void);
int test_015_3_mallocs_1_free_debe_alocar_primero(void);
int test_016_2_mallocs_1_free_1_malloc_menor_debe_alocar_primero_y_hacer_split(void);
int test_017_3_mallocs_2_free_debe_hacer_coaleasing(void);
int test_004_malloc_bloque_tamanio_pequenio(void);
int test_005_malloc_bloque_tamanio_mediano(void);
int test_006_malloc_bloque_tamanio_grande(void);
int test_106_5_mallocs_obtengo_5_bloques(void);
int test_107_5_mallocs_5_bloques_free_primero(void);
int test_108_5_mallocs_5_bloques_free_ultimo(void);
int test_109_5_mallocs_5_bloques_free_medio(void);
int test_017_3_mallocs_2_free_debe_hacer_coaleasing_bis(void);
int test_018_3_mallocs_2_free_debe_hacer_coaleasing(void);
int test_019_3_mallocs_3_free_debe_2_colleasing_0_blocks(void);
int test_020_4_mallocs_2_free_NO_debe_hacer_colleasing(void);
int test_021_3_malloc_1_free_1_malloc_menor_debe_splittar(void);
int test_022_3_malloc_2_free_adyacentes_1_malloc_que_necesita_coleasing(void);
int test_101_2_mallocs_obtengo_bloques_Big_size(void);
int test_102_2_mallocs_obtengo_bloques_small_size(void);

int test_103_2_mallocs_obtengo_bloques_medium_size(void);
int test_104_2_mallocs_obtengo_bloques_small_y_medium_size(void);
int test_104bis_2_mallocs_obtengo_bloques_small_y_medium_size(void);
int test_105_3_mallocs_obtengo_3_bloques_small_size(void);
int test_110_3_mallocs_2_bloques(void);
int test_110bis_3_mallocs_2_bloques(void);
int test_111_3_mallocs_2_bloques_libero_bloque_de_2_regiones(void);
int test_111bis_3_mallocs_2_bloques_libero_bloque_de_2_regiones(void);
int test_112_5_mallocs_3_bloques_libero_2_bloques(void);
int test_112bis_5_mallocs_3_bloques_libero_2_bloques(void);
int test_113_7_mallocs_3_bloques_3_free_1_bloque_y_malloc_hace_coleasing(void);
int test_113bis_7_mallocs_3_bloques_3_free_1_bloque_y_malloc_hace_coleasing(void);
int test_200_calloc_1_bloque(void);
int test_201_calloc_1_bloque_debe_split(void);
int test_202_calloc_1_bloque_big_size(void);
int test_203_calloc_falla_tamanio_mayor_big_size(void);
int test_204_calloc_falla_parametro_igual_0_o_NULL(void);
int test_204bis_calloc_falla_parametro_igual_0_o_NULL(void);
int test_205_calloc_parametros_negativos(void);
int test_206_calloc_y_free(void);
int test_207_3calloc_y_1_free_2_bloques(void);
int test_208_realloc_a_mayor(void);
int test_209_realloc_a_menor(void);
int test_210_realloc_size_igual_NULL_realiza_free(void);
int test_211_realloc_puntero_igual_NULL_hace_malloc(void);
int test_212_realloc_puntero_liberado(void);
int test_213_realloc_size_mas_grande_que_el_maximo(void);
int test_214_realloc_multiples_size_mas_grande_que_el_maximo(void);
int test_215_realloc_conservar_valor_previa_relocacion(void);
int test_216_realloc_verificar_que_sea_distion_puntero_cuando_el_tamaño_pedido_sea_mayor(
        void);
int test_216_realloc_verificar_que_sea_el_mismo_puntero_cuando_el_tamaño_pedido_sea_menor(
        void);
extern int amount_of_small_blocks;
extern int amount_of_media_blocks;
extern int amount_of_big_blocks;

void
iniciliazar_malloc(void)
{
	errno = 0;
	block_list = NULL;
	amount_of_blocks = 0;
	amount_of_mallocs = 0;
	amount_of_frees = 0;
	requested_memory = 0;
	amount_of_splits = 0;
	amount_of_coalesing = 0;
	amount_of_small_blocks = 0;
	amount_of_media_blocks = 0;
	amount_of_big_blocks = 0;
}

void
liberar_memoria(void)
{
	struct block *block = block_list;

	if (!block)
		return;

	while (block) {
		munmap(block->head, block->size);
		block = block->next;
	}
}

int
get_size_of_total_memory(void)
{
	struct block *block = block_list;
	size_t size = 0;
	while (block) {
		size += block->size;
		block = block->next;
	}
	return size;
}

int
main(void)
{
	int fallos =

	        // PRUEBAS 1 BLOQUE

	        /// pruebas malloc distintos tamaños
	        test_001_malloc_menor_a_MIN_SIZE() +
	        test_002_malloc_igual_a_0() + test_003_malloc_menor_a_0() +
	        test_004_malloc_bloque_tamanio_pequenio() +
	        test_005_malloc_bloque_tamanio_mediano() +
	        test_006_malloc_bloque_tamanio_grande() +
	        test_007_malloc_falla_bloque_mayor_tamanio_grande() +

	        /// pruebas splittig
	        test_011_doble_malloc_pruebo_splitting() +
	        test_021_3_malloc_1_free_1_malloc_menor_debe_splittar() +

	        /// pruebas free
	        test_008_malloc_y_free_pruebo_unmap_free_blocks() +
	        /// TODO: validate free
	        test_009_doble_free() + test_010_free_region_invalida() +

	        /// pruebas coalesing
	        test_012_doble_malloc_y_free_en_bloque_pruebo_coalesing() +
	        test_012bis_doble_malloc_y_free_en_bloque_pruebo_coalesing() +
	        test_013_doble_malloc_y_libero_ultimo_primero_en_bloque_pruebo_coalesing() +
	        test_017_3_mallocs_2_free_debe_hacer_coaleasing() +
	        test_017_3_mallocs_2_free_debe_hacer_coaleasing_bis() +
	        test_018_3_mallocs_2_free_debe_hacer_coaleasing() +
	        test_020_4_mallocs_2_free_NO_debe_hacer_colleasing() +

	        /// puebras integrando
	        test_014_doble_malloc_y_free_intercalado_en_un_bloque() +
	        test_015_3_mallocs_1_free_debe_alocar_primero() +
	        test_016_2_mallocs_1_free_1_malloc_menor_debe_alocar_primero_y_hacer_split() +
	        test_019_3_mallocs_3_free_debe_2_colleasing_0_blocks() +
	        test_022_3_malloc_2_free_adyacentes_1_malloc_que_necesita_coleasing() +

	        /// TODO: FALTAN PRUEBAS BORDES dependiendo de los sizes


	        // Pruebas MAS de un Bloque

	        /// pruebas tamaños
	        test_101_2_mallocs_obtengo_bloques_Big_size() +
	        test_102_2_mallocs_obtengo_bloques_small_size() +
	        test_103_2_mallocs_obtengo_bloques_medium_size() +
	        test_104_2_mallocs_obtengo_bloques_small_y_medium_size() +
	        test_104bis_2_mallocs_obtengo_bloques_small_y_medium_size() +
	        test_105_3_mallocs_obtengo_3_bloques_small_size() +
	        test_106_5_mallocs_obtengo_5_bloques() +

	        /// pruebas free
	        test_107_5_mallocs_5_bloques_free_primero() +
	        test_108_5_mallocs_5_bloques_free_ultimo() +
	        test_109_5_mallocs_5_bloques_free_medio() +
	        test_110_3_mallocs_2_bloques() +

	        /// pruebas #mallocs
	        test_110bis_3_mallocs_2_bloques() +

	        /// pruebas mallocs y frees
	        test_111_3_mallocs_2_bloques_libero_bloque_de_2_regiones() +
	        test_111bis_3_mallocs_2_bloques_libero_bloque_de_2_regiones() +
	        test_112_5_mallocs_3_bloques_libero_2_bloques() +
	        test_112bis_5_mallocs_3_bloques_libero_2_bloques() +

	        /// pruebas mallocs y frees con coleasing
	        test_113_7_mallocs_3_bloques_3_free_1_bloque_y_malloc_hace_coleasing() +
	        test_113bis_7_mallocs_3_bloques_3_free_1_bloque_y_malloc_hace_coleasing() +

	        // pruebas wrappers: calloc y realloc

	        /// prebas calloc
	        test_200_calloc_1_bloque() +
	        test_201_calloc_1_bloque_debe_split() +
	        test_202_calloc_1_bloque_big_size() +
	        test_203_calloc_falla_tamanio_mayor_big_size() +
	        test_204_calloc_falla_parametro_igual_0_o_NULL() +
	        test_204bis_calloc_falla_parametro_igual_0_o_NULL() +
	        test_205_calloc_parametros_negativos() +
	        test_206_calloc_y_free() + test_207_3calloc_y_1_free_2_bloques() +

	        /// pruebas realloc
	        test_208_realloc_a_mayor() + test_209_realloc_a_menor() +
	        test_210_realloc_size_igual_NULL_realiza_free() +
	        test_211_realloc_puntero_igual_NULL_hace_malloc() +
	        test_212_realloc_puntero_liberado() +
	        test_213_realloc_size_mas_grande_que_el_maximo() +
	        test_214_realloc_multiples_size_mas_grande_que_el_maximo() +
	        test_215_realloc_conservar_valor_previa_relocacion() +
	        test_216_realloc_verificar_que_sea_el_mismo_puntero_cuando_el_tamaño_pedido_sea_menor() +
	        test_216_realloc_verificar_que_sea_distion_puntero_cuando_el_tamaño_pedido_sea_mayor();

	if (!fallos)
		printfmt("\n%sPASARON TODOS LOS TESTS%s\n", GREEN, RESET);
	else
		printfmt("\n%sFallaron #%d tests%s\n", RED, fallos, RESET);

	return fallos;
}

int
test_001_malloc_menor_a_MIN_SIZE(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 50;

	char *var = NULL;
	var = malloc(memoria_pedida);
	strcpy(var, TEST_STRING);

	if (requested_memory == MIN_SIZE && var) {
		liberar_memoria();
		printfmt("OK - Test 01 - Malloc Menor a MIN SIZE\n", MIN_SIZE);
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 01 - Fallo al pedir memoria menor al "
		         "MIN_SIZE= %d\n",
		         MIN_SIZE);
		return 1;
	}
}

int
test_002_malloc_igual_a_0(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 0;

	char *var = NULL;
	var = malloc(memoria_pedida);

	if (requested_memory == memoria_pedida && !var && errno == ENOMEM) {
		printfmt("OK - Test 02 - Malloc igual a 0\n");
		return 0;
	} else {
		printfmt("ERROR - Test 02 - Malloc igual a 0\n");
		return 1;
	}
}

int
test_003_malloc_menor_a_0(void)
{
	iniciliazar_malloc();

	int memoria_pedida = -3;

	char *var = NULL;
	var = malloc(memoria_pedida);

	if (requested_memory == 0 && !var && errno == ENOMEM) {
		printfmt("OK - Test 03 - Malloc < a 0\n");
		return 0;
	} else {
		printfmt("ERROR - Test 03 - Malloc < a 0\n");
		return 1;
	}
}

int
test_004_malloc_bloque_tamanio_pequenio(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 15000;

	char *var = NULL;
	var = malloc(memoria_pedida);
	strcpy(var, TEST_STRING);

	if (amount_of_blocks == 1 && get_size_of_total_memory() == SMALL_BLOCK &&
	    requested_memory == memoria_pedida) {
		liberar_memoria();
		printfmt("OK - Test 04 - Malloc tamaño pequeño\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 04 - Malloc tamaño pequeño\n");
		return 1;
	}
}

int
test_005_malloc_bloque_tamanio_mediano(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 30000;

	char *var = NULL;
	var = malloc(memoria_pedida);
	strcpy(var, TEST_STRING);

	if (amount_of_blocks == 1 && get_size_of_total_memory() == MEDIUM_BLOCK &&
	    requested_memory == memoria_pedida) {
		liberar_memoria();
		printfmt("OK - Test 05 - Malloc tamaño mediano\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 05 - Malloc tamaño mediano\n");
		return 1;
	}
}

int
test_006_malloc_bloque_tamanio_grande(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 2000000;

	char *var = NULL;
	var = malloc(memoria_pedida);
	strcpy(var, TEST_STRING);

	if (amount_of_blocks == 1 && get_size_of_total_memory() == BIG_BLOCK &&
	    requested_memory == memoria_pedida) {
		liberar_memoria();
		printfmt("OK - Test 06 - Malloc tamaño grande\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 06 - Malloc tamaño grande\n");
		return 1;
	}
}

int
test_007_malloc_falla_bloque_mayor_tamanio_grande(void)
{
	iniciliazar_malloc();

	int memoria_pedida = BIG_BLOCK + 1;

	char *var = NULL;
	var = malloc(memoria_pedida);

	if (amount_of_blocks == 0 && requested_memory == 0 && !var) {
		liberar_memoria();
		printfmt("OK - Test 07 - Malloc falla tamaño mayor a grande\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 07 - Malloc falla tamaño mayor a "
		         "grande\n");
		return 1;
	}
}

int
test_008_malloc_y_free_pruebo_unmap_free_blocks(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 256;

	char *var = NULL;
	var = malloc(memoria_pedida);
	strcpy(var, TEST_STRING);

	free(var);

	if (amount_of_frees == 1 && requested_memory == 0 &&
	    amount_of_blocks == 0) {
		liberar_memoria();
		printfmt("OK - Test 08 - malloc y free libera - #blocks = 0\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 08 - malloc y free libera - #blocks = "
		         "0\n");
		return 1;
	}
}

int
test_009_doble_free(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 256;

	char *var = NULL;
	var = malloc(memoria_pedida);
	strcpy(var, TEST_STRING);

	free(var);
	free(var);

	if (amount_of_frees == 1 && requested_memory == 0) {
		liberar_memoria();
		printfmt("OK - Test 09 - doble free\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 09 - doble free\n");
		return 1;
	}
}

int
test_010_free_region_invalida(void)
{
	iniciliazar_malloc();

	char *var;
	free(var);

	if (amount_of_frees == 0 && requested_memory == 0) {
		liberar_memoria();
		printfmt("OK - Test 10 - free region invalida\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 10 - free region invalida\n");
		return 1;
	}
}

int
test_011_doble_malloc_pruebo_splitting(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 266;
	int memoria_pedida2 = 512;

	int dif_al = 4;

	char *var1 = NULL;
	char *var2 = NULL;
	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	int diferencia_direcciones =
	        var2 - (var1 + memoria_pedida1) - sizeof(struct region);

	if (var1 && var2 &&
	    (requested_memory - (memoria_pedida1 + memoria_pedida2) < dif_al) &&
	    amount_of_blocks == 1 && (diferencia_direcciones < dif_al)) {
		liberar_memoria();
		printfmt("OK - Test 11 - doble malloc en un bloque - splitting "
		         "correcto\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 11 - doble malloc en un bloque - "
		         "splitting correcto\n");
		return 1;
	}
}

int
test_012_doble_malloc_y_free_en_bloque_pruebo_coalesing(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 260;
	int memoria_pedida2 = 512;


	char *var1 = NULL;
	char *var2 = NULL;
	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	free(var1);
	free(var2);

	if (var1 && var2 && amount_of_frees == 2 && requested_memory == 0 &&
	    amount_of_blocks == 0 && amount_of_coalesing == 2) {
		liberar_memoria();
		printfmt("OK - Test 12 - doble malloc y free en un bloque\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 12 - doble malloc y un free en un "
		         "bloque\n");
		return 1;
	}
}

int
test_012bis_doble_malloc_y_free_en_bloque_pruebo_coalesing(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 260;
	int memoria_pedida2 = 512;


	char *var1 = NULL;
	char *var2 = NULL;
	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	free(var2);
	free(var1);

	if (var1 && var2 && amount_of_frees == 2 && requested_memory == 0 &&
	    amount_of_blocks == 0 && amount_of_coalesing == 2) {
		liberar_memoria();
		printfmt("OK - Test 12bis - doble malloc y free invertido en "
		         "un bloque\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 12bis - doble malloc y free invertido "
		         "en un bloque\n");
		return 1;
	}
}

int
test_013_doble_malloc_y_libero_ultimo_primero_en_bloque_pruebo_coalesing(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 260;
	int memoria_pedida2 = 512;


	char *var1 = NULL;
	char *var2 = NULL;
	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	free(var2);
	free(var1);

	if (var1 && var2 && amount_of_frees == 2 && requested_memory == 0 &&
	    amount_of_blocks == 0) {
		liberar_memoria();
		printfmt("OK - Test 13 - doble malloc y libero primero el "
		         "ultimo alocado en un bloque\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 13 - doble malloc y libero primero el "
		         "ultimo alocado en un bloque\n");
		return 1;
	}
}

int
test_014_doble_malloc_y_free_intercalado_en_un_bloque(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 260;
	int memoria_pedida2 = 512;


	char *var1 = NULL;
	char *var2 = NULL;

	var1 = malloc(memoria_pedida1);
	strcpy(var1, TEST_STRING);
	free(var1);

	var2 = malloc(memoria_pedida2);
	strcpy(var2, TEST_STRING);
	free(var2);

	if (var1 && var2 && requested_memory == 0 && amount_of_blocks == 0 &&
	    amount_of_frees == 2) {
		liberar_memoria();
		printfmt("OK - Test 14 - doble malloc y free intercalado\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 14 - doble malloc y free intercalado\n");
		return 1;
	}
}

int
test_015_3_mallocs_1_free_debe_alocar_primero(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 260;
	int memoria_pedida2 = 512;

	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	free(var1);
	var3 = malloc(memoria_pedida1);
	strcpy(var3, TEST_STRING);

	if (var1 == var3 && requested_memory == memoria_pedida1 + memoria_pedida2 &&
	    amount_of_frees == 1 && amount_of_splits == 2) {
		liberar_memoria();
		printfmt("OK - Test 15 - triple malloc liberando primer "
		         "direccion de 1ro y 3er iguales\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 15 - triple malloc liberando primer "
		         "direccion de 1ro y 3er iguales\n");
		return 1;
	}
}

int
test_016_2_mallocs_1_free_1_malloc_menor_debe_alocar_primero_y_hacer_split(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 460;
	int memoria_pedida2 = 512;
	int memoria_pedida3 = 256;

	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	free(var1);
	var3 = malloc(memoria_pedida3);
	strcpy(var3, TEST_STRING);

	if (var1 == var3 && requested_memory == memoria_pedida3 + memoria_pedida2 &&
	    amount_of_frees == 1 && amount_of_splits == 3) {
		liberar_memoria();
		printfmt("OK - Test 16 - 2 malloc-free-malloc debe alocar "
		         "primero y hacer split\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 16 - 2 malloc-free-malloc debe alocar "
		         "primero y hacer split\n");
		return 1;
	}
}

int
test_017_3_mallocs_2_free_debe_hacer_coaleasing(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 460;
	int memoria_pedida2 = 512;
	int memoria_pedida3 = 256;

	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);
	var3 = malloc(memoria_pedida3);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);
	free(var2);

	if (amount_of_coalesing == 1) {
		liberar_memoria();
		printfmt("OK - Test 17 - 3 mallocs 2 free debe hacer "
		         "coaleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 17 - 3 mallocs 2 free debe hacer "
		         "coaleasing\n");
		return 1;
	}
}

int
test_017_3_mallocs_2_free_debe_hacer_coaleasing_bis(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 460;
	int memoria_pedida2 = 512;
	int memoria_pedida3 = 256;

	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);
	var3 = malloc(memoria_pedida3);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var3);
	free(var2);

	if (amount_of_coalesing == 2) {
		liberar_memoria();
		printfmt("OK - Test 17bis - 3 mallocs 2 free debe hacer "
		         "coaleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 17bis - 3 mallocs 2 free debe hacer "
		         "coaleasing\n");
		return 1;
	}
}

int
test_018_3_mallocs_2_free_debe_hacer_coaleasing(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 460;
	int memoria_pedida2 = 512;
	int memoria_pedida3 = 256;

	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);
	var3 = malloc(memoria_pedida3);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);
	free(var3);

	if (amount_of_coalesing == 1) {
		liberar_memoria();
		printfmt("OK - Test 18 - 3 mallocs 2 free NO debe hacer "
		         "coaleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 18 - 3 mallocs 2 free NO debe hacer "
		         "coaleasing\n");
		return 1;
	}
}

int
test_019_3_mallocs_3_free_debe_2_colleasing_0_blocks(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 460;
	int memoria_pedida2 = 512;
	int memoria_pedida3 = 256;

	char *var1 = NULL;
	char *var2 = NULL;
	char *var3 = NULL;

	var1 = malloc(memoria_pedida1);
	var2 = malloc(memoria_pedida2);
	var3 = malloc(memoria_pedida3);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);
	free(var2);
	free(var3);

	if (amount_of_coalesing == 3 && amount_of_blocks == 0) {
		liberar_memoria();
		printfmt("OK - Test 19 - 3 mallocs 3 free = 2 coleasing y 0 "
		         "blocks\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 19 - 3 mallocs 3 free = 2 coleasing y 0 "
		         "blocks\n");
		return 1;
	}
}

int
test_020_4_mallocs_2_free_NO_debe_hacer_colleasing(void)
{
	iniciliazar_malloc();

	int memoria_pedida = 512;

	char *var1 = malloc(memoria_pedida);
	char *var2 = malloc(memoria_pedida);
	char *var3 = malloc(memoria_pedida);
	char *var4 = malloc(memoria_pedida);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);
	strcpy(var4, TEST_STRING);

	free(var1);
	free(var3);

	if (amount_of_coalesing == 0 && amount_of_frees == 2) {
		liberar_memoria();
		printfmt("OK - Test 20 - 4 mallocs y luego 2 free NO debe "
		         "hacer coleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 20 - 4 mallocs y luego 2 free NO debe "
		         "hacer coleasing\n");
		return 1;
	}
}

int
test_021_3_malloc_1_free_1_malloc_menor_debe_splittar(void)
{
	iniciliazar_malloc();

	int memoria_pedida1 = 460;
	int memoria_pedida2 = 512;
	int memoria_pedida3 = 256;
	int memoria_pedida4 = 256;

	char *var1 = malloc(memoria_pedida1);
	char *var2 = malloc(memoria_pedida2);
	char *var3 = malloc(memoria_pedida3);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);

	char *var4 = malloc(memoria_pedida4);
	strcpy(var4, TEST_STRING);

	if (var1 == var4 &&
	    requested_memory ==
	            (memoria_pedida2 + memoria_pedida3 + memoria_pedida4) &&
	    amount_of_blocks == 1 && amount_of_frees == 1 &&
	    amount_of_splits == 4) {
		liberar_memoria();
		printfmt("OK - Test 21 - 3 mallocs 1 free 1 malloc menor al "
		         "liberado debe splittear\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 21 - 3 mallocs 1 free 1 malloc menor al "
		         "liberado debe splittear\n");
		return 1;
	}
}

int
test_022_3_malloc_2_free_adyacentes_1_malloc_que_necesita_coleasing(void)
{
	iniciliazar_malloc();

	int mem1 = 460;
	int mem2 = 512;
	int mem3 = 256;
	int mem4 = 540;

	char *var1 = malloc(mem1);
	char *var2 = malloc(mem2);
	char *var3 = malloc(mem3);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);
	free(var2);

	char *var4 = malloc(mem4);
	strcpy(var4, TEST_STRING);

	if (var1 == var4 && amount_of_coalesing == 1 &&
	    requested_memory == (mem3 + mem4) && amount_of_splits == 4) {
		liberar_memoria();
		printfmt("OK - Test 22 - 3 malloc, 2 free adyacentes luego 1 "
		         "malloc que necesita coleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 22 - 3 malloc, 2 free adyacentes luego "
		         "1 malloc que necesita coleasing\n");
		return 1;
	}
}

/*********************************************************************************************************************/
/********************************************** PRUEBAS MAS DE UN BLOQUE *********************************************/
/*********************************************************************************************************************/

int
test_101_2_mallocs_obtengo_bloques_Big_size(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(BIG_BLOCK);
	char *var2 = malloc(BIG_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	if (var1 && var2 && requested_memory == (2 * BIG_BLOCK) &&
	    amount_of_blocks == 2 && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 101 - 2 malloc obtengo 2 bloques big "
		         "size\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 101 - 2 malloc obtengo 2 bloques big "
		         "size\n");
		return 1;
	}
}

int
test_102_2_mallocs_obtengo_bloques_small_size(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK);
	char *var2 = malloc(SMALL_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	if (var1 && var2 && requested_memory == (2 * SMALL_BLOCK) &&
	    amount_of_blocks == 2 && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 102 - 2 malloc obtengo 2 bloques small "
		         "size\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 102 - 2 malloc obtengo 2 bloques small "
		         "size\n");
		return 1;
	}
}

int
test_103_2_mallocs_obtengo_bloques_medium_size(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(MEDIUM_BLOCK);
	char *var2 = malloc(MEDIUM_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	if (var1 && var2 && requested_memory == (2 * MEDIUM_BLOCK) &&
	    amount_of_blocks == 2 && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 103 - 2 malloc obtengo 2 bloques medium "
		         "size\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 103 - 2 malloc obtengo 2 bloques medium "
		         "size\n");
		return 1;
	}
}

int
test_104_2_mallocs_obtengo_bloques_small_y_medium_size(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK);
	char *var2 = malloc(MEDIUM_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	if (var1 && var2 && requested_memory == (SMALL_BLOCK + MEDIUM_BLOCK) &&
	    amount_of_blocks == 2 && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 104 - 2 malloc obtengo bloques small y "
		         "medium size\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 104 - 2 malloc obtengo bloques small y "
		         "medium size\n");
		return 1;
	}
}

int
test_104bis_2_mallocs_obtengo_bloques_small_y_medium_size(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(MEDIUM_BLOCK);
	char *var2 = malloc(SMALL_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);

	if (var1 && var2 && requested_memory == (SMALL_BLOCK + MEDIUM_BLOCK) &&
	    amount_of_blocks == 2 && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 104bis - 2 malloc obtengo bloques medium y "
		         "small size\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 104bis - 2 malloc obtengo bloques "
		         "medium y small size\n");
		return 1;
	}
}

int
test_105_3_mallocs_obtengo_3_bloques_small_size(void)
{
	iniciliazar_malloc();

	int cantidad_bloques = 3;

	char *var;
	for (int i = 0; i < cantidad_bloques; i++) {
		var = malloc(SMALL_BLOCK);
		strcpy(var, TEST_STRING);
	}

	if (requested_memory == (cantidad_bloques * SMALL_BLOCK) &&
	    amount_of_blocks == cantidad_bloques && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 105 - 3 mallocs, obtengo 3 bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 105 - 3 mallocs, obtengo 3 bloques\n");
		return 1;
	}
}

int
test_106_5_mallocs_obtengo_5_bloques(void)
{
	iniciliazar_malloc();

	int cantidad_bloques = 5;

	char *var;
	for (int i = 0; i < cantidad_bloques; i++) {
		var = malloc(BIG_BLOCK);
		strcpy(var, TEST_STRING);
	}

	if (requested_memory == (cantidad_bloques * BIG_BLOCK) &&
	    amount_of_blocks == cantidad_bloques && amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 106 - 5 mallocs, obtengo 5 bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 106 - 5 mallocs, obtengo 5 bloques\n");
		return 1;
	}
}

int
test_107_5_mallocs_5_bloques_free_primero(void)
{
	iniciliazar_malloc();
	char *var_test = malloc(BIG_BLOCK);
	strcpy(var_test, TEST_STRING);

	int cantidad_bloques = 4;

	char *var;
	for (int i = 0; i < cantidad_bloques; i++) {
		var = malloc(BIG_BLOCK);
		strcpy(var, TEST_STRING);
	}

	free(var_test);

	if (requested_memory == (cantidad_bloques * BIG_BLOCK) &&
	    amount_of_blocks == cantidad_bloques) {
		liberar_memoria();
		printfmt("OK - Test 107 - 5 mallocs, 5 bloques free primero\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 107 - 5 mallocs, 5 bloques free "
		         "primero\n");
		return 1;
	}
}

int
test_108_5_mallocs_5_bloques_free_ultimo(void)
{
	iniciliazar_malloc();

	int cantidad_bloques = 4;

	char *var;
	for (int i = 0; i < cantidad_bloques; i++) {
		var = malloc(BIG_BLOCK);
		strcpy(var, TEST_STRING);
	}

	char *var_test = malloc(BIG_BLOCK);
	strcpy(var_test, TEST_STRING);

	free(var_test);

	if (requested_memory == (cantidad_bloques * BIG_BLOCK) &&
	    amount_of_blocks == cantidad_bloques) {
		liberar_memoria();
		printfmt("OK - Test 108 - 5 mallocs, 5 bloques free ultimo\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 108 - 5 mallocs, 5 bloques free "
		         "ultimo\n");
		return 1;
	}
}

int
test_109_5_mallocs_5_bloques_free_medio(void)
{
	iniciliazar_malloc();

	int cantidad_bloques = 2;

	char *var;
	for (int i = 0; i < cantidad_bloques; i++) {
		var = malloc(BIG_BLOCK);
		strcpy(var, TEST_STRING);
	}

	char *var_test = malloc(BIG_BLOCK);
	strcpy(var_test, TEST_STRING);

	for (int i = 0; i < cantidad_bloques; i++) {
		var = malloc(BIG_BLOCK);
		strcpy(var, TEST_STRING);
	}

	free(var_test);

	if (requested_memory == (2 * cantidad_bloques * BIG_BLOCK) &&
	    amount_of_blocks == 2 * cantidad_bloques) {
		liberar_memoria();
		printfmt("OK - Test 109 - 5 mallocs, 5 bloques free medio\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt(
		        "ERROR - Test 109 - 5 mallocs, 5 bloques free medio\n");
		return 1;
	}
}

int
test_110_3_mallocs_2_bloques(void)
{
	iniciliazar_malloc();
	int mem_p = SMALL_BLOCK / 4;

	char *var1 = malloc(SMALL_BLOCK - mem_p);
	char *var2 = malloc(mem_p - sizeof(struct region));
	char *var3 = malloc(BIG_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	if (amount_of_blocks == 2) {
		liberar_memoria();
		printfmt("OK - Test 110 - 3 mallocs, 2 bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 110 - 3 mallocs, 2 bloques\n");
		return 1;
	}
}

int
test_110bis_3_mallocs_2_bloques(void)
{
	iniciliazar_malloc();

	int mem_p = SMALL_BLOCK / 4;

	char *var1 = malloc(SMALL_BLOCK - mem_p);
	char *var2 = malloc(MEDIUM_BLOCK);
	char *var3 = malloc(mem_p - sizeof(struct region));

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	if (amount_of_blocks == 2) {
		liberar_memoria();
		printfmt("OK - Test 110bis - 3 mallocs, 2 bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 110bis - 3 mallocs, 2 bloques\n");
		return 1;
	}
}

int
test_111_3_mallocs_2_bloques_libero_bloque_de_2_regiones(void)
{
	iniciliazar_malloc();

	int mem_p = MEDIUM_BLOCK / 4;

	char *var1 = malloc(MEDIUM_BLOCK - mem_p);
	char *var2 = malloc(MEDIUM_BLOCK);
	char *var3 = malloc(mem_p - sizeof(struct region));

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);
	free(var3);

	if (requested_memory == (MEDIUM_BLOCK) && amount_of_blocks == 1) {
		liberar_memoria();
		printfmt("OK - Test 111 - 3 mallocs, 2 bloques, libero bloque "
		         "2 regiones\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 111 - 3 mallocs, 2 bloques, libero "
		         "bloque 2 regiones\n");
		return 1;
	}
}

int
test_111bis_3_mallocs_2_bloques_libero_bloque_de_2_regiones(void)
{
	iniciliazar_malloc();

	int mem_p = MEDIUM_BLOCK / 4;

	char *var1 = malloc(MEDIUM_BLOCK - mem_p);
	char *var3 = malloc(mem_p - sizeof(struct region));
	char *var2 = malloc(MEDIUM_BLOCK);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var3);
	free(var1);

	if (requested_memory == (MEDIUM_BLOCK) && amount_of_blocks == 1) {
		liberar_memoria();
		printfmt("OK - Test 111bis - 3 mallocs, 2 bloques, libero "
		         "bloque 2 regiones\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 111bis - 3 mallocs, 2 bloques, libero "
		         "bloque 2 regiones\n");
		return 1;
	}
}

int
test_112_5_mallocs_3_bloques_libero_2_bloques(void)
{
	iniciliazar_malloc();

	int mem_p1 = MEDIUM_BLOCK / 4;
	int mem_p2 = SMALL_BLOCK / 4;

	char *var1 = malloc(MEDIUM_BLOCK - mem_p1);
	char *var2 = malloc(mem_p1 - sizeof(struct region));
	char *var3 = malloc(SMALL_BLOCK);
	char *var4 = malloc(SMALL_BLOCK - mem_p2);
	char *var5 = malloc(mem_p2 - sizeof(struct region));

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);
	strcpy(var4, TEST_STRING);
	strcpy(var5, TEST_STRING);

	free(var1);
	free(var5);
	free(var4);
	free(var2);

	if (requested_memory == (SMALL_BLOCK) && amount_of_blocks == 1) {
		liberar_memoria();
		printfmt("OK - Test 112 - 5 mallocs, 3 bloques, libero 2 "
		         "bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 112 - 5 mallocs, 3 bloques, libero 2 "
		         "bloques\n");
		return 1;
	}
}

int
test_112bis_5_mallocs_3_bloques_libero_2_bloques(void)
{
	iniciliazar_malloc();

	int mem_p1 = MEDIUM_BLOCK / 4;
	int mem_p2 = SMALL_BLOCK / 4;

	char *var1 = malloc(MEDIUM_BLOCK - mem_p1);
	char *var2 = malloc(mem_p1 - sizeof(struct region));
	char *var4 = malloc(SMALL_BLOCK - mem_p2);
	char *var5 = malloc(mem_p2 - sizeof(struct region));
	char *var3 = malloc(SMALL_BLOCK);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);
	strcpy(var4, TEST_STRING);
	strcpy(var5, TEST_STRING);

	free(var1);
	free(var2);
	free(var4);
	free(var5);

	if (requested_memory == (SMALL_BLOCK) && amount_of_blocks == 1) {
		liberar_memoria();
		printfmt("OK - Test 112bis - 5 mallocs, 3 bloques, libero 2 "
		         "bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 112bis - 5 mallocs, 3 bloques, libero 2 "
		         "bloques\n");
		return 1;
	}
}

int
test_113_7_mallocs_3_bloques_3_free_1_bloque_y_malloc_hace_coleasing(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK / 5);
	char *var2 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var3 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var4 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var5 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));

	char *var6 = malloc(SMALL_BLOCK);
	char *var7 = malloc(SMALL_BLOCK);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);
	strcpy(var4, TEST_STRING);
	strcpy(var5, TEST_STRING);
	strcpy(var6, TEST_STRING);
	strcpy(var7, TEST_STRING);

	free(var2);
	free(var4);
	free(var3);

	var3 = malloc((SMALL_BLOCK / 5 * 3 - sizeof(struct region)));
	strcpy(var3, TEST_STRING);

	if (amount_of_blocks == 3 && amount_of_coalesing == 2) {
		liberar_memoria();
		printfmt("OK - Test 113 - 7 mallocs, 3 bloques #Reg=(5,1,1), 3 "
		         "free y malloc debe hacer coleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt(
		        "ERROR - Test 113 - 7 mallocs, 3 bloques #Reg=(5,1,1), "
		        "3 free y malloc debe hacer coleasing\n");
		return 1;
	}
}

int
test_113bis_7_mallocs_3_bloques_3_free_1_bloque_y_malloc_hace_coleasing(void)
{
	iniciliazar_malloc();

	char *var6 = malloc(SMALL_BLOCK);

	char *var2 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var3 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var4 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var5 = malloc(SMALL_BLOCK / 5 - sizeof(struct region));
	char *var1 = malloc(SMALL_BLOCK / 5);

	char *var7 = malloc(SMALL_BLOCK);

	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);
	strcpy(var4, TEST_STRING);
	strcpy(var5, TEST_STRING);
	strcpy(var6, TEST_STRING);
	strcpy(var7, TEST_STRING);

	free(var4);
	free(var3);
	free(var2);

	var3 = malloc(SMALL_BLOCK / 5 * 3 - sizeof(struct region));
	strcpy(var3, TEST_STRING);

	if (amount_of_blocks == 3 && amount_of_coalesing == 2) {
		liberar_memoria();
		printfmt(
		        "OK - Test 113bis - 7 mallocs, 3 bloques #Reg=(1,5,1), "
		        "3 free en Reg(2) y malloc debe hacer coleasing\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 113bis - 7 mallocs, 3 bloques "
		         "#Reg=(1,5,1), 3 free en Reg(2) y malloc debe hacer "
		         "coleasing\n");
		return 1;
	}
}

int
test_200_calloc_1_bloque(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(SMALL_BLOCK / 4, 4);
	strcpy(var1, TEST_STRING);

	if (requested_memory == SMALL_BLOCK && amount_of_blocks == 1 &&
	    amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 200 - calloc - 1 bloque\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 200 - calloc - 1 bloque\n");
		return 1;
	}
}

int
test_201_calloc_1_bloque_debe_split(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(SMALL_BLOCK / 8, 4);
	strcpy(var1, TEST_STRING);

	if (requested_memory == SMALL_BLOCK / 2 && amount_of_blocks == 1 &&
	    amount_of_splits == 1) {
		liberar_memoria();
		printfmt("OK - Test 201 - calloc - 1 bloque hace split\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 201 - calloc - 1 bloque hace split\n");
		return 1;
	}
}

int
test_202_calloc_1_bloque_big_size(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(BIG_BLOCK / 4, 4);
	strcpy(var1, TEST_STRING);

	if (requested_memory == BIG_BLOCK && amount_of_blocks == 1 &&
	    amount_of_splits == 0) {
		liberar_memoria();
		printfmt("OK - Test 202 - calloc - 1 bloque big size\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 202 - calloc - 1 bloque big size\n");
		return 1;
	}
}

int
test_203_calloc_falla_tamanio_mayor_big_size(void)
{
	iniciliazar_malloc();

	char *var1 = calloc((BIG_BLOCK / 4) + 1, 4);

	if (!var1 && requested_memory == 0 && amount_of_blocks == 0 &&
	    errno == ENOMEM) {
		liberar_memoria();
		printfmt("OK - Test 203 - calloc 1 bloque falla, supera limite "
		         "de memoria\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 203 - calloc 1 bloque, supera limite de "
		         "memoria\n");
		return 1;
	}
}

int
test_204_calloc_falla_parametro_igual_0_o_NULL(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(NULL, 4);

	if (!var1 && requested_memory == 0 && amount_of_blocks == 0 &&
	    errno == ENOMEM) {
		liberar_memoria();
		printfmt("OK - Test 204 - calloc 1 bloque falla, nmemb = "
		         "NULL\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 204 - calloc 1 bloque falla, nmemb = "
		         "NULL\n");
		return 1;
	}
}

int
test_204bis_calloc_falla_parametro_igual_0_o_NULL(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(SMALL_BLOCK, 0);

	if (!var1 && requested_memory == 0 && amount_of_blocks == 0 &&
	    errno == ENOMEM) {
		liberar_memoria();
		printfmt(
		        "OK - Test 204bis - calloc 1 bloque falla, size = 0\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 204bis - calloc 1 bloque falla, size = "
		         "0\n");
		return 1;
	}
}

int
test_205_calloc_parametros_negativos(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(-3, -2);

	if (!var1 && requested_memory == 0 && amount_of_blocks == 0 &&
	    errno == ENOMEM) {
		liberar_memoria();
		printfmt("OK - Test 205 - calloc parametros negativos\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 205 - calloc parametros negativos\n");
		return 1;
	}
}

int
test_206_calloc_y_free(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(SMALL_BLOCK, 6);
	strcpy(var1, TEST_STRING);
	free(var1);

	if (var1 && requested_memory == 0 && amount_of_blocks == 0 &&
	    amount_of_frees == 1) {
		liberar_memoria();
		printfmt("OK - Test 206 - calloc y free, 1 bloque\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 206 - calloc y free, 1 bloque\n");
		return 1;
	}
}

int
test_207_3calloc_y_1_free_2_bloques(void)
{
	iniciliazar_malloc();

	char *var1 = calloc(SMALL_BLOCK / 8, 6);
	char *var2 = calloc(SMALL_BLOCK / 8 - sizeof(struct region), 2);
	char *var3 = calloc(SMALL_BLOCK / 8, 6);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);

	free(var1);

	if (var1 && amount_of_blocks == 2 && amount_of_frees == 1) {
		liberar_memoria();
		printfmt("OK - Test 207 - 3 calloc y 1 free en 2 bloques\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 207 - 3 calloc y 1 free en 2 bloques\n");
		return 1;
	}
}

int
test_208_realloc_a_mayor(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK / 4);
	strcpy(var1, TEST_STRING);

	var1 = realloc(var1, SMALL_BLOCK / 2);
	strcpy(var1, TEST_STRING);
	if (var1 && amount_of_blocks == 1 &&
	    requested_memory == (SMALL_BLOCK / 2 + SMALL_BLOCK / 4)) {
		liberar_memoria();
		printfmt("OK - Test 208 - realloc, pido mayor memoria "
		         "amount_of_blocks = %d requested_memory = %d\n",
		         amount_of_blocks,
		         requested_memory);
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 208 - realloc, pido mayor memoria "
		         "amount_of_blocks = %d requested_memory = %d\n",
		         amount_of_blocks,
		         requested_memory);
		return 1;
	}
}

int
test_209_realloc_a_menor(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK / 2);
	strcpy(var1, TEST_STRING);

	var1 = realloc(var1, SMALL_BLOCK / 4);
	strcpy(var1, TEST_STRING);

	if (var1 && amount_of_blocks == 1 && requested_memory == SMALL_BLOCK / 2) {
		liberar_memoria();
		printfmt("OK - Test 209 - realloc, pido menor memoria "
		         "amount_of_blocks = %d requested_memory = %d\n",
		         amount_of_blocks,
		         requested_memory);
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 209 - realloc, pido menor memoria "
		         "amount_of_blocks = %d requested_memory = %d\n",
		         amount_of_blocks,
		         requested_memory);
		return 1;
	}
}

int
test_210_realloc_size_igual_NULL_realiza_free(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK / 2);
	strcpy(var1, TEST_STRING);

	var1 = realloc(var1, NULL);

	if (!var1 && amount_of_blocks == 0 && requested_memory == 0 &&
	    amount_of_frees == 1) {
		liberar_memoria();
		printfmt("OK - Test 210 - realloc con size=0 debe haber free "
		         "amount_of_blocks = %d requested_memory = %d "
		         "amount_of_frees = %d\n",
		         amount_of_blocks,
		         requested_memory,
		         amount_of_frees);
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 210 - realloc con size=0 debe haber "
		         "free amount_of_blocks = %d requested_memory = %d  "
		         "amount_of_frees = %d\n",
		         amount_of_blocks,
		         requested_memory,
		         amount_of_frees);
		return 1;
	}
}

int
test_211_realloc_puntero_igual_NULL_hace_malloc(void)
{
	iniciliazar_malloc();

	char *var1 = malloc(SMALL_BLOCK / 2);
	strcpy(var1, TEST_STRING);

	char *var2 = realloc(NULL, SMALL_BLOCK / 2);
	strcpy(var2, TEST_STRING);

	if (var2 && requested_memory == SMALL_BLOCK) {
		liberar_memoria();
		printfmt("OK - Test 211 - realloc con pointer=0 debe haber "
		         "malloc\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 211 - realloc con pointer=0 debe haber "
		         "malloc\n");
		return 1;
	}
}

int
test_212_realloc_puntero_liberado()
{
	iniciliazar_malloc();
	char *var1 = malloc(SMALL_BLOCK / 2);
	strcpy(var1, TEST_STRING);
	free(var1);
	free(var1);
	char *var2 = realloc(var1, SMALL_BLOCK / 2);
	if (!var2) {
		liberar_memoria();
		printfmt("OK - Test 212 - realloc de region liberada "
		         "malloc\n");
		return 0;
	} else {
		liberar_memoria();
		printfmt("ERROR - Test 211 - realloc de region liberada "
		         "malloc\n");
		return 1;
	}
}

int
test_213_realloc_size_mas_grande_que_el_maximo()
{
	iniciliazar_malloc();
	char *var1 = malloc(SMALL_BLOCK / 2);
	strcpy(var1, TEST_STRING);
	char *var2 = realloc(var1, MEDIUM_BLOCK * 2);
	if (!var2 || amount_of_small_blocks != 1 ||
	    amount_of_media_blocks != 0 || amount_of_big_blocks != 1) {
		liberar_memoria();
		printfmt("ERROR - Test 213 en la ampliacion de bloques\n");
		return 1;
	} else {
		liberar_memoria();
		printfmt("OK - Test 213 - Cantidad de SMALL_BLOCK = %d "
		         "MEDIUM_BLOCK = %d BIG_BLOCK = %d\n",
		         amount_of_small_blocks,
		         amount_of_media_blocks,
		         amount_of_big_blocks);
		return 0;
	}
}

int
test_214_realloc_multiples_size_mas_grande_que_el_maximo(void)
{
	iniciliazar_malloc();
	char *var1 = malloc(BIG_BLOCK);
	char *var2 = malloc(MEDIUM_BLOCK);
	char *var3 = malloc(MEDIUM_BLOCK);
	strcpy(var1, TEST_STRING);
	strcpy(var2, TEST_STRING);
	strcpy(var3, TEST_STRING);
	char *var4 = realloc(var1, BIG_BLOCK);

	if (!var1 || !var2 || !var3 || !var4) {
		liberar_memoria();
		printfmt("ERROR - Test 214 en la ampliacion de bloques\n");
		return 1;
	} else {
		liberar_memoria();
		printfmt("OK - Test 214 - Cantidad de SMALL_BLOCK = %d "
		         "MEDIUM_BLOCK = %d BIG_BLOCK = %d\n",
		         amount_of_small_blocks,
		         amount_of_media_blocks,
		         amount_of_big_blocks);
		return 0;
	}
}

int
test_215_realloc_conservar_valor_previa_relocacion(void)
{
	iniciliazar_malloc();
	char *var1 = malloc(MEDIUM_BLOCK / 2);
	var1[100] = 'a';
	strcpy(var1, TEST_STRING);
	char *var2 = realloc(var1, MEDIUM_BLOCK);
	if (!var1 || !var2 || var2[100] != 'a') {
		liberar_memoria();
		printfmt("ERROR - Test 215 en la ampliacion de bloques "
		         "var1[100] = %c\n",
		         var2[100]);
		return 1;
	} else {
		liberar_memoria();
		printfmt("OK - Test 215 - var1[5] = %c\n", var2[100]);
		return 0;
	}
}

int
test_216_realloc_verificar_que_sea_el_mismo_puntero_cuando_el_tamaño_pedido_sea_menor(void)
{
	iniciliazar_malloc();
	char *var1 = malloc(MEDIUM_BLOCK / 2);

	strcpy(var1, TEST_STRING);
	char *var2 = realloc(var1, SMALL_BLOCK);
	if (!var1 || !var2 || var2 != var1) {
		liberar_memoria();
		printfmt("ERROR - Test 215 en la ampliacion de bloques "
		         "var1[100] y el valor esperado es 'a' = \n");
		return 1;
	} else {
		liberar_memoria();
		printfmt("OK - Test 215 - var1 == var2 %d == %d\n", var1, var2);
		return 0;
	}
}

int
test_216_realloc_verificar_que_sea_distion_puntero_cuando_el_tamaño_pedido_sea_mayor(void)
{
	iniciliazar_malloc();
	char *var1 = malloc(MEDIUM_BLOCK / 3);

	strcpy(var1, TEST_STRING);
	char *var2 = realloc(var1, MEDIUM_BLOCK);
	if (!var1 || !var2 || var2 == var1) {
		liberar_memoria();
		printfmt("ERROR - Test 215 en la ampliacion de bloques "
		         "var1[100] y el valor esperado es 'a' = \n");
		return 1;
	} else {
		liberar_memoria();
		printfmt("OK - Test 215 - var1 == var2 %d != %d\n", var1, var2);
		return 0;
	}
}