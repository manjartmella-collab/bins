/* gradient_int.c
 * Programme très simple (niveau 2ème année) pour ajuster
 * f(x) = a * exp(b * x) par descente du gradient.
 * Utilise seulement : a, b, alpha (float) et compteurs int.
 * Lecture de donnees.txt : première ligne = n, puis n lignes "x, y"
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* prototypes */
void read_data(const char *filename, float **px, float **py, int *pn);
float compute_cost(const float *x, const float *y, int n, float a, float b);
void gradient_descent(const float *x, const float *y, int n, float *pa, float *pb, float alpha, float eps, int *piter);
void displayResult(float a, float b, float cost, int iterations, float eps);

int main(void) {
    const char *filename = "donnees.txt";
    float *x = NULL, *y = NULL;
    int n = 0;

    read_data(filename, &x, &y, &n);

    /* paramètres demandés */
    float a = 0.2f;
    float b = 0.1f;
    float alpha = 0.001f; /* pas d'apprentissage */
    float eps = 0.001f;   /* critère d'arrêt */
    int iterations = 0;

    /* exécution de la descente du gradient */
    gradient_descent(x, y, n, &a, &b, alpha, eps, &iterations);

    float cost = compute_cost(x, y, n, a, b);
    displayResult(a, b, cost, iterations, eps);

    free(x); free(y);
    return 0;
/* gradient_int.c
 * Programme très simple (niveau 2ème année).
 * Modèle : f(x) = a * exp(b * x)
 * Utilise uniquement : a, b, alpha (float) et compteurs int
 * Lecture : donnees.txt (première ligne = n)
 * Initialisation : a = 0.2, b = 0.1, alpha = 0.001
 * Boucle : mise à jour par descente du gradient pour un nombre fixe d'itérations
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
    const char *filename = "donnees.txt";
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Ouverture donnees.txt");
        return 1;
    }

    int n;
    if (fscanf(f, "%d", &n) != 1) {
        fprintf(stderr, "Erreur: première ligne doit être le nombre de points\n");
        fclose(f);
        return 1;
    }

    float *x = (float*)malloc(n * sizeof(float));
    float *y = (float*)malloc(n * sizeof(float));
    if (!x || !y) { fclose(f); fprintf(stderr, "Allocation échouée\n"); return 1; }

    int i;
    for (i = 0; i < n; i++) {
        if (fscanf(f, " %f , %f", &x[i], &y[i]) != 2) {
            fprintf(stderr, "Erreur lecture ligne %d\n", i+2);
            free(x); free(y); fclose(f); return 1;
        }
    }
    fclose(f);

    /* paramètres simples demandés */
    float a = 0.2f;
    float b = 0.1f;
    float alpha = 0.001f; /* pas d'apprentissage */

    int max_iter = 10000; /* on effectue un nombre fixe d'itérations simple */

    for (int iter = 0; iter < max_iter; iter++) {
        /* calculer gradient (formules vues en cours) */
        float grad_a = 0.0f;
        float grad_b = 0.0f;
        for (i = 0; i < n; i++) {
            float ebx = expf(b * x[i]);
            float pred = a * ebx;
            float diff = pred - y[i];
            grad_a += diff * ebx;            /* ∂J/∂a contribution */
            grad_b += diff * a * x[i] * ebx; /* ∂J/∂b contribution */
        }
        grad_a /= (float)n;
        grad_b /= (float)n;

        /* mise à jour simple */
        a = a - alpha * grad_a;
        b = b - alpha * grad_b;

        /* Optionnel: affichage chaque 2000 itérations pour suivre */
        if (iter % 2000 == 0) {
            /* calculer coût pour afficher */
            float cost = 0.0f;
            for (i = 0; i < n; i++) {
                float p = a * expf(b * x[i]);
                float e = p - y[i]; cost += e * e;
            }
            cost /= (2.0f * n);
            printf("it=%d a=%.6f b=%.6f cost=%.6f\n", iter, a, b, cost);
        }
    }

    /* coût final et affichage */
    float final_cost = 0.0f;
    for (i = 0; i < n; i++) {
        float p = a * expf(b * x[i]);
        float e = p - y[i]; final_cost += e * e;
    }
    final_cost /= (2.0f * n);

    printf("\nRésultat final (simple):\n");
    printf("a = %.6f\n", a);
    printf("b = %.6f\n", b);
    printf("cost = %.6f\n", final_cost);

    free(x); free(y);
    return 0;
}
int main(void) {
