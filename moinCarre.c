#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* ===== PROTOTYPES ===== */

/* Fonctions de lecture et affichage */
void getDataf(char *filename, float ***data, int *n, int *max_points);
void displayPoints(float **data, int n);
void displayResults(float a0, float a1, float cost);

/* Fonctions de calcul - Méthode des moindres carrés */
float computeCost(float **data, int n, float a0, float a1);
void leastSquares(float **data, int n, float *a0, float *a1);

/* Fonctions pour gnuplot */
void generatePlotData(float **data, int n, float a0, float a1, char *datafile, char *fitfile);
void plotWithGnuplot(float **data, int n, float a0, float a1);

/* Fonctions utilitaires */
void freeMemory(float **data, int n);
void error(char *message);

/* ===== PROGRAMME PRINCIPAL ===== */
int main(void) {
    printf("Regression lineaire par methode des moindres carres\n");
    printf("===================================================\n\n");
// donnees   
    float **data = NULL;
    int n = 0;
    float a0 = 0.0f, a1 = 0.0f;
    int regression_faite = 0;  // 0 = non, 1 = oui
    
// Variables pour les paramètres
    int max_points = 1000;
    
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
                printf("\n=== REGRESSION LINEAIRE PAR MOINDRES CARRES ===\n");
                
                // Afficher les données
                displayPoints(data, n);
                
                // Résolution par méthode des moindres carrés
                printf("\n=== CALCUL EN COURS ===\n");
                printf("Calcul des sommes...\n");
                leastSquares(data, n, &a0, &a1);
                
                // Calcul du coût (erreur quadratique moyenne)
                float final_cost = computeCost(data, n, a0, a1);
                
                // Affichage détaillé des résultats
                printf("\n=== RESULTATS DETAILLES ===\n");
                printf("Equation de la droite ajustee: y = %.6f + %.6f * x\n", a0, a1);
                printf("\nCoefficients calcules analytiquement:\n");
                printf("  Ordonnee a l'origine (a0): %.6f\n", a0);
                printf("  Pente (a1): %.6f\n", a1);
                printf("\nMetriques d'erreur:\n");
                printf("  Erreur quadratique moyenne: %.6f\n", final_cost);
                printf("  Racine de l'erreur quadratique moyenne: %.6f\n", sqrtf(final_cost * 2.0f * n));
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
                        printf("\n=== CALCUL EN COURS ===\n");
                        leastSquares(data, n, &a0, &a1);
                        float final_cost = computeCost(data, n, a0, a1);
                        printf("\nRegression terminee:\n");
                        printf("  a0 = %.6f, a1 = %.6f\n", a0, a1);
                        printf("  Erreur = %.6f\n", final_cost);
                        regression_faite = 1;
                    } else {
                        printf("Retour au menu principal.\n");
                        break;
                    }
                }
                
                printf("\nGeneration du graphique...\n");
                plotWithGnuplot(data, n, a0, a1);
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

/* ===== Méthode des moindres carrés ===== */
void leastSquares(float **data, int n, float *a0, float *a1) {
    float sum_x = 0.0f, sum_y = 0.0f;
    float sum_xy = 0.0f, sum_x2 = 0.0f;
    float x_mean, y_mean;
    int i;
    
    // Calcul des sommes
    for (i = 0; i < n; i++) {
        sum_x += data[i][0];
        sum_y += data[i][1];
        sum_xy += data[i][0] * data[i][1];
        sum_x2 += data[i][0] * data[i][0];
    }
    
    // Calcul des moyennes
    x_mean = sum_x / n;
    y_mean = sum_y / n;
    
    printf("Sommes calculees:\n");
    printf("  Σx = %.6f\n", sum_x);
    printf("  Σy = %.6f\n", sum_y);
    printf("  Σxy = %.6f\n", sum_xy);
    printf("  Σx² = %.6f\n", sum_x2);
    printf("  Moyenne x = %.6f\n", x_mean);
    printf("  Moyenne y = %.6f\n", y_mean);
    
    // Calcul des coefficients par la formule analytique
    // a1 = (n*Σxy - Σx*Σy) / (n*Σx² - (Σx)²)
    // a0 = y_mean - a1*x_mean
    
    float denom = n * sum_x2 - sum_x * sum_x;
    
    if (fabsf(denom) < 1e-10) {
        printf("\nAttention: Matrice singuliere ou presque!\n");
        printf("Les points sont probablement alignes verticalement.\n");
        *a1 = 0.0f;
        *a0 = y_mean;
    } else {
        *a1 = (n * sum_xy - sum_x * sum_y) / denom;
        *a0 = y_mean - (*a1) * x_mean;
    }
    
    printf("\nCoefficients calcules:\n");
    printf("  a1 = (n*Σxy - Σx*Σy) / (n*Σx² - (Σx)²)\n");
    printf("  a1 = (%d*%.6f - %.6f*%.6f) / (%d*%.6f - %.6f²)\n", 
           n, sum_xy, sum_x, sum_y, n, sum_x2, sum_x);
    printf("  a1 = %.6f / %.6f = %.6f\n", 
           n*sum_xy - sum_x*sum_y, denom, *a1);
    printf("\n  a0 = ȳ - a1*x̄\n");
    printf("  a0 = %.6f - %.6f*%.6f = %.6f\n", 
           y_mean, *a1, x_mean, *a0);
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
void plotWithGnuplot(float **data, int n, float a0, float a1) {
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
    fprintf(gnuplot_script, "set title 'Regression lineaire par moindres carres\\n");
    fprintf(gnuplot_script, "y = %.4f + %.4f x'\n", a0, a1);
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

void displayResults(float a0, float a1, float cost) {
    printf("Resultats de la regression:\n");
    printf("  a0 = %.6f\n", a0);
    printf("  a1 = %.6f\n", a1);
    printf("  Erreur = %.6f\n", cost);
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
