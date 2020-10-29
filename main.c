#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <time.h>

/*
 *          ==============================
 *              Diessy Soares Oppata
 *              a2098318
 *          ==============================
 *
 *
 *      O que falta:
 *          - Parametrização geral para controlar o comportamento (provavelmente por meio de argumentos).
 *          - Interface simples baseada em texto.
 *
 *      O que ja esta implementado:
 *          - Carregamento das bibliotecas "rom" e "pat"
 *          - Randomização dos parametros de escolha
 *          - Criacao de "novos" samples baseados nos existestes
 *          - Criacao dew novos padroes randomicos.
 *          - Sequencimanto
 *          - 8 Efeito de audio
 *              Efeitos:
 *                   pitch : controla o tom do sample
 *                   distortion : Amplia o som ate certo limite, causando um efeito de distorcao
 *                   lowpass : filtro que remove frequencias mais altas ou agudas
 *                   flanger : dois audio identicos interpolados com uma variação pequena de tempo, variacao controlada pela funcao cos() que altera a velocidade de reproducao de um dos audios por meio de uma sinewave.
 *                   wow : Pequena alteracao no tempo execução, dando o efeito de toca fitas antigo ou vinyl
 *                   decimator : um efeito que reduz a resolução da waveform, simulando a limitacao que sistemas antigos tinham nativamente
 *                   delay : efeito de repetir parte do som um numero x de vezes, gradativamente mais baixo, junto a um lowpass filter para dar uma sensação de delay de fita
 *                   sidechain : controle de volume que se baiseia em outro audio como parametro, no caso do projeto ele se baiseia no padrao de execucao, diminuido o volume do audio onde outro audio pode ter espaco.
 *          - Audio final em ".wav".
 *
 *
 *      O que pode ou nao existir:
 *          - highpass filter, filtro que retira frequecias baixa ou graves, baseado no cancelamento de fase entre um audio e o mesmo audio com lowpass filter. (nao sei se da certo)
 *          - Aphex Twin mode
 *          - Novas bibliotecas
 *          - Alguma coisa que me vier a cabeca
 *
*/


//======================================================================
//
//  	c o n s t a n t s
//
//======================================================================

#define ERR_FLE	"File error!\n"
#define ERR_MEM	"Memory error!\n"

#define KICK_T		0xf000
#define SNARE_T		0xf001
#define CLAP_T		0xf002
#define HAT_T		0xf003
#define RIDE_T		0xf004
#define PERC_T		0xf005

#define SAMPLE_END	0xff88

#define NUM_CATG	6

#define DEF_LIBRARY		".\\rom\\analogTechno.rom"
#define DEFPAT_LIBRARY	".\\pattern\\analogTechno.pat"

#define MAX_SAMPLES		52920
#define MAX_PATTERNS	64
#define MAX_ITENS		100
#define SAMPLE_RATE		44100
#define STR_SIZE		256

//======================================================================
//
//  	s t r u c t s
//
//======================================================================

typedef struct {
    short int * arr;
    int 		arr_long;
}raw_array;

typedef struct {
    raw_array 	collection[MAX_ITENS];
    raw_array 	patterns[MAX_ITENS];
    int 		itens_collection;
    int 		itens_patterns;
}lib;

typedef struct {
    short  			ra_hash;
    short 			ra_bpm;
    short 			ra_totalof_inter;
    int 			ra_sample_rate;
    short  			ra_blck_size;
    short 			ra_qnt_type[6];
    unsigned char 	ra_drumkit[6][10];
    unsigned char 	ra_pat_drumkit[6][10];
    char 			ra_path_pattern[128];
    char 			ra_path_rom[128];
}runnigAt;

//======================================================================
//
//  	m a c r o s
//
//======================================================================


#ifndef __LINE__
    #define LINE_CHK 0
#else
    #define LINE_CHK 1
#endif

#define NULLCHK(_mcrPNT, _mcrMSG, _mcrCALLBFR_QUIT)\
 	if(_mcrPNT == NULL) {\
	 	if(LINE_CHK)\
	 		printf("\nLine:%d\n", __LINE__ - 1);\
	 	_mcrCALLBFR_QUIT;\
	 	printf("\nError:"_mcrMSG"\n");\
	 	exit(1);\
 	}

// Numero randomico com limites
#define RANGE_RAND(_mcrMIN, _mcrMAX, _mcrRAND)\
 	(_mcrMIN + _mcrRAND % (_mcrMAX - _mcrMIN))

