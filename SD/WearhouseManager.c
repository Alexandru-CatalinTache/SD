#include <stdio.h>
#include "WearhouseManager.h"


Package *create_package(long priority, const char* destination){
	Package* package=(Package*)malloc(sizeof(Package));
	package->priority=priority;
	
	if(destination != NULL)
	{	
		package->destination=(char*)malloc(sizeof(char)*strlen(destination));
		strcpy(package->destination,destination);
	}
	else{
		package->destination=NULL;
	}

	return package;
}

void destroy_package(Package* package){
	free(package->destination);
	free(package);
}

Manifest* create_manifest_node(void){
	Manifest* manifest_node=(Manifest*)malloc(sizeof(Manifest));
	manifest_node->package=NULL;
	manifest_node->next=NULL;
	manifest_node->prev=NULL;
	return manifest_node;
}

void destroy_manifest_node(Manifest* manifest_node){
	free(manifest_node->package);
	free(manifest_node->next);
	free(manifest_node->prev);
	free(manifest_node);

}

Wearhouse* create_wearhouse(long capacity){
	if(capacity == 0)
		return NULL;
	else{	
	Wearhouse* wearhouse=(Wearhouse*)malloc(sizeof(Wearhouse));
	wearhouse->packages=(Package**)malloc(sizeof(Package*)*capacity);
	wearhouse->size=0;
	wearhouse->capacity=capacity;
	return wearhouse;
	}
}

Wearhouse *open_wearhouse(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Wearhouse *w = NULL;


	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	if((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		w = create_wearhouse(atol(token));

		free(line);
		line = NULL;
		len = 0;
	}

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		long priority = atol(token);
		token = strtok(NULL, ",\n ");
		Package *p = create_package(priority, token);
		w->packages[w->size++] = p;

		free(line);
		line = NULL;
		len = 0;
	}

	free(line);


	fclose(fp);
	return w;

	file_open_exception:
	return NULL;
}

long wearhouse_is_empty(Wearhouse *w){
	if(w->size == 0)
	{
		return 1;
	}
	else{
		return 0;
	}
	
}

long wearhouse_is_full(Wearhouse *w){
	if(w->capacity == w->size)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}

long wearhouse_max_package_priority(Wearhouse *w){
	long i;
	long maxim_priority=0;
	for(i = 0; i< w->size; i++)
	{
         if(w->packages[i]->priority > maxim_priority)
		 {
			 maxim_priority=w->packages[i]->priority;
		 }
	}
	return maxim_priority;
}

long wearhouse_min_package_priority(Wearhouse *w){
	long i;
	long minim_priority=0;
	for(i = 0; i< w->size; i++)
	{
         if(w->packages[i]->priority < minim_priority)
		 {
			 minim_priority=w->packages[i]->priority;
		 }
	}
	
	return minim_priority;
}


void wearhouse_print_packages_info(Wearhouse *w){
	for(long i = 0; i < w->size; i++){
		printf("P: %ld %s\n",
				w->packages[i]->priority,
				w->packages[i]->destination);
	}
	printf("\n");
}

void destroy_wearhouse(Wearhouse* wearhouse){
	long i;
	for(i=0; i < wearhouse->size; i++)
	{
		free(wearhouse->packages[i]->destination);
	}
	for(i=0; i < wearhouse->size; i++)
	{
		free(wearhouse->packages[i]);
	}
	free(wearhouse->packages);
	free(wearhouse);
}


Robot* create_robot(long capacity){
	Robot* robot=(Robot*)malloc(sizeof(Robot));
	robot->size=0;
	robot->manifest=NULL;
	robot->capacity=capacity;
	return robot;
	
}

int robot_is_full(Robot* robot){
	if(robot->capacity == robot->size)
	{
		return 1;
	}
	return 0;
}

int robot_is_empty(Robot* robot){
	if(robot->size == 0)
	{
		return 1;
	}
	else{
		return 0;
	}
}

Package* robot_get_wearhouse_priority_package(Wearhouse *w, long priority){
	long i;
	for(i=0; i < w->size; i++)
	{
		if(w->packages[i]->priority == priority)
		{
			return w->packages[i];
			
		}
	}
	return NULL;
	
}

