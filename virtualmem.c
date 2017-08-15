#define MAX_TEMP_STRING_SIZE 1000

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include	<time.h>
#include    <limits.h>

void usage();
void parse_from_terminal();
void parse_from_file();
void string_to_int_array(char*);
void optimal();
void fifo();
void lfu();
void lru_stack();
void lru_clock();
void lru_ref8();
void print_results(int, int, float, int, int);
void exec_replacement_policy();
int exists(int);
void fifo_alter(int);
int fifo_replace_index(int);
int check_count(int []);
int lru_stack_alter(int);
void existing_current_alter(int,int);
int clock_check(int);
int find_smallest_count_value(int []);

int ch, reference_string_size;
int available_frames = 5;
char *host = NULL;
char *replacement_policy = "FIFO";
char *input_file = NULL;
char *progname;
int *frames;				//available frames array 
int *reference_string;		//array for reference string
extern char *optarg;
extern int optind;
int algo_replacements = 0;
int optimal_replacements = 0;
int fifo_flag = 0;
int *fifo_order;
int *lru_replacement_stack;
int *clock_reference_stack;
int *lfu_omit_array;
int pointer = 0;
int lru_stack_flag = 0;
int lfu_fifo_alter_flag = 0;

struct byte {
   int bit_one;
   int bit_two;
   int bit_three;
   int bit_four;
   int bit_five;
   int bit_six;
   int bit_seven;
   int bit_eight;
};

void move_bits(struct byte []);

int
main(int argc , char *argv[])
{
	if ((progname = rindex(argv[0], '/')) == NULL)
	progname = argv[0];
	else
	progname++;

	while ((ch = getopt(argc, argv, "hf:r:i:")) != -1)
	switch(ch) {
		case 'h':
			usage();
			break;
		case 'f':
			available_frames = atoi(optarg);
			break;
		case 'r':
			replacement_policy = optarg;
			break;
		case 'i':
			input_file = optarg;
			break;
		case '?':
			fprintf(stderr, "unknown arg encountered\n");
		default:
			usage();
	}
	argc -= optind;
	if (argc != 0 || host != NULL)
		usage();

	frames = malloc(sizeof(int) * available_frames);
	fifo_order = malloc(sizeof(int) * available_frames);
	lru_replacement_stack = malloc(sizeof(int) * available_frames);

	if(input_file == NULL){
		parse_from_terminal();
	}
	else{
		parse_from_file();
	}

	exec_replacement_policy();
	return 0;
}

void
parse_from_terminal()
{
	char* in;
	in = malloc(MAX_TEMP_STRING_SIZE * sizeof (char)); 
	printf("Enter a reference string -> ");
	fgets(in, MAX_TEMP_STRING_SIZE, stdin);
	string_to_int_array(in);
}

void
parse_from_file()
{
	int c;
	char* string;
	int i = 0;
	FILE *in;
	in = fopen(input_file, "r");

	string = malloc(MAX_TEMP_STRING_SIZE * sizeof (char)); 

	if (input_file == 0)
    {
        perror("Canot open input file\n");
        exit(-1);
    }
    else{
    	while ((c = fgetc(in)) != EOF){
    		string[i] = c;
   			i++;
    	}
    	string_to_int_array(string);
    }
}

void
string_to_int_array(char* string)
{
	reference_string = malloc(MAX_TEMP_STRING_SIZE * sizeof (int)); 
	reference_string_size = MAX_TEMP_STRING_SIZE;

	char *delimeter = strtok(string," ");
	int i = 0;

	while(delimeter != NULL)
	{ 
		reference_string[i++] = atoi(delimeter);
		delimeter = strtok(NULL, " ");
	}

	reference_string = realloc(reference_string, i * sizeof(int));
	reference_string_size = i;
}

void optimal(){
	int i = 0;
	memset(frames, 0, available_frames*sizeof(int));
	int full_count = 0;

	for(i; i<reference_string_size; i++){
		int cur = reference_string[i];
		if (i<available_frames && exists(cur) == 0){
			frames[i] = cur;
			full_count++;
		}

		else if(exists(cur) == 0 && i >= available_frames){
			int j = 0;
			int distances[available_frames];
			memset(distances, 0, available_frames*sizeof(int));

			for(j; j<available_frames; j++){
				int z = i;
				int c_frame = frames[j];
				for(z; z<reference_string_size; z++){
					int c_ref = reference_string[z];
					if(c_ref == c_frame){
						distances[j] = z;
						break;
					}
					if(z == reference_string_size - 1){
						distances[j] = INT_MAX;
					}
				}
			}
			int t = 0;
			int greatest = 0;
			int index = 0;
			for(t; t<available_frames; t++){
				if(distances[t] > greatest){
					greatest = distances[t];
					index = t;
				}
			}
			frames[index] = cur;
			if(full_count >= available_frames){
				optimal_replacements++;
			}
			else{
				full_count++;
			}
		}
	}
}	

