#include <stdio.h>
#include <gmp.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREADS 14
#define ARQUIVO_SAIDA "piresultado.txt" // Nome do arquivo para salvar o resultado
#define ARQUIVO_TEMPO "tempodecorrido.txt" // Nome do arquivo para salvar o tempo decorrido

typedef struct {
    int id;
    int n;
    mpf_t resultado;
} DadosThread;

void *calcularPi(void *dados) {
    DadosThread *dadosThread = (DadosThread *)dados;
    int id = dadosThread->id;
    int n = dadosThread->n;

    mpf_t termo;
    mpf_init(termo);
    mpf_set_ui(termo, 0);

    for (int k = id; k < n; k += NUM_THREADS) {
        mpf_t fator;
        mpf_init(fator);
        mpf_t termocopia; // Variável temporária para armazenar o resultado

        mpf_set_ui(fator, 2 * k + 1);

        if (k % 2 != 0) {
            mpf_neg(fator, fator);
        }

        mpf_ui_div(fator, 1, fator);
        mpf_add(termo, termo, fator);

        mpf_init(termocopia);
        mpf_set(termocopia, termo);

        // Copia o resultado para dadosthread->resultado
        mpf_clear(termocopia); // Limpa a variável temporária
        mpf_clear(fator);
    }

    mpf_set(dadosThread->resultado, termo);
    mpf_clear(termo);

    return NULL;
}

int main() {
    int n = 400000000; // Aumente o valor de 'n' para mais dígitos
    mpf_t soma;
    mpf_t um;
    mpf_t quatro;

    mpf_set_default_prec(9000000); // Precisão de 900000 bits

    mpf_init(soma);
    mpf_init(um);
    mpf_init(quatro);

    mpf_set_d(um, 1.0);
    mpf_set_d(quatro, 4.0);
    mpf_set_d(soma, 0.0);

    pthread_t threads[NUM_THREADS];
    DadosThread dadosthread[NUM_THREADS];

    // Medir o tempo de início
    struct timeval tempoinicio, tempofim;
    gettimeofday(&tempoinicio, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        dadosthread[i].id = i;
        dadosthread[i].n = n;
        mpf_init(dadosthread[i].resultado);
        pthread_create(&threads[i], NULL, calcularPi, &dadosthread[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        mpf_add(soma, soma, dadosthread[i].resultado);
        mpf_clear(dadosthread[i].resultado);
    }

    // Medir o tempo de término
    gettimeofday(&tempofim, NULL);

    // Calcular o tempo decorrido em segundos e microssegundos
    double tempopassado = (tempofim.tv_sec - tempoinicio.tv_sec) +
                         (double)(tempofim.tv_usec - tempoinicio.tv_usec) / 1000000.0;

    mpf_mul(soma, soma, quatro);

    //gmp_printf("%.Ff\n", soma);
    printf("Tempo de execução: %.4f segundos\n", tempopassado);

    // Salvar o resultado em um arquivo
    FILE *arquivo = fopen(ARQUIVO_SAIDA, "w");
    if (arquivo) {
        gmp_fprintf(arquivo, "%.Ff", soma);
        fclose(arquivo);
        printf("Resultado salvo em %s\n", ARQUIVO_SAIDA);
    } else {
        printf("Erro ao salvar o resultado em arquivo.\n");
    }

    // Salvar o tempo decorrido em um arquivo
    FILE *arquivo_tempo = fopen(ARQUIVO_TEMPO, "w");
    if (arquivo_tempo) {
        fprintf(arquivo_tempo, "%.4f segundos", tempopassado);
        fclose(arquivo_tempo);
        printf("Tempo decorrido salvo em %s\n", ARQUIVO_TEMPO);
    } else {
        printf("Erro ao salvar o tempo decorrido em arquivo.\n");
    }

    mpf_clear(soma);
    mpf_clear(um);
    mpf_clear(quatro);

    return 0;
}