// booleano random
#define BOOLRAND(_mcrRAND)\
	(_mcrRAND % 2)

// booleano random
#define PERCBOOLRAND(_mcrPERC, _mcrRAND)\
	(_mcrPERC > (_mcrRAND % 100))

// Copia memoria de um vetor para outro
#define MEMCPY(_mcrDES, _mcrORG, _mcrLENG)\
	if((_mcrDES != NULL && _mcrORG != NULL) && _mcrLENG > 0) {\
		for(int i = 0;i < _mcrLENG;i++)\
				_mcrDES[i] = _mcrORG[i];\
	}

// Sempre positivo
#define ALWS_POST(_mcrMIN, _mcrVALUE)\
		((_mcrMIN - _mcrVALUE) < 0 ? -(_mcrMIN - _mcrVALUE) : (_mcrMIN - _mcrVALUE))

// Array Access Protection
#define AAP(index, buffer_length)\
	((buffer_length < 0)? 0 : index < 0? 0 : index < buffer_length? index : 0)

// Pega quantidade de samples em um beat
#define GET_SMPBEAT(_mcrBPM, _mcrSR)\
	(((float)60 / _mcrBPM) * _mcrSR)

#define NOISE(_mcrLEVEL)\
	((rand() % SHRT_MAX) * _mcrLEVEL)



//======================================================================
//
//  	W A V   W R I T E R
//
//======================================================================

/* make_wav.c
 * Creates a WAV file from an array of ints.
 * Output is monophonic, signed 16-bit samples
 * copyright
 * Fri Jun 18 16:36:23 PDT 2010 Kevin Karplus
 * Creative Commons license Attribution-NonCommercial
 *  http://creativecommons.org/licenses/by-nc/3.0/
 */

void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file) {
    unsigned buf;
    while(num_bytes>0)
    {   buf = word & 0xff;
        fwrite(&buf, 1,1, wav_file);
        num_bytes--;
        word >>= 8;
    }
}

/* information about the WAV file format from
    http://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 */

void write_wav(char * filename, unsigned long num_samples, short int * data, int s_rate)
{
    FILE * wav_file;
    unsigned int sample_rate;
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned int byte_rate;
    unsigned long i;    /* counter for samples */

    num_channels = 1;   /* monoaural */
    bytes_per_sample = 2;

    if (s_rate<=0) sample_rate = 44100;
    else sample_rate = (unsigned int) s_rate;

    byte_rate = sample_rate*num_channels*bytes_per_sample;

    wav_file = fopen(filename, "wb");
    assert(wav_file);   /* make sure it opened */

    /* write RIFF header */
    fwrite("RIFF", 1, 4, wav_file);
    write_little_endian(36 + bytes_per_sample* num_samples*num_channels, 4, wav_file);
    fwrite("WAVE", 1, 4, wav_file);

    /* write fmt  subchunk */
    fwrite("fmt ", 1, 4, wav_file);
    write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
    write_little_endian(1, 2, wav_file);    /* PCM is format 1 */
    write_little_endian(num_channels, 2, wav_file);
    write_little_endian(sample_rate, 4, wav_file);
    write_little_endian(byte_rate, 4, wav_file);
    write_little_endian(num_channels*bytes_per_sample, 2, wav_file);  /* block align */
    write_little_endian(8*bytes_per_sample, 2, wav_file);  /* bits/sample */

    /* write data subchunk */
    fwrite("data", 1, 4, wav_file);
    write_little_endian(bytes_per_sample* num_samples*num_channels, 4, wav_file);
    for (i=0; i< num_samples; i++)
    {   write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
    }

    fclose(wav_file);
}

//======================================================================
//
//  	f u n c t i o n s | d s p
//
//======================================================================


short amp(short value, float time) {
    return (short) value * time;
}

void pitch(short * buffer, int size, float pitch){
    short * buffer_cpy = malloc(sizeof(short) * size);
    //CHKPNT(buffer_cpy, "175", NULL);

    MEMCPY(buffer_cpy, buffer, size);
    float sample = 0;
    for(int i = 0;i < size;i++)
    {
        if((int)sample > size)
            break;
        buffer[i] = (buffer_cpy[(size_t)sample]);
        sample += pitch;
    }
}

