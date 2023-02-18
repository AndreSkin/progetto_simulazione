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
    @display("bgb=689,238");
    submodules:
        S1: Source {
            @display("p=53,52");
        }
        S2: Source {
            @display("p=53,189");
        }
        first: PassiveQueue {
            @display("p=187,52");
        }
        second: PassiveQueue {
            @display("p=187,189");
        }
        servente: Server {
            @display("p=315,134");
        }
        sink: Sink {
            @display("p=449,52");
        }
        sink1: Sink {
            @display("p=449,189");
        }
    connections:
        S1.out --> first.in++;
        S2.out --> second.in++;
        first.out++ --> servente.in++;
        second.out++ --> servente.in++;
        servente.out --> sink.in++;
        servente.out2 --> sink1.in++;
}
