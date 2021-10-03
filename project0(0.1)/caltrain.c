#include "pintos_thread.h"

struct station {
	struct lock* lock;
	struct condition* passenger;
	struct condition* train;
	int seat_per_pass;
	int passenger_to_board;
	int passenger_to_report;
};

void
station_init(struct station *station)
{
	station -> lock = (struct lock*)malloc(sizeof(struct lock));
	station -> passenger = (struct condition*)malloc(sizeof(struct condition));
	station -> train = (struct condition*)malloc(sizeof(struct condition));
	lock_init(station -> lock);
	cond_init(station -> passenger);
	cond_init(station -> train);
	station -> seat_per_pass = 0;
	station -> passenger_to_board = 0;
	station -> passenger_to_report = 0;
}

void
station_load_train(struct station *station, int count)
{
	lock_acquire(station -> lock);
	station -> seat_per_pass = count;
	while(station -> seat_per_pass > 0 && station -> passenger_to_board > 0){
		cond_broadcast(station -> passenger, station -> lock);
		cond_wait(station -> train, station -> lock);
	}
	station -> seat_per_pass = 0;
	lock_release(station -> lock);
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(station -> lock);
	station -> passenger_to_board++;
	while(station -> seat_per_pass == station -> passenger_to_report){
		cond_wait(station -> passenger, station -> lock);
	}
	station -> passenger_to_board--;
	station -> passenger_to_report++;
	lock_release(station -> lock);
}

void
station_on_board(struct station *station)
{
	lock_acquire(station -> lock);
	station -> seat_per_pass--;
	station -> passenger_to_report--;
	if(station -> seat_per_pass == 0 || station -> passenger_to_report == 0){
		cond_signal(station -> train, station -> lock);
	}
	lock_release(station -> lock);
}
