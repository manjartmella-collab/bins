#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ===== PROTOTYPES ===== */

/* Fonctions de lecture et affichage */
void getDataf(char *filename, float ***data, int *n, int *max_points);
void displayPoints(float **data, int n);
void displayResults(float a0, float a1, float cost, int iterations_used);

/* Fonctions de calcul */
float computeCost(float **data, int n, float a0, float a1);
int gradientDescent(float **data, int n, float *a0, float *a1, 
                     float learning_rate, int max_iterations, 
                     float convergence_threshold);

/* Fonctions pour gnuplot */
void generatePlotData(float **data, int n, float a0, float a1, char *datafile, char *fitfile);
void plotWithGnuplot(float **data, int n, float a0, float a1, int iterations_used);

/* Fonctions utilitaires */
void freeMemory(float **data, int n);
void error(char *message);
void initParameters(int *max_points, float *learning_rate, 
                    int *max_iterations, float *convergence_threshold);

/* ===== PROGRAMME PRINCIPAL ===== */
int main(void) {
    printf("Regression lineaire par descente du gradient\n");
    printf("===========================================\n\n");
// donnees    
    float **data = NULL;
    int n = 0;
    float a0 = 0.0f, a1 = 0.0f;
    int iterations_used = 0;
    int regression_faite = 0;  // 0 = non, 1 = oui
    
// Variables pour les paramètres (fixes)
    int max_points = 1000;
    float learning_rate = 0.01f;
    int max_iterations = 10000;
    float convergence_threshold = 0.0001f;
    
// Lecture des données depuis le fichier
    getDataf("donnees.txt", &data, &n, &max_points);
    
    printf("Nombre de points de donnees: %d\n\n", n);
    
// Menu de choix
    int choix;
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Effectuer la regression et afficher les resultats\n");
        printf("2. Generer un graphique avec gnuplot\n");
        printf("3. Quitter\n");
        printf("Votre choix: ");
        scanf("%d", &choix);
        
        switch(choix) {
            case 1: {
                printf("\n=== REGRESSION LINEAIRE PAR DESCENTE DU GRADIENT ===\n");
                
                // Afficher les paramètres utilisés
                printf("Parametres d'optimisation:\n");
                printf("  Taux d'apprentissage: %f\n", learning_rate);
                printf("  Nombre maximum d'iterations: %d\n", max_iterations);
                printf("  Seuil de convergence: %f\n", convergence_threshold);
                printf("\n");
                
                // Afficher les données
                displayPoints(data, n);
                
                // Initialisation des coefficients
                a0 = 0.0f;
                a1 = 0.0f;
                
                // Résolution par descente du gradient
                printf("\n=== DESCENTE DU GRADIENT EN COURS ===\n");
                iterations_used = gradientDescent(data, n, &a0, &a1, 
                               learning_rate, 
                               max_iterations, 
                               convergence_threshold);
                
                // Calcul du coût final
                float final_cost = computeCost(data, n, a0, a1);
                
                // Affichage détaillé des résultats
                printf("\n=== RESULTATS DETAILLES ===\n");
                printf("Equation de la droite ajustee: y = %.6f + %.6f * x\n", a0, a1);
                printf("\nCoefficients:\n");
                printf("  Ordonnee a l'origine (a0): %.6f\n", a0);
                printf("  Pente (a1): %.6f\n", a1);
                printf("\nMetriques d'erreur:\n");
                printf("  Erreur quadratique moyenne: %.6f\n", final_cost);
                printf("  Racine de l'erreur quadratique moyenne: %.6f\n", sqrtf(final_cost * 2.0f * n));
                printf("\nPerformances d'optimisation:\n");
                printf("  Iterations utilisees: %d\n", iterations_used);
                printf("  Iterations maximum autorisees: %d\n", max_iterations);
                printf("  Pourcentage d'iterations utilisees: %.1f%%\n", 
                       (iterations_used * 100.0f) / max_iterations);
                if (iterations_used < max_iterations) {
                    printf("  Convergence: ATTEINTE\n");
                } else {
                    printf("  Convergence: NON ATTEINTE (maximum d'iterations)\n");
                }
                printf("\nVerification avec les donnees:\n");
                for (int i = 0; i < n; i++) {
                    float prediction = a0 + a1 * data[i][0];
                    float erreur = prediction - data[i][1];
                    printf("  Point %d: x=%.3f, y_reel=%.3f, y_pred=%.3f, erreur=%.3f\n", 
                           i+1, data[i][0], data[i][1], prediction, erreur);
                }
                printf("====================================\n");
                
                regression_faite = 1;
                break;
            }
            
            case 2: {
                printf("\n=== GENERATION DE GRAPHIQUE AVEC GNUPLOT ===\n");
                
                if (!regression_faite) {
                    printf("Attention: Aucune regression n'a ete effectuee.\n");
                    printf("Voulez-vous effectuer une regression d'abord? (1=Oui, 0=Non): ");
                    int effectuer_regression;
                    scanf("%d", &effectuer_regression);
                    
                    if (effectuer_regression == 1) {
                        printf("\n=== REGRESSION EN COURS ===\n");
                        a0 = 0.0f;
                        a1 = 0.0f;
                        iterations_used = gradientDescent(data, n, &a0, &a1, 
                                       learning_rate, 
                                       max_iterations, 
                                       convergence_threshold);
                        float final_cost = computeCost(data, n, a0, a1);
                        printf("\nRegression terminee:\n");
                        printf("  a0 = %.6f, a1 = %.6f\n", a0, a1);
                        printf("  Erreur = %.6f, Iterations = %d\n", final_cost, iterations_used);
                        regression_faite = 1;
                    } else {
                        printf("Retour au menu principal.\n");
                        break;
                    }
                }
                
                printf("\nGeneration du graphique...\n");
                plotWithGnuplot(data, n, a0, a1, iterations_used);
                break;
            }
            
            case 3:
                printf("\nAu revoir!\n");
                break;
                
            default:
                printf("Choix invalide! Veuillez choisir 1, 2 ou 3.\n");
        }
    } while (choix != 3);
    
    // Libération de la mémoire
    freeMemory(data, n);
    
    return 0;
}

