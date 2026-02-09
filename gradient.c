/*
 * gradient.c
 * Programme complet pour la question 4 :
 * Ajustement de f(x) = a * exp(b x) par descente du gradient
 * Lecture de donnees.txt (première ligne = nombre de points), initialisation
 * a0 = 1.0, b0 = 0.1, eps = 0.001 (critère d'arrêt), pas fixe lr = 0.01.
 * Génère aussi des fichiers pour tracer la courbe : donnees_plot.txt et exp_plot.txt
 * et crée un script `regression_exp.gnu` (optionnellement exécutable si gnuplot est installé).
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Fonctions utilitaires */
static void error_and_exit(const char *msg) {
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

static void read_data(const char *filename, double **px, double **py, int *pn) {
	FILE *f = fopen(filename, "r");
	if (!f) error_and_exit("Impossible d'ouvrir le fichier de données");
	if (fscanf(f, "%d", pn) != 1) {
		fclose(f);
		error_and_exit("Format attendu : première ligne = nombre de points");
	}
	int n = *pn;
	*px = (double*)malloc(n * sizeof(double));
	*py = (double*)malloc(n * sizeof(double));
	if (!*px || !*py) { fclose(f); error_and_exit("Allocation mémoire"); }

	for (int i = 0; i < n; i++) {
		double xi, yi;
		if (fscanf(f, " %lf , %lf", &xi, &yi) != 2) {
			fclose(f);
			error_and_exit("Erreur de lecture des données (format x, y attendu)");
		}
		(*px)[i] = xi;
		(*py)[i] = yi;
	}
	fclose(f);
}

/* Fonction coût : J(a,b) = (1/(2n)) Σ (a e^{b x_i} - y_i)^2 */
static double compute_cost(const double *x, const double *y, int n, double a, double b) {
	double s = 0.0;
	for (int i = 0; i < n; i++) {
		double pred = a * exp(b * x[i]);
		double diff = pred - y[i];
		s += diff * diff;
	}
	return s / (2.0 * n);
}

/* Calcul du gradient : partials ga = ∂J/∂a, gb = ∂J/∂b */
static void compute_gradient(const double *x, const double *y, int n, double a, double b, double *ga, double *gb) {
	double sga = 0.0, sgb = 0.0;
	for (int i = 0; i < n; i++) {
		double ebx = exp(b * x[i]);
		double pred = a * ebx;
		double diff = pred - y[i];
		sga += diff * ebx;            /* dérivée partielle par rapport à a */
		sgb += diff * a * x[i] * ebx; /* dérivée partielle par rapport à b */
	}
	*ga = sga / (double)n;
	*gb = sgb / (double)n;
}

/* Génération des fichiers pour tracé */
static void write_plot_files(const double *x, const double *y, int n, double a, double b) {
	FILE *fd = fopen("donnees_plot.txt", "w");
	if (fd) {
		for (int i = 0; i < n; i++) fprintf(fd, "%.6f %.6f\n", x[i], y[i]);
		fclose(fd);
	}

	/* génération d'une courbe lisse pour l'exponentielle */
	double xmin = x[0], xmax = x[0];
	for (int i = 1; i < n; i++) {
		if (x[i] < xmin) xmin = x[i];
		if (x[i] > xmax) xmax = x[i];
	}
	double range = xmax - xmin;
	if (range == 0.0) { xmin -= 1.0; xmax += 1.0; range = 2.0; }
	double start = xmin - 0.1 * range;
	double end = xmax + 0.1 * range;
	FILE *fe = fopen("exp_plot.txt", "w");
	if (fe) {
		int steps = 200;
		double step = (end - start) / steps;
		for (double xv = start; xv <= end + 1e-12; xv += step) {
			double yv = a * exp(b * xv);
			fprintf(fe, "%.6f %.6f\n", xv, yv);
		}
		fclose(fe);
	}

	/* script gnuplot minimal */
	FILE *g = fopen("regression_exp.gnu", "w");
	if (g) {
		fprintf(g, "set terminal pngcairo size 800,600 enhanced font 'Arial,12'\n");
		fprintf(g, "set output 'regression_exp.png'\n");
		fprintf(g, "set grid\n");
		fprintf(g, "set xlabel 'x'\n");
		fprintf(g, "set ylabel 'y'\n");
		fprintf(g, "plot 'donnees_plot.txt' with points pt 7 ps 1.5 title 'Données', \\\n");
		fprintf(g, "     'exp_plot.txt' with lines lw 2 lc rgb 'red' title sprintf('a=%.6f b=%.6f', %.6f, %.6f)\n", a, b, a, b);
		fclose(g);
	}
}

int main(void) {
	const char *fname = "donnees.txt";
	double *x = NULL, *y = NULL;
	int n = 0;
	read_data(fname, &x, &y, &n);

	/* Paramètres demandés par l'énoncé */
	double a = 1.0;
	double b = 0.1;
	double lr = 0.01;     /* pas d'apprentissage */
	double eps = 0.001;   /* critère d'arrêt sur la norme des changements */
	int max_iter = 200000;

	printf("Descente du gradient pour f(x)=a*exp(b x)\n");
	printf("Initial: a=%.6f, b=%.6f, lr=%.6f, eps=%.6f\n", a, b, lr, eps);

	double prev_a = a, prev_b = b;
	int iter;
	for (iter = 0; iter < max_iter; iter++) {
		double ga, gb;
		compute_gradient(x, y, n, a, b, &ga, &gb);
		prev_a = a; prev_b = b;
		a -= lr * ga;
		b -= lr * gb;

		double da = a - prev_a;
		double db = b - prev_b;
		if (sqrt(da*da + db*db) < eps) break;

		if (iter % 5000 == 0) {
			double c = compute_cost(x, y, n, a, b);
			printf("it=%6d  a=%.6f  b=%.6f  cost=%.6f\n", iter, a, b, c);
		}
	}

	double final_cost = compute_cost(x, y, n, a, b);
	printf("\nTermine: iterations=%d\n", iter+1);
	printf("a = %.6f\n", a);
	printf("b = %.6f\n", b);
	printf("Cost = %.6f\n", final_cost);

	/* écrire les résultats dans reponse_exercice.txt (ajout) */
	FILE *out = fopen("reponse_exercice.txt", "w");
	if (out) {
		fprintf(out, "Résultat de gradient.c (question 4)\n\n");
		fprintf(out, "a = %.6f\n", a);
		fprintf(out, "b = %.6f\n", b);
		fprintf(out, "Cost = %.6f\n", final_cost);
		fprintf(out, "Iterations = %d\n", iter+1);
		fclose(out);
	}

	/* Générer fichiers pour tracé */
	write_plot_files(x, y, n, a, b);

	free(x); free(y);
	return 0;
}