void
fifo()
{
	int i;
	int position_to_replace = 0;
	int full_count = 0;
	
	for(i = 0; i < reference_string_size; i++) {
		int current = reference_string[i];
		if(i < available_frames && exists(current) == 0) {
			frames[(position_to_replace++) % available_frames] = current;
			full_count++;
		}
		else if(i >= available_frames && exists(current) == 0) {
			frames[(position_to_replace++)%available_frames] = current;
			if(full_count >= available_frames){
				algo_replacements++;
			}
			else{
				full_count++;
			}
		}
	}
}

void lfu() {
	int i;
	int count[reference_string_size];
	memset(count, 0, reference_string_size*sizeof(int));
	int replace = 0;
	int fifo_pointer = 0;
	int smallest_index = 0;
	int full_flag = 0;
	int frames_counter = 0;
	for(i = 0; i < reference_string_size; i++) {
		int current = reference_string[i];
		if(i >= available_frames && replace == 0 && full_flag == 1) {
			replace = 1;
		}
		else if(i < available_frames || full_flag == 0) {
			if(exists(current) == 1) {
				count[current] = count[current] + 1;
			}
			else {
				frames[frames_counter] = current;
				count[current] = count[current] + 1;
				fifo_order[frames_counter] = current;
				frames_counter++;
				if(frames_counter == available_frames) {
					full_flag = 1;
				}
			}
		}
		if(replace == 1 && exists(current) == 0) {
			smallest_index = check_count(count);
			if(fifo_flag == 1) { //implement fifo algorithm
				fifo_pointer = fifo_replace_index(current);
				count[frames[fifo_pointer]] = count[frames[fifo_pointer]] - 1;
				frames[fifo_pointer] = current;
				count[current] = count[current] + 1;
				algo_replacements++;
			}
			else{
				count[frames[smallest_index]] = count[frames[smallest_index]] - 1;
				fifo_alter(current);
				frames[smallest_index] = current;
				count[current] = count[current] + 1;
				algo_replacements++;
			}
			fifo_flag = 0;
		}
		else if (replace == 1 && exists(current) == 1) {
			count[current] = count[current] + 1;
		}
	}
}

void lru_stack(){
	int i;
	int replace = 0;
	int index_to_replace;
	int full_flag = 0;
	int frames_counter = 0;
	for(i = 0; i < reference_string_size; i++) {
		int current = reference_string[i];
		if(i >= available_frames && replace == 0 && full_flag == 1) {
			replace = 1;
		}
		else if(i < available_frames || full_flag == 0) {
			if(exists(current) == 1) {
				lru_stack_flag = 1;
				existing_current_alter(current,frames_counter);
			}
			else {
				frames[frames_counter] = current;
				lru_replacement_stack[frames_counter] = current;
				frames_counter++;
				if(frames_counter == available_frames) {
					full_flag = 1;
				}
			}
		}
		if(replace == 1 && exists(current) == 0) {
			index_to_replace = lru_stack_alter(current);
			frames[index_to_replace] = current;
			algo_replacements++;
		}
		else if (replace == 1 && exists(current) == 1) {
			existing_current_alter(current,i);
		}
	}
}

void lru_clock(){
	int i;
	int index_to_replace;
	int full_flag = 0;
	int frames_counter = 0;
	int replace = 0;
	clock_reference_stack = malloc(sizeof(int) * reference_string_size);
	memset(clock_reference_stack, 0, reference_string_size);
	for(i = 0; i < reference_string_size; i++) {
		int current = reference_string[i];
		if(i >= available_frames && replace == 0 && full_flag == 1) {
			replace = 1;
			pointer = 0;
		}
		else if(i < available_frames || full_flag == 0) {
			if(exists(current) == 1) {
				if(clock_reference_stack[current] == 0) {
					clock_reference_stack[current] = 1;
				}
			}
			else {
				frames[i] = current;
				clock_reference_stack[current] = 0;
				pointer++;
				frames_counter++;
				if(frames_counter == available_frames) {
					full_flag = 1;
				}
			}
		}
		if(replace == 1 && exists(current) == 0) {
			index_to_replace = clock_check(current);
			frames[index_to_replace] = current;
			algo_replacements++;
		}
		else if (replace == 1 && exists(current) == 1) {
			clock_reference_stack[current] = 1;
		}
	}
}

