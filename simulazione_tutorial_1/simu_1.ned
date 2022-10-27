
import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


network Rete
{
    @display("bgb=515,178");
    submodules:
        Pozzo: Sink {
            @display("p=423,77;is=vl");
        }
        Arrivi: Source {
            @display("p=80,77;is=vl");
        }
        Servente: Queue {
            @display("p=254,77;is=vl");
        }
        
        
    connections:
        Arrivi.out --> Servente.in++;
        Servente.out --> Pozzo.in++;
}

