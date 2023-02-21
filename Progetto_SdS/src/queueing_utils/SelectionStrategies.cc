//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "SelectionStrategies.h"
#include "PassiveQueue.h"
#include "Server.h"

namespace queueing {

SelectionStrategy::SelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    isInputGate(selectOnInGate), hostModule(module)
{
    gateSize = isInputGate ? hostModule->gateSize("in") : hostModule->gateSize("out");
}

SelectionStrategy *SelectionStrategy::create(const char *algName, cSimpleModule *module, bool selectOnInGate)
{
    SelectionStrategy *strategy = nullptr;

    if (strcmp(algName, "priority") == 0) {
        strategy = new PrioritySelectionStrategy(module, selectOnInGate);
    }
    else if (strcmp(algName, "random") == 0) {
        strategy = new RandomSelectionStrategy(module, selectOnInGate);
    }
    else if (strcmp(algName, "roundRobin") == 0) {
        strategy = new RoundRobinSelectionStrategy(module, selectOnInGate);
    }
    else if (strcmp(algName, "shortestQueue") == 0) {
        strategy = new ShortestQueueSelectionStrategy(module, selectOnInGate);
    }
    else if (strcmp(algName, "longestQueue") == 0) {
        strategy = new LongestQueueSelectionStrategy(module, selectOnInGate);
    }

    //Aggiunta ThresholdSelectionStrategy
    else if (strcmp(algName, "threshold") == 0) {
        strategy = new ThresholdSelectionStrategy(module, selectOnInGate);
    }

    return strategy;
}

cGate *SelectionStrategy::selectableGate(int i)
{
    if (isInputGate)
        return hostModule->gate("in", i)->getPreviousGate();
    else
        return hostModule->gate("out", i)->getNextGate();
}

bool SelectionStrategy::isSelectable(cModule *module)
{
    if (isInputGate) {
        IPassiveQueue *pqueue = dynamic_cast<IPassiveQueue *>(module);
        if (pqueue != nullptr)
            return pqueue->length() > 0;
    }
    else {
        IServer *server = dynamic_cast<IServer *>(module);
        if (server != nullptr)
            return server->isIdle();
    }

    throw cRuntimeError("Only IPassiveQueue (as input) and IServer (as output) is supported by this Strategy");
}

// --------------------------------------------------------------------------------------------

PrioritySelectionStrategy::PrioritySelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int PrioritySelectionStrategy::select()
{
    // return the smallest selectable index
    for (int i = 0; i < gateSize; i++)
        if (isSelectable(selectableGate(i)->getOwnerModule()))
            return i;

    // if none of them is selectable return an invalid no.
    return -1;
}

// --------------------------------------------------------------------------------------------

RandomSelectionStrategy::RandomSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int RandomSelectionStrategy::select()
{
    // return the smallest selectable index
    int noOfSelectables = 0;
    for (int i = 0; i < gateSize; i++)
        if (isSelectable(selectableGate(i)->getOwnerModule()))
            noOfSelectables++;

    int rnd = hostModule->intuniform(1, noOfSelectables);

    for (int i = 0; i < gateSize; i++)
        if (isSelectable(selectableGate(i)->getOwnerModule()) && (--rnd == 0))
            return i;

    return -1;
}

// --------------------------------------------------------------------------------------------

RoundRobinSelectionStrategy::RoundRobinSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int RoundRobinSelectionStrategy::select()
{
    // return the smallest selectable index
    for (int i = 0; i < gateSize; ++i) {
        lastIndex = (lastIndex+1) % gateSize;
        if (isSelectable(selectableGate(lastIndex)->getOwnerModule()))
            return lastIndex;
    }

    // if none of them is selectable return an invalid no.
    return -1;
}

// --------------------------------------------------------------------------------------------

ShortestQueueSelectionStrategy::ShortestQueueSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int ShortestQueueSelectionStrategy::select()
{
    // return the smallest selectable index
    int result = -1;  // by default none of them is selectable
    int sizeMin = INT_MAX;
    for (int i = 0; i < gateSize; ++i) {
        cModule *module = selectableGate(i)->getOwnerModule();
        int length = (check_and_cast<IPassiveQueue *>(module))->length();
        if (isSelectable(module) && (length < sizeMin)) {
            sizeMin = length;
            result = i;
        }
    }
    return result;
}

// --------------------------------------------------------------------------------------------

LongestQueueSelectionStrategy::LongestQueueSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int LongestQueueSelectionStrategy::select()
{
    // return the longest selectable queue
    int result = -1;  // by default none of them is selectable
    int sizeMax = -1;
    for (int i = 0; i < gateSize; ++i) {
        cModule *module = selectableGate(i)->getOwnerModule();
        int length = (check_and_cast<IPassiveQueue *>(module))->length();
        if (isSelectable(module) && length > sizeMax) {
            sizeMax = length;
            result = i;
        }
    }
    return result;
}

//Aggiunta implementazione ThresholdSelectionStrategy
ThresholdSelectionStrategy::ThresholdSelectionStrategy(cSimpleModule *module, bool selectOnInGate) :
    SelectionStrategy(module, selectOnInGate)
{
}

int ThresholdSelectionStrategy::select()
{//TODO: code vuote


    EV << "Threshold strategy started\n";
    const int first_queue = 0;
    const int second_queue = 1;
    int first_thr = 0;
    int second_thr = 0;
    int L1 =0;
    int L2=0;

    int inputs = 0;
    const int max_input = 2;

    EV<<"LastSelected = "<<lastSelected<<"\n";

    //Conto input per verificare siano al massimo max_input
    for (int i = 0; i < gateSize; i++)
    {
        if (isSelectable(selectableGate(i)->getOwnerModule()))
        {
            inputs++;
        }
    }
    //EV << "There are " << inputs << " inputs\n";
    if (inputs > max_input)
    {
        throw cRuntimeError("Only two input gates are supported by this Strategy");
    }


    cModule *Queue_1 = selectableGate(first_queue)->getOwnerModule();
    first_thr = Queue_1->par("Threshold");
    EV <<"Threshold "<<first_queue<<" is "<< first_thr <<"\n";

    cModule *Queue_2 = selectableGate(second_queue)->getOwnerModule();
    second_thr = Queue_2->par("Threshold");
    EV <<"Threshold "<<second_queue<<" is "<< second_thr <<"\n";

    L1 = (check_and_cast<IPassiveQueue *>(Queue_1))->length();
    L2 = (check_and_cast<IPassiveQueue *>(Queue_2))->length();
    EV << "L1 = "<<L1<<"\t L2 = "<< L2<<"\n";

    //Controllo superamento threshold
    bool over_1 = L1 >= first_thr;
    bool over_2 = L2 >= second_thr;

    if(lastSelected == first_queue) //Sto servendo la prima coda
    {
        if((over_2) && (!(over_1))) //Se la seconda coda supera threshold e la prima no
        {
            lastSelected = second_queue;
            return second_queue; //Passo alla seconda coda
        }
        else
        {
            return lastSelected;
        }
    }
    else if (lastSelected == second_queue)//Se invece sto servendo la seconda coda
    {
        if((over_1) && (!(over_2))) //Se la prima coda supera threshold e la seconda no
        {
            lastSelected = first_queue;
            return first_queue; //Passo alla prima coda
        }
        else
        {
            return lastSelected;
        }
    }
    else
    {
        return lastSelected;
    }

    EV << "Ended\n";
    throw cRuntimeError("Ended");
}

}; //namespace