void lru_ref8(){
	struct byte byte_array[reference_string_size];
	int full_count = 0;
	//initilize bits to 0
	int i=0;
	for(i; i<reference_string_size; i++){
		byte_array[i].bit_one = 0;
		byte_array[i].bit_two = 0;
		byte_array[i].bit_three = 0;
		byte_array[i].bit_four = 0;
		byte_array[i].bit_five = 0;
		byte_array[i].bit_six = 0;
		byte_array[i].bit_seven = 0;
		byte_array[i].bit_eight = 0;
	}

	i = 0;
	for(i; i<reference_string_size; i++){
		int cur = reference_string[i];
		byte_array[find_first(cur)].bit_one = 1;
		move_bits(byte_array);

		if (i<available_frames && exists(cur) == 0){
			frames[i] = cur;
			full_count++;
		}
		else if(exists(cur) == 0 && i >= available_frames){
			int t = 0;
			int small = INT_MAX;
			int ind_to_replace = 0;
			for(t; t<available_frames; t++){
				struct byte temp = byte_array[find_first(frames[t])];
				char one[10];
				sprintf(one, "%d", temp.bit_one);
				char two[10];
				sprintf(two, "%d", temp.bit_two);
				char three[10];
				sprintf(three, "%d", temp.bit_three);
				char four[10];
				sprintf(four, "%d", temp.bit_four);
				char five[10];
				sprintf(five, "%d", temp.bit_five);
				char six[10];
				sprintf(six, "%d", temp.bit_six);
				char seven[10];
				sprintf(seven, "%d", temp.bit_seven);
				char eight[10];
				sprintf(eight, "%d", temp.bit_eight);
				char str[80] = "";
				sprintf(str,"%s%s%s%s%s%s%s%s", one, two, three, four, five, six, seven, eight);

				int full_num = atoi(str);

				if(full_num < small){
					small = full_num;
					ind_to_replace = t;
				}			
			}
			frames[ind_to_replace] = cur;
			if(full_count >= available_frames){
				algo_replacements++;
			}
			else{
				full_count++;
			}
		}
	}
}

int find_first(int num){
	int i=0;
	for(i; i<reference_string_size; i++){
		if(reference_string[i] == num){
			return i;
		}
	}
	return 0;
}

void move_bits(struct byte byte_array[reference_string_size]){
	int i = 0;
	for(i; i<reference_string_size; i++){
		struct byte temp;
		temp = byte_array[i];
		temp.bit_eight = temp.bit_seven;
		temp.bit_seven = temp.bit_six;
		temp.bit_six = temp.bit_five;
		temp.bit_five = temp.bit_four;
		temp.bit_four = temp.bit_three;
		temp.bit_three = temp.bit_two;
		temp.bit_two = temp.bit_one;
		temp.bit_one = 0;
		byte_array[i] = temp;
	}
}

int 
exists(int current)
{
	int i;
	for(i = 0; i < available_frames; i++) {
		if(frames[i] == current) {
			return 1;
		}
	}
	return 0;
}

int exist_in_lfu_array(int current) {
	int i = 0;
	for(i; i < available_frames; i++) {
		if(current == lfu_omit_array[i]) {
			return 1;
		}
	}
	return 0;
}

int check_count(int count[]) {
	int i = 0;
	int smallest = 0;
	int smallest_index = 0;
	lfu_omit_array = malloc(sizeof(int) * reference_string_size);
	memset(lfu_omit_array, 0, reference_string_size);
	int smallest_count_value = find_smallest_count_value(count);
	int count_of_smallest = 0;
	for(i; i < available_frames; i++) {
		int current = count[frames[i]];
		if(current == smallest_count_value) {
			count_of_smallest++;
		}
	}
	if(count_of_smallest >= 2) {
		fifo_flag = 1;
	}
	else {
		int j;
		for(j = 0; j < available_frames; j++) {
			int c = count[frames[j]];
			if(c == smallest_count_value) {
				smallest_index = j;
				break;
			}
		}		
	}
	return smallest_index;
}

int find_smallest_count_value(int count[]) {
	int smallest_count_value;
	int i;
	for(i = 0; i < available_frames; i++) {
		int current_count_value = count[frames[i]];
		if(current_count_value <= smallest_count_value || i == 0) {
			smallest_count_value = current_count_value;
		}
		else if(current_count_value > smallest_count_value) {
			lfu_omit_array[frames[i]] = 1;
		}
	}
	return smallest_count_value;
}

int fifo_replace_index(int current) {
	int replace_index;
	int frames_replace_index = 0;
	int i;
	for(i = 0; i < available_frames; i++) {
		int replace_page = fifo_order[i];
		if(lfu_omit_array[replace_page] == 0) {
			replace_index = i;
			break;
		}
	}
	
	int k;
	for(k = 0; k < available_frames; k++) {
		int value = frames[k];
		if(value == fifo_order[replace_index]) {
			frames_replace_index = k;
			break;
		}
	}
	
	int j;
	for(j = replace_index; j < available_frames - 1; j++) {
		fifo_order[j] = fifo_order[j + 1];
	}
	fifo_order[available_frames - 1] = current;
	return frames_replace_index;	
}

