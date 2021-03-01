#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp_header.h"

/* am folosit Visual Studio Code unde tab-ul ocupă 4 spații (deschiderea cu
alte editoare de text poate duce la un număr mai mare de caractere pe linie) */

void open_files(char *input_file_name, char *input_task3_name,
char *input_task4_name, char *input_task5_name)
{
    FILE *input = fopen("input.txt", "rt");
    fscanf(input, "%s", input_file_name);
    fscanf(input, "%s", input_task3_name);
    fscanf(input, "%s", input_task4_name);
    fscanf(input, "%s", input_task5_name);
    fclose(input);
    // citim căile pentru toate fișierele folosite.
}

void allocImage(bmp_file **image)
{
    // alocarea și eliberarea imaginii în caz de eroare de alocare.
    (*image)->bitmap = malloc((*image)->info.height * sizeof(unsigned char *));
    if((*image)->bitmap == NULL)
    {
        fprintf(stderr, "ERROR!");
        free(image);
        return;      
    }
    int i, j;
    for(i = 0; i < (*image)->info.height; ++i)
    {
        (*image)->bitmap[i] = malloc((*image)->info.width *
        sizeof(unsigned char *));
        if((*image)->bitmap[i] == NULL)
        {
            for(j = 0; j < i; ++j)
                free((*image)->bitmap[j]);
            free((*image)->bitmap);
            free(*image);
            fprintf(stderr, "ERROR!");
            return;
        }
    }
    for(i = 0; i < (*image)->info.height; ++i)
    {
        for(j = 0; j < (*image)->info.width; ++j)
        {
            (*image)->bitmap[i][j] = malloc(NO_COLORS * sizeof(unsigned char));
            if((*image)->bitmap[i][j] == NULL)
            {
                fprintf(stderr, "ERROR!");
                int ii, jj;
                for(ii = 0; ii < i - 1; ++ii)
                {
                    for(jj = 0; jj < (*image)->info.width; ++jj)
                        free((*image)->bitmap[ii][jj]);
                }
                for(jj = 0; jj < j; ++jj)
                    free((*image)->bitmap[i][jj]);
                for(ii = 0; ii < i; ++ii)
                    free((*image)->bitmap[ii]);
                free((*image)->bitmap);
                free(*image);
                return;
            }
        }
    }
}

void freeImage(bmp_file **image)
{
    // eliberarea imaginii
    int i, j;
    for(i = 0; i < (*image)->info.height; ++i)
        for(j = 0; j < (*image)->info.width; ++j)
            free((*image)->bitmap[i][j]);
    for(i = 0; i < (*image)->info.height; ++i)
        free((*image)->bitmap[i]);
    free((*image)->bitmap);
    free(*image);
}

void read_image(bmp_file **image, char *input_file_name)
{
    // alocare folosind funcția allocImage și citirea imaginii
    FILE * input_file = fopen(input_file_name, "rb");
    *image = malloc(sizeof(bmp_file));
    if((*image) == NULL)
    {
        fprintf(stderr, "ERROR!");
        return;
    }
    fread(&(*image)->file, sizeof(bmp_fileheader), 1, input_file);
    fread(&(*image)->info, sizeof(bmp_infoheader), 1, input_file);
    allocImage(image);
    int i, j;
    for(i = 0; i < (*image)->info.height; ++i){
        for(j = 0; j < (*image)->info.width; ++j)
            fread((*image)->bitmap[i][j], sizeof(unsigned char), NO_COLORS,
            input_file);
        fseek(input_file, (*image)->info.width % 4, SEEK_CUR);
        // fseek folosit pentru a nu citi paddingul
    }
    fclose(input_file);
}