/* ===== DEFINITIONS DES FONCTIONS ===== */

/* ===== Lecture des données depuis fichier ===== */
void getDataf(char *filename, float ***data, int *n, int *max_points) {
    FILE *pf = NULL;
    int i;
    char ligne[100];
    char *token;
    
    if ((pf = fopen(filename, "r")) == NULL) {
        error("Probleme a l'ouverture du fichier...");
    }
    
    // Lire le nombre de points (première ligne)
    if (fgets(ligne, sizeof(ligne), pf) == NULL) {
        error("Erreur de lecture de la premiere ligne...");
    }
    
    *n = atoi(ligne);
    if (*n <= 0 || *n > *max_points) {
        error("Nombre de points invalide...");
    }
    
    // Allocation de la matrice pour stocker les points
    *data = (float **)malloc((*n) * sizeof(float *));
    if (!(*data)) error("Probleme d'allocation memoire pour les donnees...");
    
    for (i = 0; i < *n; i++) {
        (*data)[i] = (float *)malloc(2 * sizeof(float));
        if (!(*data)[i]) error("Probleme d'allocation memoire pour les points...");
    }
    
    // Lire chaque ligne de données
    for (i = 0; i < *n; i++) {
        if (fgets(ligne, sizeof(ligne), pf) == NULL) {
            error("Erreur de lecture des donnees...");
        }
        
        // Supprimer le retour à la ligne
        ligne[strcspn(ligne, "\n")] = 0;
        
        // Parser la ligne: format "x, y"
        token = strtok(ligne, ",");
        if (token == NULL) {
            error("Format de donnees invalide (x manquant)...");
        }
        (*data)[i][0] = atof(token);  // x
        
        token = strtok(NULL, ",");
        if (token == NULL) {
            error("Format de donnees invalide (y manquant)...");
        }
        (*data)[i][1] = atof(token);  // y
    }
    
    fclose(pf);
}

