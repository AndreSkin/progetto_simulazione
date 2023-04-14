//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Server.h"
#include "Job.h"
#include "SelectionStrategies.h"
#include "IPassiveQueue.h"

namespace queueing {

Define_Module(Server);

Server::~Server()
{
    delete selectionStrategy;
    delete jobServiced;
    cancelAndDelete(Begin_switchMsg);
    cancelAndDelete(End_switchMsg);
    cancelAndDelete(endServiceMsg);
}

void Server::initialize()
{
    busySignal = registerSignal("busy");
    emit(busySignal, false);

    endServiceMsg = new cMessage("end-service");

    switch_over_time=par("switchOverTime");

    jobServiced = nullptr;
    allocated = false;
    selectionStrategy = SelectionStrategy::create(par("fetchingAlgorithm"), this, true);
    if (!selectionStrategy)
        throw cRuntimeError("invalid selection strategy");
}

void Server::handleMessage(cMessage *msg)
{
    const int first = 0;
    const int second = 1;
    const int allempty = -1;
    int k = 0;

    if(msg==endServiceMsg)
    {
        char const* out = nullptr;
        EV<<"END SERVICE t= "<<simTime()<<"\n";

        //In base alla provenienza mando messaggio in sink differente

        switch(last)
       {
           case first:
           {
               //std::cout<<"OUT_1\n";
               out="out";
               break;
           }
           case second:
           {
               //std::cout<<"OUT_2\n";
               out="out2";
               break;
           }
           default:
           {
               //std::cout<<"Default\n";
               throw cRuntimeError("L'esecuzione non dovrebbe mai arrivare qui");
           }
       }

        ASSERT(jobServiced != nullptr);
        ASSERT(allocated);
        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);
        send(jobServiced, out);
        jobServiced = nullptr;
        allocated = false;
        emit(busySignal, false);

        // examine all input queues, and request a new job from a non empty queue
        k = selectionStrategy->select();
        if (k >= 0)
        {
            if(k==last)//No switch
            {
                EV << "Requesting job from queue " << k << endl;
                cGate *gate = selectionStrategy->selectableGate(k);
                check_and_cast<IPassiveQueue *>(gate->getOwnerModule())->request(gate->getIndex());
            }
            else
            {
                EV<<"SWITCH OVER\n";
                last= k;
                simtime_t sendtime = endServiceMsg->isScheduled()?endServiceMsg->getArrivalTime():simTime();
                scheduleAt(sendtime, Begin_switchMsg);
            }
        }
        else if(k == allempty)
        {
            EV<<"Code vuote, servente aspetta\n";
        }
    }
    else if(msg==Begin_switchMsg)
    {
       EV<<"BEGIN SWITCH t= "<<simTime()<<"\n";
       allocated=true;
       scheduleAt(simTime()+switch_over_time, End_switchMsg);
    }
    else if(msg==End_switchMsg)
    {
        EV<<"END SWITCH t= "<<simTime()<<"\n";
        allocated=false;

        k = selectionStrategy->select();
        EV << "Requesting after switch from: " << k << endl;
        cGate *gate = selectionStrategy->selectableGate(k);
        check_and_cast<IPassiveQueue *>(gate->getOwnerModule())->request(gate->getIndex());
    }
    else //JOB
    {
        EV<<"JOB t= "<<simTime()<<"\n";

        if (!allocated)
            error("job arrived, but the sender did not call allocate() previously");
        if (jobServiced)
            throw cRuntimeError("a new job arrived while already servicing one");

        simtime_t serviceTime= 0;
        //Aggiunto switch per serviceTime differenti
        switch(last)
        {
            case first:
            {
                serviceTime = par("serviceTime");
                break;
            }
            case second:
            {
                serviceTime = par("serviceTime_2");
                break;
            }
            default:
            {
                throw cRuntimeError("Coda scelta non presente");
            }
        }

        jobServiced = check_and_cast<Job *>(msg);
        scheduleAt(simTime()+serviceTime, endServiceMsg);
        emit(busySignal, true);
    }

}

void Server::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
}

void Server::finish()
{
}

bool Server::isIdle()
{
    return !allocated;  // we are idle if nobody has allocated us for processing
}

void Server::allocate()
{
    allocated = true;
}

}; //namespace

