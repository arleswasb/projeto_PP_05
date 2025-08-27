#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Função para verificar se um número é primo
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    for (int i = 5; i * i <= num; i = i + 6) {
        if (num % i == 0 || num % (i + 2) == 0) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int n = 30000000;
    double start_time, sequential_time, parallel_time_corr, parallel_time_incorr;
    int total_primes_seq = 0, total_primes_par_corr = 0, total_primes_par_incorr = 0;

    // --- 1. VERSÃO SEQUENCIAL (BASELINE) ---
    printf("--- Executando Versão Sequencial (Baseline) ---\n");
    start_time = omp_get_wtime();
    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            total_primes_seq++;
        }
    }
    sequential_time = omp_get_wtime() - start_time;
    printf("Resultado: %d | Tempo: %f segundos\n\n", total_primes_seq, sequential_time);

    // --- 2. VERSÃO PARALELA COM CONDIÇÃO DE CORRIDA ---
    printf("--- Executando Versão Paralela (Com Condição de Corrida) ---\n");
    for (int num_threads = 2; num_threads <= 16; num_threads *= 2) {
        omp_set_num_threads(num_threads);
        total_primes_par_incorr = 0;
        start_time = omp_get_wtime();
        #pragma omp parallel for
        for (int i = 2; i <= n; i++) {
            if (is_prime(i)) {
                total_primes_par_incorr++;
            }
        }
        parallel_time_incorr = omp_get_wtime() - start_time;
        printf("Threads: %2d | Resultado (Incorreto): %d | Tempo: %f segundos\n", num_threads, total_primes_par_incorr, parallel_time_incorr);
    }
    printf("\n");

    // --- 3. VERSÃO PARALELA CORRIGIDA COM REDUCTION ---
printf("--- Executando Versão Paralela (Corrigida com Reduction e Guided Schedule) ---\n");
// A correção está na terceira parte do for: 'num_threads *= 2'
for (int num_threads = 1; num_threads <= 16; num_threads *= 2) {
    omp_set_num_threads(num_threads);
    total_primes_par_corr = 0;
    start_time = omp_get_wtime();

    #pragma omp parallel for reduction(+:total_primes_par_corr) schedule(guided)
    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            total_primes_par_corr++;
        }
    }
    
    parallel_time_corr = omp_get_wtime() - start_time;
    printf("Threads: %2d | Resultado (Correto):   %d | Tempo: %f segundos\n", num_threads, total_primes_par_corr, parallel_time_corr);
}

    return 0;
}