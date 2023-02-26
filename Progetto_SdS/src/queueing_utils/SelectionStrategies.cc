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
{
    EV << "Threshold strategy started\n";

    //Le due code e il caso in cui siano vuote
    const int first_queue = 0;
    const int second_queue = 1;
    const int empty_queues = -1;

    //Threshold delle code
    int first_thr = 0;
    int second_thr = 0;

    //Lunghezza delle code e dell'ultima coda scelta
    int L1 =0;
    int L2=0;
    int L_last=0;

    //Numero di code collegate al servente (massimo 2)
    int inputs = 0;
    const int max_input = 2;

    EV<<"LastSelected = "<<lastSelected;
    if(switched)
    {
        EV<<" (switch over)\n";
    }
    else
    {
        EV<<"\n";
    }

    //Conto input per verificare siano al massimo max_input
    for (int i = 0; i < gateSize; i++)
    {
        if (isSelectable(selectableGate(i)->getOwnerModule()))
        {
            inputs++;
        }
    }

    if (inputs > max_input)
    {
        throw cRuntimeError("Questa strategy supporta solo due input");
    }

    //Ottenimento delle threshold delle code
    //Prima coda
    cModule *Queue_1 = selectableGate(first_queue)->getOwnerModule();
    first_thr = Queue_1->par("Threshold");

    //Seconda coda
    cModule *Queue_2 = selectableGate(second_queue)->getOwnerModule();
    second_thr = Queue_2->par("Threshold");
    EV <<"T0 = "<<first_thr<<" T1 = "<< second_thr <<"\n";

    //Ottenimento lunghezza delle due code e dell'ultima selezionata
    L1 = (check_and_cast<IPassiveQueue *>(Queue_1))->length();
    L2 = (check_and_cast<IPassiveQueue *>(Queue_2))->length();
    EV << "L0 = "<<L1<<"\t L1 = "<< L2<<"\n";

    L_last = (lastSelected == first_queue ? L1 : L2); //Lunghezza di lastSelected


    //Controllo superamento threshold
    bool over_1 = L1 >= first_thr;
    bool over_2 = L2 >= second_thr;
    //Controllo code vuote
    bool empty_1 = L1 <= 0;
    bool empty_2 = L2 <= 0;

    if((empty_1) && (empty_2))//Code vuote
    {
        EV<<"Tutte le code sono vuote\n";
        were_empty = true; //Per ricordare che le code potrebbero ancora essere vuote
        return empty_queues; //-1
    }

    if (were_empty) //Se le code prima erano vuote
    {
        switch(lastSelected)
        {
            case first_queue:
            {
                if(!(empty_1) && (empty_2)) //Controllo se Q1 non è vuota e Q2 si
                {
                    were_empty = false;
                    return first_queue; //Riprendo a servire Q1
                }
                else if(!(empty_2) && (empty_1)) //Se invece Q2 non è vuota e Q1 si
                {
                    switched=true;
                    lastSelected = second_queue;
                    were_empty = false;
                    return second_queue; //Passo a servire Q2
                }
                break;
            }
            case second_queue:
            {
                if(!(empty_2) && (empty_1)) //Controllo se Q2 non è vuota e Q1 si
               {
                   were_empty = false;
                   return second_queue; //Riprendo a servire Q2
               }
               else if(!(empty_1) && (empty_2)) //Se invece Q1 non è vuota e Q2 si
               {
                   switched=true;
                   lastSelected = first_queue;
                   were_empty = false;
                   return first_queue; //Passo a servire Q1
               }
                break;
            }
            default://Se sono ancora tutte e due vuote lo controllo sopra e ritorno -1
            {
                throw cRuntimeError("L'esecuzione non dovrebbe mai arrivare qui");
            }
        }
    }

    if(switched) //Se ho appena fatto switch over
    {
      if(((lastSelected==first_queue && !(empty_1))) || ((lastSelected==second_queue && !(empty_2))))
      {
          switched=false;
          return lastSelected;
      }
      else
      {
          switched=true;
          lastSelected = 1- lastSelected;
          return 1- lastSelected;
      }
  }

    if(L_last == 0) //Se solo l'ultima selezionata è vuota, cambio coda
    {
        switch(lastSelected)
        {
            case first_queue:
            {
               switched=true;
               lastSelected = second_queue;
               return second_queue; //Passo alla seconda coda
                break;
            }
            case second_queue:
            {
                switched=true;
                lastSelected = first_queue;
                return first_queue; //Passo alla prima coda
                break;
            }
            default: //NB: le code non possono essere entrambe vuote a questo punto
            {
                throw cRuntimeError("L'esecuzione non dovrebbe mai arrivare qui");
            }
        }
    }

    switch(lastSelected)
    {
        case first_queue:
        {
            if((over_2) && (!(over_1))) //Se la seconda coda supera threshold e la prima no
            {
                switched=true;
                lastSelected = second_queue;
                return second_queue; //Passo alla seconda coda
            }
            else
            {
                return lastSelected;
            }
            break;
        }
        case second_queue:
        {
            if((over_1) && (!(over_2))) //Se la prima coda supera threshold e la seconda no
            {
                switched=true;
                lastSelected = first_queue;
                return first_queue; //Passo alla prima coda
                }
            else
            {
                return lastSelected;
            }
            break;
        }
        default:
        {
            throw cRuntimeError("L'esecuzione non dovrebbe mai arrivare qui");
        }
    }
}

}; //namespace