void robot_remove_wearhouse_package(Wearhouse *w, Package* package){
	long i, poz;
	for(i=0; i < w->size; i++)
	{
        if(w->packages[i] == package)
		{
			poz=i;
		}
	}
	for(i=poz; i < w->size-1; i++)
	{
		w->packages[i] = w->packages[i+1];
	}
	w->size--;
	
}


void robot_load_one_package(Robot* robot, Package* package){
	Manifest* new_manifest=create_manifest_node();
	new_manifest->package=create_package(package->priority, package->destination);
	
	if(robot->capacity == robot->size)
	{
		return ;
	}
	if(robot->size == 0)
		robot->manifest=new_manifest;
	
	else
	{
		Manifest *robot_manifest = robot->manifest;
		while(robot_manifest->next != NULL)
		{   
			if(robot_manifest->package->priority < package->priority)
			{   
				break;
			}
			else if(robot_manifest->package->priority == package->priority)
			{
				if(strcmp(robot_manifest->package->destination, package->destination) > 0)
				{
					break;
				}
			}
			robot_manifest=robot_manifest->next;
		}
		if((robot_manifest->next != NULL) && (robot_manifest->prev != NULL))
		{   
			new_manifest->prev=robot_manifest->prev;
			robot_manifest->prev->next=new_manifest;
			robot_manifest->prev=new_manifest;
			new_manifest->next=robot_manifest;
			
		}
		else if((robot_manifest->next == NULL) && (robot_manifest->prev != NULL))
		{  
			if(robot_manifest->package->priority > new_manifest->package->priority)
	       {
			   robot_manifest->next=new_manifest;
		   	   new_manifest->prev=robot_manifest;
		   }
		    else if(robot_manifest->package->priority < new_manifest->package->priority)
			{
				new_manifest->prev=robot_manifest->prev;
				robot_manifest->prev->next=new_manifest;
				robot_manifest->prev=new_manifest;
				new_manifest->next=robot_manifest;
			}
			else if(robot_manifest->package->priority == new_manifest->package->priority)
			{	
				if(strcmp(robot_manifest->package->destination, package->destination) > 0)
				{	
					new_manifest->prev=robot_manifest->prev;
					robot_manifest->prev->next=new_manifest;
					robot_manifest->prev=new_manifest;
					new_manifest->next=robot_manifest;
				}
				else if(strcmp(robot_manifest->package->destination, package->destination) < 0)
				{
				   robot_manifest->next=new_manifest;
		   		   new_manifest->prev=robot_manifest;
				}
			}

		}
		else if((robot_manifest->prev == NULL) && (robot_manifest->next != NULL))
		{   
			robot_manifest->prev=new_manifest;
			new_manifest->next=robot_manifest;
			robot->manifest = new_manifest;
		}

		else if (robot_manifest->prev == NULL && robot_manifest->next == NULL)
		{
			if(robot_manifest->package->priority > new_manifest->package->priority)
			{
			   robot_manifest->next=new_manifest;
			   new_manifest->prev = robot_manifest;
			}
		   
		    else if(robot_manifest->package->priority < new_manifest->package->priority)
			{
				robot_manifest->prev=new_manifest;
				new_manifest->next=robot_manifest;
				robot->manifest = new_manifest;
			}
			else if(robot_manifest->package->priority == new_manifest->package->priority)
			{	
				if(strcmp(robot_manifest->package->destination, package->destination) > 0)
				{	robot_manifest->prev=new_manifest;
					new_manifest->next=robot_manifest;
					robot->manifest = new_manifest;
				}
				else if(strcmp(robot_manifest->package->destination, package->destination) < 0)
				{
					robot_manifest->next=new_manifest;
			   		new_manifest->prev = robot_manifest;
				}
			}
		}
	
	}
	robot->size++;
}

long robot_load_packages(Wearhouse* wearhouse, Robot* robot){
	long priority;
	Package* pachet;
	long nr_pachete=0;
    while((robot->size < robot->capacity) && (wearhouse->size > 0))
	{           priority=wearhouse_max_package_priority(wearhouse);
				pachet=robot_get_wearhouse_priority_package(wearhouse, priority);	
				if(robot->size == robot->capacity)
					{
							break;
					}				
				else {   
						  	robot_load_one_package(robot, pachet);
						  	robot_remove_wearhouse_package(wearhouse, pachet);
						  	nr_pachete++;
					 }
				
				
			
		
		if((robot->size == robot->capacity) || (wearhouse->size == 0))
		{
			break;
		}
	}
	return nr_pachete;
}