void print_image(bmp_file *image, char *input_file_name, int test_no)
{
    // afișarea și eliberarea imaginii (dezalocare cu funcția freeImage)
    char *output_file_name = malloc(NAME_FILE_LENGHT * sizeof(char));
    strcpy(output_file_name, input_file_name);
    output_file_name[5] = '\0';
    switch(test_no) // redenumim fișierul pentru afișare în funcție de task
    {
        case 1:
            strcat(output_file_name, "_black_white.bmp");
            break;
        case 2: 
            strcat(output_file_name, "_nocrop.bmp");
            break;
        case 3:
            strcat(output_file_name, "_filter.bmp");
            break;
        case 4:
            strcat(output_file_name, "_pooling.bmp");
            break;
        case 5:
            strcat(output_file_name, "_clustered.bmp");
    }
    FILE *output_file = fopen(output_file_name, "wb");
    fwrite(&image->file, sizeof(bmp_fileheader), 1, output_file);
    fwrite(&image->info, sizeof(bmp_infoheader), 1, output_file);
    int i, j;
    unsigned int pad = 0;
    for(i = 0; i < image->info.height; ++i){
        for(j = 0; j < image->info.width; ++j)
        {
            fwrite(&image->bitmap[i][j][0], sizeof(unsigned char), 1,
            output_file);
            fwrite(&image->bitmap[i][j][1], sizeof(unsigned char), 1,
            output_file);
            fwrite(&image->bitmap[i][j][2], sizeof(unsigned char), 1,
            output_file);
        }
        fwrite(&pad, sizeof(unsigned char), image->info.width % 4,
        output_file); // afișăm și paddingul care nu a fost citit inițial
    }
    fclose(output_file);
    free(output_file_name);
}

void copy(bmp_file **image, bmp_file *new_image)
{
    /* copiază toate informațiile (mai puțin bitmap-ul) de la imaginea inițială
    în imaginea nouă */
    new_image->file.bfSize = (*image)->file.bfSize;
    new_image->file.fileMarker1 = (*image)->file.fileMarker1;
    new_image->file.fileMarker2 = (*image)->file.fileMarker2;
    new_image->file.imageDataOffset = (*image)->file.imageDataOffset;
    new_image->file.unused1 = (*image)->file.unused1;
    new_image->file.unused2 = (*image)->file.unused2;
    new_image->info.biClrImportant = (*image)->info.biClrImportant;
    new_image->info.biClrUsed = (*image)->info.biClrUsed;
    new_image->info.biCompression = (*image)->info.biCompression;
    new_image->info.biSize = (*image)->info.biSize;
    new_image->info.biSizeImage = (*image)->info.biSizeImage;
    new_image->info.bitPix = (*image)->info.bitPix;
    new_image->info.biXPelsPerMeter = (*image)->info.biXPelsPerMeter;
    new_image->info.biYPelsPerMeter = (*image)->info.biYPelsPerMeter;
    new_image->info.height = (*image)->info.height;
    new_image->info.planes = (*image)->info.planes;
    new_image->info.width = (*image)->info.width;
}

void Black_White(bmp_file **image, char *input_file_name) // TASK 1
{
    bmp_file *image_bw = malloc(sizeof(bmp_file));
    if(image_bw == NULL)
    {
        fprintf(stderr, "ERROR!");
        return;
    }
    copy(image, image_bw);
    allocImage(&image_bw);
    int i, j, k;
    for(i = 0; i < image_bw->info.height; ++i)
        for(j = 0; j < image_bw->info.width; ++j) // folosește formula
        {
            unsigned int x;
            x = (unsigned int) ((*image)->bitmap[i][j][0] +
            (*image)->bitmap[i][j][1]);
            x += (unsigned int) (*image)->bitmap[i][j][2];
            x /= 3;
            image_bw->bitmap[i][j][0] = (unsigned char) x;
            image_bw->bitmap[i][j][1] = (unsigned char) x;
            image_bw->bitmap[i][j][2] = (unsigned char) x;
        }
    print_image(image_bw, input_file_name, 1);
    freeImage(&image_bw);
}

