#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// Função para verificar se um número é primo
// Retorna 1 se for primo, 0 caso contrário.
int is_prime(int num) {
    if (num <= 1) return 0;
    if (num <= 3) return 1;
    if (num % 2 == 0 || num % 3 == 0) return 0;
    
    // Um número primo (maior que 3) pode ser expresso como 6k ± 1.
    // Verificamos divisores a partir de 5.
    for (int i = 5; i * i <= num; i = i + 6) {
        if (num % i == 0 || num % (i + 2) == 0) {
            return 0;
        }
    }
    return 1;
}

int main() {
    //Valor máximo para a contagem.
    int n = 20000000; 

    double start_time, end_time;
    
    // --- VERSÃO SEQUENCIAL ---
    printf("--- Executando Versão Sequencial ---\n");
    int total_primes_seq = 0;
    start_time = omp_get_wtime();

    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            total_primes_seq++;
        }
    }

    end_time = omp_get_wtime();
    printf("Numeros primos encontrados: %d\n", total_primes_seq);
    printf("Tempo de execucao sequencial: %f segundos\n\n", end_time - start_time);


    // --- VERSÃO PARALELA ---
    printf("--- Executando Versao Paralela ---\n");
    int total_primes_par = 0;
    start_time = omp_get_wtime();

    // A diretiva #pragma omp parallel for divide as iterações do laço
    // entre as threads disponíveis.
    #pragma omp parallel for
    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            // CUIDADO: Esta linha causa um problema!
            total_primes_par++; 
        }
    }

    end_time = omp_get_wtime();
    printf("Resultado (incorreto) da versao paralela: %d\n", total_primes_par);
    printf("Tempo de execucao paralela (incorreto): %f segundos\n\n", end_time - start_time);


    // --- VERSÃO PARALELA (CORRIGIDA) ---
    printf("--- Executando Versao Paralela (Corrigida) ---\n");
    total_primes_par = 0; // Reinicia a contagem
    start_time = omp_get_wtime();
    
    // Usamos a cláusula 'reduction' para evitar a condição de corrida.
    // Cada thread terá sua própria cópia local de 'total_primes_par'.
    // No final, o OpenMP soma os valores de todas as cópias locais.
    #pragma omp parallel for reduction(+:total_primes_par)
    for (int i = 2; i <= n; i++) {
        if (is_prime(i)) {
            total_primes_par++;
        }
    }

    end_time = omp_get_wtime();
    printf("Resultado (correto) da versao paralela: %d\n", total_primes_par);
    printf("Tempo de execucao paralela (correto): %f segundos\n", end_time - start_time);

    return 0;
}