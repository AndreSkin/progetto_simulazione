package progetto_sds;

import progetto_sds.queueing_utils.PassiveQueue;
import progetto_sds.queueing_utils.Server;
import progetto_sds.queueing_utils.Sink;
import progetto_sds.queueing_utils.Source;


@license(LGPL);
//
// TODO documentation
//
network Network
{
    @display("bgb=673,359");
    submodules:
        S1: Source {
            @display("p=53,66;is=vl");
        }
        S2: Source {
            @display("p=53,259;is=vl");
        }
        first: PassiveQueue {
            @display("p=219,66;is=vl");
        }
        second: PassiveQueue {
            @display("p=219,259;is=vl");
        }
        servente: Server {
            @display("p=378,172;is=vl");
        }
        sink: Sink {
            @display("p=565,66;is=vl");
        }
        sink1: Sink {
            @display("p=565,278;is=vl");
        }
    connections:
        S1.out --> first.in++;
        S2.out --> second.in++;
        first.out++ --> servente.in++;
        second.out++ --> servente.in++;
        servente.out --> sink.in++;
        servente.out2 --> sink1.in++;
}