void No_Crop(bmp_file **image, char *input_file_name) // TASK 2
{
    bmp_file *image_nc = malloc(sizeof(bmp_file));
    if(image_nc == NULL)
    {
        fprintf(stderr, "ERROR!");
        return;
    }
    copy(image, image_nc);
    int dif;
    if(image_nc->info.height > image_nc->info.width)
    {
        dif = image_nc->info.height - image_nc->info.width;
        image_nc->info.width = image_nc->info.height;
    }
    else
    {
        dif = image_nc->info.width - image_nc->info.height;
        image_nc->info.height = image_nc->info.width;
    }    
    allocImage(&image_nc);
    int i, j, k;
    if((*image)->info.width > (*image)->info.height)
    {        
        if(dif % 2 == 0) // țin cont de faptul că imaginea este răsturnată
        {
            for(i = 0; i < dif / 2; ++i){
                for(j = 0; j < image_nc->info.width; ++j)
                    for(k = 0; k < NO_COLORS; ++k)
                        image_nc->bitmap[i][j][k] = 255;
            }
            for(i = dif / 2; i < (*image)->info.height + dif / 2; ++i){
                for(j = 0; j < image_nc->info.width; ++j)
                    for(k = 0; k < NO_COLORS; ++k)
                        image_nc->bitmap[i][j][k] =
                        (*image)->bitmap[i - dif / 2][j][k];
            }
        }
        else
        {
            for(i = 0; i < dif / 2 + 1; ++i){
                for(j = 0; j < image_nc->info.width; ++j)
                    for(k = 0; k < NO_COLORS; ++k)
                        image_nc->bitmap[i][j][k] = 255;
            }
            for(i = dif / 2 + 1; i < (*image)->info.height + dif / 2 + 1; ++i){
                for(j = 0; j < image_nc->info.width; ++j)
                    for(k = 0; k < NO_COLORS; ++k)
                        image_nc->bitmap[i][j][k] =
                        (*image)->bitmap[i - dif / 2 - 1][j][k];
            }
        }
        for(i = image_nc->info.height - dif / 2; i < image_nc->info.height;
        ++i)
            for(j = 0; j < image_nc->info.width; ++j)
                for(k = 0; k < NO_COLORS; ++k)
                    image_nc->bitmap[i][j][k] = 255;
    }
    else
    {
        if(dif % 2 == 0)
        {
            for(i = 0; i < image_nc->info.height; ++i){
                for(j = 0; j < dif / 2; ++j)
                        for(k = 0; k < NO_COLORS; ++k)
                            image_nc->bitmap[i][j][k] = 255;
            }
            for(i = 0; i < image_nc->info.height; ++i){
                for(j = dif / 2; j < (*image)->info.width + dif / 2; ++j)
                    for(k = 0; k < NO_COLORS; ++k)
                        image_nc->bitmap[i][j][k] = 
                        (*image)->bitmap[i][j - dif / 2][k];
            }
        }
        else
        {
           for(i = 0; i < image_nc->info.height; ++i){
                for(j = 0; j < dif / 2 + 1; ++j)
                        for(k = 0; k < NO_COLORS; ++k)
                            image_nc->bitmap[i][j][k] = 255;
            }            
            for(i = 0; i < image_nc->info.height; ++i){
                for(j = dif / 2 + 1; j < (*image)->info.width + dif / 2 + 1;
                ++j)
                    for(k = 0; k < NO_COLORS; ++k)
                        image_nc->bitmap[i][j][k] = 
                        (*image)->bitmap[i][j - dif / 2 - 1][k];
            }
        }
        for(i = 0; i < image_nc->info.height; ++i)
            for(j = image_nc->info.width - dif / 2;
            j < image_nc->info.width; ++j)
                for(k = 0; k < NO_COLORS; ++k)
                    image_nc->bitmap[i][j][k] = 255;
    }
    print_image(image_nc, input_file_name, 2);
    freeImage(&image_nc);
}

int apply_filter(int i, int j, int k, bmp_file **image, int n,
int **matrix_filter) // funcție care aplică efectiv filtrul după formulă
{
    int sum, ii, jj;
    sum = 0;
    for(ii = 0; ii < n; ++ii)
    {
        if(i - n/2 + ii >= 0 && i - n/2 + ii < (*image)->info.height)
            for(jj = 0; jj < n; ++jj)
            {
                if(j - n/2 + jj >= 0 && j - n/2 + jj < (*image)->info.width)
                    sum += (*image)->bitmap[i - n/2 + ii][j - n/2 +jj][k]
                    * matrix_filter[ii][jj];
            }
    }
    return sum;
}

