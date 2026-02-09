/*
  Ajustement non-linéaire par descente du gradient pour f(x) = a * exp(b * x)
  Format attendu pour donnees.txt :
    première ligne : nombre de points n
    puis n lignes : x, y

  Compilation : gcc gauchy_exp.c -o gauchy_exp -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void read_data(const char *filename, double **x, double **y, int *n) {
    FILE *f = fopen(filename, "r");
    if (!f) { perror("fopen"); exit(1); }
    if (fscanf(f, "%d\n", n) != 1) { fprintf(stderr, "Format attendu : n en première ligne\n"); fclose(f); exit(1); }
    *x = (double*)malloc((*n) * sizeof(double));
    *y = (double*)malloc((*n) * sizeof(double));
    for (int i = 0; i < *n; i++) {
        double xi, yi;
        if (fscanf(f, "%lf , %lf\n", &xi, &yi) != 2) {
            // essayer sans espace après la virgule
            fseek(f, 0, SEEK_SET);
            // saut de la première ligne
            char buffer[256]; fgets(buffer, sizeof(buffer), f);
            for (int j = 0; j < *n; j++) {
                if (fscanf(f, "%lf , %lf\n", &xi, &yi) != 2) {
                    fprintf(stderr, "Erreur de lecture des données à la ligne %d\n", j+2);
                    fclose(f); exit(1);
                }
                (*x)[j] = xi; (*y)[j] = yi;
            }
            break;
        }
        (*x)[i] = xi; (*y)[i] = yi;
    }
    fclose(f);
}

double cost(const double *x, const double *y, int n, double a, double b) {
    double s = 0.0;
    for (int i = 0; i < n; i++) {
        double pred = a * exp(b * x[i]);
        double e = pred - y[i];
        s += e * e;
    }
    return s / (2.0 * n);
}

void gradient_step(const double *x, const double *y, int n, double a, double b, double *ga, double *gb) {
    // Cost J = (1/(2n)) sum (a e^{b x_i} - y_i)^2
    // dJ/da = (1/n) sum (a e^{b x_i} - y_i) * e^{b x_i}
    // dJ/db = (1/n) sum (a e^{b x_i} - y_i) * a * x_i * e^{b x_i}
    double sga = 0.0, sgb = 0.0;
    for (int i = 0; i < n; i++) {
        double ebx = exp(b * x[i]);
        double pred = a * ebx;
        double diff = pred - y[i];
        sga += diff * ebx;            // = (a e^{b xi} - yi) e^{b xi}
        sgb += diff * a * x[i] * ebx; // = (a e^{b xi} - yi) * a * xi * e^{b xi}
    }
    *ga = sga / (double)n;
    *gb = sgb / (double)n;
}

int main(void) {
    const char *filename = "donnees.txt";
    double *x = NULL, *y = NULL; int n = 0;
    read_data(filename, &x, &y, &n);

    // Paramètres initiaux selon l'énoncé proposé
    double a = 1.0;
    double b = 0.1;
    double learning_rate = 0.01; // pas d'apprentissage
    double eps = 0.001; // critère d'arrêt pour la norme des différences de paramètres
    int max_iter = 200000;

    printf("Ajustement exponentiel f(x)=a*exp(b x) par descente du gradient\n");
    printf("Points: %d\n", n);
    printf("Init: a=%.6f, b=%.6f, lr=%.6f, eps=%.6f\n", a, b, learning_rate, eps);

    double prev_a = a, prev_b = b;
    int iter;
    for (iter = 0; iter < max_iter; iter++) {
        double ga, gb;
        gradient_step(x, y, n, a, b, &ga, &gb);
        // mise à jour
        prev_a = a; prev_b = b;
        a -= learning_rate * ga;
        b -= learning_rate * gb;

        double da = a - prev_a;
        double db = b - prev_b;
        if (sqrt(da*da + db*db) < eps) {
            break;
        }
        // affichage périodique
        if (iter % 5000 == 0) {
            double c = cost(x,y,n,a,b);
            printf("it=%6d  a=%.6f  b=%.6f  cost=%.6f\n", iter, a, b, c);
        }
    }

    double final_cost = cost(x,y,n,a,b);
    printf("\nTermine: iterations=%d\n", iter+1);
    printf("a = %.6f\n", a);
    printf("b = %.6f\n", b);
    printf("Cost = %.6f\n", final_cost);

    // Ecrire un fichier texte de sortie résumé
    FILE *out = fopen("reponse_exercice.txt", "w");
    if (out) {
        fprintf(out, "Ajustement exponentiel par descente du gradient\n\n");
        fprintf(out, "Fichier de données : %s\n", filename);
        fprintf(out, "Paramètres initiaux : a0=1.0, b0=0.1\n");
        fprintf(out, "Méthode : descente du gradient (pas fixe lr=%.6f)\n\n", learning_rate);
        fprintf(out, "Résultat:\n");
        fprintf(out, "a = %.6f\n", a);
        fprintf(out, "b = %.6f\n", b);
        fprintf(out, "Cost = %.6f\n", final_cost);
        fprintf(out, "Iterations = %d\n\n", iter+1);

        fprintf(out, "D(a,b) = (1/(2n)) * Σ_i (y_i - a e^{b x_i})^2\n");
        fprintf(out, "∂D/∂a = (1/n) Σ_i (a e^{b x_i} - y_i) e^{b x_i}\n");
        fprintf(out, "∂D/∂b = (1/n) Σ_i (a e^{b x_i} - y_i) * a * x_i * e^{b x_i}\n\n");

        fprintf(out, "Critère d'arret utilisé : norme des changements de paramètres < %.6f\n", eps);
        fclose(out);
    }

    free(x); free(y);
    return 0;
}
