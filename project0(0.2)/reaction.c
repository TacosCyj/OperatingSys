#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
//make_water()在reaction-runner.c中声明了，可能会在这个文件中的reaction_h和reaction_o中调用
void make_water();

struct reaction {
	struct lock* lock;
	struct condition* H_cond;
	struct condition* O_cond;
	int num_of_H;	
	int num_of_O;
	int num_of_H2O;
	int num_of_consume_H;
	int num_of_consume_O;
};

void
reaction_init(struct reaction *reaction)
{
	reaction -> lock = (struct lock*)malloc(sizeof(struct lock));
	reaction -> H_cond = (struct condition*)malloc(sizeof(struct condition));
	reaction -> O_cond = (struct condition*)malloc(sizeof(struct condition));
	lock_init(reaction -> lock);
	cond_init(reaction -> H_cond);
	cond_init(reaction -> O_cond);
	reaction -> num_of_H = 0;
	reaction -> num_of_O = 0;
	reaction -> num_of_H2O = 0;
	reaction -> num_of_consume_H = 0;
	reaction -> num_of_consume_O = 0;
}

void
reaction_h(struct reaction *reaction)
{
	lock_acquire(reaction -> lock);
	reaction -> num_of_H++;
	if(reaction -> num_of_H >= 2 && reaction -> num_of_O >= 1){
		cond_broadcast(reaction -> H_cond, reaction -> lock);
		cond_broadcast(reaction -> O_cond, reaction -> lock);
	}
	while((reaction -> num_of_H < 2 || reaction -> num_of_O < 1) && reaction -> num_of_H2O * 2 == reaction -> num_of_consume_H){
		cond_wait(reaction -> H_cond, reaction -> lock);
	}
	if(reaction -> num_of_H >= 2 && reaction -> num_of_O >= 1 && reaction -> num_of_H2O * 2 == reaction -> num_of_consume_H) {
		make_water();
		reaction -> num_of_H -= 2;
		reaction -> num_of_O -= 1;
		reaction -> num_of_H2O++;
	}
	reaction -> num_of_consume_H++;
	lock_release(reaction -> lock);
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(reaction -> lock);
	reaction -> num_of_O++;
	if(reaction -> num_of_H >= 2 && reaction -> num_of_O >= 1){
		cond_broadcast(reaction -> H_cond, reaction -> lock);
		cond_broadcast(reaction -> O_cond, reaction -> lock);
	}
	while((reaction -> num_of_H < 2 || reaction -> num_of_O < 1) && reaction ->  num_of_H2O == reaction -> num_of_consume_O){
		cond_wait(reaction -> O_cond, reaction -> lock);
	}
	if(reaction -> num_of_H >= 2 && reaction -> num_of_O >= 1 && reaction ->  num_of_H2O == reaction -> num_of_consume_O) {
		make_water();
		reaction -> num_of_H -= 2;
		reaction -> num_of_O -= 1;
		reaction -> num_of_H2O++;
	}
	reaction -> num_of_consume_O++;
	lock_release(reaction -> lock);
}