/*void saturation(short * buffer, int buffer_length, unsigned char amount){
    for(int i = 0;i < buffer_length;i++) {
        buffer[i] = (short) atan(buffer[i] * (amount * 0.0001)) * 15000 ;
    }
}*/

void flanger(short * temp, int size, int time, int amount){
    short * cpy = malloc(sizeof(short) * size);
    NULLCHK(cpy, ERR_MEM, free(temp));

    MEMCPY(cpy, temp, size);

    float sample = 0;

    for(int i = 0;i < size;i++){
        temp[i] += cpy[(size_t)sample] / 4;
        sample += 1.0 + (cos(i / time) / amount);
        temp[i] += cpy[i] / 4;
        temp[i] += cpy[(size_t)sample] / 4;
    }
}

void wow(short * temp, int size, int amount){
    short * cpy = malloc(sizeof(short) * size);
    //CHKPNT(cpy, "204", NULL);

    MEMCPY(cpy, temp, size);
    float sample = 0;

    for(int i = 0;i < size;i++) {
        temp[i] = (cpy[(size_t)sample]);
        sample += 1.0 + cos(i / 2000 / amount);
    }
}

void lowpass(short * buffer, int buffer_length, int filter_amount, int pases){
    int holder = 0;
    for(int k = 0;k < pases;k++) {
        for(int i = 1;i < buffer_length - filter_amount;i++) {
            for(int j = 0;j < filter_amount;j++)
                holder += buffer[(i + j) - 1];
            buffer[i] = (holder / filter_amount);
            holder = 0;
        }
    }
}

void decimator(short * buffer, int buffer_length, int filter_amount){
    short holder = buffer[0];
    for(int i = 1;i < buffer_length;i++) {
        if(i % filter_amount == 0)
            holder = buffer[i];
        buffer[i] =  holder;
    }
}

void distortion(short * buffer, int buffer_length, short treshold){
    for(int i = 0;i < buffer_length;i++) {
        buffer[i] = buffer[i] > treshold? treshold: buffer[i] < -treshold? -treshold:buffer[i];
    }
}

void delay(runnigAt * cfg, short * buffer, int buffer_length, short voices, int beat){
    short * voice = calloc(sizeof(short), buffer_length), decay = 0;
    //CHKPNT(voice, "241", NULL);

    beat = (int)(GET_SMPBEAT(cfg->ra_bpm, cfg->ra_sample_rate) / 32) * beat;
    MEMCPY(voice, buffer, buffer_length);
    for(int i = 0;i < voices;i++) {
        lowpass(voice, buffer_length, 3 * (i * i), (i * i));
        for(int j = beat;j < buffer_length;j++)
            buffer[j] += (short) amp(voice[AAP((j - beat), buffer_length)], 1 - (decay * 0.1));
        decay++;
        beat *= 2;
    }
}

void sidechain(short * audio, int steps[], int samples_long){
    const double base = 1 / (samples_long / 2);
    double control = 0.1;

    for(int i = 0;i < 64;i += 8) {
        for(int j = 0;j < samples_long;j++, control += base){
            if(control > 1 || steps[i] + j > samples_long)
                break;
            audio[steps[i] + j] = amp(audio[steps[i] + j], control);
        }
    }
}

//======================================================================
//
//  	f u n c t i o n s
//
//======================================================================

lib * init_lib() {
    lib * library = calloc(sizeof(lib), NUM_CATG);
    NULLCHK(library, ERR_MEM, NULL);

    for(int i = 0; i < NUM_CATG; i++) {
        library[i].itens_collection = 0;
        library[i].itens_patterns = 0;
        
        for(int j = 0;j < MAX_ITENS;j++) {
        	library[i].collection[j].arr = NULL;
        	library[i].collection[j].arr_long = 0;
        	library[i].patterns[j].arr = NULL;
        	library[i].patterns[j].arr_long = 0;
        }
        
    }

    return library;
}

runnigAt  * init_cfg(){
    runnigAt * cfg = calloc(sizeof(runnigAt), 1);
    NULLCHK(cfg, ERR_MEM, NULL);

    srand(time(NULL));
    cfg->ra_hash = rand() % INT_MAX;
    srand(cfg->ra_hash);

    return cfg;
}

