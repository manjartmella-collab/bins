/* gradient_simple.c
 * Programme minimal en C utilisant int pour compteurs et float pour paramètres
 * Initialise a0 = 0.2, b0 = 0.1 et alpha = 0.001
 * Modèle : f(x) = a * exp(b * x)
 * Lecture de donnees.txt : première ligne = n, puis n lignes "x, y"
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
        fprintf(stderr, "Format: première ligne nombre de points\n");
        fclose(f);
        return 1;
    }

    float *xs = (float*)malloc(n * sizeof(float));
    float *ys = (float*)malloc(n * sizeof(float));
    if (!xs || !ys) { fclose(f); fprintf(stderr, "Erreur allocation\n"); return 1; }

    for (int i = 0; i < n; i++) {
        if (fscanf(f, " %f , %f", &xs[i], &ys[i]) != 2) {
            fprintf(stderr, "Erreur lecture ligne %d\n", i+2);
            free(xs); free(ys); fclose(f); return 1;
        }
    }
    fclose(f);

    /* Paramètres demandés */
    float a = 0.2f;
    float b = 0.1f;
    float alpha = 0.001f; /* pas d'apprentissage */
    float eps = 0.0001f;
    int max_iter = 200000;

    for (int iter = 0; iter < max_iter; iter++) {
        /* calcul gradient (utilisant float) */
        float ga = 0.0f;
        float gb = 0.0f;
        for (int i = 0; i < n; i++) {
            float ebx = expf(b * xs[i]);
            float pred = a * ebx;
            float diff = pred - ys[i];
            ga += diff * ebx;           /* dérivée partielle par rapport à a */
            gb += diff * a * xs[i] * ebx;/* dérivée partielle par rapport à b */
        }
        ga /= (float)n;
        gb /= (float)n;

        float prev_a = a;
        float prev_b = b;

        a -= alpha * ga;
        b -= alpha * gb;

        float da = a - prev_a;
        float db = b - prev_b;
        float norm = sqrtf(da*da + db*db);
        if (norm < eps) {
            printf("Converge en %d iterations\n", iter+1);
            break;
        }
        /* affichage simple toutes les 50000 itérations */
        if (iter % 50000 == 0) {
            /* calcul coût pour suivre */
            float cost = 0.0f;
            for (int i = 0; i < n; i++) {
                float pred = a * expf(b * xs[i]);
                float e = pred - ys[i]; cost += e*e;
            }
            cost /= (2.0f * n);
            printf("it=%d a=%.6f b=%.6f cost=%.6f\n", iter, a, b, cost);
        }
    }

    /* coût final */
    float final_cost = 0.0f;
    for (int i = 0; i < n; i++) {
        float pred = a * expf(b * xs[i]);
        float e = pred - ys[i]; final_cost += e*e;
    }
    final_cost /= (2.0f * n);

    printf("Resultat final:\n");
    printf("a = %.6f\n", a);
    printf("b = %.6f\n", b);
    printf("cost = %.6f\n", final_cost);

    free(xs); free(ys);
    return 0;
}