Package* robot_get_destination_highest_priority_package(Robot* robot, const char* destination){
	Package* pachet;
	Manifest* copie;
	copie=robot->manifest;
	int maxim=0;
	while(copie->next != NULL)
	{
		if(strcmp(copie->package->destination, destination) == 0)
		{
             if(copie->package->priority > maxim)
			 {
				 pachet=copie->package;
				 maxim=copie->package->priority;
			 }
		}
		copie=copie->next;
	}
	return pachet;
	
}

void destroy_robot(Robot* robot){
	long marime;
	marime=robot->size;
	while(marime != 0)
	{
		free(robot->manifest->package);
		robot->manifest=robot->manifest->next;
		marime--;
	}
	free(robot->manifest);
	free(robot->next);
}


void robot_unload_packages(Truck* truck, Robot* robot){
	Manifest* manifest_truck;
	Manifest* nod_parcurgere_ultimul=truck->manifest;
	Manifest* copie_manifest;
	copie_manifest=robot->manifest;
	if(truck->manifest == NULL)
	{
		nod_parcurgere_ultimul=NULL;
	}
	else{while(nod_parcurgere_ultimul->next != NULL)
	{
		nod_parcurgere_ultimul=nod_parcurgere_ultimul->next;
	}
	}
	manifest_truck=nod_parcurgere_ultimul;
	while(copie_manifest != NULL)
	{   
		Manifest* copie_next=copie_manifest->next;
		if(strcmp(truck->destination, copie_manifest->package->destination) == 0)
		{   
			if(copie_manifest->next != NULL)
			{
				copie_manifest->next->prev=copie_manifest->prev;
			}
			if(copie_manifest->prev != NULL)
			{
				copie_manifest->prev->next=copie_manifest->next;
			}
			if(copie_manifest == robot->manifest)
			{
				robot->manifest=robot->manifest->next;
			}
            copie_manifest->next=NULL;
			copie_manifest->prev=NULL;
			if(manifest_truck == NULL)
			{
				copie_manifest->prev=NULL;
				truck->manifest=copie_manifest;
			}
			else if(manifest_truck != NULL)
			{
				copie_manifest->prev=manifest_truck;
				manifest_truck->next=copie_manifest;
			}
			manifest_truck=copie_manifest;
			truck->size++;
			robot->size--;
			if(truck->capacity == truck->size)
			{
				return;
			}
			
			
		}
		copie_manifest=copie_next;
	}
	return;
	
}



// Attach to specific truck
int robot_attach_find_truck(Robot* robot, Parkinglot *parkinglot){
	int found_truck = 0;
	long size = 0;
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	Manifest* m_iterator = robot->manifest;


	while(m_iterator != NULL){
		while(arrived_iterator != parkinglot->arrived_trucks){
			size  = truck_destination_robots_unloading_size(arrived_iterator);
			if(strncmp(m_iterator->package->destination, arrived_iterator->destination, MAX_DESTINATION_NAME_LEN) == 0 &&
					size < (arrived_iterator->capacity-arrived_iterator->size)){
				found_truck = 1;
				break;
			}

			arrived_iterator = arrived_iterator->next;
		}

		if(found_truck)
			break;
		m_iterator = m_iterator->next;
	}

	if(found_truck == 0)
		return 0;


	Robot* prevr_iterator = NULL;
	Robot* r_iterator = arrived_iterator->unloading_robots;
	while(r_iterator != NULL){
		Package *pkg = robot_get_destination_highest_priority_package(r_iterator, m_iterator->package->destination);
		if(m_iterator->package->priority >= pkg->priority)
			break;
		prevr_iterator = r_iterator;
		r_iterator = r_iterator->next;
	}

	robot->next = r_iterator;
	if(prevr_iterator == NULL)
		arrived_iterator->unloading_robots = robot;
	else
		prevr_iterator->next = robot;

	return 1;
}

