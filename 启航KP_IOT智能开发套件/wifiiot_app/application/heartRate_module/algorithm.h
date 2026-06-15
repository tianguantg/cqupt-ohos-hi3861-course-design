#ifndef ALGORITHM_H_
#define ALGORITHM_H_

//#include "mbed.h"

#define true 1
#define false 0
#define FS 100
#define BUFFER_SIZE  (FS* 5) 
#define HR_FIFO_SIZE 7
#define MA4_SIZE  4 // DO NOT CHANGE
#define HAMMING_SIZE  5// DO NOT CHANGE
#define min(x,y) ((x) < (y) ? (x) : (y))


static  int an_dx[ BUFFER_SIZE-MA4_SIZE]; // delta
static  int an_x[ BUFFER_SIZE]; //ir
static  int an_y[ BUFFER_SIZE]; //red

void maxim_heart_rate_and_oxygen_saturation(int *pun_ir_buffer,int n_ir_buffer_length,int *pun_red_buffer,int *pn_spo2, char *pch_spo2_valid,int *pn_heart_rate,char *pch_hr_valid);
void maxim_find_peaks(int *pn_locs,int *pn_npks,int *pn_x,int n_size,int n_min_height,int n_min_distance,int n_max_num );
void maxim_peaks_above_min_height(int *pn_locs,int *pn_npks,int *pn_x,int n_size,int n_min_height );
void maxim_remove_close_peaks(int *pn_locs,int *pn_npks,int *pn_x,int n_min_distance );
void maxim_sort_ascend(int *pn_x,int n_size );
void maxim_sort_indices_descend(int *pn_x,int *pn_indx,int n_size);

#endif /* ALGORITHM_H_ */
