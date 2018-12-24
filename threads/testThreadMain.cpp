#include "scheduler.hpp"
#include <iostream>
JobScheduler * myJobScheduler;

int main(int argc, char const *argv[]) {

    myJobScheduler = new JobScheduler();

    myJobScheduler->Init(4);

    for (uint64_t i = 0; i < 15; i++) {
        myJobScheduler->Schedule(new JoinJob(i));
    }

    myJobScheduler->Barrier();

    myJobScheduler->Stop();

    myJobScheduler->Destroy();

    delete myJobScheduler;

    return 0;
}