void robot_print_manifest_info(Robot* robot){
	Manifest *iterator = robot->manifest;
	while(iterator != NULL){
		printf(" R->P: %s %ld\n", iterator->package->destination, iterator->package->priority);
		iterator = iterator->next;
	}

	printf("\n");
}



Truck* create_truck(const char* destination, long capacity, long transit_time, long departure_time){
	Truck* truck=(Truck*)malloc(sizeof(Truck));
	truck->size=0;
    truck->capacity=capacity;
	truck->departure_time=departure_time;
	truck->transit_end_time=transit_time;
	truck->in_transit_time=0;
	truck->manifest=NULL;
	truck->unloading_robots=NULL;
	truck->next=NULL;
	if(destination == NULL){
	truck->destination=NULL;
	}
	else{
		truck->destination=(char*)malloc(sizeof(char)*strlen(destination));
		strcpy(truck->destination, destination);
	}
	return truck;
}

int truck_is_full(Truck *truck){
	if(truck->size == truck->capacity)
	{
		return 1;
	}
		return 0;
	
}

int truck_is_empty(Truck *truck){
	if(truck->size == 0)
	{
		return 1;
	}
		return 0;
	
}

long truck_destination_robots_unloading_size(Truck* truck){
	long nr_pachete=0;
	if(truck->destination == NULL)
	{
		return 0;
	}
	else{
	Robot* copie_robot;
	copie_robot=truck->unloading_robots;
	
	while(copie_robot != NULL)
	{   
		Manifest* copie_manifest;
		copie_manifest=copie_robot->manifest;
		while(copie_manifest->next != NULL)
		{
			if(strcmp(truck->destination, copie_manifest->package->destination) == 0)
			{
				nr_pachete=nr_pachete+copie_robot->size;
				break;
			}
			copie_manifest=copie_manifest->next;
		}
		copie_robot=copie_robot->next;

	}
	return nr_pachete;
	}
}


void truck_print_info(Truck* truck){
	printf("T: %s %ld %ld %ld %ld %ld\n", truck->destination, truck->size, truck->capacity,
			truck->in_transit_time, truck->transit_end_time, truck->departure_time);

	Manifest* m_iterator = truck->manifest;
	while(m_iterator != NULL){
		printf(" T->P: %s %ld\n", m_iterator->package->destination, m_iterator->package->priority);
		m_iterator = m_iterator->next;
	}

	Robot* r_iterator = truck->unloading_robots;
	while(r_iterator != NULL){
		printf(" T->R: %ld %ld\n", r_iterator->size, r_iterator->capacity);
		robot_print_manifest_info(r_iterator);
		r_iterator = r_iterator->next;
	}
}


void destroy_truck(Truck* truck){
	free(truck->manifest);
	free(truck->unloading_robots);
	free(truck->destination);
	free(truck->next);
	free(truck);
	
}


Parkinglot* create_parkinglot(void){
	Parkinglot* parkinglot=(Parkinglot*)malloc(sizeof(Parkinglot));
	if(parkinglot == NULL)
	{
		return NULL;
	}
	Truck** dummy1=&parkinglot->arrived_trucks;
	Truck** dummy2=&parkinglot->departed_trucks;
	Robot** dummy3=&parkinglot->pending_robots;
	Robot** dummy4=&parkinglot->standby_robots;

	*dummy1=create_truck(NULL, 0, 0, 0);
	*dummy2=create_truck(NULL, 0, 0, 0);
	*dummy3=create_robot(0);
	*dummy4=create_robot(0);

	parkinglot->arrived_trucks->next=(*dummy1);
	parkinglot->departed_trucks->next=(*dummy2);
	parkinglot->pending_robots->next=(*dummy3);
	parkinglot->standby_robots->next=(*dummy4);

	return parkinglot;
}