/* ===== Descente du gradient ===== */
int gradientDescent(float **data, int n, float *a0, float *a1, 
                     float learning_rate, int max_iterations, 
                     float convergence_threshold) {
    float temp_a0, temp_a1;
    float grad_a0, grad_a1;
    int iteration;
    int i;
    
    printf("Iteration    a0        a1        Cout\n");
    printf("-------------------------------------\n");
    
    for (iteration = 0; iteration < max_iterations; iteration++) {
        // Initialisation des gradients
        grad_a0 = 0.0f;
        grad_a1 = 0.0f;
        
        // Calcul des gradients
        for (i = 0; i < n; i++) {
            float prediction = *a0 + *a1 * data[i][0];
            float error = prediction - data[i][1];
            
            grad_a0 += error;
            grad_a1 += error * data[i][0];
        }
        
        // Moyenne des gradients
        grad_a0 /= (float)n;
        grad_a1 /= (float)n;
        
        // Mise à jour des paramètres
        temp_a0 = *a0 - learning_rate * grad_a0;
        temp_a1 = *a1 - learning_rate * grad_a1;
        
        // Vérification de la convergence
        if (fabsf(temp_a0 - *a0) < convergence_threshold && 
            fabsf(temp_a1 - *a1) < convergence_threshold) {
            float cost = computeCost(data, n, *a0, *a1);
            printf("%6d    %8.4f  %8.4f  %8.4f  (Convergence)\n", 
                   iteration, *a0, *a1, cost);
            *a0 = temp_a0;
            *a1 = temp_a1;
            return iteration + 1;
        }
        
        *a0 = temp_a0;
        *a1 = temp_a1;
        
        // Affichage tous les 1000 itérations
        if (iteration % 1000 == 0) {
            float cost = computeCost(data, n, *a0, *a1);
            printf("%6d    %8.4f  %8.4f  %8.4f\n", 
                   iteration, *a0, *a1, cost);
        }
    }
    
    // Dernier affichage
    float final_cost = computeCost(data, n, *a0, *a1);
    printf("%6d    %8.4f  %8.4f  %8.4f  (Maximum atteint)\n", 
           max_iterations - 1, *a0, *a1, final_cost);
    
    return max_iterations;
}

/* ===== Calcul du coût ===== */
float computeCost(float **data, int n, float a0, float a1) {
    float cost = 0.0f;
    int i;
    
    for (i = 0; i < n; i++) {
        float prediction = a0 + a1 * data[i][0];
        float error = prediction - data[i][1];
        cost += error * error;
    }
    return cost / (2.0f * (float)n);
}

/* ===== Génération des fichiers pour gnuplot ===== */
void generatePlotData(float **data, int n, float a0, float a1, char *datafile, char *fitfile) {
    FILE *fdata = fopen(datafile, "w");
    FILE *ffit = fopen(fitfile, "w");
    int i;
    
    if (!fdata || !ffit) {
        printf("Erreur lors de la creation des fichiers pour gnuplot.\n");
        if (fdata) fclose(fdata);
        if (ffit) fclose(ffit);
        return;
    }
    
    // Écrire les données dans le fichier
    for (i = 0; i < n; i++) {
        fprintf(fdata, "%.6f %.6f\n", data[i][0], data[i][1]);
    }
    
    // Trouver les limites x
    float xmin = data[0][0];
    float xmax = data[0][0];
    for (i = 1; i < n; i++) {
        if (data[i][0] < xmin) xmin = data[i][0];
        if (data[i][0] > xmax) xmax = data[i][0];
    }
    
    // Étendre un peu les limites
    xmin = xmin - 0.1 * (xmax - xmin);
    xmax = xmax + 0.1 * (xmax - xmin);
    
    // Générer la droite de régression
    float step = (xmax - xmin) / 100.0f;
    for (float x = xmin; x <= xmax; x += step) {
        float y = a0 + a1 * x;
        fprintf(ffit, "%.6f %.6f\n", x, y);
    }
    
    fclose(fdata);
    fclose(ffit);
}

