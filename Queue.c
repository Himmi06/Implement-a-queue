
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <queue>
using namespace std;

#define THREADS_COUNT 2

bool addNewProcess(); // To add new process, return true if added and false if not added
int getNewProcessId(); // To get new process id to assign to new process
void *CPU(void*); // Thread act like CPU
void *processSchedular(void*); // Thread act like Schedular

static bool simulation_flag = false;
static priority_queue<vector<int>> level_one_queue;
static queue<vector<int>> level_two_queue;
static priority_queue<vector<int>> process_scheduling_ready_queue;
static long long int live_simulation_time = 0;
static vector<int> all_process_id_info_in_queue;
static int cpu_time_quantum = -1;
static int cpu_process_id = -1;
static int cpu_process_bursttime = -1;
static int cpu_process_priority = -1;
static int cpu_process_arivaltime = -1;
static int process_scheduler_work_info = -1;
static bool cpu_simulation_process_finish_interept = true;
static int cpu_simulation_look = 0; // 1 for CPU , 2 for scheduler


int main(int argc, const char * argv[]) {
    // Variable Declaration
    pthread_t threads[THREADS_COUNT];
    bool flag = true; // To exit the program
    int choose; // To choose between options
    int thread;
    cout<<"Simulation of two level priority and round robin queue scheduling algorithm"<<endl;
    cout<<endl<<endl<<"Starting CPU and Schedular"<<endl<<endl;
    thread = pthread_create(&threads[0], NULL, CPU, NULL);
    thread = pthread_create(&threads[1], NULL, processSchedular, NULL);
    while(flag){
        cout<<endl<<"Simulation of process will start when new process will be added"<<endl;
        cout<<"To see the simulation it should be enabled"<<endl<<endl;
        cout<<"1 for add new process"<<endl;
        cout<<"2 to see / enable the simulation"<<endl;
        cout<<"3 for EXIT"<<endl;
        cout<<"Enter Value : ";
        cin>>choose;
        switch (choose) {
            case 1:
                cout<<endl<<endl;
                while (!addNewProcess()) {
                    cout<<endl<<endl<<"Unable to add new process. Try again"<<endl<<endl;
                }
                break;
                
            case 2:
                simulation_flag = true;
                break;
            
            case 3:
                flag = false;
                cout<<endl<<"PROGRAM EXITS"<<endl;
                break;
            
            default:
                cout<<endl<<endl<<endl<<"Wrong value entered enter again"<<endl<<endl;
                break;
        }
    }
    return 0;
}


bool addNewProcess(){
    bool flag = true;
    int p_id = -1, p_priority = -1 ,p_bursttime = 0;
    p_id = getNewProcessId();
    cout<<endl<<"Enter Process Priority (0 - high) : ";
    cin>>p_priority;
    cout<<endl<<"Enter Process Burst Time (0 - high) : ";
    cin>>p_bursttime;
    if(p_id <= -1 && p_priority <= -1 && p_bursttime <= 0){
        cout<<endl<<"Process Burst Time should be greater than 0 and Process Priority should be greater or equal to 0"<<endl;
        flag = false;
    }else {
        // Add New Process To Process Scheduling
        process_scheduling_ready_queue.push({p_priority,p_id,p_bursttime,(int)live_simulation_time});
        all_process_id_info_in_queue.push_back(p_id);
    }
    return flag;
}

int getNewProcessId(){
    int id = 0;
    auto check = find(all_process_id_info_in_queue.begin(), all_process_id_info_in_queue.end(), id);
    while(check != all_process_id_info_in_queue.end()){
        id++;
    }
    return id;
}

void *processSchedular(void* value){
    while(true){
        // Check Weather CPU is going to ideal only for simulation
        if(cpu_simulation_process_finish_interept){
            // Assign New Process to the CPU
            if(!level_one_queue.empty()){
               // Normal Priority Queue Scheduling
                vector<int> i = level_one_queue.top();
                cpu_process_priority = i[0];
                cpu_process_id = i[1];
                cpu_process_bursttime = i[2];
                cpu_process_arivaltime = i[3];
                level_one_queue.pop();
            }else if (!level_two_queue.empty()){
               // Round Robin Scheduling
                
                vector<int> i = level_two_queue.front();
                // CPU TIME QUANTUM for Round Robin Algorithm
                cpu_time_quantum = 2;
                cpu_process_priority = i[0];
                cpu_process_id = i[1];
                cpu_process_bursttime = i[2];
                cpu_process_arivaltime = i[3];
                level_two_queue.pop();
            }
            cpu_simulation_process_finish_interept = false;
        }
        if(!process_scheduling_ready_queue.empty()){
            vector<int> j = process_scheduling_ready_queue.top();
            if(j[0]<cpu_process_priority){
                // Asign process to CPU
                while(cpu_simulation_look != 0)
                {
                    // Set Lock to Prempt CPU Process
                    cpu_simulation_look = 2;
                    
                        // Send Current Process to Level 2 queue if burst time is not 0
                    if(cpu_process_bursttime != 0){
                        level_two_queue.push({cpu_process_priority,cpu_process_id,cpu_process_bursttime,cpu_process_arivaltime});
                    }
                        // Assign High Priority Process to CPU
                    cpu_time_quantum = -1;
                    cpu_process_priority = j[0];
                    cpu_process_id = j[1];
                    cpu_process_bursttime = j[2];
                    cpu_process_arivaltime = j[3];
                    
                    // Realease look to let CPU Work
                    cpu_simulation_look = 0;
                }
            }else {
                // Send Process to Level One Queue
                level_one_queue.push({j[0],j[1],j[2],j[3]});
            }
            process_scheduling_ready_queue.pop();
        
        }
    }
    pthread_exit(NULL);
}

void *CPU(void* value){
    while (true) {
        live_simulation_time++;
        while(cpu_simulation_look != 0 && cpu_process_bursttime != 0){
            cpu_simulation_look = 1;
            cpu_process_bursttime--;
            if(cpu_process_bursttime == 0){
                // Send Interpt to scheduler for next process
                cpu_simulation_process_finish_interept = true;
            }
            cpu_simulation_look = 0;
        }
        
        sleep(1);
    }
    pthread_exit(NULL);
}