Parkinglot* open_parckinglot(const char* file_path){
	ssize_t read_size;
	char* line = NULL;
	size_t len = 0;
	char* token = NULL;
	Parkinglot *parkinglot = create_parkinglot();

	FILE *fp = fopen(file_path, "r");
	if(fp == NULL)
		goto file_open_exception;

	while((read_size = getline(&line, &len, fp)) != -1){
		token = strtok(line, ",\n ");
		// destination, capacitym transit_time, departure_time, arrived
		if(token[0] == 'T'){
			token = strtok(NULL, ",\n ");
			char *destination = token;

			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			token = strtok(NULL, ",\n ");
			long transit_time = atol(token);

			token = strtok(NULL, ",\n ");
			long departure_time = atol(token);

			token = strtok(NULL, ",\n ");
			int arrived = atoi(token);

			Truck *truck = create_truck(destination, capacity, transit_time, departure_time);

			if(arrived)
				truck_arrived(parkinglot, truck);
			else
				truck_departed(parkinglot, truck);

		}else if(token[0] == 'R'){
			token = strtok(NULL, ",\n ");
			long capacity = atol(token);

			Robot *robot = create_robot(capacity);
			parkinglot_add_robot(parkinglot, robot);

		}

		free(line);
		line = NULL;
		len = 0;
	}
	free(line);

	fclose(fp);
	return parkinglot;

	file_open_exception:
	return NULL;
}

void parkinglot_add_robot(Parkinglot* parkinglot, Robot *robot){
	
	if(robot_is_empty(robot) == 1)
	{   Robot* santinela=parkinglot->standby_robots;
		Robot* parinte_standby=parkinglot->standby_robots;
		Robot* santinela_standby=parinte_standby->next;
        if(santinela_standby == parinte_standby)
		{
				parinte_standby->next=robot;
				robot->next=parinte_standby;
			
		}

		
		else{
			
			while((santinela_standby->next != santinela) && (robot->capacity < santinela_standby->capacity))
		{   
			parinte_standby=santinela_standby;
			santinela_standby=santinela_standby->next;
		}
		if((santinela_standby->capacity > robot->capacity) && (santinela_standby->next==santinela))
		{
			santinela_standby->next=robot;
			robot->next=santinela;
		}
		else{
			parinte_standby->next=robot;
			robot->next=santinela_standby;
		}
		
		
		}
		
		parkinglot->standby_robots->size++;
	}
	else{
        Robot* santinela=parkinglot->pending_robots;
		Robot* parinte_pending=parkinglot->pending_robots;
		Robot* santinela_pending=parinte_pending->next;
		if(santinela_pending == parinte_pending)
		{
			
			parinte_pending->next=robot;
			robot->next=parinte_pending;
	
		}
		else{
		while((santinela_pending->next != santinela) && (robot->size < santinela_pending->size))
		{
			
			parinte_pending=santinela_pending;
			santinela_pending=santinela_pending->next;
		}
		if((santinela_pending->size > robot->size) && (santinela_pending->next==santinela))
		{
			santinela_pending->next=robot;
			robot->next=santinela;
		}
		else{
			parinte_pending->next=robot;
			robot->next=santinela_pending;
		}
		}
		parkinglot->pending_robots->size++;
	}
	
  return;
}


void parkinglot_remove_robot(Parkinglot *parkinglot, Robot* robot){
	 if(robot_is_empty(robot)==1)
	{
		Robot* santinela=parkinglot->standby_robots;
		Robot* santinela_next=santinela->next;
		
		if((santinela == santinela_next) && (robot == santinela))
		{
           santinela=NULL;
		   parkinglot->standby_robots->size=0;
		}
		else{

			while(santinela->next != robot)
			{   
				santinela=santinela_next;
				santinela_next=santinela_next->next;
			}
			santinela->next=santinela_next->next;
			parkinglot->standby_robots->size--;
		}
	}
	else{
		Robot* santinela=parkinglot->pending_robots;
		Robot* santinela_next=santinela->next;
		
		if((santinela == santinela_next) && (robot == santinela))
		{
           santinela=NULL;
		   parkinglot->pending_robots->size=0;
		}
		else{

			while(santinela->next != robot)
			{   
				santinela=santinela_next;
				santinela_next=santinela_next->next;
				
			}
			santinela->next=santinela_next->next;
		}
		parkinglot->pending_robots->size--;
	}
}

int parckinglot_are_robots_peding(Parkinglot* parkinglot){
	if(parkinglot->pending_robots->next != parkinglot->pending_robots)
	{
		return 1;
	}
	return 0;
}