/* ===== Fonction pour créer et exécuter un script gnuplot ===== */
void plotWithGnuplot(float **data, int n, float a0, float a1, int iterations_used) {
    // Générer les fichiers de données
    generatePlotData(data, n, a0, a1, "donnees_plot.txt", "droite_plot.txt");
    
    // Créer le script gnuplot
    FILE *gnuplot_script = fopen("regression.gnu", "w");
    if (!gnuplot_script) {
        printf("Erreur lors de la creation du script gnuplot.\n");
        return;
    }
    
    // Trouver les limites
    float xmin = data[0][0];
    float xmax = data[0][0];
    float ymin = data[0][1];
    float ymax = data[0][1];
    
    for (int i = 1; i < n; i++) {
        if (data[i][0] < xmin) xmin = data[i][0];
        if (data[i][0] > xmax) xmax = data[i][0];
        if (data[i][1] < ymin) ymin = data[i][1];
        if (data[i][1] > ymax) ymax = data[i][1];
    }
    
    // Ajouter des marges
    float xrange = xmax - xmin;
    float yrange = ymax - ymin;
    xmin -= 0.1 * xrange;
    xmax += 0.1 * xrange;
    ymin -= 0.1 * yrange;
    ymax += 0.1 * yrange;
    
    fprintf(gnuplot_script, "# Script gnuplot pour la regression lineaire\n");
    fprintf(gnuplot_script, "set terminal pngcairo enhanced font 'Arial,12' size 800,600\n");
    fprintf(gnuplot_script, "set output 'regression_plot.png'\n");
    fprintf(gnuplot_script, "set title 'Regression lineaire\\n");
    fprintf(gnuplot_script, "y = %.4f + %.4f x (Iterations: %d)'\n", a0, a1, iterations_used);
    fprintf(gnuplot_script, "set xlabel 'x'\n");
    fprintf(gnuplot_script, "set ylabel 'y'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top left\n");
    fprintf(gnuplot_script, "set xrange [%f:%f]\n", xmin, xmax);
    fprintf(gnuplot_script, "set yrange [%f:%f]\n", ymin, ymax);
    fprintf(gnuplot_script, "\n");
    fprintf(gnuplot_script, "plot 'donnees_plot.txt' with points pt 7 ps 1.5 lc rgb 'blue' title 'Donnees', \\\n");
    fprintf(gnuplot_script, "     'droite_plot.txt' with lines lw 2 lc rgb 'red' title 'Droite de regression'\n");
    
    fclose(gnuplot_script);
    
    // Exécuter gnuplot
    printf("Execution de gnuplot...\n");
    int result = system("gnuplot regression.gnu");
    
    if (result == 0) {
        printf("\nGraphique genere avec succes!\n");
        printf("Fichier cree: regression_plot.png\n");
        
        // Essayer d'afficher l'image
        #ifdef _WIN32
            system("start regression_plot.png");
        #elif __APPLE__
            system("open regression_plot.png");
        #else
            system("xdg-open regression_plot.png 2>/dev/null &");
        #endif
    } else {
        printf("\nErreur: gnuplot non trouve ou erreur d'execution.\n");
        printf("Installez gnuplot:\n");
        printf("  Ubuntu: sudo apt install gnuplot\n");
        printf("  Mac: brew install gnuplot\n");
        printf("  Windows: telechargez depuis gnuplot.info\n");
    }
}

/* ===== Fonctions d'affichage ===== */
void displayPoints(float **data, int n) {
    int i;
    
    printf("Donnees chargees (%d points):\n", n);
    printf("-----------------------------\n");
    for (i = 0; i < n; i++) {
        printf("  [%d] x = %6.3f, y = %6.3f\n", i+1, data[i][0], data[i][1]);
    }
    printf("\n");
}

void displayResults(float a0, float a1, float cost, int iterations_used) {
    printf("Resultats de la regression:\n");
    printf("  a0 = %.6f\n", a0);
    printf("  a1 = %.6f\n", a1);
    printf("  Erreur = %.6f\n", cost);
    printf("  Iterations = %d\n", iterations_used);
}

/* ===== Fonctions utilitaires ===== */
void freeMemory(float **data, int n) {
    int i;
    
    if (data) {
        for (i = 0; i < n; i++) {
            if (data[i]) {
                free(data[i]);
            }
        }
        free(data);
    }
}

void error(char *message) {
    printf("\n=== ERREUR ===\n");
    printf("%s\n", message);
    printf("==============\n");
    printf("Format du fichier 'donnees.txt':\n");
    printf("  9\n");
    printf("  1, 6.008\n");
    printf("  1.1, 5.257\n");
    printf("  ...\n");
    exit(EXIT_FAILURE);
}