void Filter(bmp_file **image, char *input_file_name, char *input_task3_name)
// TASK 3
{
    bmp_file *image_f = malloc(sizeof(bmp_file));
    if(image_f == NULL)
    {
        fprintf(stderr, "ERROR!");
        return;
    }
    copy(image, image_f);
    allocImage(&image_f);
    int n, i, j, **matrix_filter;
    FILE *task3_input = fopen(input_task3_name, "rt");
    fscanf(task3_input, "%d", &n);
    matrix_filter = malloc(n * sizeof(int *));
    if(matrix_filter == NULL)
    {
        fprintf(stderr, "ERROR!");
        return;
    }
    for(i = 0; i < n; ++i)
    {
        matrix_filter[i] = malloc(n * sizeof(int));
        if(matrix_filter[i] == NULL)
        {
            fprintf(stderr, "ERROR!");
            for(j = 0; j < i; ++j)
                free(matrix_filter[j]);
            free(matrix_filter);
            return;
        }
    }
    for(i = 0; i < n; ++i)
        for(j = 0; j < n; ++j)
            fscanf(task3_input, "%d", &matrix_filter[n - i - 1][j]);
    fclose(task3_input);
    for(i = 0; i < (*image)->info.height; ++i)
        for(j = 0; j < (*image)->info.width; ++j) /* aplicăm filtrul cu funcția
        ajutătoare */
        {
            if(apply_filter(i, j, 0, image, n, matrix_filter) > 255) // blue
                image_f->bitmap[i][j][0] = 255;
            else
            {
                if(apply_filter(i, j, 0, image, n, matrix_filter) < 0)
                    image_f->bitmap[i][j][0] = 0;
                else
                    image_f->bitmap[i][j][0] = apply_filter(i, j, 0, image, n,
                    matrix_filter);
                
            }
            if(apply_filter(i, j, 1, image, n, matrix_filter) > 255) // green
                image_f->bitmap[i][j][1] = 255;
            else
            {
                if(apply_filter(i, j, 1, image, n, matrix_filter) < 0)
                    image_f->bitmap[i][j][1] = 0;
                else
                    image_f->bitmap[i][j][1] = apply_filter(i, j, 1, image, n,
                    matrix_filter);
                
            }
            if(apply_filter(i, j, 2, image, n, matrix_filter) > 255) // red
                image_f->bitmap[i][j][2] = 255;
            else
            {
                if(apply_filter(i, j, 2, image, n, matrix_filter) < 0)
                    image_f->bitmap[i][j][2] = 0;
                else
                    image_f->bitmap[i][j][2] = apply_filter(i, j, 2, image, n,
                    matrix_filter);
                
            }
        }
    for(i = 0; i < n; ++i)
        free(matrix_filter[i]);
    free(matrix_filter);
    print_image(image_f, input_file_name, 3);
    freeImage(&image_f);
}

unsigned char apply_pooling(int i, int j, int k, bmp_file **image, int n,
char mode) // funcție care aplică efectiv filtrul de pooling după formulă
{
    int ii, jj;
    unsigned char color;
    if(mode == 'm')
        color = 255;
    else
        color = 0;    
    for(ii = 0; ii < n; ++ii)
        for(jj = 0; jj < n; ++jj)
        {
            if(i - n/2 + ii >= 0 && i - n/2 + ii < (*image)->info.height)
            {
                if(j - n/2 + jj >= 0 && j - n/2 + jj < (*image)->info.width)
                {
                    if(mode == 'm' && color >
                    (*image)->bitmap[i - n/2 + ii][j - n/2 + jj][k])
                        color = (*image)->bitmap[i - n/2 +ii][j - n/2 +jj][k];
                    if(mode == 'M' && color <
                    (*image)->bitmap[i - n/2 + ii][j - n/2 + jj][k])
                        color = (*image)->bitmap[i - n/2 +ii][j - n/2 +jj][k];
                }
                else
                    if(mode == 'm')
                        color = 0;
            }
            else
                if(mode == 'm')
                    color = 0;            
        }
    return color;
}