int parkinglot_are_arrived_trucks_empty(Parkinglot* parkinglot){
	Truck* item=parkinglot->arrived_trucks;
	int ok=1;
		while(item->next != parkinglot->arrived_trucks)
	{
		if(item->size != 0)
		{
			ok=0;
			return 0;
		}
		item=item->next;
	}
	if(item->size != 0)
	{
		ok=0;
	}	

	return ok;
}


int parkinglot_are_trucks_in_transit(Parkinglot* parkinglot){
	if(parkinglot->departed_trucks != parkinglot->departed_trucks->next)
	{
		return 1;
	}

	return 0;
}


void destroy_parkinglot(Parkinglot* parkinglot){
	free(parkinglot->arrived_trucks);
	free(parkinglot->departed_trucks);
	free(parkinglot->pending_robots);
	free(parkinglot->standby_robots);
	free(parkinglot);

}

void parkinglot_print_arrived_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->arrived_trucks->next;
	while(iterator != parkinglot->arrived_trucks){

		truck_print_info(iterator);
		iterator = iterator->next;
	}

	printf("\n");

}

void parkinglot_print_departed_trucks(Parkinglot* parkinglot){
	Truck *iterator = parkinglot->departed_trucks->next;
	while(iterator != parkinglot->departed_trucks){
		truck_print_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_pending_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->pending_robots->next;
	while(iterator != parkinglot->pending_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}

void parkinglot_print_standby_robots(Parkinglot* parkinglot){
	Robot *iterator = parkinglot->standby_robots->next;
	while(iterator != parkinglot->standby_robots){
		printf("R: %ld %ld\n", iterator->size, iterator->capacity);
		robot_print_manifest_info(iterator);
		iterator = iterator->next;
	}
	printf("\n");

}


void truck_departed(Parkinglot *parkinglot, Truck* truck){
	Truck* santinela_arrived=parkinglot->arrived_trucks;
	Truck* parinte_arrived=santinela_arrived;
	santinela_arrived=santinela_arrived->next;
		while((santinela_arrived->next != parkinglot->arrived_trucks) && (santinela_arrived != truck))
	{   
		
        parinte_arrived=santinela_arrived;
		santinela_arrived=santinela_arrived->next;
	}
	
		if(truck == santinela_arrived)
		{
			parinte_arrived->next=santinela_arrived->next;
			parkinglot->arrived_trucks->size--;
		}
		
	
		Truck* santinela=parkinglot->departed_trucks;
		Truck* parinte=parkinglot->departed_trucks;
		Truck* santinela_departured=parinte->next;
	if(santinela_departured == parinte)
		{
			
			parinte->next=truck;
			truck->next=parinte;
	
		}
		else{
		while((santinela_departured->next != santinela) && (truck->departure_time > santinela_departured->departure_time))
		{
			
			parinte=santinela_departured;
			santinela_departured=santinela_departured->next;
		}
		if((santinela_departured->departure_time < truck->departure_time) && (santinela_departured->next==santinela))
		{
			santinela_departured->next=truck;
			truck->next=santinela;
		}
		else{
			parinte->next=truck;
			truck->next=santinela_departured;
		}
		}
		parkinglot->departed_trucks->size++;
	
  return;
	
}


void truck_arrived(Parkinglot *parkinglot, Truck* truck){
	if(parkinglot == NULL || truck == NULL) return;
    Truck* santinela=parkinglot->arrived_trucks;
	Truck* parinte=santinela;
	Truck* urmator=santinela->next;
	Truck* prev=parkinglot->departed_trucks;
	Truck* urm=prev->next;
	Truck* dummy=prev;
	while(urm != dummy)
	{
      if(urm == truck)
	  {
		  prev->next=urm->next;
		  parkinglot->departed_trucks->size--;
		  break;
	  }
	  urm=urm->next;
	}
	Manifest* nod=truck->manifest;
	while(nod != NULL)
	{     
		  destroy_package(nod->package);
		  free(nod);
		  nod=nod->next;
	}
	      truck->manifest=NULL;
		  truck->size=0;
	
	if(parinte == urmator)
	{
		parinte->next=truck;
		truck->next=parinte;
		parkinglot->arrived_trucks->size++;
	}
	else{
		while((strcmp(urmator->destination, truck->destination) < 0) && (urmator->next != santinela))
		{
			parinte=urmator;
			urmator=urmator->next;
		}
			while((strcmp(urmator->destination, truck->destination) == 0) && (urmator->next != santinela))
		{
			if(urmator->departure_time > truck->departure_time)
			{
				break;
			}
			parinte=urmator;
			urmator=urmator->next;
		}
		if(urmator->next == santinela && strcmp(urmator->destination, truck->destination) < 0)
		{
            urmator->next=truck;
			truck->next=santinela;
		}
		else if(urmator->next == santinela && strcmp(urmator->destination, truck->destination) == 0 && truck->departure_time > urmator->departure_time)
		{
            urmator->next=truck;
			truck->next=santinela;
		}
		else{
			parinte->next=truck;
			truck->next=urmator;
		}
		parkinglot->arrived_trucks->size++;
	}

}

void truck_transfer_unloading_robots(Parkinglot* parkinglot, Truck* truck){
	Robot* robot;
	robot=truck->unloading_robots;
	Robot* parinte;
	while(robot != NULL)
	{
      parinte=robot;
	  robot=robot->next;
	  parkinglot_add_robot(parkinglot, parinte);
	}
	truck->unloading_robots=NULL;

}


// Depends on parking_turck_departed
void truck_update_depatures(Parkinglot* parkinglot, long day_hour){
	Truck* tir=parkinglot->arrived_trucks;
	Truck* parinte=tir;
	Truck* santinela=tir;
	tir=tir->next;
	while(tir != santinela)
	{
		parinte=tir;
		tir=tir->next;
		if(parinte->departure_time == day_hour)
		{
			truck_transfer_unloading_robots(parkinglot, parinte);
			truck_departed(parkinglot, parinte);
		}
		
		
	}
}

// Depends on parking_turck_arrived
void truck_update_transit_times(Parkinglot* parkinglot){
	Truck* tir=parkinglot->departed_trucks;
	Truck* santinela=tir;
	tir=tir->next;
	while(tir != santinela)
	{
        Truck* truck=tir;
		truck->in_transit_time++;
		tir=tir->next;		
		if(truck->in_transit_time == truck->transit_end_time)
	{
		truck->in_transit_time=0;
		truck_arrived(parkinglot, truck);
		
	}
	
	}
}

void robot_swarm_collect(Wearhouse *wearhouse, Parkinglot *parkinglot){
	Robot *head_robot = parkinglot->standby_robots;
	Robot *current_robot = parkinglot->standby_robots->next;
	while(current_robot != parkinglot->standby_robots){

		// Load packages from wearhouse if possible
		if(!robot_load_packages(wearhouse, current_robot)){
			break;
		}

		// Remove robot from standby list
		Robot *aux = current_robot;
		head_robot->next = current_robot->next;
		current_robot = current_robot->next;

		// Add robot to the
		parkinglot_add_robot(parkinglot, aux);
	}
}


void robot_swarm_assign_to_trucks(Parkinglot *parkinglot){

	Robot *current_robot = parkinglot->pending_robots->next;

	while(current_robot != parkinglot->pending_robots){
		Robot* aux = current_robot;
		current_robot = current_robot->next;
		parkinglot_remove_robot(parkinglot, aux);
		int attach_succeded = robot_attach_find_truck(aux, parkinglot);
		if(!attach_succeded)
			parkinglot_add_robot(parkinglot, aux);
	}
}

void robot_swarm_deposit(Parkinglot* parkinglot){
	Truck *arrived_iterator = parkinglot->arrived_trucks->next;
	while(arrived_iterator != parkinglot->arrived_trucks){
		Robot *current_robot = arrived_iterator->unloading_robots;
		while(current_robot != NULL){
			robot_unload_packages(arrived_iterator, current_robot);
			Robot *aux = current_robot;
			current_robot = current_robot->next;
			arrived_iterator->unloading_robots = current_robot;
			parkinglot_add_robot(parkinglot, aux);
		}
		arrived_iterator = arrived_iterator->next;
	}
}