void fifo_alter(int current) {
	int j;
	for(j = 0; j < available_frames - 1; j++) {
		fifo_order[j] = fifo_order[j + 1];	
	}
	fifo_order[available_frames - 1] = current;
}

int lru_stack_alter(int current) {
	int j;
	int i;
	int replace_index;
	int page_to_replace = lru_replacement_stack[0];
	for(j = 0; j < available_frames - 1; j++) {
		lru_replacement_stack[j] = lru_replacement_stack[j + 1];
	}
	lru_replacement_stack[available_frames - 1] = current;
	for(i = 0; i < available_frames; i++) {
		if(frames[i] == page_to_replace) {
			replace_index = i;
			break;
		}
	}
	return replace_index;	
	
}

void existing_current_alter(int current, int index) {
	int i;
	int j;
	
	for(i = 0; i < available_frames; i++) {
		if(lru_replacement_stack[i] == current) {
			if(lru_stack_flag == 1) {
				for(j = i; j < index - 1; j++) {
					lru_replacement_stack[j] = lru_replacement_stack[j + 1];
				}
				lru_replacement_stack[index - 1] = current;
				lru_stack_flag = 0;
				break;
			}
			else {
				for(j = i; j < available_frames - 1; j++) {
					lru_replacement_stack[j] = lru_replacement_stack[j + 1];
				}
				lru_replacement_stack[available_frames - 1] = current;
				break;
			}
		}
	}
}

int clock_check(int current) {
	int index_to_replace;
	int to_check;
	while(1) {
		to_check = clock_reference_stack[frames[pointer]];
		if(to_check == 0) {
			index_to_replace = pointer;
			clock_reference_stack[current] = 0;
			pointer++;
			if(pointer == available_frames) {
				pointer = 0;
			}
			break;
		}
		else if (to_check == 1){
			clock_reference_stack[frames[pointer]] = 0;
			pointer++;
			if(pointer == available_frames) {
				pointer = 0;
			}
		}
	}
	return index_to_replace;
}


void
usage()
{
	fprintf(stderr, "Usage: virtualmem [-h] [-f available frames] [-r replacement-policy] [-i input_file]\n");
	exit(1);
}

void
exec_replacement_policy()
{
	struct timeval start_time, stop_time;

	if (!strcmp(replacement_policy,"LFU")){
		gettimeofday(&start_time, NULL);
		lfu();
		gettimeofday(&stop_time, NULL);
	}

	else if (!strcmp(replacement_policy,"LRU-STACK")){
		gettimeofday(&start_time, NULL);
		lru_stack();
		gettimeofday(&stop_time, NULL);
	}

	else if (!strcmp(replacement_policy,"LRU-CLOCK")){
		gettimeofday(&start_time, NULL);
		lru_clock();
		gettimeofday(&stop_time, NULL);
	}

	else if (!strcmp(replacement_policy,"LRU-REF8")){
		gettimeofday(&start_time, NULL);
		lru_ref8();
		gettimeofday(&stop_time, NULL);
	}

	else{	
		//default fifo case
		gettimeofday(&start_time, NULL);
		fifo();
		gettimeofday(&stop_time, NULL);
	}

	struct timeval opt_start_time, opt_stop_time;

	gettimeofday(&opt_start_time, NULL);
	optimal();
	gettimeofday(&opt_stop_time, NULL);

	printf("# of page replacements with %s: %d\n", replacement_policy, algo_replacements);
	printf("# of page replacements with Optimal: %d\n", optimal_replacements);
	float percent_penalty = (((float)algo_replacements - (float)optimal_replacements) / (float)optimal_replacements) * 100;
	printf("%% page replacement penalty using %s: %.1f \n\n", replacement_policy, percent_penalty);

	unsigned long algo_time = stop_time.tv_usec - start_time.tv_usec;
	unsigned long opt_time = opt_stop_time.tv_usec - opt_start_time.tv_usec;

	printf("Total time to run %s algorithm: %lu msec \n", replacement_policy, algo_time);	
	printf("Total time to run Optimal algorithm: %lu msec \n", opt_time);

	if(opt_time > algo_time){
		printf("%s is %.1f%% faster than Optimal algorithm\n", replacement_policy, (((float)opt_time - (float)algo_time) / (float)(opt_time)) * 100);
	}
	else{
		printf("Optimal is %.1f%% faster than %s \n", (((float)algo_time - (float)opt_time) / (float)(algo_time)) * 100, replacement_policy);
	}
}