void Pooling(bmp_file **image, char *input_file_name, char *input_task4_name)
// TASK 4
{
    bmp_file *image_p = malloc(sizeof(bmp_file));
    if(image_p == NULL)
    {
        fprintf(stderr, "ERROR!");
        return;
    }
    copy(image, image_p);
    allocImage(&image_p);
    FILE *input_task4 = fopen(input_task4_name, "rt");
    int n, i, j, k;
    char mode;
    fscanf(input_task4, "%c", &mode); // m sau M
    fscanf(input_task4, "%d", &n); // numărul de linii/coloane
    fclose(input_task4);
    for(i = 0; i < (*image)->info.height; ++i)
        for(j = 0; j < (*image)->info.width; ++j)
            for(k = 0; k < NO_COLORS; ++k)
                image_p->bitmap[i][j][k] = apply_pooling(i, j, k, image, n,
                mode);
    print_image(image_p, input_file_name, 4);
    freeImage(&image_p);
}

pixel find_color_island(int i, int j, int *island_no,
bmp_file **image, unsigned int threshold, int **verify_bitmap)
{
    /* fill iterativ (folosind coadă). Funcție folosită pentru a determina o
    insulă de culoare precum și culoarea ei */
    int u = 0, k;
    unsigned int xb, xg, xr;
    unsigned int n = 1;
    int *dx = malloc(NO_DIRECTIONS * sizeof(int));
    int *dy = malloc(NO_DIRECTIONS * sizeof(int));
    dx[0] = 1, dx[1] = 0, dx[2] = -1, dx[3] = 0;
    dy[0] = 0, dy[1] = 1, dy[2] = 0, dy[3] = -1;
    (*island_no) += 1;
    verify_bitmap[i][j] = *island_no; /* va reține indicele insulei din care
    face parte acel pixel */
    pixel bgr; // valorea întoarsă este reținută aici
    bgr.r = (*image)->bitmap[i][j][2], bgr.g = (*image)->bitmap[i][j][1];
    bgr.b = (*image)->bitmap[i][j][0];
    location *coada = malloc(sizeof(location)); /* coada va fi realocată cu unu
    în plus de fiecare dată când găsim o nouă poziție pe care să o adăugăm și
    va fi realocată cu unu în minus de fiecare dată când am terminat de
    verificat vecinii pixelului de pe poziția coada[0].x, coada[0].y (acesta
    urmând să fie șters din coadă) */
    coada[u].x = i, coada[u].y = j;
    while(u >= 0){
        for(k = 0; k < NO_DIRECTIONS; ++k){
            if(coada[0].x + dx[k] >= 0 && coada[0].x + dx[k] <
            (*image)->info.height && coada[0].y + dy[k] >= 0 &&
            coada[0].y + dy[k] < (*image)->info.width){
                xb = abs((*image)->bitmap[i][j][0] -
                (*image)->bitmap[coada[0].x + dx[k]][coada[0].y + dy[k]][0]);
                xg = abs((*image)->bitmap[i][j][1] -
                (*image)->bitmap[coada[0].x + dx[k]][coada[0].y + dy[k]][1]);
                xr = abs((*image)->bitmap[i][j][2] -
                (*image)->bitmap[coada[0].x + dx[k]][coada[0].y + dy[k]][2]);
                if(xb + xr + xg <= threshold &&
                verify_bitmap[coada[0].x + dx[k]][coada[0].y + dy[k]] == 0){
                    u++;
                    coada = realloc(coada, (u + 1) * sizeof(location));
                    coada[u].x = coada[0].x + dx[k];
                    coada[u].y = coada[0].y + dy[k]; // adăugare
                    bgr.b +=
                    (*image)->bitmap[coada[u].x][coada[u].y][0];
                    bgr.g +=
                    (*image)->bitmap[coada[u].x][coada[u].y][1];
                    bgr.r +=
                    (*image)->bitmap[coada[u].x][coada[u].y][2];
                    n++;
                    verify_bitmap[coada[u].x][coada[u].y] = *island_no;
                }
            }
        }
        for(k = 1; k < u + 1; ++k){
            coada[k - 1].x = coada[k].x;
            coada[k - 1].y = coada[k].y;
        } 
        coada = realloc(coada, u * sizeof(location));
        --u; // scoatere
    }
    free(coada);
    free(dx);
    free(dy);
    bgr.r /= n, bgr.g /= n;
    bgr.b /= n;
    return bgr;
}

