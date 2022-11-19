#include <iostream>
#include <string>
#include <omp.h>
#include <vector>
#define NUM_THREADS 20
using namespace std;

struct my_pixel{
  unsigned char r, g, b;
};

void mirror(int num)
{
   //cout << "thread" << omp_get_thread_num() << endl;
   FILE *image, *outputImage, *lecturas;
   string in = "image" + to_string(num) + ".bmp";
   image = fopen(in.c_str(),"rb");          //Imagen original a transformar
   string out = "img_rot" +  to_string(num) + ".bmp";
   outputImage = fopen(out.c_str(),"wb");    //Imagen transformada
   long ancho;
   long alto;
   unsigned char r, g, b;               //Pixel

   unsigned char xx[54];
   for(int i=0; i<54; i++) {
   xx[i] = fgetc(image);
   fputc(xx[i], outputImage);   //Copia cabecera a nueva imagen
   }
   ancho = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];
   alto = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];
   // printf("largo img %li\n",alto);
   // printf("ancho img %li\n",ancho);

   unsigned char* arr_in = (unsigned char*)malloc(ancho*alto*3*sizeof(unsigned char));
   unsigned char* arr_out = (unsigned char*)malloc(ancho*alto*3*sizeof(unsigned char));

   int padding= 0;
   int j= 0;
   while(!feof(image)){
         arr_in[j] = fgetc(image);
         arr_out [j] = arr_in [j];
         j++;
      }
      padding = ancho%4;
      //printf("%d\n", j);
      int count = 0;
      for(int i = 0; i < ancho*alto*3; i+=3){
         b = arr_in [i];
         g = arr_in [i + 1];
         r = arr_in [i + 2];
         unsigned char pixel = 0.21*r+0.72*g+0.07*b;
         // arr_in  [i] = pixel;
         // arr_in  [i + 1] = pixel;
         // arr_in  [i + 2] = pixel;
         arr_in  [i] = b;
         arr_in  [i + 1] = g;
         arr_in  [i + 2] = r;
         count += 3;
         if(count == ancho*3){
               i+=padding;
               count = 0;
         }
    }
    for(int i = 0; i < alto; i++){
        for(int j = 0; j < (ancho*3); j+= 3){
            arr_out [(i*((ancho*3)+padding))+j-3] = arr_in  [(i*((ancho*3)+padding))+ (ancho*3) - j];
            arr_out [(i*((ancho*3)+padding))+j-4]  = arr_in  [(i*((ancho*3)+padding))+ (ancho*3 )- j - 1];
            arr_out [(i*((ancho*3)+padding))+j-5]  = arr_in  [(i*((ancho*3)+padding))+ (ancho*3) - j - 2];
        }
    }

   for(int i = 0; i < ancho*alto*3; i++){
      fputc(arr_out [i], outputImage);
   }

   fclose(image);
   fclose(outputImage);
   return;
}

int main(){
   //Se utiliza paralelizacion de tareas. Cada thread se encarga de cada iteración, por lo que cada thread llama a la función con 
   //una imagen diferente y la procesa completamente.
   double t1=omp_get_wtime();
   #pragma omp parallel
   #pragma omp for
   for (int i = 0; i < 2; i++){
      mirror(i+1);
   }
   double t2=omp_get_wtime();
   double tiempo=t2-t1;
   printf("tomo (%lf) segundos\n", tiempo);
   return 0;
}