void load(lib * library, char load_type){

    FILE * rom = fopen(load_type == 'r'? DEF_LIBRARY : DEFPAT_LIBRARY, "r");
    NULLCHK(rom, ERR_FLE, free(library));

    int * buffer = calloc(sizeof(int), (load_type == 'r'? MAX_SAMPLES : MAX_PATTERNS));
    NULLCHK(buffer, ERR_MEM, free(library);fclose(rom));

    int audtp_indx = 0, smp_bufsize = 0, value = 0;

    while(fscanf(rom, "%d,", &value) == 1){

        if (value != SAMPLE_END) {

            if (value >= KICK_T && value <= PERC_T)
                audtp_indx = ALWS_POST(KICK_T, value);
            else {
                switch(load_type) {
                    case 'r':
                        if(value > SHRT_MIN && value < SHRT_MAX && smp_bufsize < MAX_SAMPLES)
                            buffer[smp_bufsize++] = value;
                        break;

                    case 'p':
                        if(value > -1 && value < MAX_PATTERNS && smp_bufsize < MAX_PATTERNS)
                            buffer[smp_bufsize++] = value;
                        break;
                }
            }

        } else {
            
            if(smp_bufsize > 0 && load_type == 'r'? (smp_bufsize < MAX_SAMPLES) : (smp_bufsize < MAX_PATTERNS)){
                switch(load_type) {
                    case 'r':

                        library[audtp_indx].collection[library[audtp_indx].itens_collection].arr = calloc(sizeof(short int), smp_bufsize);
                        NULLCHK(library[audtp_indx].collection[library[audtp_indx].itens_collection].arr, ERR_MEM, free(library);fclose(rom);free(buffer));

                        MEMCPY(library[audtp_indx].collection[library[audtp_indx].itens_collection].arr, buffer, smp_bufsize);
                        library[audtp_indx].collection[library[audtp_indx].itens_collection++].arr_long = smp_bufsize;
                        

                    break;
                    case 'p':

                        library[audtp_indx].patterns[library[audtp_indx].itens_patterns].arr = calloc(sizeof(short int), smp_bufsize);
                        NULLCHK(library[audtp_indx].patterns[library[audtp_indx].itens_patterns].arr, ERR_MEM, free(library);fclose(rom);free(buffer));

                        MEMCPY(library[audtp_indx].patterns[library[audtp_indx].itens_patterns].arr, buffer, smp_bufsize);
                        library[audtp_indx].patterns[library[audtp_indx].itens_patterns++].arr_long = smp_bufsize;

                    break;
                }
            }
             
            smp_bufsize = 0;
        }
    }
}

bool sample_sequencer(lib * library, short sample_id, short pattern_id, short * buffer, int buffer_length, float volume, int steps[]) {
    int watch = 0;

    if(library != NULL && library->itens_patterns > 0 && library->itens_collection > 0) {
        if (pattern_id <= library->itens_patterns  && sample_id <= library->itens_collection){

            for (int i = 0; i < library->patterns[pattern_id].arr_long; i++) {

                for (int j = 0; j < library->collection[sample_id].arr_long; j++) {

                    watch = steps[library->patterns[pattern_id].arr[i]] + j;

                    
                    if (watch < buffer_length)
                        buffer[watch] += amp(library->collection[sample_id].arr[j], volume);

                }
            }
        }
    }
    return false;
}

void initWaveXGroove(runnigAt * cfg, lib *library) {
    cfg->ra_totalof_inter = 0;
    cfg->ra_bpm 		= RANGE_RAND(110, 130, rand());
    cfg->ra_blck_size 	= 8;
    cfg->ra_sample_rate = SAMPLE_RATE;
    cfg->ra_qnt_type[0] = 1;
    cfg->ra_qnt_type[1] = rand() % 2;
    cfg->ra_qnt_type[2] = rand() % 2;
    cfg->ra_qnt_type[3] = rand() % 3;
    cfg->ra_qnt_type[4] = rand() % 2;
    cfg->ra_qnt_type[5] = rand() % 4;

    for(int i = 0;i < 6;i++){
        if(cfg->ra_qnt_type[i] > 0) {
            if (library[i].itens_collection > 0){
                cfg->ra_totalof_inter += cfg->ra_qnt_type[i];
                for (int j = 0; j < cfg->ra_qnt_type[i]; j++) {
                    cfg->ra_drumkit[i][j] = rand() % library[i].itens_collection;
                    cfg->ra_pat_drumkit[i][j] = rand() % library[i].itens_patterns;
                }
            }
        }
    }
}

#define volume 0.25