void Clustering(bmp_file **image, char *input_file_name,
char *input_task5_name) // TASK 5
{
    FILE *input_task5 = fopen(input_task5_name, "rt");
    unsigned int threshold;
    fscanf(input_task5, "%u", &threshold);
    fclose(input_task5);
    if(threshold == 0){
        print_image(*image, input_file_name, 5);
        return;
    } /* dacă threshold-ul este 0 înseamnă, conform formulei, că toate modulele
    vor fi egale cu 0 (sunt numere pozitive) ceea ce înseamnă că pixelii sunt
    egali deci imaginea nu se va schimba cu nimic și, prin urmare, este mai
    simplu să afișăm direct imaginea */
    bmp_file *image_c = malloc(sizeof(bmp_file));
    if(image_c == NULL){
        fprintf(stderr, "ERROR!");
        return;
    }
    copy(image, image_c);
    allocImage(&image_c);
    int i, j, k, island_no = 0;
    pixel *bgr = malloc(sizeof(pixel)); /* va reține pentru fiecare indice
    pixelul corespunzător */
    int **verify_bitmap = malloc(sizeof(unsigned int *) *
    image_c->info.height);
    if(verify_bitmap == NULL){
        fprintf(stderr, "ERROR!");
        return;
    }
    for(i = 0; i < image_c->info.height; ++i){
        verify_bitmap[i] = calloc(image_c->info.width, sizeof(unsigned int));
        if(verify_bitmap[i] == NULL){
        fprintf(stderr, "ERROR!");
        for(j = 0; j < i; ++j)
            free(verify_bitmap[j]);
        free(verify_bitmap);
        return;
        }
    }
    for(i = image_c->info.height - 1; i >= 0; --i) // apicăm algoritmul
        for(j = 0; j < image_c->info.width; ++j)
            if(verify_bitmap[i][j] == 0){
                bgr = realloc(bgr, (island_no + 2) * sizeof(pixel));
                bgr[island_no + 1] = find_color_island(i, j, &island_no, image,
                threshold, verify_bitmap);
            }
    for(i = 0; i < image_c->info.height; ++i) // înlocuim în noua imagine
        for(j = 0; j < image_c->info.width; ++j){
            image_c->bitmap[i][j][0] = (unsigned char)
            bgr[verify_bitmap[i][j]].b;
            image_c->bitmap[i][j][1] = (unsigned char)
            bgr[verify_bitmap[i][j]].g;
            image_c->bitmap[i][j][2] = (unsigned char)
            bgr[verify_bitmap[i][j]].r;
        }
    free(bgr);
    for(i = 0; i < image_c->info.height; ++i)
        free(verify_bitmap[i]);
    free(verify_bitmap);
    print_image(image_c, input_file_name, 5);
    freeImage(&image_c);
}

int main()
{
    char *input_file_name = malloc(NAME_FILE_LENGHT * sizeof(char));
    char *input_task3_name = malloc(NAME_FILE_LENGHT * sizeof(char));
    char *input_task4_name = malloc(NAME_FILE_LENGHT * sizeof(char));
    char *input_task5_name = malloc(NAME_FILE_LENGHT * sizeof(char));
    open_files(input_file_name, input_task3_name, input_task4_name,
    input_task5_name);
    bmp_file *image;
    read_image(&image, input_file_name);

    Black_White(&image, input_file_name);

    No_Crop(&image, input_file_name);

    Filter(&image, input_file_name, input_task3_name);
    free(input_task3_name);

    Pooling(&image, input_file_name, input_task4_name);
    free(input_task4_name);

    Clustering(&image, input_file_name, input_task5_name);
    free(input_task5_name);

    free(input_file_name);
    freeImage(&image);
    return 0;
}