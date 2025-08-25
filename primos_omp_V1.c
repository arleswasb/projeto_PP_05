#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Função para verificar se um número é primo (sem alterações)
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
    // Aumentar o valor de 'n' tornará os tempos mais longos e a análise mais clara.
    int n = 30000000; 

    double start_time, sequential_time, parallel_time;
    
    // --- 1. BASELINE: VERSÃO SEQUENCIAL ---
    printf("--- Executando Versão Sequencial (Baseline) ---\n");
    int total_primes_seq = 0;
    start_time = omp_get_wtime();

    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            total_primes_seq++;
        }
    }

    sequential_time = omp_get_wtime() - start_time;
    printf("Números primos encontrados: %d\n", total_primes_seq);
    printf("Tempo de execução sequencial: %f segundos\n\n", sequential_time);


    // --- 2. TESTE DE ESCALABILIDADE PARALELA ---
    printf("--- Iniciando Teste de Escalabilidade Paralela ---\n");
    printf("------------------------------------------------------------------\n");
    printf("| Threads | Tempo (s)  | Speedup | Eficiência |\n");
    printf("------------------------------------------------------------------\n");

    // Laço para testar com 1 a 16 threads
    for (int num_threads = 1; num_threads <= 16; num_threads++) {
        // Define o número de threads para a próxima região paralela
        omp_set_num_threads(num_threads);

        int total_primes_par = 0;
        start_time = omp_get_wtime();
        
        #pragma omp parallel for reduction(+:total_primes_par) schedule(guided)
        for (int i = 2; i <= n; i++) {
            if (is_prime(i)) {
                total_primes_par++;
            }
        }

        parallel_time = omp_get_wtime() - start_time;
        
        // Verificação para garantir que o resultado está correto
        if (total_primes_par != total_primes_seq) {
            printf("Erro: O resultado com %d threads é inconsistente!\n", num_threads);
        }

        double speedup = sequential_time / parallel_time;
        double efficiency = speedup / num_threads;

        printf("| %-7d | %-10.6f | %-7.2fx | %-10.2f%% |\n", 
               num_threads, parallel_time, speedup, efficiency * 100.0);
    }
    printf("------------------------------------------------------------------\n");

    return 0;
}