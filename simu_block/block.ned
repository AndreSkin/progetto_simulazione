import org.omnetpp.queueing.Allocate;
import org.omnetpp.queueing.Deallocate;
import org.omnetpp.queueing.Delay;
import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.ResourcePool;
import org.omnetpp.queueing.SourceOnce;


//
//
network Block
{
     @display("bgb=465,318");
    submodules:
        alloca: Allocate {
            @display("p=140,187");
        }
        dealloca: Deallocate {
            @display("p=354,187");
        }
        sorgente: SourceOnce {
            @display("p=41,73");
        }
        risorse: ResourcePool {
            @display("p=233,248");
        }
        is: Delay {
            @display("p=141,71");
        }
        servente: Queue {
            @display("p=276,80");
        }
    connections:
        sorgente.out --> is.in++;
        is.out --> alloca.in++;
        alloca.out --> servente.in++;
        servente.out --> dealloca.in++;
        dealloca.out --> is.in++;

}
