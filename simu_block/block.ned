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
            @display("p=141,198");
        }
        dealloca: Deallocate {
            @display("p=294,64");
        }
        sorgente: SourceOnce {
            @display("p=39,64");
        }
        risorse: ResourcePool {
            @display("p=69,262");
        }
        is: Delay {
            @display("p=141,64");
        }
        servente: Queue {
            @display("p=294,198");
        }
    connections:
        sorgente.out --> is.in++;
        is.out --> alloca.in++;
        alloca.out --> servente.in++;
        servente.out --> dealloca.in++;
        dealloca.out --> is.in++;

}