bool render(runnigAt * cfg, lib * library){
    int steps[64], sum = 0, buffer_length = GET_SMPBEAT(cfg->ra_bpm, 44100) * cfg->ra_blck_size;

    short * buffer = calloc(sizeof(short), buffer_length);
    NULLCHK(buffer, ERR_MEM, free(library));

    int part = (buffer_length / 64);

    for(int i = 0;i < 64;i++) {
        steps[i] = sum;
        sum += part;
    }

    for (int i = NUM_CATG - 1; i > -1; i--) {
        for(int j = 0;j < cfg->ra_qnt_type[i];j++){
            sample_sequencer(&library[i], cfg->ra_drumkit[i][j], cfg->ra_pat_drumkit[i][j], buffer, buffer_length, volume, steps);
        }
    }

    char name[STR_SIZE];

    sprintf(name, ".\\output\\%d.wav", cfg->ra_hash);

    write_wav(name, buffer_length , buffer, 44100);

    return true;
}



void insert_rand_morph_sample(lib * library, int qnt){
    short candidate_index[2];
    int buffer_length;
    short * buffer;

    for(int ctg = 1;ctg < NUM_CATG;ctg++) {

        if(library[ctg].itens_collection > 0) {

            for (int j = 0; j < qnt; j++) {

                candidate_index[0] = (rand() % library[ctg].itens_collection);
                candidate_index[1] = (rand() % library[ctg].itens_collection);

                buffer_length = library[ctg].collection[candidate_index[0]].arr_long <
                                library[ctg].collection[candidate_index[1]].arr_long
                                ? library[ctg].collection[candidate_index[0]].arr_long
                                : library[ctg].collection[candidate_index[1]].arr_long;

                if (buffer_length > 0) {
                    buffer = calloc(sizeof(short), buffer_length);
                    NULLCHK(buffer, ERR_MEM, free(library));

                    for (int i = 0; i < buffer_length; i++)
                        buffer[i] = sin(library[ctg].collection[candidate_index[1]].arr[i] * 0.000008) * 12000;

                    pitch(buffer, buffer_length, RANGE_RAND(8, 12, rand()) * 0.1);

                    for (int i = 0; i < buffer_length; i++)
                        buffer[i] += library[ctg].collection[candidate_index[0]].arr[i] / 2 + NOISE(0.002);

                    library[ctg].collection[library[ctg].itens_collection].arr = calloc(sizeof(short), buffer_length);
                    NULLCHK(library[ctg].collection[library[ctg].itens_collection].arr, ERR_MEM,
                            free(library);free(buffer));

                    MEMCPY(library[ctg].collection[library[ctg].itens_collection].arr, buffer, buffer_length);
                    library[ctg].collection[library[ctg].itens_collection++].arr_long = buffer_length;

                    buffer_length = 0;
                    free(buffer);
                }
            }
        }
    }
}

void insert_rand_pattern(lib * library, int qnt){
    short buffer[MAX_PATTERNS];
    short buffer_length = 0;

    for(int ctg = 1;ctg < NUM_CATG;ctg++) {

        for(int j = 0;j < qnt;j++) {

            for(int i = 0 ;i < MAX_PATTERNS;i++){
                if(i % 2 == 0  && PERCBOOLRAND(25, rand()))
                    buffer[buffer_length++] = i;
            }

            if(buffer_length > 0) {

                library[ctg].patterns[library[ctg].itens_patterns].arr = calloc(sizeof(short), buffer_length);
                NULLCHK(library[ctg].patterns[library[ctg].itens_patterns].arr, ERR_MEM, free(library));

                MEMCPY(library[ctg].patterns[library[ctg].itens_patterns].arr, buffer, buffer_length);
                library[ctg].patterns[library[ctg].itens_patterns++].arr_long = buffer_length;
            }

            buffer_length = 0;
        }
    }
}

int main() {
    lib *library = init_lib();
    NULLCHK(library, ERR_MEM, NULL);

    // Carregando SamplePoints
    load(library, 'r');

    // Carregando Padroes
    load(library, 'p');

    runnigAt *cfg = init_cfg();
    NULLCHK(cfg, ERR_MEM, free(library));

    insert_rand_pattern(library, 5);

    insert_rand_morph_sample(library, 20);
    
    initWaveXGroove(cfg, library);

    render(cfg, library);
    
    free(library);
    free(cfg);
}